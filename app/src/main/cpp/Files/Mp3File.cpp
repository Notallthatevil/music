//
// Created by natet on 3/27/2018.
//

#include "Mp3File.h"


Mp3File::Mp3File(string *filepath) : File(filepath) {
    char header[10];
    fileStream->read(header, 10);
    //TODO Add try catch for this statement
    unsigned long tagSize = (unsigned long) ID3Tag::getTagSize(header);
    vector<char> tag;
    tag.resize(tagSize);
    fileStream->seekg(0);
    fileStream->read(tag.data(), tagSize);
    id3Tag = new ID3Tag(&tag);

    // FIXME causes slow execution
//    auto start = std::chrono::high_resolution_clock::now();
////Method to time
    unsigned long mp3Size = getFileSize() - tagSize;
    mp3Data = new Byte[mp3Size];
    fileStream->read((char *) mp3Data, mp3Size);
//    auto end = std::chrono::high_resolution_clock::now();
//    std::chrono::duration<double> time_span = (end - start);
//    __android_log_print(ANDROID_LOG_DEBUG, "TIME", "Time Retrieving file paths = %f",
//                        time_span.count());

}

Mp3File::~Mp3File() {
    delete id3Tag;
    id3Tag = NULL;
    delete [] mp3Data;
    mp3Data = NULL;
}

ID3Tag *Mp3File::getId3Tag() const {
    return id3Tag;
}

//const vector<char> &Mp3File::getMp3Data() const {
//    return NULL;
//}

