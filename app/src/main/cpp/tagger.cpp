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
#include "SqlHelper.h"

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

/**
 * Returns an int based on the success of creating a database
 *
 * -1 = unable to access files i.e. user has not granted permission
 * -2 = unable to create database file
 *
 */
extern "C"
JNIEXPORT jint
JNICALL
Java_com_trippntechnology_tagger_MainActivity_generateDatabase(JNIEnv *env, jobject /* this */) {
    vector<string> files;
    try {
        files = getFiles("/storage/emulated/0/Music/");
    } catch (fileAccessException) {
        __android_log_print(ANDROID_LOG_ERROR, "ERROR",
                            "Unable to access external drive. Insufficient permissions");
        return -1;
    }
    vector<Song> allSongs;
    allSongs.resize(files.size());
    for (int i = 0; i < files.size(); i++) {

        string sub = files[i].substr(files[i].find_last_of(".") + 1);
        if (sub == "mp3") {
            Mp3File mp3File(files[i].c_str());
            Song *newSong = new Song(files[i], mp3File.getId3Tag());
            allSongs[i] = *newSong;
        }
    }

    SqlHelper sqlHelper;
    sqlHelper.dropTable(sqlHelper.SONG_TABLE);
    sqlHelper.createTable(sqlHelper.SONG_TABLE);
    int insertErrors = 0;
    for (Song s:allSongs) {
        if (sqlHelper.insertSong(s) != 101) {
            insertErrors++;
        }
    }
    allSongs.clear();
    return insertErrors;
}


extern "C"
JNIEXPORT jobjectArray
JNICALL Java_com_trippntechnology_tagger_MainActivity_retrieveSongs(JNIEnv *env, jobject) {
    SqlHelper sqlHelper;
    return sqlHelper.retrieveAllSongs(env);
}