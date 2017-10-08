#pragma once

#include "ofMain.h"
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
    
        int camW, camH;
        int vidWidth = 1920;
        int vidHeight = 1080;
    
        string debugInfo;
    
        string mons[12] = {
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
    
        int fakeSpinnerNumber;
        int posMod(int x, int y);
    
        void spinnerChanged(const int newSpinnerNumber);
        int spinDistance;
    int frameDistance;
        int spinnerNumber;
    int prevSpinnerNumber;
        int loopFrameNumber;
        int frameNumberShown;
        int frameNumberToShow;
        void calculateFrameToShow();
        int getSpinDistance(int prev, int next, int max);
        ofImage frameToShow;
    
        // load in the schedule
        ofXml scheduleOfVideos;
    
        vector<Vid> allVids;
        vector<string> fullScene_1960;
    
        bool showDecorativeFrame = true;
        ofImage decorativeFrame; // The branches graphic
    
        ofFbo vidBuffer;
        
        // === Arduino =============================
    
        ofArduino ard;
        bool bSetupArduino;
        void setupArduino(const int & version);
        void analogPinChanged(const int & pinNum);
        void digitalPinChanged(const int & pinNum);
        void updateArduino();
    
        bool buttonOneState;
        bool buttonTwoState;
    
    
        // === Sounds  =============================
    
        ofSoundPlayer ambientSound;
        ofSoundPlayer WTSounds;
        ofSoundPlayer JuncoSounds;
    
        int trigWT_sound_1960;
        int trigJUNCO_sound_1960;
    
        int trigWT_sound_2010;
        int trigJUNCO_sound_2010;
    
    
        // === Is Spinning?  =============================
    
        bool isSpinMode;

        string encoderVal;
        int lastSensorValue;
    
        deque<int> diffList;
        deque<int> spinDiffList;
    
        float averageDiff;
        float averageSpinDiff;
    
        void updateDiffQueue(int value);
        void updateSpinDiffQueue(int value);
    
        //helper
        float averageOfList(deque<int> list);


        // === Debugging =============================
    
        ofFbo checkerboardTex;
        bool bShowGui;
        bool showGuide;
        ofxBezierWarpManager bezManager;
    
        void drawDebugMasks();
        void drawCheckerboard(float x, float y, int width, int height, int size);

};
