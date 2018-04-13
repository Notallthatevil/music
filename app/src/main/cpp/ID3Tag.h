//
// Created by natet on 3/27/2018.
//

#ifndef TAGGER_ID3TAG_H
#define TAGGER_ID3TAG_H

#include <exception>
#include <string>
#include <vector>
#include <regex>
#include "File.h"
typedef unsigned char Byte;


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
    HeaderFlags flags;
    SongData songData;


private:

    string getUTF16String(vector<char> *buffer, int offset, size_t frameSize);

    string getTextFrame(vector<char> *buffer, int offset, size_t frameSize);

public:
    ID3Tag(vector<char> *buffer);

    ~ID3Tag();

    void getBits(Byte byte, Byte *bits);

    HeaderFlags findFlags(vector<char> *tags);

    const SongData &getSongData() const;

    static int getTagSize(char *tagHeader);

};

struct ID3TagException : public exception {

    const char *what() const throw() {
        return "ID3 Tag is not valid. May have errors parsing the file";
    }
};



#endif //TAGGER_ID3TAG_H
