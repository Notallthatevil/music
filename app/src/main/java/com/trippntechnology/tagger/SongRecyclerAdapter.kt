package com.trippntechnology.tagger

import android.support.v7.widget.RecyclerView
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView

class SongRecyclerAdapter(val songs: MutableList<Song>) : RecyclerView.Adapter<SongRecyclerAdapter.SongViewHolder>() {

    var onItemClickListener:RecyclerViewOnItemClickListener? = null

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): SongViewHolder {
        val itemView = LayoutInflater.from(parent.context).inflate(R.layout.list_item_song, parent, false)
        return SongViewHolder(itemView,onItemClickListener)
    }

    override fun getItemCount(): Int {
        return songs.size
    }

    override fun onBindViewHolder(holder: SongViewHolder, position: Int) {
        holder.filepathTextView.text = songs[position].Filepath
    }

    class SongViewHolder(view: View, recyclerViewOnItemClickListener: RecyclerViewOnItemClickListener?) : RecyclerView.ViewHolder(view) {
        var filepathTextView: TextView = view.findViewById(R.id.textViewFilepath)
        init {
            if(recyclerViewOnItemClickListener!=null){
                itemView.setOnClickListener {
                    recyclerViewOnItemClickListener.onItemclick(itemView,adapterPosition)
                }
            }
        }
    }

}