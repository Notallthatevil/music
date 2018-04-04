//
// Created by natet on 3/27/2018.
//

#include "Mp3File.h"

Mp3File::Mp3File(const char *filepath) : File(filepath) {

    char header[10];
    fileStream->read(header,10);
    //TODO Add try catch for this statement
    int size = ID3Tag::getTagSize(header);
    vector<char> tag;
    tag.resize((unsigned long) size);
    fileStream->seekg(0);
    fileStream->read(&tag[0],size);
    id3Tag = new ID3Tag(tag);
    __android_log_print(ANDROID_LOG_ERROR,"MP3FILE","Tags successfully retrieved %s",filepath);
}

Mp3File::~Mp3File() {
    delete id3Tag;
    id3Tag = NULL;
}

ID3Tag *Mp3File::getId3Tag() const {
    return id3Tag;
}
