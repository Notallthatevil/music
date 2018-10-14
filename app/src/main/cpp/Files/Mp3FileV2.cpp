//
// Created by Nate on 5/21/2018.
//

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


/*	Returns value based on success
	-1 = header was null
	0 = success
	1 = invalid header
	*/
int Mp3FileV2::parse(bool findTags) {
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
		return 0;
	}
	else {
		return rc;
	}
}






Mp3FileV2::Mp3FileV2(vector<char> deserialize) : AudioFile(deserialize) {
    mId3Tag = new ID3TagV2;
    setID(int((unsigned char) deserialize[1] << 24 |
              (unsigned char) deserialize[2] << 16 |
              (unsigned char) deserialize[3] << 8 |
              (unsigned char) deserialize[4]));
    int assign = 0;
    int offset = 5;
    while (offset < deserialize.size()) {
        unsigned long length = (unsigned long) ((unsigned char) deserialize[offset] << 24 |
                                                (unsigned char) deserialize[offset + 1] << 16 |
                                                (unsigned char) deserialize[offset + 2] << 8 |
                                                (unsigned char) deserialize[offset + 3]);
        offset += 4;
        vector<char> subBuffer(deserialize.begin() + offset, deserialize.begin() + offset + length);
        if (length > 0) {
            switch (assign) {
                case 0:
                    getTag()->setTitle(string(subBuffer.data(), length));
                    break;
                case 1:
                    getTag()->setArtist(string(subBuffer.data(), length));
                    break;
                case 2:
                    getTag()->setAlbum(string(subBuffer.data(), length));
                    break;
                case 3:
                    getTag()->setTrack(string(subBuffer.data(), length));
                    break;
                case 4:
                    getTag()->setYear(string(subBuffer.data(), length));
                    break;
                case 5:
                    setFilePath(string(subBuffer.data(), length));
                    break;
                case 6:
                    //getTag()->setCover((unsigned char *) subBuffer.data());
                    //getTag()->setCoverSize(length);
                default:
                    break;
            }
        }
        offset += length;
        assign++;
    }
}

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
