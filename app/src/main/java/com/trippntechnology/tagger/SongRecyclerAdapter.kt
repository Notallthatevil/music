package com.trippntechnology.tagger

import android.databinding.DataBindingUtil
import android.support.v7.widget.RecyclerView
import android.view.LayoutInflater
import android.view.ViewGroup
import com.trippntechnology.tagger.databinding.ListItemSongBinding

class SongRecyclerAdapter(val songs: MutableList<Song>) : RecyclerView.Adapter<SongRecyclerAdapter.SongViewHolder>() {

    var onItemClickListener: RecyclerViewOnItemClickListener? = null

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): SongViewHolder {
        return SongViewHolder(parent,onItemClickListener)
    }

    override fun getItemCount(): Int {
        return songs.size
    }

    override fun onBindViewHolder(holder: SongViewHolder, position: Int) {
        holder.binding.song = songs[position]
    }


    class SongViewHolder(parent: ViewGroup, private val recyclerViewOnItemClickListener: RecyclerViewOnItemClickListener?, val binding: ListItemSongBinding =
            DataBindingUtil.inflate(LayoutInflater.from(parent.context), R.layout.list_item_song, parent, false)) : RecyclerView.ViewHolder(binding.root){
        init {
            if(recyclerViewOnItemClickListener!=null){
                itemView.setOnClickListener {
                    recyclerViewOnItemClickListener.onItemclick(itemView,adapterPosition)
                }
            }
        }
    }
}