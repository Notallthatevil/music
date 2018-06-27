//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_TAG_H
#define TAGGER_TAG_H


#include "../Byte.h"
#include <string>
#include <vector>

using namespace std;

class Tag {

protected: //NOTE:Tag data
    string Title;
    string Artist;
    string Album;
    string Track;
    string Year;
    unsigned char *Cover = nullptr;
    long coverSize = 0;
    long tagSize = 0;


public:
    Tag();
    virtual ~Tag();
    virtual vector<char> generateTags()=0;
    virtual void readTags(unsigned char *tagBuffer)=0;


    const string &getTitle() const;

    const string &getArtist() const;

    const string &getAlbum() const;

    const string &getTrack() const;

    const string &getYear() const;

    unsigned char *getCover() const;

    void setTitle(const string &Title);

    void setArtist(const string &Artist);

    void setAlbum(const string &Album);

    void setTrack(const string &Track);

    void setYear(const string &Year);

    void setCover(unsigned char *Cover);

    long getCoverSize() const;

    long getTagSize() const;

};


#endif //TAGGER_TAG_H
