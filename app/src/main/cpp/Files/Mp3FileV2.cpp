//
// Created by Nate on 5/21/2018.
//

#include <cmath>
#include "Mp3FileV2.h"


Mp3FileV2::Mp3FileV2(string *filePath, bool findTags) : AudioFile(filePath) {
    //open();
    unsigned char header[ID3TagV2::HEADER_SIZE];
    mStream->read((char *) &header, ID3TagV2::HEADER_SIZE);
    if (header[0] == 'I' && header[1] == 'D' && header[2] == '3') {
        mId3Tag = new ID3TagV2(header);
        if (findTags) {
            //Get tag size

            //Read tag into a buffer
            long bufferSize = mId3Tag->getTagSize() - mId3Tag->getHeaderSize();
            unsigned char *tagBuffer = new unsigned char[bufferSize];
            mStream->read((char *) tagBuffer, bufferSize);

            //Read data from buffer into id3tag
            mId3Tag->readTags(tagBuffer);

            delete[] tagBuffer;
            tagBuffer = nullptr;
        }
    }
}






/// PARSING



/*	Returns value based on success
	-2 = unable to open file
	-1 = header was null
	0 = success
	1 = invalid header
	*/
int Mp3FileV2::parse(bool findTags) {
	if (!open()) {
		return -2;
	}
	unsigned char header[ID3TagV2::HEADER_SIZE];
	mStream->read((char *)&header, ID3TagV2::HEADER_SIZE);
	
	mId3Tag = new ID3TagV2();
	int rc = mId3Tag->readHeader(header);
	if (rc != 0) {
		return rc;
	}
	if (findTags) {
		//get tag size
		int bufferSize = mId3Tag->getTagSize() - mId3Tag->getHeaderSize();

		//read tags into buffer
		unsigned char *tagBuffer = new unsigned char[bufferSize];

		mStream->read((char*)tagBuffer, bufferSize);
		//parse tags
		rc = mId3Tag->readTags(tagBuffer);

		delete[] tagBuffer;
		tagBuffer = nullptr;
	}
	
	rc = parseMp3Data();

	return rc;
}



int Mp3FileV2::parseMp3Data() {

	int rc;

	//Seek to audio
	mStream->seekg(mId3Tag->getTagSize(), ios::beg);

	//Find audio frame 0xFFE
	char c;
	// NEED TO FIND OUT WHAT HAPPENS IF THIS IS NEVER FOUND
	while (mStream->get(c)) {
		if (c == (char)0xff && ((mStream->peek() >> 5) == (char)0x07)) {
			//header found
			mStream->get(c);
			Mp3FileV2::MP3_VERSION = ((c >> 3) & (char)0x03);
			Mp3FileV2::MP3_LAYER = ((c >> 1) & (char)0x03);

			mStream->get(c);
			AudioFile::BITRATE = findBitrate((c >> 4) & (char)0x0F);
			AudioFile::SAMPLERATE = findSampleRate((c>> 2) & (char)0x03);
			break;
		}
	}

	//Discover if file is CBR or VBR
	//NEEDS TO BE FIXED TO PULL IN THE WHOLE FRAME SIZE
	rc = checkForVBRHeader();
	if (rc == 0) {
		char xingBuffer[116];
		mStream->read(xingBuffer, 116);
		rc = parseXingHeader(xingBuffer);
		if (rc = 0) {
			AudioFile::mDuration = (Mp3FileV2::MP3_NUM_OF_FRAMES * (getSamples() / AudioFile::SAMPLERATE)) * 1000;
		}
	}else if(rc == 1){
		char vbriHeader[28];
		mStream->read(vbriHeader, 28);
		rc = parseVBRIHeader(vbriHeader);
		if (rc = 0) {
			AudioFile::mDuration = (Mp3FileV2::MP3_NUM_OF_FRAMES * (getSamples() / AudioFile::SAMPLERATE)) * 1000;
		}
	}

	//No VBR header not found/header doesn't contain number of frames. Need to check frames
	if (rc == -1) {
		AudioFile::mDuration = (((AudioFile::mFileSize - Mp3FileV2::mId3Tag->getTagSize()) / getAverageBitrate()) * 8) * 1000;
	}

	return 0;
}

int Mp3FileV2::findBitrate(char bitrateChar) {
	switch (bitrateChar) {
	case (char)0x00:
		return 0;
	case (char)0x01:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 32;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 32;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 8;
			}
		
		}
	case (char)0x02:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 64;
			case Mp3FileV2::LAYER_II:
				return 48;
			case Mp3FileV2::LAYER_III:
				return 40;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 48;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 16;
			}

		}
		
	case (char)0x03:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 96;
			case Mp3FileV2::LAYER_II:
				return 56;
			case Mp3FileV2::LAYER_III:
				return 48;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 56;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 24;
			}

		}

	case (char)0x04:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 128;
			case Mp3FileV2::LAYER_II:
				return 64;
			case Mp3FileV2::LAYER_III:
				return 56;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 64;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 32;
			}

		}

	case (char)0x05:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 160;
			case Mp3FileV2::LAYER_II:
				return 80;
			case Mp3FileV2::LAYER_III:
				return 64;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 80;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 40;
			}

		}

	case (char)0x06:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 192;
			case Mp3FileV2::LAYER_II:
				return 96;
			case Mp3FileV2::LAYER_III:
				return 80;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 96;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 48;
			}

		}

	case (char)0x07:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 224;
			case Mp3FileV2::LAYER_II:
				return 112;
			case Mp3FileV2::LAYER_III:
				return 96;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 112;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 56;
			}

		}

	case (char)0x08:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 256;
			case Mp3FileV2::LAYER_II:
				return 128;
			case Mp3FileV2::LAYER_III:
				return 112;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 128;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 64;
			}

		}

	case (char)0x09:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 288;
			case Mp3FileV2::LAYER_II:
				return 160;
			case Mp3FileV2::LAYER_III:
				return 128;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 144;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 80;
			}

		}

	case (char)0x0A:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 320;
			case Mp3FileV2::LAYER_II:
				return 192;
			case Mp3FileV2::LAYER_III:
				return 160;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 160;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 96;
			}

		}

	case (char)0x0B:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 352;
			case Mp3FileV2::LAYER_II:
				return 224;
			case Mp3FileV2::LAYER_III:
				return 192;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 176;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 112;
			}

		}

	case (char)0x0C:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 384;
			case Mp3FileV2::LAYER_II:
				return 256;
			case Mp3FileV2::LAYER_III:
				return 224;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 192;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 128;
			}

		}

	case(char)0x0D:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 416;
			case Mp3FileV2::LAYER_II:
				return 320;
			case Mp3FileV2::LAYER_III:
				return 256;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 224;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 144;
			}

		}

	case (char)0x0E:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 448;
			case Mp3FileV2::LAYER_II:
				return 384;
			case Mp3FileV2::LAYER_III:
				return 320;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 256;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 160;
			}

		}
	case (char) 0x0F:
		return 0;
	default:
		return -3;
	}
	
}

int Mp3FileV2::findSampleRate(char sampleRateChar) {
	switch (sampleRateChar){
	case (char)0x00:
		switch (Mp3FileV2::MP3_VERSION){
		case VERSION_1:
			return 44100;
		case VERSION_2:
			return 22050;
		case VERSION_2_5:
			return 11025;
		}
	case (char)0x01:
		switch (Mp3FileV2::MP3_VERSION) {
		case VERSION_1:
			return 48000;
		case VERSION_2:
			return 24000;
		case VERSION_2_5:
			return 12000;
		}
	case(char)0x02:
		switch (Mp3FileV2::MP3_VERSION) {
		case VERSION_1:
			return 32000;
		case VERSION_2:
			return 16000;
		case VERSION_2_5:
			return 8000;
		}
	default:
		return -2;
	}
}

int Mp3FileV2::getSamples() {
	switch (Mp3FileV2::MP3_VERSION){
	case Mp3FileV2::VERSION_1:
		switch (Mp3FileV2::MP3_LAYER){
		case Mp3FileV2::LAYER_I:
			return 384;
		case Mp3FileV2::LAYER_II:
		case Mp3FileV2::LAYER_III:
			return 1152;

		}
	case Mp3FileV2::VERSION_2:
		switch (Mp3FileV2::MP3_LAYER) {
		case Mp3FileV2::LAYER_I:
			return 384;
		case Mp3FileV2::LAYER_II:
			return 1152;
		case Mp3FileV2::LAYER_III:
			return 576;
		}
	case Mp3FileV2::VERSION_2_5:
		switch (Mp3FileV2::MP3_LAYER) {
		case Mp3FileV2::LAYER_I:
			return 384;
		case Mp3FileV2::LAYER_II:
			return 1152;
		case Mp3FileV2::LAYER_III:
			return 576;
		}

	default:
		return -2;
	}
}


/* Parses first Mp3 frame looking for a VBR header returns an int based on success
* 0 = Found a Xing header
* 1 = Found a VBRI header
* -1 = Found another Mp3 header
*
* If 2 is returned the file is either CBR or the average bitrate must be found. To be safe we look at a couple other frames
* and compare the bitrates to check for consistency. 
*/
int Mp3FileV2::checkForVBRHeader(){
	char VBRHeader[4];
	int tempPos = 0;
	while (mStream->get(VBRHeader[0])) {
		if (VBRHeader[0] == 'X' || VBRHeader[0] == 'I') {
			tempPos = mStream->tellg();
			mStream->read(&VBRHeader[1], 3);
			if ((VBRHeader[1] == 'i' && VBRHeader[2] == 'n' & VBRHeader[3] == 'g') ||
				(VBRHeader[1] == 'n' && VBRHeader[2] == 'f' & VBRHeader[3] == 'o')) {
					return 0;
				
			}
			else {
				mStream->seekg(tempPos);
			}
		}
		else if (VBRHeader[0] == 'V') {
			tempPos = mStream->tellg();
			mStream->read(&VBRHeader[1], 3);
			if (VBRHeader[1] == 'B' && VBRHeader[2] == 'R' & VBRHeader[3] == 'I') {
				return 1;
			}

		}
		else if (VBRHeader[0] == (char)0xff && ((mStream->peek() >> 5) == (char)0x07)) {
			return -1;
		}
	}
}

int Mp3FileV2::parseXingHeader(char * xingBuffer){
	bool framesField = xingBuffer[7] & (char)0x01;
	bool bytesField = xingBuffer[7] & (char)0x02;
	bool tocfield = xingBuffer[7] & (char)0x04;
	bool qualityfield = xingBuffer[7] & (char)0x08;
	
	int offset = 4;
	if (framesField) {
		Mp3FileV2::MP3_NUM_OF_FRAMES = xingBuffer[offset++] << 24 | xingBuffer[offset++] << 16 | xingBuffer[offset++] << 8 | xingBuffer[offset++];
		return 0;
	}
	return -1;
	//Other two flags not necessary at this time
}

int Mp3FileV2::parseVBRIHeader(char * vbriBuffer){
	int offset = 14;
	Mp3FileV2::MP3_NUM_OF_FRAMES = vbriBuffer[offset++] << 24 | vbriBuffer[offset++] << 16 | vbriBuffer[offset++] << 8 | vbriBuffer[offset++];
	return 0;
}






int Mp3FileV2::getAverageBitrate(){
	int tempPos = mStream->tellg();
	mStream->seekg(AudioFile::mFileSize / 3);
	int bitrate1 = -1;
	int bitrate2 = -1;
	char c;
	while (mStream->get(c)) {
		if (c == (char)0xff && ((mStream->peek() >> 5) == (char)0x07)) {

			mStream->get(c);
			mStream->get(c);
			if (bitrate1 == -1) {
				bitrate1 = findBitrate((c >> 4) & (char)0x0F);
				mStream->seekg(mStream->tellg()*2);
			}
			else {
				bitrate2 = findBitrate((c >> 4) &(char)0x0F);
				break;
			}
		}
	}
	if (AudioFile::BITRATE == bitrate1 && bitrate1 == bitrate2) {
		return AudioFile::BITRATE;
	}

	else {
		int frames = 1;
		int totalBitrate = AudioFile::BITRATE;
		mStream->seekg(tempPos);
		int allowedAttempts = AudioFile::mFileSize / 50;
		int attempts = 0;
		while (mStream->get(c)) {
			if (c == (char)0xff && ((mStream->peek() >> 5) == (char)0x07)) {

				mStream->get(c);
				mStream->get(c);
				
				int frameBitrate = findBitrate((c >> 4) & (char)0x0F);
				if (frameBitrate > 0) {
						totalBitrate += frameBitrate;
						frames++;
						int frameSize = calculateFrameSize(getSamples(), frameBitrate, findSampleRate(c & 0x0C), (c & 0x02));
						int currentOffset = mStream->tellg();
						if (frameSize < 0) {
							return-1;
						}

						//Subtract 3 from offset for already being 3 bytes in
						mStream->seekg(currentOffset-3 + frameSize);

						attempts = 0;
				}
			}
			attempts++;
			if (attempts>allowedAttempts){
				return -1;
			}
		}
		return totalBitrate / frames;
	}
}


inline int Mp3FileV2::calculateFrameSize(int samplesPerFrame, int frameBitrate, int samplerate, int padding)
{
	return round(((samplesPerFrame / 8 * frameBitrate * 1000) / samplerate) + padding);
}


/*


DURATION
 
DURATION IN MS = ((filesize in bits)/(bitrate * 1000)) * 1000

FRAMESIZE

FRAMESIZE IN BYTES = (((Samples Per Frame / 8 * Bitrate) / Sampling Rate) + Padding Size) * 1000





*/






























//NEED TO PREPARE A TAG AND ATTACH INSTEAD OF CREATE A NEW MP3

//Mp3FileV2::Mp3FileV2(vector<char> deserialize) : AudioFile(deserialize) {
//    mId3Tag = new ID3TagV2;
//    setID(int((unsigned char) deserialize[1] << 24 |
//              (unsigned char) deserialize[2] << 16 |
//              (unsigned char) deserialize[3] << 8 |
//              (unsigned char) deserialize[4]));
//    int assign = 0;
//    int offset = 5;
//    while (offset < deserialize.size()) {
//        unsigned long length = (unsigned long) ((unsigned char) deserialize[offset] << 24 |
//                                                (unsigned char) deserialize[offset + 1] << 16 |
//                                                (unsigned char) deserialize[offset + 2] << 8 |
//                                                (unsigned char) deserialize[offset + 3]);
//        offset += 4;
//        vector<char> subBuffer(deserialize.begin() + offset, deserialize.begin() + offset + length);
//        if (length > 0) {
//            switch (assign) {
//                case 0:
//                    getTag()->setTitle(string(subBuffer.data(), length));
//                    break;
//                case 1:
//                    getTag()->setArtist(string(subBuffer.data(), length));
//                    break;
//                case 2:
//                    getTag()->setAlbum(string(subBuffer.data(), length));
//                    break;
//                case 3:
//                    getTag()->setTrack(string(subBuffer.data(), length));
//                    break;
//                case 4:
//                    getTag()->setYear(string(subBuffer.data(), length));
//                    break;
//                case 5:
//                    setFilePath(string(subBuffer.data(), length));
//                    break;
//                case 6:
//                    //getTag()->setCover((unsigned char *) subBuffer.data());
//                    //getTag()->setCoverSize(length);
//                default:
//                    break;
//            }
//        }
//        offset += length;
//        assign++;
//    }
//}

Mp3FileV2::~Mp3FileV2() {
    delete mId3Tag;
    mId3Tag = nullptr;
}

vector<char> Mp3FileV2::getAudio()  {
    return AudioFile::getAudio();
}

Tag *Mp3FileV2::getTag() {
    return mId3Tag;
}

//FIXME trims the first little bit of audio
void Mp3FileV2::setAudio() {
    mAudioData.resize(getFileSize() - mId3Tag->getTagSize());
    mStream->seekg(mId3Tag->getTagSize(),ios_base::beg);
    AudioFile::setAudio();
}
