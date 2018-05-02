#include <jni.h>
#include <string>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <algorithm>
#include <exception>
#include "Song.h"
#include "File.h"
#include "Mp3File.h"
#include "SqlHelper.h"
#include <errno.h>

using namespace std;

typedef vector<vector<string>> VectorList;


//TODO create UTF8 string for c++ to be able to write utf8 strings and retrieve them. Especially helpful for database stuff
vector<string> getFiles(string directory) {
    DIR *d;
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
 * 0  = all operations were successful
 * -1 = unable to access files i.e. user has not granted permission
 * -2 = unable to creating database file
 * else returns the number of songs that failed to be inserted into the database
 */
extern "C"
JNIEXPORT jint
JNICALL
Java_com_trippntechnology_tagger_NativeWrapper_generateDatabase(JNIEnv *env, jobject /* this */) {
//    ID3Tag tag;
//    SongData data;
//    data.Album = "Album";
//    data.Artist="Artist";
//    data.Title="Title";
//    data.Track="12";
//    data.Year="2018";
//    tag.generateTags(data);
//    return 0;
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
            Mp3File mp3File(&files[i]);
            Song *newSong = new Song(files[i], mp3File.getId3Tag());
            allSongs[i] = *newSong;
        }
    }
    int insertErrors = 0;
    try {
        SqlHelper sqlHelper;
        sqlHelper.dropTable(sqlHelper.SONG_TABLE);
        sqlHelper.createTable(sqlHelper.SONG_TABLE);
        for (Song s:allSongs) {
            if (sqlHelper.insertSong(s) != 101) {
                insertErrors++;
            }
        }
    } catch (databaseCreationError()) {
        return -2;
    }

    allSongs.clear();
    return insertErrors;
}

extern "C"
JNIEXPORT jobjectArray
JNICALL Java_com_trippntechnology_tagger_NativeWrapper_retrieveSongs(JNIEnv *env, jobject) {
    SqlHelper sqlHelper;
    return sqlHelper.retrieveAllSongs(env);
}

extern "C"
JNIEXPORT jint
JNICALL Java_com_trippntechnology_tagger_NativeWrapper_saveNewTag(JNIEnv *env, jobject,
                                                                  jbyteArray jSerialized) {
    int length = env->GetArrayLength(jSerialized);
    vector<char> buf(length);
    env->GetByteArrayRegion(jSerialized, 0, length, (jbyte *) &buf[0]);
    Song song(buf);
    SqlHelper sqlHelper;
    string filepath = sqlHelper.selectSong(song);
    Mp3File mp3(&filepath);
    ID3Tag tag;
    SongData data;
    data.Title = song.getTitle();
    data.Artist = song.getArtist();
    data.Album = song.getAlbum();
    data.Year = song.getYear();
    data.Track = song.getTrack();
    vector<char> newSong = tag.generateTags(data);
    newSong.insert(newSong.end(),mp3.getMp3Data().begin(),mp3.getMp3Data().end());
    FILE * f = fopen(song.getFilepath().c_str(),"w+");
    fwrite(newSong.data(), sizeof(char), newSong.size(),f);
    fclose(f);
    sqlHelper.updateSong(song);
    return 1;
}