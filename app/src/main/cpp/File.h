//
// Created by natet on 3/27/2018.
//


#ifndef TAGGER_FILE_H
#define TAGGER_FILE_H

#include <fstream>
#include <string>
#include "Byte.h"
using namespace std;

struct fileAccessException : public exception {

    const char *what() const throw() {
        return "Unable to access files. i.e. Permission not granted";
    }
};



class File {
private:
    const char *filepath;
    unsigned long position = 0;
    unsigned long size = 0;


protected:
    ifstream *fileStream;
public:
    File(const char *filepath);

    virtual ~File();

    char getNext();

    const char *getFilepath();

    unsigned long getSize();


};


#endif //TAGGER_FILE_H
