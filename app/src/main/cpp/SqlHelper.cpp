//
// Created by natet on 4/7/2018.
//

#include "SqlHelper.h"
#include "Song.h"

SqlHelper::SqlHelper() {
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
        string sql = "CREATE TABLE " + SONG_TABLE + "(" + SONG_TITLE_COLOUMN + " TEXT, " +
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
//    int i = 0;
    jclass jSongClass = env->FindClass("com/trippntechnology/tagger/Song");
    jmethodID jSongConstructor = env->GetMethodID(jSongClass, "<init>",
                                                  "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    vector<jobject> songList;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
//        env->PushLocalFrame(1);
        jstring jTitle = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_TITLE_NUMBER));
        jstring jArtist = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_ARTIST_NUMBER));
        jstring jAlbum = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_ALBUM_NUMBER));
        jstring jTrack = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_TRACK_NUMBER));
        jstring jYear = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_YEAR_NUMBER));
        jstring jFilepath = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_FILEPATH_NUMBER));
        jobject jSong = env->NewObject(jSongClass,jSongConstructor,jTitle,jArtist,jAlbum,jTrack,jYear,jFilepath);
//        env->PopLocalFrame(jSong);
        songList.push_back(jSong);

//        Song *song = new Song();
//        song->setTitle((char *) sqlite3_column_text(stmt, SONG_TITLE_NUMBER));
//        song->setArtist((char *) sqlite3_column_text(stmt, SONG_ARTIST_NUMBER));
//        song->setAlbum((char *) sqlite3_column_text(stmt, SONG_ALBUM_NUMBER));
//        song->setTrack((char *) sqlite3_column_text(stmt, SONG_TRACK_NUMBER));
//        song->setYear((char *) sqlite3_column_text(stmt, SONG_YEAR_NUMBER));
//        song->setFilepath((char *) sqlite3_column_text(stmt, SONG_FILEPATH_NUMBER));
//        __android_log_print(ANDROID_LOG_DEBUG, "DATABASE", "insterted %i %s", i,
//                            song->getFilepath().c_str());
//        songsList.push_back(*song);
//        i++;
    }
    jobjectArray jSongList = env->NewObjectArray(songList.size(),jSongClass,NULL);
    for (int i = 0;i<songList.size();i++){
        env->SetObjectArrayElement(jSongList,i,songList[i]);
    }
    return jSongList;
}
