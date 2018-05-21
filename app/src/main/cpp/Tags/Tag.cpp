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

char *Tag::getCover() const {
    return Cover;
}



