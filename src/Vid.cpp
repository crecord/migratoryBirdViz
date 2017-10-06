//
//  vids.cpp
//  manyVideos
//
//  Created by Caroline Record on 8/22/17.
//
//

#include "Vid.h"

float Vid::percent = 0;
int Vid::frame = 0;
bool Vid::isAfter = false;

void Vid::setup(string name, int firstFrame_1960, int lastFrame_1960, vector <string> loopFiles){

    debugInfo = "group: " + ofToString(name) + "\n 1960 frame range: " + ofToString(firstFrame_1960) + "-" + ofToString(lastFrame_1960);
  
    name = name;
    
    string stillURL = frameToFilename(lastFrame_1960 - 1, false);
    ofLog() << stillURL;
    still.load(stillURL);

    startFrame_1960 = firstFrame_1960;
    endFrame_1960 = lastFrame_1960;
    
    loopFiles = loopFiles;
  
    for (int i =0; i< loopFiles.size(); i++){
      ofVideoPlayer temp;
      string filen = "videos/"+ loopFiles.at(i);
      temp.load(filen);
      videos.push_back(temp);
    }
  
    isCurrentlyPlaying = false;
    
    loopIndex = 0;
}

void Vid::update(){
    // currently looking at the 2010s
    if (isAfter){
      if ((frame >= startFrame_1960)&(frame <= endFrame_1960)){
        if (!isCurrentlyPlaying){
          setupVideoBlock();
        }
        isCurrentlyPlaying = true;
        updateVideoBlock();
      } else{
        if (isCurrentlyPlaying) {
          stopVideoBlock();
        }
        isCurrentlyPlaying = false;
      }
    }
    
    // looking at the 1960s
    else{
      if (frame > startFrame_1960 & frame <= endFrame_1960 ||
          frame == 0 & startFrame_1960 == 0) {
        if (!isCurrentlyPlaying){
          setupVideoBlock();
          isCurrentlyPlaying = true;
        } else if (frameQ.empty()) {
          updateVideoBlock();
        }
      } else{
        if (isCurrentlyPlaying) {
          stopVideoBlock();
          isCurrentlyPlaying = false;
        }
      }
    }
}


void Vid::setupVideoBlock(){
  loopIndex = 0;
  if (videos.size() == 0) {
    loopIndex = -1; // has no loops
  }
  isStill = true;
  delay = ofRandom(5000, 10000);
  startTime = ofGetElapsedTimeMillis();
    
  // also pop any remaining action frames onto frameQ
    for (int i = frame; i < endFrame_1960; i++) {
        frameQ.push_back(frameToFilename(i, false));
    }
}

void Vid::updateVideoBlock(){
  if (loopIndex != -1) { // has loops
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
}


void Vid::draw(){
  if (!frameQ.empty()) {
    ofImage frameToDraw;
    frameToDraw.load(frameQ.front());
    frameToDraw.draw(0, 0);
    frameQ.pop_front();
  }
  else if (isStill) {
    still.draw(0, 0);
  }
  else if (loopIndex != -1) { // has loops
    videos.at(loopIndex).draw(0, 0);
  }
}

void Vid::stopVideoBlock(){
    loopIndex = 0;
    isStill = false;
    delay = 0;
    for(int i =0; i< videos.size(); i++){
        videos.at(i).stop();
    }
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
