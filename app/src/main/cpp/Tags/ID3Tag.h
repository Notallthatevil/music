//
// Created by natet on 3/27/2018.
//

#ifndef TAGGER_ID3TAG_H
#define TAGGER_ID3TAG_H

#include <exception>
#include <string>
#include <vector>
#include <regex>
#include "../Byte.h"
#include "Tag.h"

using namespace std;
#define GET_VARIABLE_NAME(Variable) (#Variable);

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
    //CONSTANTS
    const char *HEADER = "ID3";
    const char MAJOR_VERSION = 0x04;
    const char MINOR_VERSION = 0x00;
    const string TITLETAG = "TIT2";
    const string ALBUMTAG = "TALB";
    const string ARTISTTAG = "TPE1";
    const string TRACKTAG = "TRCK";
    const string YEARTAG = "TYER";

    HeaderFlags flags;
    SongData songData;

    string getUTF16String(vector<char> *buffer, int offset, size_t frameSize);

    string getTextFrame(vector<char> *buffer, int offset, size_t frameSize);

    void getBits(Byte byte, Byte *bits);

    HeaderFlags findFlags(vector<char> *tags);

    vector<char> calculateFrameSize(int dataSize);

    vector<char> concatenateVectors(vector<char> vector1, vector<char> vector2);

    vector<char> createFrameFlags();

    vector<char>
    createTextFrame(const string frameHeader, vector<char> synchSafeLength, vector<char> frameData);

    vector<char> createTextFrame(const string frameHeader, string frameData);

    vector<char> getTextFrameData(string frameData);

public:
    ID3Tag(vector<char> *buffer);

    ID3Tag() {}

    ~ID3Tag();

    vector<char> generateTags(SongData data);

    const SongData &getSongData() const;

    static int getTagSize(char *tagHeader);




};




#endif //TAGGER_ID3TAG_H
