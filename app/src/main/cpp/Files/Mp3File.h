//
// Created by natet on 3/27/2018.
//

#ifndef TAGGER_MP3FILE_H
#define TAGGER_MP3FILE_H


#include "File.h"
#include "../Tags/ID3Tag.h"
#include <vector>
#include <android/log.h>

class Mp3File : File {
private:
    ID3Tag *id3Tag;
    Byte* mp3Data;

public:
    Mp3File(string *filepath);

    ~Mp3File();

    ID3Tag *getId3Tag() const;
//    const vector<char> &getMp3Data() const;

};


#endif //TAGGER_MP3FILE_H
