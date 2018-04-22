package com.trippntechnology.tagger

import android.os.Parcel
import android.os.Parcelable


class Song(var ID: Int, var Title: String?, var Album: String?, var Artist: String?, var Track: String?, var Year: String?, var Filepath: String): Parcelable{
    constructor(parcel: Parcel) : this(
            parcel.readInt(),
            parcel.readString(),
            parcel.readString(),
            parcel.readString(),
            parcel.readString(),
            parcel.readString(),
            parcel.readString()) {
    }

    override fun writeToParcel(parcel: Parcel, flags: Int) {
        parcel.writeInt(ID)
        parcel.writeString(Title)
        parcel.writeString(Album)
        parcel.writeString(Artist)
        parcel.writeString(Track)
        parcel.writeString(Year)
        parcel.writeString(Filepath)
    }

    override fun describeContents(): Int {
        return 0
    }

    companion object CREATOR : Parcelable.Creator<Song> {
        override fun createFromParcel(parcel: Parcel): Song {
            return Song(parcel)
        }

        override fun newArray(size: Int): Array<Song?> {
            return arrayOfNulls(size)
        }
    }

}