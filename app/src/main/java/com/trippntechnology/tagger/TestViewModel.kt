package com.trippntechnology.tagger

import android.arch.lifecycle.LiveData
import android.arch.lifecycle.MutableLiveData
import android.arch.lifecycle.ViewModel

class TestViewModel(val nativeWrapper: NativeWrapper) : ViewModel() {

    val songList: MutableLiveData<MutableList<Song>> = MutableLiveData()

    init {
        songList.value = nativeWrapper.retrieveSongsMutable()
    }
}