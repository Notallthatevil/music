//
// Created by Nate on 5/21/2018.
//

#include "Tag.h"

Tag::Tag() {

}

Tag::~Tag() {

}

const string &Tag::getTitle() const {
    return Title;
}

const string &Tag::getArtist() const {
    return Artist;
}

const string &Tag::getAlbum() const {
    return Album;
}

const string &Tag::getTrack() const {
    return Track;
}

const string &Tag::getYear() const {
    return Year;
}

unsigned char *Tag::getCover() const {
    return Cover;
}

void Tag::setTitle(const string &Title) {
    Tag::Title = Title;
}

void Tag::setArtist(const string &Artist) {
    Tag::Artist = Artist;
}

void Tag::setAlbum(const string &Album) {
    Tag::Album = Album;
}

void Tag::setTrack(const string &Track) {
    Tag::Track = Track;
}

void Tag::setYear(const string &Year) {
    Tag::Year = Year;
}

void Tag::setCover(unsigned char *Cover) {
    if (Cover[0] == 'N' && Cover[1] == 'U' && Cover[2] == 'L' && Cover[3] == 'L') {
        Tag::Cover = nullptr;
    } else {
        Tag::Cover = Cover;
    }
}



