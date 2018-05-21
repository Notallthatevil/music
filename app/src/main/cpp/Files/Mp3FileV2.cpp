//
// Created by Nate on 5/21/2018.
//

#include "Mp3FileV2.h"

Mp3FileV2::Mp3FileV2(string *filePath, bool findTags) : AudioFile(filePath) {
    if (findTags) {
        //Get tag size
        char header[ID3TagV2::HEADER_SIZE];
        stream->read(header, ID3TagV2::HEADER_SIZE);
        id3Tag = new ID3TagV2(header);

        //Read tag into a buffer
        unsigned char *tagBuffer = new unsigned char[id3Tag->getTagSize()];
        stream->read((char*)tagBuffer, id3Tag->getTagSize());

        //Read data from buffer into id3tag
        id3Tag->readTags(tagBuffer);

        delete[]tagBuffer;
        tagBuffer = NULL;
    }
}

Mp3FileV2::~Mp3FileV2() {

}

char *Mp3FileV2::getAudio() {
    return nullptr;
}

Tag *Mp3FileV2::getTag() {
    return id3Tag;
}
