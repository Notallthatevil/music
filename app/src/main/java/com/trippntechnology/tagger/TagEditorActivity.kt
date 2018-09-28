package com.trippntechnology.tagger

import android.content.Intent
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.provider.MediaStore
import android.support.v7.app.AppCompatActivity
import android.widget.TextView
import android.widget.Toast
import kotlinx.android.synthetic.main.activity_tag_editor.*
import java.io.ByteArrayOutputStream
import java.io.IOException

class TagEditorActivity : AppCompatActivity() {

    private val GALLERY = 1
    private lateinit var song: Song
    private var newCover: ByteArray? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_tag_editor)
        song = Song(intent.getByteArrayExtra("SONG"))
        editTextTitle.setText(song.Title, TextView.BufferType.EDITABLE)
        editTextAlbum.setText(song.Album, TextView.BufferType.EDITABLE)
        editTextArtist.setText(song.Artist, TextView.BufferType.EDITABLE)
        editTextTrack.setText(song.Track, TextView.BufferType.EDITABLE)
        editTextYear.setText(song.Year, TextView.BufferType.EDITABLE)
        if (song.Cover != null) {
            val bitmap = BitmapFactory.decodeByteArray(song.Cover, 0, song.Cover!!.size)
            imageView.setImageBitmap(bitmap)
        }
        val nativeWrapper = NativeWrapper()
        buttonAccept.setOnClickListener {
            val newSong = Song(song.ID, editTextTitle.text.toString(), editTextArtist.text.toString(), editTextAlbum.text.toString(), editTextTrack.text.toString(), editTextYear.text.toString(), song.FilePath, newCover)
            var rc = nativeWrapper.saveNewTag(newSong.serialize())
            Toast.makeText(applicationContext, "Tags successfully saved", Toast.LENGTH_SHORT).show()
        }
        imageView.setOnClickListener {
            val galleyIntent = Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI)
            startActivityForResult(galleyIntent, GALLERY)
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)

        if (requestCode == GALLERY) {
            if (data != null) {
                val contentURI = data.data

                try {
                    val bitmap = MediaStore.Images.Media.getBitmap(this.contentResolver, contentURI)
                    val stream = ByteArrayOutputStream()
                    bitmap.compress(Bitmap.CompressFormat.JPEG, 100, stream)
                    newCover = stream.toByteArray()
                    imageView.setImageBitmap(bitmap)
                } catch (e: IOException) {
                    e.printStackTrace()
                }
            }
        }
    }

}