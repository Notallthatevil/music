//
// Created by natet on 3/27/2018.
//

#include "File.h"


File::File(string *filepath) {
    this->filepath = filepath;
    fileStream = new ifstream(filepath->c_str(), ios::ate);
    fileSize = (unsigned long) fileStream->tellg();
    fileStream->seekg(position);
}

File::~File() {
    if (fileStream)
        delete fileStream;
    fileStream = NULL;
}

char File::getNext() {
    return (Byte) fileStream->get();
}

string *File::getFilepath() {
    return filepath;
}

unsigned long File::getFileSize() {
    return fileSize;
}

//File::File(Song newSong, Song oldSong) {
//
//}


