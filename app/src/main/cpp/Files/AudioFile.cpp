//
// Created by Nate on 5/21/2018.
//

#include "AudioFile.h"

AudioFile::AudioFile(string *filePath) {
    this->mFilePath = *filePath;
   /* mStream = new ifstream(filePath->c_str(), ios::ate);
    mFileSize = (unsigned long) mStream->tellg();
    mStream->seekg(0, ios::beg);*/
}

bool AudioFile::open() {
	mStream = new ifstream(getFilePath().c_str(), ios::ate);
	bool isOpen = mStream->is_open();
	if (!isOpen) {
		return false;
	}
	mFileSize = (unsigned long)mStream->tellg();
	mStream->seekg(0,ios::beg);
	return true;
}

AudioFile::~AudioFile() {
    if (mStream != nullptr) {
        delete mStream;
        mStream = nullptr;
    }
}

string AudioFile::getFilePath() const {
    return mFilePath;
}

unsigned long AudioFile::getFileSize() const {
    return mFileSize;
}

int AudioFile::getID() const {
    return mSqlID;
}

void AudioFile::setID(int ID) {
    AudioFile::mSqlID = ID;
}

void AudioFile::setFilePath(const string &filePath) {
    AudioFile::mFilePath = filePath;
}

vector<char> AudioFile::getAudio() {
    return mAudioData;
}

void AudioFile::setAudio() {
    if(mAudioData.size()<1){
        throw invalidAudioDataSize();
    }
    mStream->read(&mAudioData[0], mAudioData.size());
}

