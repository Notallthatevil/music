//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_ID3TAGV2_H
#define TAGGER_ID3TAGV2_H


#include "Tag.h"
#include <string>
#include <vector>

using namespace std;

class ID3TagV2: public Tag {
public:     //NOTE: CONSTANTS
    static const int HEADER_SIZE = 10;
    const string TITLETAG = "TIT2";
    const string ALBUMTAG = "TALB";
    const string ARTISTTAG = "TPE1";
    const string TRACKTAG = "TRCK";
    const string YEARTAG = "TYER";
    const string COVERTAG = "APIC";
    const unsigned char MAJOR_VERSION = 0x04;
    const unsigned char MINOR_VERSION = 0x00;


private: //NOTE:Header flags
    char majorVersion;
    char minorVersion;
    bool unsynchronisation;
    bool extendedHeader;
    bool experimental;
    bool footer;

private:
    long tagSize = 0;
    int headerSize = 0;


    //NOTE:Methods
private:
    void readFlags(char flagByte);

    string getTextFrame(unsigned char *buffer, int offset, int frameSize);

    string getUTF16String(unsigned char *buffer, int offset, int frameSize);

public:
    ID3TagV2() : Tag() {}

    ID3TagV2(unsigned char *header);

    ~ID3TagV2();

    unsigned char *generateTags();

    void readTags(unsigned char *tagBuffer);

    long getTagSize() const;

    int getHeaderSize() const;

};

struct ID3TagException : public exception {

    const char *what() const throw() {
        return "ID3 Tag is not valid. May have errors parsing the file";
    }
};


#endif //TAGGER_ID3TAGV2_H
