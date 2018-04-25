package com.trippntechnology.tagger

import android.databinding.DataBindingUtil
import android.support.v7.widget.RecyclerView
import android.view.LayoutInflater
import android.view.ViewGroup
import com.trippntechnology.tagger.databinding.ListItemSongBinding

class SongViewModelAdapter(val viewModel: TestViewModel): RecyclerView.Adapter<SongViewModelAdapter.SongViewHolder>() {

    var onItemClickListener: RecyclerViewOnItemClickListener? = null

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): SongViewHolder {
        return SongViewHolder(parent,onItemClickListener).apply {
            binding.viewModel = viewModel
        }
    }

    override fun getItemCount(): Int {
        return viewModel.songList.value!!.size
    }

    override fun onBindViewHolder(holder: SongViewHolder, position: Int) {
        holder.binding.song = viewModel.songList.value!![position]
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