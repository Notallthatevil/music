//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_TAG_H
#define TAGGER_TAG_H


#include "../Byte.h"
#include <string>

using namespace std;

class Tag {

protected: //NOTE:Tag data
    string Title;
    string Artist;
    string Album;
    string Track;
    string Year;
    char * Cover;

public:
    Tag();
    virtual ~Tag();
    virtual unsigned char *generateTags()=0;
    virtual void readTags(unsigned char *tagBuffer)=0;


    const string &getTitle() const;

    const string &getArtist() const;

    const string &getAlbum() const;

    const string &getTrack() const;

    const string &getYear() const;

    char *getCover() const;
};


#endif //TAGGER_TAG_H
