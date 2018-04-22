package com.trippntechnology.tagger

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.widget.TextView
import kotlinx.android.synthetic.main.activity_tag_editor.*

class TagEditorActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_tag_editor)
        val song = intent.getParcelableExtra<Song>("SONG")
        editTextTitle.setText(song.Title,TextView.BufferType.EDITABLE)
        editTextAlbum.setText(song.Album,TextView.BufferType.EDITABLE)
        editTextArtist.setText(song.Artist,TextView.BufferType.EDITABLE)
        editTextTrack.setText(song.Track,TextView.BufferType.EDITABLE)
        editTextYear.setText(song.Year,TextView.BufferType.EDITABLE)
    }
}