package com.trippntechnology.tagger

import java.io.ByteArrayOutputStream
import java.nio.ByteBuffer

class Song {
    var ID: Int = 0
    var Title: String? = null
    var Album: String? = null
    var Artist: String? = null
    var Track: String? = null
    var Year: String? = null
    lateinit var Filepath: String

    constructor(ID: Int, Title: String, Artist: String, Album: String, Track: String, Year: String, Filepath: String) {
        this.ID = ID
        this.Title = Title
        this.Artist = Artist
        this.Album = Album
        this.Track = Track
        this.Year = Year
        this.Filepath = Filepath
    }

    constructor(deserialize: ByteArray) {
        ID = ByteBuffer.wrap(deserialize, 0, 4).int
        var assign = 0
        var offset = 4
        while (offset < deserialize.size) {
            val length = ByteBuffer.wrap(deserialize, offset, 4).int
            when (assign) {
                0 -> Title = String(deserialize, offset + 4, length)
                1 -> Artist = String(deserialize, offset + 4, length)
                2 -> Album = String(deserialize, offset + 4, length)
                3 -> Track = String(deserialize, offset + 4, length)
                4 -> Year = String(deserialize, offset + 4, length)
                5 -> Filepath = String(deserialize, offset + 4, length)
            }
            offset += 4 + length
            assign++
        }
    }

    fun serialize(): ByteArray {
        val idBytes = ByteBuffer.allocate(4).putInt(ID).array()
        val titBytes: ByteArray = if (Title == null || Title == "") {
            "NULL".toByteArray()
        } else {
            Title!!.toByteArray()

        }
        val albBytes: ByteArray = if (Album == null || Album == "") {
            "NULL".toByteArray()
        } else {
            Album!!.toByteArray()

        }
        val artBytes: ByteArray = if (Artist == null || Artist == "") {
            "NULL".toByteArray()
        } else {
            Artist!!.toByteArray()

        }
        val traBytes: ByteArray = if (Track == null || Track == "") {
            "NULL".toByteArray()
        } else {
            Track!!.toByteArray()

        }
        val yeaBytes: ByteArray = if (Year == null || Year == "") {
            "NULL".toByteArray()
        } else {
            Year!!.toByteArray()

        }
        val filBytes: ByteArray = Filepath.toByteArray()
        val outputStream = ByteArrayOutputStream()
        outputStream.write(idBytes)
        outputStream.write(ByteBuffer.allocate(4).putInt(titBytes.size).array())
        outputStream.write(titBytes)
        outputStream.write(ByteBuffer.allocate(4).putInt(artBytes.size).array())
        outputStream.write(artBytes)
        outputStream.write(ByteBuffer.allocate(4).putInt(albBytes.size).array())
        outputStream.write(albBytes)
        outputStream.write(ByteBuffer.allocate(4).putInt(traBytes.size).array())
        outputStream.write(traBytes)
        outputStream.write(ByteBuffer.allocate(4).putInt(yeaBytes.size).array())
        outputStream.write(yeaBytes)
        outputStream.write(ByteBuffer.allocate(4).putInt(filBytes.size).array())
        outputStream.write(filBytes)
        return outputStream.toByteArray()
    }
}