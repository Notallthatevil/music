//
// Created by natet on 4/7/2018.
//

#include <vector>
#include "SqlHelper.h"

inline bool file_exists(const std::string &name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

SqlHelper::SqlHelper() {
    if (!file_exists(DATABASE_DIRECTORY)) {
        mkdir(DATABASE_DIRECTORY.c_str(), S_IRWXU | S_IRWXG | S_IXOTH);
    }
    int rc = sqlite3_open_v2(DATABASE_NAME.c_str(), &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,
                             NULL);
    if (rc) {
        throw databaseCreationError();
    }
}

SqlHelper::~SqlHelper() {
    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);
}

int SqlHelper::createTable(string tableName) {
    if (tableName == SONG_TABLE) {
        string sql = "CREATE TABLE " + SONG_TABLE + "(" +
                     SONG_ID_COLUMN + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                     SONG_TITLE_COLUMN + " TEXT, " +
                     SONG_ARTIST_COLUMN + " TEXT, " +
                     SONG_ALBUM_COLUMN + " TEXT, " +
                     SONG_TRACK_COLUMN + " TEXT, " +
                     SONG_YEAR_COLUMN + " TEXT, " +
                     SONG_FILEPATH_COLUMN + " TEXT NOT NULL);";
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        int rc = sqlite3_step(stmt);
        return rc;
    }
    return -1;
}

int SqlHelper::dropTable(string tableName) {
    string sql = "DROP TABLE IF EXISTS " + tableName;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    int rc = sqlite3_step(stmt);
    return rc;
}

int SqlHelper::insertSong(AudioFile *audioFile) {
    if (audioFile->getFilePath().find("'") != audioFile->getFilePath().npos) {
        makeSqlFriendly(audioFile->getFilePath(), "'");
    }
    string sql = "INSERT INTO " + SONG_TABLE + "(TITLE,ARTIST,ALBUM,TRACK,YEAR,FILEPATH) VALUES('" +
                 audioFile->getTag()->getTitle() + "', '" +
                 audioFile->getTag()->getArtist() + "', '" +
                 audioFile->getTag()->getAlbum() + "', '" +
                 audioFile->getTag()->getTrack() + "', '" +
                 audioFile->getTag()->getYear() + "', '" +
                 audioFile->getFilePath() + "');";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    int rc = sqlite3_step(stmt);
    return rc;
}


void SqlHelper::makeSqlFriendly(string str, const char *symbol) {
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == symbol[0]) {
            str.insert(i, symbol);
            i++;
        }
    }
}

jobjectArray SqlHelper::retrieveAllSongs(JNIEnv *env) {
    string sql = "SELECT * FROM " + SONG_TABLE;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    jclass jSongClass = env->FindClass("com/trippntechnology/tagger/Song");
    jmethodID jSongConstructor = env->GetMethodID(jSongClass, "<init>",
                                                  "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    vector<jobject> songList;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        jint jID = sqlite3_column_int(stmt, SONG_ID_NUMBER);
        jstring jTitle = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_TITLE_NUMBER));
        jstring jArtist = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_ARTIST_NUMBER));
        jstring jAlbum = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_ALBUM_NUMBER));
        jstring jTrack = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_TRACK_NUMBER));
        jstring jYear = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_YEAR_NUMBER));
        jstring jFilepath = env->NewStringUTF(
                (char *) sqlite3_column_text(stmt, SONG_FILEPATH_NUMBER));
        jobject jSong = env->NewObject(jSongClass, jSongConstructor, jID, jTitle, jArtist, jAlbum,
                                       jTrack, jYear, jFilepath);
        songList.push_back(jSong);
    }
    jobjectArray jSongList = env->NewObjectArray((jint) songList.size(), jSongClass, NULL);
    for (int i = 0; i < songList.size(); i++) {
        env->SetObjectArrayElement(jSongList, i, songList[i]);
    }
    return jSongList;
}

//RETURNS the filepath of the song with the corresponding ID

string SqlHelper::selectSong(AudioFile *audioFile) {
    stringstream ss;
    ss << audioFile->getID();
    string sql =
            "SELECT " + SONG_FILEPATH_COLUMN + " FROM " + SONG_TABLE + " WHERE " + SONG_ID_COLUMN +
            " = " + ss.str();
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
    return string((char *) sqlite3_column_text(stmt, 0));
}

int SqlHelper::updateSong(AudioFile *audioFile) {
    stringstream ss;
    ss << audioFile->getID();
    //TODO make sure strings are sql friendly i.e. no ' symbols without doubling them up first
    string sql =
            "UPDATE " + SONG_TABLE + " SET " + SONG_TITLE_COLUMN + " = '" +
            audioFile->getTag()->getTitle() + "', " +
            SONG_ARTIST_COLUMN + " = '" + audioFile->getTag()->getArtist() + "', " +
            SONG_ALBUM_COLUMN + " = '" + audioFile->getTag()->getAlbum() + "', " +
            SONG_TRACK_COLUMN + " = '" + audioFile->getTag()->getTrack() + "', " +
            SONG_YEAR_COLUMN + " = '" + audioFile->getTag()->getYear() +
            "' WHERE " + SONG_ID_COLUMN + " = " + ss.str() +
            ";";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    int rc = sqlite3_step(stmt);
    return rc;
}
