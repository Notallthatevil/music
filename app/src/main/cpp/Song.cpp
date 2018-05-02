//
// Created by natet on 3/26/2018.
//

#include "Song.h"


Song::Song(string filepath) {
}

Song::Song(string filepath, ID3Tag *id3Tag) {
    setTitle(id3Tag->getSongData().Title);
    setArtist(id3Tag->getSongData().Artist);
    setAlbum(id3Tag->getSongData().Album);
    setYear(id3Tag->getSongData().Year);
    setTrack(id3Tag->getSongData().Track);
    setFilepath(filepath);
}

Song::Song(vector<char> deserialize) {
    setId(int((unsigned char) deserialize[0] << 24 |
              (unsigned char) deserialize[1] << 16 |
              (unsigned char) deserialize[2] << 8 |
              (unsigned char) deserialize[3]));
    int assign = 0;
    for (int offset = 4; offset < deserialize.size();) {
        int length = int((unsigned char) deserialize[offset] << 24 |
                         (unsigned char) deserialize[offset + 1] << 16 |
                         (unsigned char) deserialize[offset + 2] << 8 |
                         (unsigned char) deserialize[offset + 3]);
        string info(deserialize.begin()+offset+4,deserialize.begin()+offset+4+length);
        switch (assign){
            case 0:
                setTitle(info);
                break;
            case 1:
                setArtist(info);
                break;
            case 2:
                setAlbum(info);
                break;
            case 3:
                setTrack(info);
                break;
            case 4:
                setYear(info);
                break;
            case 5:
                setFilepath(info);
                break;
            default:break;
        }
        offset+=4+length;
        assign++;
    }
}

Song::~Song() {

}

void Song::setTitle(const string &Title) {
    Song::Title = Title;
}


void Song::setAlbum(const string &Album) {
    Song::Album = Album;
}

void Song::setArtist(const string &Artist) {
    Song::Artist = Artist;
}

void Song::setYear(const string &year) {
    Song::year = year;
}

void Song::setTrack(const string &track) {
    Song::track = track;
}

const string &Song::getTitle() const {
    return Title;
}

const string &Song::getAlbum() const {
    return Album;
}

const string &Song::getArtist() const {
    return Artist;
}


const string &Song::getYear() const {
    return year;
}

const string &Song::getTrack() const {
    return track;
}

string &Song::getFilepath() {
    return filepath;
}

void Song::setFilepath(const string &filepath) {
    Song::filepath = filepath;
}

Song::Song() {

}

int Song::getId() const {
    return id;
}

void Song::setId(int id) {
    Song::id = id;
}






