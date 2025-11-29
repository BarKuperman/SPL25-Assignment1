#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {} //added library() to suppress compiler warning

/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    for(const SessionConfig::TrackInfo& info : library_tracks){
        if(info.type == "MP3"){
            library.push_back(new MP3Track(info.title, info.artists, info.duration_seconds, info.bpm, info.extra_param1, info.extra_param2));
            //std::cout << "MP3: MP3Track created: " <<info.extra_param1<< " kbps"<< std::endl;
        } 
        if(info.type == "WAV"){
            library.push_back(new WAVTrack(info.title, info.artists, info.duration_seconds, info.bpm, info.extra_param1, info.extra_param2));
            //std::cout << "WAV: WAVTrack created: "<<info.extra_param1<< "Hz/" <<info.extra_param2<< "bit" << std::endl;
        }
    }
    std::cout << "[INFO] Track library built: " <<library_tracks.size()<< " tracks loaded" << std::endl;

}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    AudioTrack* track = playlist.find_track(track_title);
    if(!track){
        return nullptr; 
    }
    return track;
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    std::cout << "[INFO] Loading playlist: "<<playlist_name<<"" << std::endl;
    playlist = Playlist(playlist_name);
    for(int i : track_indices){
        if(i<1 || static_cast<size_t>(i - 1) >= library.size()){ //static_cast<int>(i-1) to suppress compiler warning
            std::cout << "[WARNING] Invalid track index: " <<i<<"" << std::endl;
            continue;
        }
        AudioTrack *track = library[i - 1]->clone().release();
        if (!track)
        {
            std::cout << "[ERROR] Track failed to clone" << std::endl;
            continue;
        }
        track->load();
        track->analyze_beatgrid();
        playlist.add_track(track);
        //std::cout << "Added ''"<<track->get_title()<< "' to playlist '"<<playlist_name<<"'" << std::endl;
    }
    std::cout << "[INFO] Playlist loaded: "<<playlist_name<< " (" <<track_indices.size()<<" tracks)" << std::endl;
    //playlist.display();

}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> titles;
    for(AudioTrack* track : playlist.getTracks()){
        titles.push_back(track->get_title());
    }
    return titles;
}
