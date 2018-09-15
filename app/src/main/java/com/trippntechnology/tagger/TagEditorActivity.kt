package com.trippntechnology.tagger

import android.graphics.BitmapFactory
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.widget.TextView
import android.widget.Toast
import kotlinx.android.synthetic.main.activity_tag_editor.*

class TagEditorActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_tag_editor)
        val song = Song(intent.getByteArrayExtra("SONG"))
        editTextTitle.setText(song.Title,TextView.BufferType.EDITABLE)
        editTextAlbum.setText(song.Album,TextView.BufferType.EDITABLE)
        editTextArtist.setText(song.Artist,TextView.BufferType.EDITABLE)
        editTextTrack.setText(song.Track,TextView.BufferType.EDITABLE)
        editTextYear.setText(song.Year,TextView.BufferType.EDITABLE)
        if(song.Cover != null){
            val bitmap = BitmapFactory.decodeByteArray(song.Cover,0, song.Cover!!.size)
            imageView.setImageBitmap(bitmap)
        }
        val nativeWrapper = NativeWrapper()
        buttonAccept.setOnClickListener {
            val newSong = Song(song.ID,editTextTitle.text.toString(), editTextArtist.text.toString(),editTextAlbum.text.toString(),editTextTrack.text.toString(),editTextYear.text.toString(),song.FilePath,null)
            var rc = nativeWrapper.saveNewTag(newSong.serialize())
            Toast.makeText(applicationContext,"Tags successfully saved",Toast.LENGTH_SHORT).show()
        }
    }
}