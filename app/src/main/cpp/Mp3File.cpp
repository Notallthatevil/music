//
// Created by natet on 3/27/2018.
//

#include "Mp3File.h"


Mp3File::Mp3File(string *filepath) : File(filepath) {
    char header[10];
    fileStream->read(header,10);
    //TODO Add try catch for this statement
    unsigned long tagSize = (unsigned long)ID3Tag::getTagSize(header);
    vector<char> tag;
    tag.resize(tagSize);
    fileStream->seekg(0);
    fileStream->read(tag.data(),tagSize);
    id3Tag = new ID3Tag(&tag);
    unsigned long mp3Size = getFileSize()-tagSize;
    mp3Data.resize(mp3Size);
    fileStream->read(mp3Data.data(),mp3Size);
    int i =1;
}

Mp3File::~Mp3File() {
    delete id3Tag;
    id3Tag = NULL;
}

ID3Tag *Mp3File::getId3Tag() const {
    return id3Tag;
}

const vector<char> &Mp3File::getMp3Data() const {
    return mp3Data;
}

