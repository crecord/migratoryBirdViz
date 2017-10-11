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
      vector <string> loopFiles,
      vector <int> loopDelays,
      string stillLoop
    );
    
    string name_;
    string debugInfo_;
  
    bool isInRange(int frame);
    int calculateFrameToShow();
    bool isLoopFinished(); // returns true if loop has finished
    void drawVid();
  
    bool isInRange_ = false;
    bool isPlayingTransition_ = false;
    bool isPlayingStill_ = false;
    bool isPlayingLoop_ = false;
    void setupTransition(int frame);

    
private:
    
    deque<int> frameQ_;
    
    vector <string> loopFiles_;    // Names of videos
    vector <int> loopDelays_;      // The delay for each video
    vector <ofVideoPlayer> videos; // The video player object
    int loopIndex_ = 0;            // Which loop should be played next
    bool hasLoops = false;
    
    void setupStill();
    void setupLooping();
    void updateLooping();
    
    float startFrame_1960_;
    float stillFrame_1960_;
    float startFrame_2010_;
    float stillFrame_2010_;
    float startFrame_;
    float stillFrame_;
    
    ofImage still_;
    ofVideoPlayer stillLoop_;

    // manage the block of videos
    void setupVideoBlock(int frame);
    void updateVideoBlock(); // returns if movie is playing
    void drawVideoBlock();
    void stopVideoBlock();
    
    string frameToFilename(int frameNumber, bool isAfter);
    
    
    int MAX_DELAY = 15000;
    int delay_;
    int startTime_;
};
