package com.trippntechnology.tagger

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.os.Debug
import android.os.SystemClock
import android.support.v4.app.ActivityCompat
import android.support.v4.content.ContextCompat
import android.util.Log
import android.view.WindowManager
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    private val PERMSSIONREADEXTERNALSTORAGE: Int = 6745

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        if (BuildConfig.DEBUG){
            if(Debug.isDebuggerConnected()){
                Log.d("SCREEN", "Debugger is attached.")
                window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
            }
        }
        if (getPermission()) {
            val start = System.currentTimeMillis()
            generateDatabase()
            val end = System.currentTimeMillis()
            sample_text.text = ((end-start).toString())
        }
    }


    private fun getPermission():Boolean {
        return if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE), PERMSSIONREADEXTERNALSTORAGE)
            false
        }else{
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

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("tagger")
        }
    }
}
