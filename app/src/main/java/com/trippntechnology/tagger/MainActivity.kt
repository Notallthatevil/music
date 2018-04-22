package com.trippntechnology.tagger

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.support.v4.app.ActivityCompat
import android.support.v4.content.ContextCompat
import android.support.v7.widget.DefaultItemAnimator
import android.support.v7.widget.LinearLayoutManager
import android.util.Log
import android.view.View
import android.widget.Toast
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    private val PERMSSIONREADEXTERNALSTORAGE: Int = 6745

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        if (getPermission()) {
            val start = System.currentTimeMillis()
            generateDatabase()
            val end = System.currentTimeMillis()
            Log.d("EXECUTION TIME", (end - start).toString())
            val songs = retrieveSongs().toMutableList()
            val adapter = SongRecyclerAdapter(songs)
            recyclerView.layoutManager = LinearLayoutManager(applicationContext)
            recyclerView.itemAnimator = DefaultItemAnimator()
            adapter.onItemClickListener = object : RecyclerViewOnItemClickListener{
                override fun onItemclick(view: View, position: Int) {
                    val intent = Intent(this@MainActivity,TagEditorActivity::class.java)
                    intent.putExtra("SONG",adapter.songs[position])
                    startActivity(intent)
                }
            }
            recyclerView.adapter = adapter
        }

    }


    private fun getPermission(): Boolean {
        return if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE), PERMSSIONREADEXTERNALSTORAGE)
            false
        } else {
            true
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        when (requestCode) {
            PERMSSIONREADEXTERNALSTORAGE -> {
                if ((grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED)) {
                    generateDatabase()
                } else {
                    return
                }
                return
            }
        }
    }

    external fun generateDatabase(): Int
    external fun retrieveSongs(): Array<Song>

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("tagger")
        }
    }
}
