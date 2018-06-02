package com.trippntechnology.tagger

import android.view.View

interface RecyclerViewOnItemClickListener {
    fun onItemclick(view: View, position: Int)
}