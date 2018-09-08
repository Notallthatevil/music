//
// Created by natet on 4/7/2018.
//

#include "SqlHelper.h"


//TODO rewrite sqlite to use binding

inline bool file_exists(const std::string &name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

/*
 * Creates database directory if it doesn't exist then opens the database connection.
 */
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

/*
 * Basic destructor
 * Closes the database connection
 */
SqlHelper::~SqlHelper() {
    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);
}

/*
 * Creates the database
 *
 * @param tableName - The name of the database table
 */
int SqlHelper::createTable(string tableName) {
    if (tableName == SONG_TABLE) {
        string sql = "CREATE TABLE " + SONG_TABLE + "(" +
                     SONG_ID_COLUMN + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                     SONG_TITLE_COLUMN + " TEXT, " +
                     SONG_ARTIST_COLUMN + " TEXT, " +
                     SONG_ALBUM_COLUMN + " TEXT, " +
                     SONG_TRACK_COLUMN + " TEXT, " +
                     SONG_YEAR_COLUMN + " TEXT, " +
                     SONG_FILEPATH_COLUMN + " TEXT NOT NULL, " +
                     SONG_ARTWORK_COLUMN + " BLOB);";
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        int rc = sqlite3_step(stmt);
        return rc;
    }
    return -1;
}

/*
 * Deletes the specified table
 *
 * @param tableName - The name of the table to be deleted
 */
int SqlHelper::dropTable(string tableName) {
    string sql = "DROP TABLE IF EXISTS " + tableName;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    int rc = sqlite3_step(stmt);
    return rc;
}


/*
 * Adds a song to the database while also making sure the contents in the file
 * are ready to be inserted.
 *
 * @param audioFile - Pointer the audio file set to be added
 */
int SqlHelper::insertSong(AudioFile *audioFile) {
//    if (audioFile->getFilePath().find("'") != audioFile->getFilePath().npos) {
//        makeSqlFriendly(audioFile->getFilePath(), "'");
//    }
    string sql = "INSERT INTO " + SONG_TABLE +
                 "(TITLE,ARTIST,ALBUM,TRACK,YEAR,FILEPATH,ARTWORK) " +
                 "VALUES(:TIT,:ART,:ALB,:TRA,:YEA,:FIL,?);";
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "prepare failed: %s",
                            sqlite3_errmsg(db));
    } else {
        sqlite3_bind_text(stmt, SONG_TITLE_NUMBER, audioFile->getTag()->getTitle().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, SONG_ARTIST_NUMBER, audioFile->getTag()->getArtist().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, SONG_ALBUM_NUMBER, audioFile->getTag()->getAlbum().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, SONG_TRACK_NUMBER, audioFile->getTag()->getTrack().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, SONG_YEAR_NUMBER, audioFile->getTag()->getYear().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, SONG_FILEPATH_NUMBER, audioFile->getFilePath().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_blob(stmt, SONG_ARTWORK_NUMBER, audioFile->getTag()->getCover(),
                          audioFile->getTag()->getCoverSize(), SQLITE_STATIC);
        rc = sqlite3_step(stmt);
    }
    return rc;



//    string sql = "INSERT INTO " + SONG_TABLE + "(TITLE,ARTIST,ALBUM,TRACK,YEAR,FILEPATH,ARTWORK) VALUES('" +
//                 audioFile->getTag()->getTitle() + "', '" +
//                 audioFile->getTag()->getArtist() + "', '" +
//                 audioFile->getTag()->getAlbum() + "', '" +
//                 audioFile->getTag()->getTrack() + "', '" +
//                 audioFile->getTag()->getYear() + "', '" +
//                 audioFile->getFilePath() + "' );";
//    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
//    sqlite3_step(stmt);
//    return rc;
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
                                                  "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[B)V");
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
                                       jTrack, jYear, jFilepath, NULL);
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
