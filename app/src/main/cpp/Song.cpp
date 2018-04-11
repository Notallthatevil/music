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

 string &Song::getFilepath()  {
    return filepath;
}

void Song::setFilepath(const string &filepath) {
    Song::filepath = filepath;
}

Song::Song() {

}


