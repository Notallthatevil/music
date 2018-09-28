//
// Created by Nate on 5/21/2018.
//

#include "Tag.h"

Tag::Tag() {

}

Tag::~Tag() {
    if (Cover != nullptr){
        delete [] Cover;
        Cover = nullptr;
    }
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
    Tag::Cover = Cover;
}

int Tag::getCoverSize() const {
    return coverSize;
}

int Tag::getTagSize() const {
    return tagSize;
}

void Tag::setCoverSize(int lengthInBytes) {
    coverSize = lengthInBytes;
}




