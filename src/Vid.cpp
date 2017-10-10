//
//  vids.cpp
//  manyVideos
//
//  Created by Caroline Record on 8/22/17.
//
//

#include "Vid.h"

Vid::Vid(
    string name,
    int firstFrame_1960,
    int endFrame_1960,
    int firstFrame_2010,
    int endFrame_2010,
    vector <string> loopFiles,
    vector <int> loopDelays,
    string stillLoop) {

    name_ = name;
  
    startFrame_1960_ = firstFrame_1960 - 1; // Minus one to convert to 0 index
    stillFrame_1960_ = endFrame_1960 - 1;
    startFrame_2010_ = firstFrame_1960 - 1;
    stillFrame_2010_ = endFrame_1960 - 1;
    
    // We are in 1960 to start
    startFrame_ = startFrame_1960_;
    stillFrame_ = stillFrame_1960_;
    
    still_.load(frameToFilename(stillFrame_, false));
    
    loopFiles_ = loopFiles;
    loopDelays_ = loopDelays;
    for (int i =0; i< loopFiles.size(); i++){
        ofVideoPlayer temp;
        string filen = "videos/"+ loopFiles.at(i);
        temp.load(filen);
        temp.setLoopState(OF_LOOP_NONE);
        videos.push_back(temp);
        hasLoops = true;
   }

    // If a short loop should be playing when the scene is 'still' (rare case)
    if (stillLoop != "") {
        stillLoop_.load("videos/"+stillLoop);
        stillLoop_.setLoopState(OF_LOOP_NORMAL);
    }
}


bool Vid::isInRange(int frame) {
    bool nowInRange = (startFrame_1960_ <= frame) && (frame <= stillFrame_1960_);
    if (nowInRange && !isInRange_) {
        // If it just became in range, start the transition
        setupTransition(frame);
    } else if (isInRange_ && !nowInRange) {
        // If it just exited range
        stopVideoBlock();
    }
    isInRange_ = nowInRange;
    return isInRange_;
}

int Vid::calculateFrameToShow() {
    if (isPlayingTransition_) {
        if (frameQ_.empty()) {
            isPlayingTransition_ = false;
            isPlayingStill_ = true;
            setupStill();
            setupLooping();
            return stillFrame_;
        } else {
            int QFrame = frameQ_.front();
            frameQ_.pop_front();
            return QFrame;
        }
    } else if (isPlayingStill_ || isPlayingLoop_) {
        updateLooping();
        return stillFrame_;
    }
}

void Vid::setupTransition(int frame) {
    frameQ_ = {};
    for (int i = frame + 1; i <= stillFrame_1960_; i++) {
        frameQ_.push_back(i);
    }
    isPlayingTransition_ = true;
}

void Vid::setupStill() {
    if (stillLoop_.isLoaded()){
        stillLoop_.play();
    }
}

void Vid::setupLooping() {
    if (hasLoops) {
        loopIndex_ = 0;
        delay_ = loopDelays_.at(loopIndex_);
        startTime_ = ofGetElapsedTimeMillis();
    }
}

void Vid::updateLooping() {
    if (hasLoops && videos.at(loopIndex_).isPlaying()) { // is playing a loop
        videos.at(loopIndex_).update();
        if (videos.at(loopIndex_).getCurrentFrame() < 0) { // Check if video is done
            // Once a loop is done playing: stop it, set the delay time before playing the next one
            videos.at(loopIndex_).stop();
            if ((loopIndex_+ 1) < videos.size()){
                loopIndex_ += 1;
            } else {
                loopIndex_ = 0;
            }
            delay_ = loopDelays_.at(loopIndex_);
            startTime_ = ofGetElapsedTimeMillis();
            isPlayingStill_ = true;
            isPlayingLoop_ = false;
        }
    } if (hasLoops && delay_ != -1 && (ofGetElapsedTimeMillis() - startTime_) >= delay_) {
        // no loop is playing but there are loops to play and delay is over
        videos.at(loopIndex_).play();
        delay_ = -1;
        isPlayingLoop_ = true;
        isPlayingStill_ = false;
    }
}


void Vid::stopVideoBlock(){
    delay_ = -1;
    if (stillLoop_.isLoaded()){ stillLoop_.stop(); }
    for(int i =0; i < videos.size(); i++){
        videos.at(i).stop();
    }
}


void Vid::drawVid() {
    if (isPlayingStill_) {
        stillLoop_.update();
        stillLoop_.draw(0, 0);
    } else if (isPlayingLoop_) {
        videos.at(loopIndex_).draw(0, 0);
    }
}


string Vid::frameToFilename(int frameNumber, bool isAfter) {
    string leadingZeros = "";
    if ((frameNumber + 1) < 10) {
        leadingZeros = "000";
    } else if ((frameNumber + 1) < 100) {
        leadingZeros = "00";
    } else if ((frameNumber + 1) < 1000) {
        leadingZeros = "0";
    }
    if (isAfter) {
        return "";
    } else {
        return "./videos/1960_scrubLevel/1960_" + leadingZeros + ofToString(frameNumber + 1) + ".jpg";
    }
}
