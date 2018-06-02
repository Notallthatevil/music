//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_FILEV2_H
#define TAGGER_FILEV2_H

#include <string>
#include <fstream>
#include <vector>
#include "../Byte.h"
#include "../Tags/Tag.h"

using namespace std;

struct fileAccessException : public exception {

    const char *what() const throw() {
        return "Unable to access files. i.e. Permission not granted";
    }
};

struct audioFileDeserializationException : public exception {
    const char *what() const throw() {
        return "Audio file was unable to deserialize the byte stream";
    }
};

class AudioFile {
private:
    int sqlID = -1;
    string filePath = "";
    unsigned long fileSize = 0;

protected:
    ifstream *stream;
public:
    AudioFile() {}

    AudioFile(string *filePath);

    AudioFile(vector<unsigned char> deserialize) {};

    virtual ~AudioFile();

    string getFilePath() const;

    void setFilePath(const string &filePath);

    unsigned long getFileSize() const;

    virtual char *getAudio() =0;

    virtual Tag* getTag() = 0;

    int getID() const;

    void setID(int ID);
};


#endif //TAGGER_FILEV2_H
