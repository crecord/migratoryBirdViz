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
    static string mons[12];
  
    void setup(string name, int firstFrame_1960, int lastFrame_1960, vector <string> loopFiles);
  
    void update();
    void draw();
        
    vector <int> currentlyPlayingIndexes;
    vector <ofVideoPlayer> videos;

  
    bool isCurrentlyPlaying;
    bool isSetUpToPlay;
    bool isStill = true;
  
    float startFrame_1960;
    float endFrame_1960;
    float startFrame_2010;
    float endFrame_2010;
  
    string debugInfo;
    ofImage still;
    
private:
    string name;

    vector <string> loopFiles;


    // manage the block of videos
    void updateVideoBlock();
    void setupVideoBlock();
    void stopVideoBlock();
    void drawVideoBlock();
    
    int currentlyPlayingIndex;
    int numberCurrentlyPlaying;
    
    int delay;
    int startTime;
    bool delayCompleted;
    
    int randomIndex1;
    
};
