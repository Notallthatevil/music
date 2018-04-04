//
// Created by natet on 3/27/2018.
//

#include "File.h"


File::File(const char *filepath) {
    this->filepath = filepath;
    fileStream = new ifstream(filepath, ios::ate);
    size = (unsigned long) fileStream->tellg();
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

const char *File::getFilepath() {
    return filepath;
}

unsigned long File::getSize() {
    return size;
}



