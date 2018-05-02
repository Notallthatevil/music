package com.trippntechnology.tagger

import android.arch.lifecycle.ViewModel
import android.arch.lifecycle.ViewModelProvider

class TestViewModelFactory(val nativeWrapper: NativeWrapper):ViewModelProvider.Factory {
    override fun <T : ViewModel> create(modelClass: Class<T>): T {
        return TestViewModel(nativeWrapper) as T
    }
}
