

#include "ID3Tag.h"


ID3Tag::ID3Tag(vector<char> *buf) {
    findFlags(buf);
    int pos = 10;
    size_t frameSize = 0;
    string frameHeader;
    songData = SongData();
    while (pos < buf->size()) {
        frameHeader += (*buf)[pos];
        frameHeader += (*buf)[pos + 1];
        frameHeader += (*buf)[pos + 2];
        frameHeader += (*buf)[pos + 3];
        if (frameHeader.compare(0, frameHeader.npos, "\0\0\0\0", 4) == 0) {
            break;
        }
        frameSize =
                (Byte) (*buf)[pos + 4] << 21 | (Byte) (*buf)[pos + 5] << 14 |
                (Byte) (*buf)[pos + 6] << 7 | (Byte) (*buf)[pos + 7];

        if (frameHeader == TITLETAG) {
            songData.Title = getTextFrame(buf, pos + 10, frameSize);
        } else if (frameHeader == ARTISTTAG) {
            songData.Artist = getTextFrame(buf, pos + 10, frameSize);
        } else if (frameHeader == ALBUMTAG) {
            songData.Album = getTextFrame(buf, pos + 10, frameSize);
        } else if (frameHeader == TRACKTAG) {
            songData.Track = getTextFrame(buf, pos + 10, frameSize);
        } else if (frameHeader == YEARTAG) {
            songData.Year = getTextFrame(buf, pos + 10, frameSize);
        }

        pos += frameSize + 10;
        frameHeader = "";
    }
}


vector<char> ID3Tag::generateTags(SongData data) {
    vector<char> tag;
    if (data.Title != "") {
        tag = concatenateVectors(tag, createTextFrame(TITLETAG, data.Title));
    }
    if (data.Artist != "") {
        tag = concatenateVectors(tag, createTextFrame(ARTISTTAG, data.Artist));
    }
    if (data.Album != "") {
        tag = concatenateVectors(tag, createTextFrame(ALBUMTAG, data.Album));
    }
    if (data.Track != "") {
        tag = concatenateVectors(tag, createTextFrame(TRACKTAG, data.Track));
    }
    if (data.Year != "") {
        tag = concatenateVectors(tag, createTextFrame(YEARTAG, data.Year));
    }
    vector<char> header(6);
    header[0] = 'I';
    header[1] = 'D';
    header[2] = '3';
    header[3] = MAJOR_VERSION;
    header[4] = MINOR_VERSION;
    //TODO add the ability to use ID3 flags
    header[5] = 0x00;
    header = concatenateVectors(header,calculateFrameSize((int)tag.size()));
    tag = concatenateVectors(header,tag);
    return tag;
}

vector<char> ID3Tag::calculateFrameSize(int dataSize) {
    vector<char> unsynchFrameSize(4);
    for (int i = 0; i < 4; i++) {
        unsynchFrameSize[i] = (unsigned char) ((dataSize >> 21 - (7 * i)) & 0x7F);
    }
    return unsynchFrameSize;
}


//Returns array of bytes representing the data in the frame
vector<char> ID3Tag::getTextFrameData(string frameData) {
    const char *cData = frameData.c_str();
    vector<char> newFrameData(frameData.size()+1);

    //UTF-8 encoding
    newFrameData[0] = 0x04;

    for (int i = 0; i < frameData.length() + 1; i++) {
        newFrameData[i + 1] = cData[i];
    }
    return newFrameData;
}

vector<char> ID3Tag::createTextFrame(const string frameHeader, string frameData) {
    vector<char> tag;
    tag = concatenateVectors(tag, getTextFrameData(frameData));
    tag = concatenateVectors(createFrameFlags(), tag);
    tag = concatenateVectors(calculateFrameSize((int) frameData.size()+1), tag);
    vector<char> header(4);
    for (int i = 0; i < 4; i++) {
        header[i] = frameHeader.c_str()[i];
    }
    tag = concatenateVectors(header, tag);
    return tag;
}

vector<char> ID3Tag::createFrameFlags() {
    vector<char> flags(2);
    flags[0] = 0x00;
    flags[1] = 0x00;
    return flags;
}

vector<char> ID3Tag::concatenateVectors(vector<char> vector1, vector<char> vector2) {
    vector<char> newVector;
    newVector.reserve(vector1.size() + vector2.size());
    newVector.insert(newVector.end(), vector1.begin(), vector1.end());
    newVector.insert(newVector.end(), vector2.begin(), vector2.end());
    return newVector;
}


string ID3Tag::getTextFrame(vector<char> *buffer, int offset, size_t frameSize) {
    string frameData = "";
    switch ((*buffer)[offset]) {
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
                if ((*buffer)[i + offset] == '\'') {
                    frameData += "''";
                } else {
                    frameData += (*buffer)[i + offset];
                }
            }
            break;
    }
    return frameData;
}

string ID3Tag::getUTF16String(vector<char> *buffer, int offset, size_t frameSize) {
    string frameData = "";
    int big = 1;
    int little = 0;
    if ((*buffer)[1 + offset] == 0xff) {
        little = 1;
        big = 0;
    }
    for (int i = 3; i < frameSize; i += 2) {
        //TODO Fix to work with utf-16 characters
        if ((*buffer)[i + offset + big] == 0x00 && (*buffer)[i + offset + little] == 0x00) {
            break;
        }
        if ((*buffer)[i + offset + big] == '\'') {
            frameData += "''";
        } else {
            if ((*buffer)[i + offset + big] == 0xe9) {
                frameData += 'e';
            } else if ((*buffer)[i + offset + big] == 0xf1) {
                frameData += 'n';
            } else {
                frameData += (*buffer)[i + offset + big];
            }
        }

    }
    return frameData;
}

int ID3Tag::getTagSize(char *tagHeader) {
    if (tagHeader[0] == 'I' && tagHeader[1] == 'D' && tagHeader[2] == '3' && tagHeader[3] >= 3) {
        unsigned int byte0 = (Byte) tagHeader[6];
        unsigned int byte1 = (Byte) tagHeader[7];
        unsigned int byte2 = (Byte) tagHeader[8];
        unsigned int byte3 = (Byte) tagHeader[9];
        int synchSafeInt = byte0 << 21 | byte1 << 14 | byte2 << 7 | byte3;
        if (tagHeader[3] == 4 && ((tagHeader[5] & (0b00010000)) != 0)) {
            return synchSafeInt + 20;
        } else {
            return synchSafeInt + 10;
        }
    } else {
        throw ID3TagException();
    }
}

HeaderFlags ID3Tag::findFlags(vector<char> *tags) {
    HeaderFlags flags = HeaderFlags();
    Byte flagBits[8];
    flags.size = (int) (tags->size() - 10);
    getBits((Byte) (*tags)[5], flagBits);
    if ((*tags)[4] == 3) {
        flags.version = 3;
    } else if ((*tags)[4] == 4) {
        flags.version = 4;
    }
    if (flagBits[0] == 1) {
        flags.unsynchronisation = true;
    }
    if (flagBits[1] == 1) {
        flags.extendedHeader = true;
    }
    if (flagBits[2] == 1) {
        flags.experimental = true;
    }
    if (flagBits[3] == 1) {
        flags.footer = true;
        flags.size -= 10;
    }
    return flags;

}

void ID3Tag::getBits(Byte byte, Byte bits[]) {
    for (Byte i = 0; i < 8; i++) {
        bits[7 - i] = ((byte & (1 << i)) != 0);
    }
}

const SongData &ID3Tag::getSongData() const {
    return songData;
}

ID3Tag::~ID3Tag() {

}












