package com.trippntechnology.tagger

import android.Manifest
import android.arch.lifecycle.Observer
import android.arch.lifecycle.ViewModelProvider
import android.arch.lifecycle.ViewModelProviders
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



//Notice: All kotlin code is for testing purposes only and does not represent the final project



class MainActivity : AppCompatActivity() {

    private val PERMISSION_WRITE_EXTERNAL_STORAGE: Int = 6745
    private val nativeWrapper = NativeWrapper()
//    private val factory = TestViewModelFactory(nativeWrapper)

//    private lateinit var viewModel:TestViewModel
    private val binding by lazy { DataBindingUtil.setContentView<ActivityMainBinding>(this, R.layout.activity_main) }
//    private lateinit var adapterModel: SongViewModelAdapter



    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        if (getPermission()) {

            //NATIVE CALLs
            val start = System.currentTimeMillis()
            nativeWrapper.generateDatabase()
            val end = System.currentTimeMillis()
            Log.d("TIME_EXECUTION", (end - start).toString())
            val songs = nativeWrapper.retrieveSongs().toMutableList()

            //VIEWMODEL SETUP
//            viewModel = ViewModelProviders.of(this,factory).get(TestViewModel::class.java)
//            adapterModel = SongViewModelAdapter(viewModel)

            //BINDING SETUP
            binding.apply {
                setLifecycleOwner(this@MainActivity)
//                viewModel = this@MainActivity.viewModel
            }

            val adapter = SongRecyclerAdapter(songs)

            binding.recyclerView.layoutManager = LinearLayoutManager(applicationContext)
            binding.recyclerView.itemAnimator = DefaultItemAnimator()
            adapter.onItemClickListener = object : RecyclerViewOnItemClickListener {
                override fun onItemclick(view: View, position: Int) {
                    val intent = Intent(this@MainActivity, TagEditorActivity::class.java)
                    intent.putExtra("SONG", adapter.songs[position].serialize())
                    startActivity(intent)
                }
            }
            binding.recyclerView.adapter = adapter
//            val songObserver = Observer<MutableList<Song>>{
//                binding.recyclerView.adapter.notifyDataSetChanged()
//            }
//            viewModel.songList.observe(this,songObserver)
        }
    }


    private fun getPermission(): Boolean {
        return if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE), PERMISSION_WRITE_EXTERNAL_STORAGE)
            false
        } else {
            true
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        when (requestCode) {
            PERMISSION_WRITE_EXTERNAL_STORAGE -> {
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
