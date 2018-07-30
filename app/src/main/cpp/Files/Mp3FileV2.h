//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_MP3FILEV2_H
#define TAGGER_MP3FILEV2_H


#include "AudioFile.h"
#include "../Tags/ID3TagV2.h"

class Mp3FileV2 : public AudioFile {

private:
    ID3TagV2 *id3Tag = nullptr;

public:
    Mp3FileV2(string *filePath, bool findTags);

    Mp3FileV2(vector<char> deserialize);

    Mp3FileV2() {}

    ~Mp3FileV2();

    vector<char> getAudio() override;

    Tag *getTag() override;


};


#endif //TAGGER_MP3FILEV2_H
