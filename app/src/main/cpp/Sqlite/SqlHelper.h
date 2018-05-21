//
// Created by natet on 4/7/2018.
//

#ifndef TAGGER_SQLHELPER_H
#define TAGGER_SQLHELPER_H

#include <string>
#include "sqlite3.h"
#include <android/log.h>
#include <jni.h>
#include <sys/stat.h>
#include <sstream>
#include "../Files/AudioFile.h"

using namespace std;


class SqlHelper {
private:
    const string DATABASE_DIRECTORY = "/data/data/com.trippntechnology.tagger/databases";
    const string DATABASE_NAME = DATABASE_DIRECTORY+"/TNT.db";

    const string SONG_ID_COLUMN = "ID";
    const string SONG_TITLE_COLUMN = "TITLE";
    const string SONG_ARTIST_COLUMN = "ARTIST";
    const string SONG_ALBUM_COLUMN = "ALBUM";
    const string SONG_TRACK_COLUMN = "TRACK";
    const string SONG_YEAR_COLUMN = "YEAR";
    const string SONG_FILEPATH_COLUMN = "FILEPATH";
    const string SONG_ARTWORK_COLUMN = "ARTWORK";

    const int SONG_ID_NUMBER = 0;
    const int SONG_TITLE_NUMBER = 1;
    const int SONG_ARTIST_NUMBER = 2;
    const int SONG_ALBUM_NUMBER = 3;
    const int SONG_TRACK_NUMBER = 4;
    const int SONG_YEAR_NUMBER = 5;
    const int SONG_FILEPATH_NUMBER = 6;
    const int SONG_ARTWORK_NUMBER = 7;
    sqlite3 *db;
    sqlite3_stmt *stmt;

public:
    const string SONG_TABLE = "SONGS";

    SqlHelper();

    ~SqlHelper();

    int dropTable(string tableName);

    int createTable(string tableName);

    int insertSong(AudioFile *audioFile);

    int updateSong(AudioFile *audioFile);

    string selectSong(AudioFile *audioFile);

    void makeSqlFriendly(string str, const char *symbol);

    jobjectArray retrieveAllSongs(JNIEnv *env);

};

struct databaseCreationError : public exception {

    const char *what() const throw() {
        return "Unable to create database ";
    }
};


#endif //TAGGER_SQLHELPER_H
