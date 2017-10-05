#pragma once

#include "ofMain.h"
#include "ofxChromaKeyShader.h"
#include "ofxGui.h"
#include "ofxBezierWarpManager.h"
#include "Vid.h"



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    void drawDebugMasks();
    void drawCheckerboard(float x, float y, int width, int height, int size);
    float setPosition;
    int setFrame;
  
    // === Variables =============================
    ofImage bg_image;
  
    ofxChromaKeyShader *chromakey;
    ofxChromaKeyShader *chromakey1;
    ofVideoGrabber webcam;
    int camW, camH;
    
    ofFbo checkerboardTex;
    
    ofxPanel chromaGui;
    bool bShowGui;
    bool bUpdateBgColor;
  
    ofFbo drawAllVid;
    
    //ofFbo allThelayers;
    ofFbo first_fbo;
    ofFbo second_fbo;
    ofFbo third_fbo;
    ofFbo fourth_fbo;
    
    bool showGuide;
    ofxBezierWarpManager bezManager;
  
    // load in the schedule
    ofXml scheduleOfVideos;
    
    vector<Vid> allVids;
    vector<string> fullScene_1960;
    
    // arduino variables
    ofArduino ard;
    bool bSetupArduino;
    void setupArduino(const int & version);
    void analogPinChanged(const int & pinNum);
    void digitalPinChanged(const int & pinNum);
    void updateArduino();
    
    string encoderVal;
    string dateOfYear; 
    
    
    int vidWidth;
    int vidHeight;
    int lastSensorValue;
    
    // determine whether in spin mode
    bool isSpinMode;
    float lastValue;
    int diffCount;
    
    deque<int> diffList;
    
    float averageOfList(deque<int> list);
    
    ofVideoPlayer spinLevelVid;
    
    bool buttonOneState;
    bool buttonTwoState;
    
};
