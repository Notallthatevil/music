package com.trippntechnology.tagger

import android.content.Context
import android.support.v7.widget.RecyclerView
import android.view.GestureDetector
import android.view.MotionEvent
import android.view.View

interface RecyclerViewOnItemClickListener {
    fun onItemclick(view: View, position: Int)
}