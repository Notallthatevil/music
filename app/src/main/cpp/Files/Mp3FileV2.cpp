//
// Created by Nate on 5/21/2018.
//

#include "Mp3FileV2.h"

Mp3FileV2::Mp3FileV2(string *filePath, bool findTags) : AudioFile(filePath) {
    unsigned char header[ID3TagV2::HEADER_SIZE];
    stream->read((char *) &header, ID3TagV2::HEADER_SIZE);

    if (header[0] == 'I' && header[1] == 'D' && header[2] == '3') {
        id3Tag = new ID3TagV2(header);
        if (findTags) {
            //Get tag size

            //Read tag into a buffer
            long bufferSize = id3Tag->getTagSize() - id3Tag->getHeaderSize();
            unsigned char *tagBuffer = new unsigned char[bufferSize];
            stream->read((char *) tagBuffer, bufferSize);

            //Read data from buffer into id3tag
            id3Tag->readTags(tagBuffer);

            delete[] tagBuffer;
            tagBuffer = nullptr;
        }
    }

}

Mp3FileV2::Mp3FileV2(vector<char> deserialize) : AudioFile(deserialize) {
    id3Tag = new ID3TagV2;
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
                    getTag()->setCover((unsigned char *) subBuffer.data());
                default:
                    break;
            }
        }
        offset += length;
        assign++;
    }
}

Mp3FileV2::~Mp3FileV2() {
    delete id3Tag;
    id3Tag = nullptr;
}

AudioData Mp3FileV2::getAudio()  {
    audioData.size = getFileSize() - id3Tag->getTagSize();
    stream->seekg(id3Tag->getTagSize(),ios_base::beg);
    return AudioFile::getAudio();
}

Tag *Mp3FileV2::getTag() {
    return id3Tag;
}
