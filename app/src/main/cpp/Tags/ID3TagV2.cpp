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
                tagBuffer[pos + 4] << 24 | tagBuffer[pos + 5] << 16 |
                tagBuffer[pos + 6] << 8 | tagBuffer[pos + 7];

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
        } else if (frameHeader == COVERTAG) {
            int apicFrameOffset = pos + 10;
            int frameSizeOffset = frameSize;
            unsigned char encoding = tagBuffer[apicFrameOffset++];
            frameSizeOffset--;
            string mimeType = "";
            while (tagBuffer[apicFrameOffset] != 0x00) {
                mimeType += tagBuffer[apicFrameOffset++];
                frameSizeOffset--;
            }
            char pictureType = tagBuffer[++apicFrameOffset];
            frameSizeOffset--;
            apicFrameOffset++;
            frameSizeOffset--;
            if (encoding == 0x01 /*UTF-16*/) {
                while (tagBuffer[apicFrameOffset] != 0x00 ||
                       tagBuffer[apicFrameOffset + 1] != 0x00) {
                    apicFrameOffset += 2;
                    frameSizeOffset -= 2;
                }
                apicFrameOffset += 2;
                frameSizeOffset -= 2;
            } else {
                while (tagBuffer[apicFrameOffset] != 0x00) {
                    apicFrameOffset++;
                    frameSizeOffset--;
                }
                apicFrameOffset++;
                frameSizeOffset--;
            }
            Cover = new unsigned char[frameSizeOffset];
            coverSize = frameSizeOffset;
            copy(tagBuffer + apicFrameOffset, tagBuffer + apicFrameOffset + frameSizeOffset, Cover);
        }

        pos += frameSize + 10;
        frameHeader = "";
    }
}

string ID3TagV2::getTextFrame(unsigned char *buffer, int offset, int frameSize) {
    string frameData = "";
    switch (buffer[offset]) {
        case 0x01:
            //UTF-16
            frameData = getUTF16String(buffer, offset, frameSize);
            break;
        case 0x00:
            //ISO-8859-1
        case 0x03:
            //UTF-8
        default:
            for (int i = 1; i < frameSize; i++) {
//                if (buffer[i + offset] == '\'') {
//                    frameData += "''";
//                } else {
                    frameData += buffer[i + offset];
//                }
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




//TAG GENERATION


vector<char> ID3TagV2::generateTags() {
    vector<char> tag(0);
    if (Title != "") {
        auto frame = createTextFrame(TITLETAG, Title);
        tag.insert(tag.end(), frame.begin(), frame.end());
    }
    if (Artist != "") {
        auto frame = createTextFrame(ARTISTTAG, Artist);
        tag.insert(tag.end(), frame.begin(), frame.end());
    }
    if (Album != "") {
        auto frame = createTextFrame(ALBUMTAG, Album);
        tag.insert(tag.end(), frame.begin(), frame.end());
    }
    if (Track != "") {
        auto frame = createTextFrame(TRACKTAG, Track);
        tag.insert(tag.end(), frame.begin(), frame.end());
    }
    if (Year != "") {
        auto frame = createTextFrame(YEARTAG, Year);
        tag.insert(tag.end(), frame.begin(), frame.end());
    }
    if (Cover != nullptr) {
        auto frame = createAPICFrame(COVERTAG, Cover, coverSize);
        tag.insert(tag.end(), frame.begin(), frame.end());
    }

    vector<char> header{'I', 'D', '3', UTF_8, 0x00, getFlagByte()};
    auto size = calculateFrameSize(tag.size(), true);
    header.insert(header.end(), size.begin(), size.end());
    header.insert(header.end(), tag.begin(), tag.end());
    tagSize = header.size();
    return header;
}

vector<char> ID3TagV2::createTextFrame(const string frameID, string data) {
    //Frame Size
    auto frameSize = calculateFrameSize((data.size() + 1), false);
    //Frame Flags
    auto flags = createFrameFlags();
    //Text incoding
    char textEncoding = UTF_8;
    //Frame ID
    vector<char> frame{frameID[0], frameID[1], frameID[2], frameID[3], frameSize[0],
                       frameSize[1], frameSize[2], frameSize[3], flags[0], flags[1],
                       textEncoding};

    //Frame data
    frame.insert(frame.end(), data.begin(), data.begin() + data.size());

    return frame;
}

vector<char> ID3TagV2::createFrameFlags() {
    return vector<char>(2, 0);
}

char ID3TagV2::getFlagByte() {
    return 0;
}

vector<char> ID3TagV2::createAPICFrame(const string frameID, unsigned char *cover, long length) {
    vector<char> frameData{0x03, 'i', 'm', 'a', 'g', 'e', '/', 'j', 'p', 'e', 'g', 0x00, 0x03, 'C',
                           'o', 'v', 'e', 'r', 0x00};
    frameData.insert(frameData.end(), cover, cover + length);

    auto frameSize = calculateFrameSize((unsigned int) frameData.size(), false);
    int size = frameSize[0] << 24 | frameSize[1] << 16 | frameSize[2] << 8 | frameSize[3];
    auto flags = createFrameFlags();

    vector<char> frame{frameID[0], frameID[1], frameID[2], frameID[3], frameSize[0],
                       frameSize[1], frameSize[2], frameSize[3], flags[0], flags[1]};

    frame.insert(frame.end(), frameData.begin(), frameData.end());
    return frame;
}

//"image/jpeg"

vector<char> ID3TagV2::calculateFrameSize(int dataSize, bool synchSafe) {
    vector<char> frameSize(4);
    if (synchSafe) {
        for (int i = 0; i < 4; i++) {
            frameSize[i] = (unsigned char) (((dataSize >> 21) - (7 * i)) & 0x7F);
        }
    } else {
        for (int i = 0; i < 4; i++)
            frameSize[3 - i] = (unsigned char) (dataSize >> (i * 8));
    }
    return frameSize;

}


int ID3TagV2::getHeaderSize() const {
    return headerSize;
}



