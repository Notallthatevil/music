//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_FILEV2_H
#define TAGGER_FILEV2_H

#include <string>
#include <fstream>
#include <vector>
#include "../Tags/Tag.h"

using namespace std;

struct fileAccessException : public exception {

    const char *what() const throw() {
        return "Unable to access files. i.e. Permission not granted";
    }
};

struct invalidAudioDataSize : public exception {

    const char *what() const throw() {
        return "mAudioData.size() < 1; unable to copy bytes";
    }
};

class AudioFile {
private:
    int mSqlID = -1;
    string mFilePath = "";
    unsigned long mFileSize = 0;

protected:
    ifstream *mStream = nullptr;
    vector<char> mAudioData;
	virtual void setAudio();

public:
    AudioFile() {}

    AudioFile(string *filePath);

	bool open();


    AudioFile(vector<char> deserialize) {};

    virtual ~AudioFile();

    string getFilePath() const;

    void setFilePath(const string &filePath);

    unsigned long getFileSize() const;

    virtual vector<char> getAudio();

    /*
     * Before calling setAudio this method must be overridden and mStream.n_pos
     * must be set to the start of the audio data and mAudioData size must be set prior
     */
    
    virtual Tag* getTag() = 0;

    int getID() const;

    void setID(int ID);
};


#endif //TAGGER_FILEV2_H
