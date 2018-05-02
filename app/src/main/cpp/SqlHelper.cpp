//
// Created by natet on 4/7/2018.
//

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
    songsList.clear();
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

int SqlHelper::insertSong(Song song) {
    if (song.getFilepath().find("'") != song.getFilepath().npos) {
        makeSqlFriendly(&song.getFilepath(), "'");
    }
    string sql = "INSERT INTO " + SONG_TABLE + "(TITLE,ARTIST,ALBUM,TRACK,YEAR,FILEPATH) VALUES('" +
                 song.getTitle() + "', '" +
                 song.getArtist() + "', '" +
                 song.getAlbum() + "', '" +
                 song.getTrack() + "', '" +
                 song.getYear() + "', '" +
                 song.getFilepath() + "');";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    int rc = sqlite3_step(stmt);
    return rc;
}


void SqlHelper::makeSqlFriendly(string *str, const char *symbol) {
    for (size_t i = 0; i < str->length(); i++) {
        if ((*str)[i] == symbol[0]) {
            str->insert(i, symbol);
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

string SqlHelper::selectSong(Song song) {
    stringstream ss;
    ss << song.getId();
    string sql =
            "SELECT " + SONG_FILEPATH_COLUMN + " FROM " + SONG_TABLE + " WHERE " + SONG_ID_COLUMN +
            " = " + ss.str();
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
    return string((char *) sqlite3_column_text(stmt, 0));
}

int SqlHelper::updateSong(Song song) {
    stringstream ss;
    ss << song.getId();
    //TODO make sure strings are sql friendly i.e. no ' symbols without doubling them up first
    string sql =
            "UPDATE " + SONG_TABLE + " SET " + SONG_TITLE_COLUMN + " = '" + song.getTitle() + "', " +
            SONG_ARTIST_COLUMN + " = '" + song.getArtist() + "', " +
            SONG_ALBUM_COLUMN + " = '" + song.getAlbum() + "', " +
            SONG_TRACK_COLUMN + " = '" + song.getTrack() + "', " +
            SONG_YEAR_COLUMN + " = '" + song.getYear() +
            "' WHERE " + SONG_ID_COLUMN + " = " + ss.str() +
            ";";
    sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,NULL);
    int rc = sqlite3_step(stmt);
    return rc;
}
