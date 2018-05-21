//
// Created by Nate on 5/21/2018.
//

#include "AudioFile.h"

AudioFile::AudioFile(string *filePath) {
    this->filePath = *filePath;
    stream = new ifstream(filePath->c_str(), ios::ate);
    fileSize = stream->tellg();
    stream->seekg(0,ios::beg);
}

AudioFile::~AudioFile() {
    delete stream;
    stream = NULL;
}

string AudioFile::getFilePath() const {
    return filePath;
}

unsigned long AudioFile::getFileSize() const {
    return fileSize;
}

int AudioFile::getID() const {
    return sqlID;
}


