//
// Created by natet on 3/26/2018.
//

#ifndef TAGGER_SONG_H
#define TAGGER_SONG_H

#include <string>
#include "ID3Tag.h"

using namespace std;

class Song {
private:
    int id;
    string Title;
    string Album;
    string Artist;
    string filepath;
    string track;
    string year;
public:
    Song(string filepath, ID3Tag *id3Tag);

    Song(string filepath);

    Song();

    Song(vector<char> deserialize);

    ~Song();

    vector<char> generateTag();

    string &getFilepath();

    void setFilepath(const string &filepath);

    void setTitle(const string &Title);

    void setAlbum(const string &Album);

    void setArtist(const string &Artist);

    void setYear(const string &year);

    void setTrack(const string &track);

    const string &getTitle() const;

    const string &getAlbum() const;

    const string &getArtist() const;

    const string &getYear() const;

    const string &getTrack() const;

    int getId() const;

    void setId(int id);

};

#endif //TAGGER_SONG_H
