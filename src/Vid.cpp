//
//  vids.cpp
//  manyVideos
//
//  Created by Caroline Record on 8/22/17.
//
//

#include "Vid.h"

Vid::Vid(string name, int firstFrame_1960, int endFrame_1960, int firstFrame_2010, int endFrame_2010, vector <string> loopFiles) {

    name_ = name;
    debugInfo_ = "group: " + ofToString(name) + "\n 1960 frame range: " + ofToString(firstFrame_1960) + "-" + ofToString(endFrame_1960);
  
    startFrame_1960_ = firstFrame_1960;
    stillFrame_1960_ = endFrame_1960;
    startFrame_2010_ = firstFrame_1960;
    stillFrame_2010_ = endFrame_1960;
    
    still_.load(frameToFilename(stillFrame_1960_ - 1, false));
    
    loopFiles_ = loopFiles;
    for (int i =0; i< loopFiles.size(); i++){
        ofVideoPlayer temp;
        string filen = "videos/"+ loopFiles.at(i);
        temp.load(filen);
        videos.push_back(temp);
        loopIndex_ = 0;
   }
  
    isCurrentlyPlaying = false;
    frameQ_ = {};
}

void Vid::update(int frame){
    bool inRange = (frame > startFrame_1960_) && (frame <= stillFrame_1960_);
    if (!isCurrentlyPlaying && inRange) {
        // Set to currently playing
        isCurrentlyPlaying = true;
        setupVideoBlock(frame);
    } else if (isCurrentlyPlaying && !inRange) {
        // set to not currently playing
        isCurrentlyPlaying = false;
        // clearFrameQ
        frameQ_ = {};
        stopVideoBlock();
    }
}


int Vid::calculateFrameToShow() {
    if (frameQ_.empty()) {
        updateVideoBlock();
        return stillFrame_1960_ - 1;
    } else {
        int QFrame = frameQ_.front();
        int framesToPop = 1;
        int framesPopped = 0;
        while (!frameQ_.empty() && framesPopped < framesToPop) {
            frameQ_.pop_front();
            framesPopped++;
        }
        return QFrame;
    }
}


void Vid::setupVideoBlock(int frame){
  // isStill_ = true;
  delay_ = ofRandom(3000, 6000);
  startTime_ = ofGetElapsedTimeMillis();

  for (int i = frame + 1; i < stillFrame_1960_; i++) {
    frameQ_.push_back(i);
   }
    
}

void Vid::updateVideoBlock(){
  ofLog() << "update video block";
  if (videos.size() > 0 && videos.at(loopIndex_).isPlaying()) { // is playing a loop
      videos.at(loopIndex_).update();
      if (videos.at(loopIndex_).getPosition() > .99) {
          ofLog() << " A LOOP IS OVER";
        // Once a loop is complete, stop it, set a delay time before playing the next one
        videos.at(loopIndex_).stop();
        videos.at(loopIndex_).setPosition(0);
        if ((loopIndex_+ 1) < videos.size()){
          loopIndex_ ++;
        } else {
         loopIndex_ = 0;
        }
        delay_ = ofRandom(3000, 6000);
        startTime_ = ofGetElapsedTimeMillis();
    }
  } else if (videos.size() > 0) {
    if ((delay_ != 0) && (ofGetElapsedTimeMillis() - startTime_) >= delay_) {
        videos.at(loopIndex_).setLoopState(OF_LOOP_NONE);
        videos.at(loopIndex_).play();
        delay_ = 0;
    }
  }
}

void Vid::drawVid() {
    ofLog() << loopIndex_;
    
    if (videos.size() > 0 && videos.at(loopIndex_).isPlaying()) {
        videos.at(loopIndex_).draw(0, 0);
    }
}

void Vid::stopVideoBlock(){
    ofLog() << "stopping video";
    loopIndex_ = 0;
    delay_ = 0;
    startTime_ = 0;
    for(int i =0; i< videos.size(); i++){
        videos.at(i).stop();
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
