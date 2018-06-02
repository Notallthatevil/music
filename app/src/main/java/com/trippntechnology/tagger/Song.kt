package com.trippntechnology.tagger

import java.io.ByteArrayOutputStream
import java.nio.ByteBuffer

class Song {
    var ID: Int = 0
    var Title: String? = null
    var Artist: String? = null
    var Album: String? = null
    var Track: String? = null
    var Year: String? = null
    lateinit var FilePath: String
    var Cover: ByteArray? = null

    constructor(ID: Int, Title: String?, Artist: String?, Album: String?, Track: String?, Year: String?, Filepath: String, Cover: ByteArray?) {
        this.ID = ID
        this.Title = Title
        this.Artist = Artist
        this.Album = Album
        this.Track = Track
        this.Year = Year
        this.FilePath = Filepath
        this.Cover = Cover
    }

    constructor(deserialize: ByteArray) {
        ID = ByteBuffer.wrap(deserialize, 1, 4).int
        var assign = 0
        var offset = 5
        while (offset < deserialize.size) {
            val length = ByteBuffer.wrap(deserialize, offset, 4).int
            offset += 4
            if (length != 0) {
                when (assign) {
                    0 -> Title = deserializeString(deserialize.copyOfRange(offset, offset + length))
                    1 -> Artist = deserializeString(deserialize.copyOfRange(offset, offset + length))
                    2 -> Album = deserializeString(deserialize.copyOfRange(offset, offset + length))
                    3 -> Track = deserializeString(deserialize.copyOfRange(offset, offset + length))
                    4 -> Year = deserializeString(deserialize.copyOfRange(offset, offset + length))
                    5 -> FilePath = deserializeString(deserialize.copyOfRange(offset, offset + length))!!
                    6 -> Cover = deserializeCover(deserialize.copyOfRange(offset, offset + length))
                }
            }
            offset += length
            assign++
        }
    }

    private fun deserializeCover(buffer: ByteArray): ByteArray? {
        //Represents "'N' 'U' 'L' 'L'"
        val flagBuf = byteArrayOf(78, 85, 76, 76)
        if (buffer.contentEquals(flagBuf)) {
            return null
        }
        return buffer
    }

    private fun deserializeString(buffer: ByteArray): String? {
        return String(buffer)
    }

    fun serialize(): ByteArray {
        var serialized = identifyFileType(FilePath)
        serialized = concatenateByteArrays(serialized, intToBytes(ID))
        serialized = concatenateByteArrays(serialized, serializeString(Title))
        serialized = concatenateByteArrays(serialized, serializeString(Artist))
        serialized = concatenateByteArrays(serialized, serializeString(Album))
        serialized = concatenateByteArrays(serialized, serializeString(Track))
        serialized = concatenateByteArrays(serialized, serializeString(Year))
        serialized = concatenateByteArrays(serialized, serializeString(FilePath))
        serialized = concatenateByteArrays(serialized, serializeCover(Cover))
        return serialized
    }

    private fun concatenateByteArrays(byteArray1: ByteArray, byteArray2: ByteArray): ByteArray {
        val newArray = ByteArray(byteArray1.size + byteArray2.size)
        System.arraycopy(byteArray1, 0, newArray, 0, byteArray1.size)
        System.arraycopy(byteArray2, 0, newArray, byteArray1.size, byteArray2.size)
        return newArray
    }

    private fun serializeCover(cover: ByteArray?): ByteArray {
        return if (cover == null){
            intToBytes(0)
        }else{
            concatenateByteArrays(intToBytes(cover.size),cover)
        }
    }

    private fun serializeString(frameData: String?): ByteArray {
        val output = frameData!!.toByteArray()
        return concatenateByteArrays(intToBytes(output.size), output)
    }

    private fun intToBytes(int: Int): ByteArray {
        return ByteBuffer.allocate(4).putInt(int).array()
    }

    private fun identifyFileType(FilePath: String): ByteArray {
        val extension = FilePath.substringAfterLast('.')
        val byteArray = ByteArray(1)
        when (extension) {
            "mp3" -> byteArray[0] = 0x00
            "m4a" -> byteArray[0] = 0x01
            "flac" -> byteArray[0] = 0x02
            else -> byteArray[0] = -0x01
        }
        return byteArray
    }
}