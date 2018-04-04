

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
                (Byte) (*buf)[pos + 4] << 24 | (Byte) (*buf)[pos + 5] << 16 |
                (Byte) (*buf)[pos + 6] << 8 | (Byte) (*buf)[pos + 7];

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

ID3Tag::~ID3Tag() {

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
                frameData += (*buffer)[i + offset];
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
        //TODO Fix output if two byte character
        if ((*buffer)[i + offset + big] == 0x00 && (*buffer)[i + offset + little] == 0x00) {
            break;
        }
        frameData += (*buffer)[i + offset + big];
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
        bits[7 - i] = (byte & (1 << i) != 0);
    }
}

const SongData &ID3Tag::getSongData() const {
    return songData;
}