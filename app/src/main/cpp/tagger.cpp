#include <jni.h>
#include <string>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <algorithm>
#include <exception>
#include <android/log.h>
#include "Song.h"
#include "File.h"
#include "Mp3File.h"
#include "sqlite3.h"

using namespace std;
typedef vector<vector<string>> VectorList;

vector<string> getFiles(string directory) {
    DIR *d;
    // vector<vector<string> > vectorList;
    VectorList vectorList;
    vector<string> stringList;
    d = opendir(directory.c_str());
    if (d) {
        struct dirent *dir;
        while ((dir = readdir(d)) != NULL) {
            string dirName(dir->d_name);
            if (dirName != "." && dirName != "..") {
                string newDir = directory + dirName + "/";
                //If it is a directory search it
                if (opendir(newDir.c_str()) != NULL) {
                    vectorList.push_back(getFiles(newDir));
                } else {
                    string sub = dirName.substr(dirName.find_last_of(".") + 1);
                    if (sub == "mp3") {
                        stringList.push_back(directory + dirName);
                    }
                }
            }
//            delete dir;
        }
    } else {
        throw fileAccessException();
    }
    closedir(d);
    unsigned long wholeSize = 0;
    for (int i = 0; i < vectorList.size(); i++) {
        wholeSize += vectorList[i].size();
    }
    vector<string> completeList;
    completeList.reserve(wholeSize + stringList.size());
    for (int i = 0; i < vectorList.size(); i++) {
        completeList.insert(completeList.end(), vectorList[i].begin(), vectorList[i].end());
    }
    completeList.insert(completeList.end(), stringList.begin(), stringList.end());
    return completeList;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        __android_log_print(ANDROID_LOG_DEBUG, "%s = %s\n", azColName[i],
                            argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

/**
 * Returns an int based on the success of creating a database
 *
 * -1 = unable to access files i.e. user has not granted permission
 *
 */
extern "C"
JNIEXPORT jint
JNICALL
Java_com_trippntechnology_tagger_MainActivity_generateDatabase(JNIEnv *env, jobject /* this */) {
    vector<string> files;
    vector<Song> songs;
    try {
        files = getFiles("/storage/emulated/0/Music/");
    } catch (fileAccessException) {
        __android_log_print(ANDROID_LOG_ERROR, "ERROR",
                            "Unable to access external drive. Insufficient permissions");
        return -1;
    }
    sort(files.begin(), files.end());
    vector<Song> allSongs;
    allSongs.resize(files.size());
    for (int i = 0; i < files.size(); i++) {

        string sub = files[i].substr(files[i].find_last_of(".") + 1);
        if (sub == "mp3") {
            Mp3File mp3File(files[i].c_str());
//            Song *newSong = new Song(files[i], mp3File->getId3Tag());
//            allSongs[i] = *newSong;
        }
    }

//    sqlite3 *db;
//    char *errormsg = 0;
//    int rc = sqlite3_open_v2("/data/data/com.trippntechnology.tagger/databases/TNT.db", &db,
//                             SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
//    if (rc) {
//        __android_log_print(ANDROID_LOG_ERROR, "DATABASE", "%s", sqlite3_errmsg(db));
//    } else {
//        __android_log_print(ANDROID_LOG_DEBUG, "DATABASE", "DB OPENED");
//    }
//    const char *sql = "CREATE TABLE SONGS(ID INT PRIMARY KEY NOT NULL,TITLE TEXT,ARTIST TEXT,ALBUM TEXT,TRACK TEXT,YEAR TEXT,FILEPATH TEXT NOT NULL);";
//    rc = sqlite3_exec(db, sql, callback, 0, &errormsg);
//    if (rc) {
//        __android_log_print(ANDROID_LOG_ERROR, "DATABASE", "%s", errormsg);
//        sqlite3_free(errormsg);
//    } else {
//        __android_log_print(ANDROID_LOG_DEBUG, "DATABASE", "TABLE CREATED SUCCESSFUL");
//    }
//
//    for(Song s:allSongs){
//        string insertSql = "INSERT INTO SONG (TITLE,ARTIST,ALBUM,TRACK,YEAR,FILEPATH) VALUES("+s.getTitle()+", "+s.getArtist()+", "+s.getAlbum()+", "+s.getTrack()+", "+s.getYear()+", "+s.getFilepath()+");";
//        sqlite3_exec(db,insertSql.c_str(),callback,0,&errormsg);
//        if (rc) {
//            __android_log_print(ANDROID_LOG_ERROR, "DATABASE", "%s", errormsg);
//            sqlite3_free(errormsg);
//        } else {
//            __android_log_print(ANDROID_LOG_DEBUG, "DATABASE", "INSERT SUCCESSFUL");
//        }
//    }
//
//    sqlite3_close(db);


    //TODO add delete for allSongs
    return 1;
}
