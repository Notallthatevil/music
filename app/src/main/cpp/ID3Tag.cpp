

#include "ID3Tag.h"


ID3Tag::ID3Tag(vector<char> buf) {
    findFlags(buf);
    int pos = 10;
    int frameSize = 0;
    string frameHeader;
    songData = SongData();
    while (pos < buf.size()) {
        frameHeader += buf[pos];
        frameHeader += buf[pos + 1];
        frameHeader += buf[pos + 2];
        frameHeader += buf[pos + 3];
        if (frameHeader.compare(0, frameHeader.npos, "\0\0\0\0", 4) == 0) {
            break;
        }
        frameSize =
                (Byte) buf[pos + 4] << 24 | (Byte) buf[pos + 5] << 16 | (Byte) buf[pos + 6] << 8 |
                (Byte) buf[pos + 7];
//        char *frame;
//        copy(buf.begin() + pos + 10, buf.begin() + pos + 10 + frameSize, frame);
        if (frameHeader == TITLETAG) {
            int big = 1;
            int little = 0;
            switch (buf[pos + 10]) {
                case 0x01:
                    //utf-16
//                    songData.Title = getUTF16String(buf, frameSize);

                    if (buf[pos + 11] == 0xff) {
                        little = 1;
                        big = 0;
                    }
                    for (int i = 3; i < frameSize; i += 2) {
                        //TODO Fix output if two byte character
//            if (buffer[i + 1] == 0x00) {
                        if (buf[i + 10 + big] == 0x00 && buf[10 + i + little] == 0x00) {
                            break;
                        }
                        songData.Title += buf[i + 10 + big];
//            } else {
//                char16_t bigChar = (char16_t) (buffer[i + 1] << 8 | buffer[i]);
//            }
                    }
                    break;
                case 0x00:
                    //iso
                case 0x03:
                    //utf-8
                default:
                    for (int i = 1; i < frameSize; i++) {
                        songData.Title += buf[i + pos + 10];
                    }
                    break;
            }
//            songData.Title = getTextFrame(frame, frameSize);
        } else if (frameHeader == ALBUMTAG) {
//            songData.Album = getTextFrame(frame, frameSize);
        } else if (frameHeader == ARTISTTAG) {
//            songData.Artist = getTextFrame(frame, frameSize);
        } else if (frameHeader == TRACKTAG) {
//            songData.Track = getTextFrame(frame, frameSize);
        } else if (frameHeader == YEARTAG) {
//            songData.Year = getTextFrame(frame, frameSize);
        }
        pos += frameSize + 10;
        frameHeader = "";
//        delete frame;
    }
}

ID3Tag::~ID3Tag() {

}

string ID3Tag::getTextFrame(char *buffer, int frameSize) {
    string frameData = "";
    switch (buffer[0]) {
        case 0x01:
            //utf-16
            frameData = getUTF16String(buffer, frameSize);
            break;
        case 0x00:
            //iso
        case 0x03:
            //utf-8
        default:
            for (int i = 1; i < frameSize; i++) {
                frameData += buffer[i];
            }
            break;
    }
    return frameData;
}

string ID3Tag::getUTF16String(char *buffer, int frameSize) {
    string frameData = "";
    int big = 1;
    int little = 0;
    if (buffer[1] == 0xff) {
        little = 1;
        big = 0;
    }
    for (int i = 3; i < frameSize; i += 2) {
        //TODO Fix output if two byte character
//            if (buffer[i + 1] == 0x00) {
        if (buffer[i + big] == 0x00 && buffer[i + little] == 0x00) {
            break;
        }
        frameData += buffer[i + big];
//            } else {
//                char16_t bigChar = (char16_t) (buffer[i + 1] << 8 | buffer[i]);
//            }
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


HeaderFlags ID3Tag::findFlags(vector<char> tags) {
    HeaderFlags flags = HeaderFlags();
    Byte flagBits[8];
    flags.size = (int) (tags.size() - 10);
    getBits((Byte) tags[5], flagBits);
    if (tags[4] == 3) {
        flags.version = 3;
    } else if (tags[4] == 4) {
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