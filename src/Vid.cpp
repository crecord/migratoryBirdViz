//
//  vids.cpp
//  manyVideos
//
//  Created by Caroline Record on 8/22/17.
//
//

#include "Vid.h"

string Vid::mons[12] = {
  "january",
  "february",
  "march",
  "april",
  "may",
  "june",
  "july",
  "august",
  "september",
  "october",
  "november",
  "december"
};

float Vid::percent = 0;
int Vid::frame = 0;
bool Vid::isAfter = false;

void Vid::setup(string name, string stillName, int firstFrame_1960, int lastFrame_1960, vector <string> loopFiles){

    debugInfo = "group ID: " + ofToString(name) + "\n 1960 frame range: " + ofToString(firstFrame_1960) + "-" + ofToString(lastFrame_1960);
  
    name = name;
    still.load("stills/" + stillName);
    startFrame_1960 = firstFrame_1960;
    endFrame_1960 = lastFrame_1960;
    loopFiles = loopFiles;
  
    ofLog() << name;
  ofLog() << loopFiles.size();
    for (int i =0; i< loopFiles.size(); i++){
      ofVideoPlayer temp;
      string filen = "videos/"+ loopFiles.at(i);
      temp.load(filen);
      videos.push_back(temp);
    }
  
    isCurrentlyPlaying = false;
    
    currentlyPlayingIndex = 0;
    ofLog() << debugInfo;
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
      if (frame >= startFrame_1960 & frame <= endFrame_1960) {
        if (!isCurrentlyPlaying){
          setupVideoBlock();
          isCurrentlyPlaying = true;
        }
        updateVideoBlock();
      } else{
        if (isCurrentlyPlaying) {
          stopVideoBlock();
          isCurrentlyPlaying = false;
        }
      }
    }
}

/*
 0 - static, non looping use the position to dictate where in the video we are
 1 - Looping a video none at once and doing it evenly - can work for multiple or one
 2 - looping with a default video but other ones playing occasionally
 3 - looping but only occasionally- not too many at once.
*/

void Vid::setupVideoBlock(){
  currentlyPlayingIndex = 0;
  if (videos.size() == 0) {
    currentlyPlayingIndex = -1; // has no loops
  }
  isStill = true;
  delay = ofRandom(5000);
  startTime = ofGetElapsedTimeMillis();
}

void Vid::updateVideoBlock(){
  if (currentlyPlayingIndex != -1) { // has loops
    videos.at(currentlyPlayingIndex).update();
    if(videos.at(currentlyPlayingIndex).getIsMovieDone()) {
      // Once a loop is complete, stop it, set a delay time before playing the next one
      videos.at(currentlyPlayingIndex).stop();
      if (currentlyPlayingIndex < videos.size()-1){
        currentlyPlayingIndex ++;
      } else {
        currentlyPlayingIndex= 0;
      }
      isStill = true;
      delay = ofRandom(5000);
      startTime = ofGetElapsedTimeMillis();
    }
       
    if (delay != 0 & (ofGetElapsedTimeMillis() - startTime) >= delay) {
      isStill = false;
      // Once a delay is complete, playing the next loop
      videos.at(currentlyPlayingIndex).setLoopState(OF_LOOP_NONE);
      videos.at(currentlyPlayingIndex).play();
      delay = 0;
    }
  }
}


void Vid::draw(){
  if (currentlyPlayingIndex != -1) { // has loops
    videos.at(currentlyPlayingIndex).draw(0, 0);
  }
}


void Vid::stopVideoBlock(){
    currentlyPlayingIndex = 0;
    isStill = false;
    delay = 0;
    for(int i =0; i< videos.size(); i++){
        videos.at(i).stop();
    }
}

void Vid::drawVideoBlock(){
  
}
