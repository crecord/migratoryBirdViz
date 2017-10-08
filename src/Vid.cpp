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
    
   //loopFiles_ = loopFiles;
  
   //   for (int i =0; i< loopFiles.size(); i++){
   //   ofVideoPlayer temp;
    //  string filen = "videos/"+ loopFiles.at(i);
     // temp.load(filen);
     // videos.push_back(temp);
   // }
  
    isCurrentlyPlaying = false;
    
    // loopIndex = 0;
    frameQ_ = deque<int>();
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
    }
}

int Vid::calculateFrameToShow() {
    if (frameQ_.empty()) {
        return stillFrame_1960_ - 1;
    } else {
        int QFrame = frameQ_.front();
        int framesToPop = 2;
        int framesPopped = 0;
        while (!frameQ_.empty() && framesPopped < framesToPop) {
            frameQ_.pop_front();
            framesPopped++;
        }
        return QFrame;
    }
}


void Vid::setupVideoBlock(int frame){
  // loopIndex = 0;
  // if (videos.size() == 0) {
  //   loopIndex = -1; // has no loops
  // }
  // isStill = true;
  // delay = ofRandom(5000, 10000);
  // startTime = ofGetElapsedTimeMillis();
    


  for (int i = frame; i < stillFrame_1960_; i++) {
    frameQ_.push_back(i);
   }
    
}

void Vid::updateVideoBlock(){
  /*if (loopIndex != -1) { // has loops
    videos.at(loopIndex).update();
    if(videos.at(loopIndex).getIsMovieDone()) {
      // Once a loop is complete, stop it, set a delay time before playing the next one
      videos.at(loopIndex).stop();
      if (loopIndex < videos.size()-1){
        loopIndex ++;
      } else {
        loopIndex= 0;
      }
      isStill = true;
      delay = ofRandom(5000);
      startTime = ofGetElapsedTimeMillis();
    }
       
    if (delay != 0 & (ofGetElapsedTimeMillis() - startTime) >= delay) {
      isStill = false;
      // Once a delay is complete, playing the next loop
      videos.at(loopIndex).setLoopState(OF_LOOP_NONE);
      videos.at(loopIndex).play();
      delay = 0;
    }
  }
   */
}

void Vid::stopVideoBlock(){
    /*
    loopIndex = 0;
    isStill = false;
    delay = 0;
    for(int i =0; i< videos.size(); i++){
        videos.at(i).stop();
    }
     */
}

void Vid::drawVideoBlock(){
  
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
