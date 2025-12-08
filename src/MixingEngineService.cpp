#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : decks(), active_deck(1), auto_sync(false), bpm_tolerance(0) 
{
    // Initialize deck pointers to null
    decks[0] = nullptr;
    decks[1] = nullptr;

    std::cout << "[MixingEngineService] Initialized with 2 empty decks." << std::endl;
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    std::cout << "[MixingEngineService] Cleaning up decks..." << std::endl;
    if(decks[0]){
        delete decks[0];
    }
    if(decks[1]){
        delete decks[1];
    }
    decks[0] = nullptr;
    decks[1] = nullptr;
}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {

    std::cout << "\n=== Loading Track to Deck ===" << std::endl;

    PointerWrapper<AudioTrack> p = track.clone();
    if(!p.get()){
        std::cout << "Track:"<< track.get_title() <<" failed to clone" << std::endl;
        return -1;
    } 
    int target = 1-active_deck;
    std::cout << "[Deck Switch] Target deck: " <<target<< "" << std::endl;
    if(decks[target]){
        delete decks[target];
        decks[target] = nullptr;
    }
    p -> load();
    p -> analyze_beatgrid();

    if(can_mix_tracks(p)){
        sync_bpm(p);
    }


    decks[target] = p.release();
    std::cout << "[Load Complete] '"<< decks[target]->get_title() <<"' is now loaded on deck "<< target<<"" << std::endl;
    
    
    // if(decks[active_deck]){ 
    //     std::cout << "[Unload] Unloading previous deck " <<active_deck<< " ("<<decks[active_deck]->get_title()<<")" << std::endl;
    //     delete decks[active_deck];
    //     decks[active_deck] = nullptr;
    // }

    // according to pdf this is needed but according to output example it is not


    active_deck = target;
    std::cout << "[Active Deck] Switched to deck "<<target<<"" << std::endl;


    return target;
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    if(!decks[active_deck]){
        std::cout << "[Sync BPM] Cannot sync - one of the decks is empty.\n"; //according to pdf this is not needed.
        return false;
    }
    if(!track){
        return false;
    }
    if(std::abs(decks[active_deck]->get_bpm() - track->get_bpm()) <= bpm_tolerance ){//according to example output
        return false;
    }

    // if(std::abs(decks[active_deck]->get_bpm() - track->get_bpm()) > bpm_tolerance ){//according to pdf
    //     return false;
    // }
    return true;
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    if(!decks[active_deck]||!track){
        return;
    }

    int original = track->get_bpm();
    int newBPM = (original + decks[active_deck]->get_bpm())/2;
    track->set_bpm(newBPM);
    std::cout << "[Sync BPM] Syncing BPM from " <<original<<" to " <<newBPM<< "\n";
}
