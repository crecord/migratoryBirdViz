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
    static bool isAfter;
    static string mons[12];
    
    void setup(int groupID, vector <string> filename, string dateAfter, string dateBefore, int loopState, string alphaRange);
    void update();
    void draw();
    int chooseIndex(int lastIndex);
    
    string percentageToMonth (float percent);
    
    vector <int> currentlyPlayingIndexes;
    vector <ofVideoPlayer> videos;

    bool isCurrentlyPlaying;
    
    string debugInfo;
    bool isPNG;
    ofImage pngImage;
    
    float alphaStartClip;
    float alphaEndClip;
    
private:
    
    
    
    
    
    float convertDate (string date);
    float startPercentBefore;
    float endPercentBefore;
    float startPercentAfter;
    float endPercentAfter;
    int loopingState;
    
    bool isSeamBefore;
    bool isSeamAfter;
    
    string fileN;
    
    // manage the block of videos
    void updateVideoBlock();
    void setupVideoBlock();
    void stopVideoBlock();
    void drawVideoBlock();
    
    int currentlyPlayingIndex;
    int numberCurrentlyPlaying;
    
    int waitTime1;
    int waitTime2;
    int startTime1;
    int startTime2;

  
    
    bool isWait1;
    bool isWait2;
    
    int randomIndex1;
    int randomIndex2;
    
};