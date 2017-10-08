//
//  vids.h
//  manyVideos
//
//  Created by Caroline Record on 8/22/17.
//
//

#include "ofMain.h"

class Vid{
public:
    Vid(
      string name,
      int firstFrame_1960,
      int endFrame_1960,
      int firstFrame_2010,
      int endFrame_2010,
      vector <string> loopFiles
    );
  
    void update(int frame);
        
   // vector <int> currentlyPlayingIndexes;
   // vector <ofVideoPlayer> videos;
    
    int calculateFrameToShow();
  
    bool isCurrentlyPlaying= false;
    string debugInfo_;
    
private:

    vector <string> loopFiles_;
    float startFrame_1960_;
    float stillFrame_1960_;
    float startFrame_2010_;
    float stillFrame_2010_;
    
    ofImage still_;
    deque<int> frameQ_;

    string name_;

    // manage the block of videos
    void setupVideoBlock(int frame);
    void updateVideoBlock();
    void drawVideoBlock();
    void stopVideoBlock();
    
    string frameToFilename(int frameNumber, bool isAfter);
    
    //int loopIndex;
    
    //int delay;
    //int startTime;
};
