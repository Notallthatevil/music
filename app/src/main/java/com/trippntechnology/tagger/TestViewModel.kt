package com.trippntechnology.tagger

import android.arch.lifecycle.MutableLiveData
import android.arch.lifecycle.ViewModel

class TestViewModel(val nativeWrapper: NativeWrapper):ViewModel() {

    var songList : MutableLiveData<MutableList<Song>>

    init {
        songList = loadSongList()
    }

    fun loadSongList():MutableLiveData<MutableList<Song>>{
        val mutableList = nativeWrapper.retrieveSongsMutable()
        val mutableLiveData = MutableLiveData<MutableList<Song>>()
        mutableLiveData.value = mutableList
        return mutableLiveData
    }

}