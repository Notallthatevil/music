//
// Created by natet on 3/27/2018.
//

#ifndef TAGGER_MP3FILE_H
#define TAGGER_MP3FILE_H


#include "File.h"
#include "ID3Tag.h"
#include <vector>
#include <android/log.h>

class Mp3File: File {
private:
    ID3Tag *id3Tag;


public:
    Mp3File(const char *filepath);
    ~Mp3File();
    ID3Tag *getId3Tag() const;
};


#endif //TAGGER_MP3FILE_H
