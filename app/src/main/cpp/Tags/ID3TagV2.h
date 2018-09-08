//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_ID3TAGV2_H
#define TAGGER_ID3TAGV2_H


#include "Tag.h"
#include <string>

using namespace std;

class ID3TagV2 : public Tag {
public: //NOTE: CONSTANTS
    static const int HEADER_SIZE = 10;
    const string TITLETAG = "TIT2";
    const string ALBUMTAG = "TALB";
    const string ARTISTTAG = "TPE1";
    const string TRACKTAG = "TRCK";
    const string YEARTAG = "TYER";
    const string COVERTAG = "APIC";
    const unsigned char MAJOR_VERSION = 0x04;
    const unsigned char MINOR_VERSION = 0x00;
    const char ISO_8859_1 = 0x00;
    const char UTF_16 = 0x01;
    const char UTF_16BE = 0x02;
    const char UTF_8 = 0x03;


private: //NOTE:Header flags
    char majorVersion;
    char minorVersion;
    bool unsynchronisation;
    bool extendedHeader;
    bool experimental;
    bool footer;

private:
    int headerSize = 0;


private: //NOTE:Methods
    void readFlags(char flagByte);

    string getTextFrame(unsigned char *buffer, int offset, int frameSize);

    string getUTF16String(unsigned char *buffer, int offset, int frameSize);

//    unsigned char* getAPICFrame(unsigned char *buffer,int offset, int frameSize);

    vector<char> createTextFrame(const string frameID, string data);

    vector<char> toSynchSafeInt(unsigned long dataSize);

    vector<char> createFrameFlags();

//    vector<char> createAPICFrame(const string frameID, unsigned char *cover);
    char getFlagByte();


public:
    ID3TagV2() : Tag() {}

    ID3TagV2(unsigned char *header);

    ~ID3TagV2();

    vector<char> generateTags();

    void readTags(unsigned char *tagBuffer);

    int getHeaderSize() const;

};

struct ID3TagException : public exception {

    const char *what() const throw() {
        return "ID3 Tag is not valid. May have errors parsing the file";
    }
};


#endif //TAGGER_ID3TAGV2_H
