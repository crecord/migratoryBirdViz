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
    static float percent;
    static int frame;
    static bool isAfter;
  
    void setup(string name, int firstFrame_1960, int lastFrame_1960, vector <string> loopFiles);
  
    void update();
    void draw();
        
    vector <int> currentlyPlayingIndexes;
    vector <ofVideoPlayer> videos;
  
    bool isCurrentlyPlaying;
    bool isStill = true;
    ofImage still;
    deque<string> frameQ;
  
    float startFrame_1960;
    float endFrame_1960;
    float startFrame_2010;
    float endFrame_2010;
      
    string debugInfo;
    
private:
    string name;

    vector <string> loopFiles;

    // manage the block of videos
    void setupVideoBlock();
    void updateVideoBlock();
    void drawVideoBlock();
    void stopVideoBlock();
    
    string frameToFilename(int frameNumber, bool isAfter);
    
    int loopIndex;
    
    int delay;
    int startTime;
};
