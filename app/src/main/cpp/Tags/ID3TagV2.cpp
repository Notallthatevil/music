//
// Created by Nate on 5/21/2018.
//

#include "ID3TagV2.h"

ID3TagV2::ID3TagV2(unsigned char *header) : Tag() {
    if (header[0] == 'I' && header[1] == 'D' && header[2] == '3' && header[3] >= 3) {
        unsigned int byte0 = (Byte) header[6];
        unsigned int byte1 = (Byte) header[7];
        unsigned int byte2 = (Byte) header[8];
        unsigned int byte3 = (Byte) header[9];
        int synchSafeInt = byte0 << 21 | byte1 << 14 | byte2 << 7 | byte3;
        majorVersion = header[3];
        minorVersion = header[4];
        if (header[3] == 4 && ((header[5] & (0b00010000)) != 0)) {
            footer = true;
            headerSize = 20;
        } else {
            footer = false;
            headerSize = 10;
        }
        tagSize = headerSize + synchSafeInt;
        readFlags(header[5]);
    } else {
        throw ID3TagException();
    }
}

void ID3TagV2::readFlags(char flagByte) {
    if (((flagByte & 0x80) >> 7) != 0) {
        unsynchronisation = true;
    }
    if (((flagByte & 0x40) >> 6) != 0) {
        extendedHeader = true;
    }
    if (((flagByte & 0x20) >> 5) != 0) {
        experimental = true;
    }
}

void ID3TagV2::readTags(unsigned char *tagBuffer) {
    if (tagSize == 0) {
        throw ID3TagException();
    }
    int pos = 0;
    int frameSize = 0;
    string frameHeader;
    while (pos < tagSize - headerSize) {
        frameHeader += tagBuffer[pos];
        frameHeader += tagBuffer[pos + 1];
        frameHeader += tagBuffer[pos + 2];
        frameHeader += tagBuffer[pos + 3];
        if (frameHeader.compare(0, frameHeader.npos, "\0\0\0\0", 4) == 0) {
            break;
        }
        frameSize =
                tagBuffer[pos + 4] << 21 | tagBuffer[pos + 5] << 14 |
                tagBuffer[pos + 6] << 7 | tagBuffer[pos + 7];

        if (frameHeader == TITLETAG) {
            Title = getTextFrame(tagBuffer, pos + 10, frameSize);
        } else if (frameHeader == ARTISTTAG) {
            Artist = getTextFrame(tagBuffer, pos + 10, frameSize);
        } else if (frameHeader == ALBUMTAG) {
            Album = getTextFrame(tagBuffer, pos + 10, frameSize);
        } else if (frameHeader == TRACKTAG) {
            Track = getTextFrame(tagBuffer, pos + 10, frameSize);
        } else if (frameHeader == YEARTAG) {
            Year = getTextFrame(tagBuffer, pos + 10, frameSize);
        }

        pos += frameSize + 10;
        frameHeader = "";
    }
}

string ID3TagV2::getTextFrame(unsigned char *buffer, int offset, int frameSize) {
    string frameData = "";
    switch (buffer[offset]) {
        case 0x01:
            //utf-16
            frameData = getUTF16String(buffer, offset, frameSize);
            break;
        case 0x00:
            //iso
        case 0x03:
            //utf-8
        default:
            for (int i = 1; i < frameSize; i++) {
                if (buffer[i + offset] == '\'') {
                    frameData += "''";
                } else {
                    frameData += buffer[i + offset];
                }
            }
            break;
    }
    return frameData;
}

string ID3TagV2::getUTF16String(unsigned char *buffer, int offset, int frameSize) {
    string frameData = "";
    int big = 1;
    int little = 0;
    if (buffer[1 + offset] == 0xff) {
        little = 1;
        big = 0;
    }
    for (int i = 3; i < frameSize; i += 2) {
        //FIXME Fix to work with utf-16 characters
        if (buffer[i + offset + big] == 0x00 && buffer[i + offset + little] == 0x00) {
            break;
        }
        if (buffer[i + offset + big] == '\'') {
            frameData += "''";
        } else {
            if (buffer[i + offset + big] == 0xe9) {
                frameData += 'e';
            } else if (buffer[i + offset + big] == 0xf1) {
                frameData += 'n';
            } else {
                frameData += buffer[i + offset + big];
            }
        }
    }
    return frameData;
}

ID3TagV2::~ID3TagV2() {}

vector<char> ID3TagV2::generateTags() {
    vector<char> tag(0);
    if (Title != "") {
        auto frame = createTextFrame(TITLETAG,Title);
        tag.insert(tag.end(),frame.begin(),frame.end());
    }
    if (Artist != ""){
        auto frame = createTextFrame(ARTISTTAG,Artist);
        tag.insert(tag.end(),frame.begin(),frame.end());
    }
    if (Album != ""){
        auto frame = createTextFrame(ALBUMTAG,Album);
        tag.insert(tag.end(),frame.begin(),frame.end());
    }
    if (Track != ""){
        auto frame = createTextFrame(TRACKTAG,Track);
        tag.insert(tag.end(),frame.begin(),frame.end());
    }
    if (Year != ""){
        auto frame = createTextFrame(YEARTAG,Year);
        tag.insert(tag.end(),frame.begin(),frame.end());
    }
//    if (Cover != nullptr){
//        auto frame = createAPICFrame(COVERTAG,Cover){
//
//        }
    vector<char> header{'I','D','3',0x04,0x00,getFlagByte()};
    auto size = toSynchSafeInt(tag.size());
    header.insert(header.end(),size.begin(),size.end());
    header.insert(header.end(),tag.begin(),tag.end());
    tagSize = header.size();
    return header;
}

vector<char> ID3TagV2::createTextFrame(const string frameID, string data) {
    //Frame Size
    auto synchSafeInt = toSynchSafeInt(data.size()+1);
    //Frame Flags
    auto flags = createFrameFlags();
    //Text incoding
    char textEncoding = 0x04;
    //Frame ID
    vector<char> frame{frameID[0],frameID[1],frameID[2],frameID[3],synchSafeInt[0],synchSafeInt[1],synchSafeInt[2],synchSafeInt[3],flags[0],flags[1],textEncoding};

    //Frame data
    frame.insert(frame.end(),data.begin(),data.begin()+data.size());

    return frame;
}

vector<char> ID3TagV2::createFrameFlags() {
    return vector<char>(2,0);
}

char ID3TagV2::getFlagByte() {
    return 0;
}

//vector<char> ID3TagV2::createAPICFrame(const string frameID, unsigned char *cover) {
//    vector<char> frame()
//}


vector<char> ID3TagV2::toSynchSafeInt(unsigned long dataSize) {
    vector<char> unsynchFrameSize(4);
    for (int i = 0; i < 4; i++) {
        unsynchFrameSize[i] = (unsigned char) ((dataSize >> 21 - (7 * i)) & 0x7F);
    }
    return unsynchFrameSize;
}



int ID3TagV2::getHeaderSize() const {
    return headerSize;
}












