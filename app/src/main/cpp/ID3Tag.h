//
// Created by natet on 3/27/2018.
//

#ifndef TAGGER_ID3TAG_H
#define TAGGER_ID3TAG_H

#include <exception>
#include <string>
#include <vector>
#include "Byte.h"
#include <regex>

using namespace std;

struct HeaderFlags {
    Byte version;
    int size;
    bool unsynchronisation;
    bool extendedHeader;
    bool experimental;
    bool footer;
};

struct SongData {
    string Title;
    string Artist;
    string Album;
    string Track;
    string Year;
};

class ID3Tag {
private:
    const string TITLETAG = "TIT2";
    const string ALBUMTAG = "TALB";
    const string ARTISTTAG = "TPE1";
    const string TRACKTAG = "TRCK";
    const string YEARTAG = "TYER";
//    vector<char> buffer;
    HeaderFlags flags;
    SongData songData;
public:
    const SongData &getSongData() const;

private:

    string getUTF16String(char *buffer, int frameSize);

    string getTextFrame(char *buffer,int frameSize);

public:
    ID3Tag(vector<char> buffer);

    ~ID3Tag();

    static int getTagSize(char *tagHeader);

//    vector<char> getBuffer();

    HeaderFlags findFlags(vector<char> tags);

    void getBits(Byte byte, Byte *bits);

};

struct ID3TagException : public exception {

    const char *what() const throw() {
        return "ID3 Tag is not valid. May have errors parsing the file";
    }
};

typedef unsigned char Byte;


#endif //TAGGER_ID3TAG_H
