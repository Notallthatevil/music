package com.trippntechnology.tagger

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import android.databinding.DataBindingUtil
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.support.v4.app.ActivityCompat
import android.support.v4.content.ContextCompat
import android.support.v7.widget.DefaultItemAnimator
import android.support.v7.widget.LinearLayoutManager
import android.util.Log
import android.view.View
import com.trippntechnology.tagger.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private val PERMSSIONREADEXTERNALSTORAGE: Int = 6745
    private val nativeWrapper = NativeWrapper()

//    private val viewModel by lazy { ViewModelProviders.of(this).get(TestViewModel::class.java) }
    private val binding by lazy { DataBindingUtil.setContentView<ActivityMainBinding>(this, R.layout.activity_main) }
//    private val adapterModel = SongViewModelAdapter(viewModel)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        if (getPermission()) {
            val start = System.currentTimeMillis()
            nativeWrapper.generateDatabase()
            val end = System.currentTimeMillis()
            Log.d("EXECUTION TIME", (end - start).toString())

            binding.apply {
                setLifecycleOwner(this@MainActivity)
            }

            val songs = nativeWrapper.retrieveSongs().toMutableList()
            val adapter = SongRecyclerAdapter(songs)

            binding.recyclerView.layoutManager = LinearLayoutManager(applicationContext)
            binding.recyclerView.itemAnimator = DefaultItemAnimator()
            adapter.onItemClickListener = object : RecyclerViewOnItemClickListener {
                override fun onItemclick(view: View, position: Int) {
                    val intent = Intent(this@MainActivity, TagEditorActivity::class.java)
                    intent.putExtra("SONG", adapter.songs[position])
                    startActivity(intent)
                }
            }
            binding.recyclerView.adapter = adapter
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
                    nativeWrapper.generateDatabase()
                } else {
                    return
                }
                return
            }
        }
    }

}
