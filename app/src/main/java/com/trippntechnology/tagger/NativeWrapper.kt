package com.trippntechnology.tagger

class NativeWrapper {

    fun retrieveSongsMutable():MutableList<Song>{
        return retrieveSongs().toMutableList()
    }

    external fun retrieveSongs():Array<Song>
    external fun generateDatabase()
    companion object {
        init {
            System.loadLibrary("tagger")
        }
    }
}