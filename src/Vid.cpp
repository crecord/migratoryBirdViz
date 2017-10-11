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
    int firstFrame,
    int endFrame,
    vector <string> loopFiles,
    vector <int> loopDelays,
    string stillLoop,
    string year
) {

    name_ = name;
  
    startFrame_ = firstFrame - 1; // Minus one to convert to 0 index
    stillFrame_ = endFrame - 1;
    
    still_.load(frameToFilename(stillFrame_, year));
    
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
    bool nowInRange = (startFrame_ <= frame) && (frame <= stillFrame_);
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
        } else {
            int QFrame = frameQ_.front();
            frameQ_.pop_front();
            return QFrame;
        }
    } else if (isPlayingLoop_) {
        if (hasLoops) {
            updateLooping();
        }
    } else if (isPlayingStill_) {
        if (hasLoops && (ofGetElapsedTimeMillis() - startTime_) >= delay_) {
            // no loop is playing but there are loops to play and delay is over
            videos.at(loopIndex_).setSpeed(1);
            videos.at(loopIndex_).play();
            delay_ = MAX_DELAY;
            isPlayingLoop_ = true;
            isPlayingStill_ = false;
        }
    }
    return stillFrame_;
}

void Vid::setupTransition(int frame) {
    frameQ_ = {};
    for (int i = frame + 1; i <= stillFrame_; i++) {
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
    if (videos.at(loopIndex_).isPlaying()) { // is playing a loop
        videos.at(loopIndex_).update();
        if (videos.at(loopIndex_).getCurrentFrame() < 0) { // Check if video done
            // Once a loop is done playing: rewind it, stop it, set the delay time before playing the next one
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
    }
}

bool Vid::isLoopFinished() {
    if (!isPlayingLoop_) {
        return true;
    }
    if (isPlayingLoop_ && videos.at(loopIndex_).getCurrentFrame() < 0) { // Loop Finish
        videos.at(loopIndex_).stop();
        videos.at(loopIndex_).setSpeed(1);
        isPlayingLoop_ = false;
        return true;
    }
    return false;
}

void Vid::stopVideoBlock(){
    delay_ = MAX_DELAY;
    if (stillLoop_.isPlaying()){ stillLoop_.stop(); }
    if (isPlayingLoop_) {
        videos.at(loopIndex_).setSpeed(2);
    }
}


void Vid::drawVid() {
    if (isPlayingStill_ && stillLoop_.isPlaying()) {
        stillLoop_.update();
        stillLoop_.draw(0, 0);
    } else if (isPlayingLoop_) {
        videos.at(loopIndex_).draw(0, 0);
    }
}


string Vid::frameToFilename(int frameNumber, string year) {
    string leadingZeros = "";
    if ((frameNumber + 1) < 10) {
        leadingZeros = "000";
    } else if ((frameNumber + 1) < 100) {
        leadingZeros = "00";
    } else if ((frameNumber + 1) < 1000) {
        leadingZeros = "0";
    }
    return "./videos/"+year+"_scrubLevel/"+year+"_" + leadingZeros + ofToString(frameNumber + 1) + ".jpg";
}
