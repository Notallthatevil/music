//
// Created by Nate on 5/21/2018.
//

#include "AudioFile.h"

AudioFile::AudioFile(string *filePath) {
    this->filePath = *filePath;
    stream = new ifstream(filePath->c_str(), ios::ate);
    fileSize = (unsigned long) stream->tellg();
    stream->seekg(0, ios::beg);
}

AudioFile::~AudioFile() {
    delete stream;
    stream = nullptr;
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

void AudioFile::setID(int ID) {
    AudioFile::sqlID = ID;
}

void AudioFile::setFilePath(const string &filePath) {
    AudioFile::filePath = filePath;
}

