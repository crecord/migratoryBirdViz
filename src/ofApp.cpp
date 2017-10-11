#include "ofApp.h"


//----------------------   SETUP   --------------------------//


void ofApp::setup() {
    ofSetFullscreen(false);
    
    
    // Initialize Frames //
    scrubLevelFrame = 0;
    loopLevelFrame = 0;
    frameShown = 0;
    activeVidIndex = 0;
    
    // Initialize Spinner //
    prevSpinnerNumber = 0;
    spinnerNumber = 0;
    fakeSpinnerNumber = 0;
    isSpinMode = true;
    encoderVal = "nothing yet";
    spinDistanceList.assign(5, 0);

    
    // Setup Arduino //
    ard.connect("tty.usbmodem1411", 57600);
    // listen for EInitialized notification. this indicates that
    // the arduino is ready to receive commands and it is safe to
    // call setupArduino()
    ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
    // to do: set up one to be on for start up.
    buttonOneState = false;
    buttonTwoState = false;
    
    
    // Setup View //
    vidBuffer.allocate(vidWidth, vidHeight);
    bezManager.setup(10); //WarpResolution
    bezManager.addFbo(&vidBuffer);
    bezManager.loadSettings();
    decorativeFrame.load("./Frame.png");
    
    
    // Load In 1960s Vids To Custom Vid Class //
    scheduleOfVideos.load("1960_sched.xml");
    scheduleOfVideos.setTo("VIDEOS");
    for (int i =0; i <scheduleOfVideos.getNumChildren(); i++ ) {
        vector <string> loopFiles;
        vector <int> loopDelays;
        string stillLoop = "";
        string name = scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/NAME");
        int firstFrame = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/FIRST_FRAME"));
        int endFrame = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/LAST_FRAME"));
        string flag = scheduleOfVideos.getAttribute("GROUP[" + ofToString(i) +"]/NAME[@flag]");
        if(flag == "WT"){
            int trigWT_sound_1960 = firstFrame;
        }
        else if(flag == "JUNCO"){
            int trigJUNCO_sound_1960 = firstFrame;
        }
        scheduleOfVideos.setTo("GROUP[" + ofToString(i) +"]/LOOPS");
          for(int j=0; j < scheduleOfVideos.getNumChildren(); j++){
            string loopFlag = scheduleOfVideos.getAttribute("FILENAME[" + ofToString(j) +"][@flag]");
            if (loopFlag == "STILL") {
                stillLoop = scheduleOfVideos.getValue("FILENAME[" + ofToString(j)+ "]");
            } else {
                string delayFlag = scheduleOfVideos.getAttribute("FILENAME[" + ofToString(j) +"][@delay]");
                if (delayFlag == "") delayFlag = 15000;
                loopDelays.push_back(ofToInt(delayFlag));
                loopFiles.push_back(scheduleOfVideos.getValue("FILENAME[" + ofToString(j)+ "]"));
            }
        }
        scheduleOfVideos.setTo("../../");
        Vid temp(name, firstFrame, endFrame, loopFiles, loopDelays, stillLoop, "1960");
        vids_1960.push_back(temp);
    }
    
    // Load In 2010s Vids To Custom Vid Class //
    scheduleOfVideos.load("2010_sched.xml");
    scheduleOfVideos.setTo("VIDEOS");
    for (int i =0; i <scheduleOfVideos.getNumChildren(); i++ ) {
        vector <string> loopFiles;
        vector <int> loopDelays;
        string stillLoop = "";
        string name = scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/NAME");
        int firstFrame = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/FIRST_FRAME"));
        int endFrame = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/LAST_FRAME"));
        string flag = scheduleOfVideos.getAttribute("GROUP[" + ofToString(i) +"]/NAME[@flag]");
        if(flag == "WT"){
            int trigWT_sound_2010 = firstFrame;
        }
        else if(flag == "JUNCO"){
            int trigJUNCO_sound_2010 = firstFrame;
        }
        scheduleOfVideos.setTo("GROUP[" + ofToString(i) +"]/LOOPS");
        for(int j=0; j < scheduleOfVideos.getNumChildren(); j++){
            string loopFlag = scheduleOfVideos.getAttribute("FILENAME[" + ofToString(j) +"][@flag]");
            if (loopFlag == "STILL") {
                stillLoop = scheduleOfVideos.getValue("FILENAME[" + ofToString(j)+ "]");
            } else {
                string delayFlag = scheduleOfVideos.getAttribute("FILENAME[" + ofToString(j) +"][@delay]");
                if (delayFlag == "") delayFlag = 15000;
                loopDelays.push_back(ofToInt(delayFlag));
                loopFiles.push_back(scheduleOfVideos.getValue("FILENAME[" + ofToString(j)+ "]"));
            }
        }
        scheduleOfVideos.setTo("../../");
        Vid temp(name, firstFrame, endFrame, loopFiles, loopDelays, stillLoop, "2010");
        vids_2010.push_back(temp);
    }

    
    // Formatting Scrub Level Image Filenames //
    for (int i = 0; i < 3600; i++){
      ofImage temp;
      string leadingZeros = "";
      if (i+1 < 10) { leadingZeros = "000"; }
      else if (i+1 < 100) { leadingZeros = "00"; }
      else if (i+1 < 1000) { leadingZeros = "0"; }
      string imageURL_1960 = "./videos/1960_scrubLevel/1960_" + leadingZeros + ofToString(i+1) + ".jpg";
      string imageURL_2010 = "./videos/2010_scrubLevel/2010_" + leadingZeros + ofToString(i+1) + ".jpg";
      images_1960.push_back(imageURL_1960);
      images_2010.push_back(imageURL_2010);
    }
  
    
    // Load Sounds //
    ambientSound.load("sounds/ambient.mp3");
    ambientSound.setLoop(true); 
    WTSounds.load("sounds/WT_chirp.mp3");
    JuncoSounds.load("sounds/JUNCO_Chirp.mp3");
    
    
    setTo1960s();
}

void ofApp::setTo1960s() {
    activeVids = &vids_1960;
    activeImages = &images_1960;
    trigWT_sound = trigWT_sound_1960;
    trigJUNCO_sound= trigJUNCO_sound_1960;
    years = "1960s";
}

void ofApp::setTo2010s() {
    // TODO may have to reset what is being spun...
    activeVids = &vids_2010;
    activeImages = &images_2010;
    trigWT_sound = trigWT_sound_2010;
    trigJUNCO_sound= trigJUNCO_sound_2010;
    years = "2010s";
}


void ofApp::setupArduino(const int & version) {
    // remove listener because we don't need it anymore
    ofRemoveListener(ard.EInitialized, this, &ofApp::setupArduino);
    // it is now safe to send commands to the Arduino
    bSetupArduino = true;
    // print firmware name and version to the console
    ofLogNotice() << ard.getFirmwareName();
    ofLogNotice() << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion();
    // set pin A0 to analog input
    ard.sendAnalogPinReporting(0, ARD_ANALOG);
    ard.sendDigitalPinMode(2, ARD_INPUT);
    ard.sendDigitalPinMode(3, ARD_INPUT);
    ofAddListener(ard.EDigitalPinChanged, this, &ofApp::digitalPinChanged);
    ofAddListener(ard.EAnalogPinChanged, this, &ofApp::analogPinChanged);
    ard.sendDigitalPinMode(4, ARD_INPUT);
}


//----------------------   UPDATE   --------------------------//


void ofApp::update(){
    bool rejectSpinMode = false;
    
    // Recalculate which loop section in range
    // If a loop is currently playing, let it finish first...
    for(int i = 0; i < activeVids->size(); i++) {
        if (activeVids->at(i).isInRange(frameShown)) {
            activeVidIndex = i;
            //if (allVids.at(activeVidIndex).isLoopFinished()) {
            //    activeVidIndex = i;
            //} else {
            //    rejectSpinMode = true;
            // }
        }
    }
    
    // Update Distance Between Spinner and Scrub Level Frame
    spinDistance = getSpinDistance(scrubLevelFrame, spinnerNumber, 3600);
    spinDistanceList.pop_back();
    spinDistanceList.push_front(spinDistance);
    averageSpinDistance = averageOfList(spinDistanceList);
    
    if (averageSpinDistance == 0.0){
        if (isSpinMode) {
            // trigger ambient sound to stop
            ambientSound.setPaused(true);
            activeVids->at(activeVidIndex).setupTransition(scrubLevelFrame);
        }
        // stop the spin mode
        isSpinMode = false;
    }
    else if (abs(averageSpinDistance) > 0.0 && !rejectSpinMode){
        if (!isSpinMode) {
            ambientSound.play();
            ambientSound.setPaused(false);
        }
        isSpinMode = true;
    }
    
    // Figure out which frame to show
    calculateFrameToShow();

    // Display frame rate
    ofSetWindowTitle(ofToString(ofGetFrameRate()));

    // update the arduino, get any data or messages.
    ard.update();
}

void ofApp::calculateFrameToShow() {

    if (!isSpinMode) {
        loopLevelFrame = activeVids->at(activeVidIndex).calculateFrameToShow();
    }

    if(isSpinMode){
        // Spinning
        int adjustedSpinnerNumber = spinnerNumber;
        int frameAdjustment = averageSpinDistance;
        
        if (spinDistance >= 0) {
            // go forwards
            if (adjustedSpinnerNumber < scrubLevelFrame) {
                adjustedSpinnerNumber += 3600;
            }
            if (frameShown + frameAdjustment < adjustedSpinnerNumber) {
                scrubLevelFrame = posMod(int(frameShown + frameAdjustment),3600);
            } else {
                scrubLevelFrame = posMod(spinnerNumber, 3600);
            }
        } else {
            // go backwards
            if (adjustedSpinnerNumber > frameShown){
                adjustedSpinnerNumber -= 3600;
            }
            if (frameShown + frameAdjustment > adjustedSpinnerNumber ) {
                scrubLevelFrame = posMod(int(frameShown + frameAdjustment), 3600);
            } else {
                scrubLevelFrame = posMod(spinnerNumber, 3600);
            }
        }
    }
}


void ofApp::spinnerChanged(const int newSpinnerNumber){
    if (abs(newSpinnerNumber - spinnerNumber) > spinnerChangedThreshold) {
        prevSpinnerNumber = spinnerNumber;
        spinnerNumber = newSpinnerNumber;
    }
}


int ofApp::getSpinDistance(int prev, int next, int max){
    int diff = abs(next - prev);
    if(next >= prev){
        if (diff < max/2){
            return diff;
        }
        return diff - max;
    } else {
        if (diff < max/2){
            return -diff;
        }
        return max - diff;
    }
    
}


//----------------------   DRAW   --------------------------//


void ofApp::draw(){
    ofBackground(0);
    ofSetColor(255, 255, 255);

    vidBuffer.begin();
      ofClear(0, 0, 0, 0);
    
    ofImage frameImage;
    if (!isSpinMode) {
        frameImage.load(activeImages->at(loopLevelFrame));
        frameImage.draw(0, 0);
        activeVids->at(activeVidIndex).drawVid();
        frameShown = loopLevelFrame;
    } else {
        frameImage.load(activeImages->at(scrubLevelFrame));
        frameImage.draw(0, 0);
        frameShown = scrubLevelFrame;
    }

    if (showDecorativeFrame) {
      decorativeFrame.draw(0, 0);
    }
  
    vidBuffer.end();
    bezManager.draw();
    
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(encoderVal, 10, 20);
    ofDrawBitmapString(isSpinMode ? "SPIN MODE" : "LOOP MODE", 200, 20);
    ofDrawBitmapString("Frame # shown: " + ofToString(frameShown), 10, 40);
    ofDrawBitmapString("Spinner #: " + ofToString(spinnerNumber), 200, 40);
    ofDrawBitmapString("Spinner Dist: " + ofToString(spinDistance), 600, 40);
    ofDrawBitmapString("Average Diff" + ofToString(averageSpinDistance), 400, 40);

    ofDrawBitmapString("month = " + ofToString(mons[(int)trunc((frameShown / 3600.0) * 12)]), 10, 60);
    ofDrawBitmapString("day = " + ofToString(((frameShown / 10) % 30) + 1), 150, 60);
    ofDrawBitmapString("years = " + years, 300, 60);
    ofDrawBitmapString(debugInfo, 10, 80);
}


void ofApp::drawDebugMasks() {
    ofSetColor(255);
    int camW = vidHeight/3;
    int camH = vidWidth/3;
    int previewW = camW/2, previewH = camH/2, labelOffset = 10;
    drawCheckerboard(camW, camH, previewW, previewH, 5);
}


void ofApp::drawCheckerboard(float x, float y, int width, int height, int size) {
    if (!checkerboardTex.isAllocated()) {
        checkerboardTex.allocate(width, height);
        ofPushStyle();
        checkerboardTex.begin();
        ofClear(255, 255, 255, 255);
        int numWidth = width/size;
        int numHeight = height/size;
        for(int h=0; h<numHeight; h++) {
            for(int w=0; w<numWidth; w++) {
                if ((h+w)%2 == 0) {
                    ofSetColor(ofColor::black);
                    ofDrawRectangle(w*size, h*size, size, size);
                }
            }
        }
        checkerboardTex.end();
        ofPopStyle();
    }
    ofSetColor(255, 255);
    checkerboardTex.draw(x, y);
}


//----------------------   KEY PRESS   --------------------------//


void ofApp::keyPressed(int key){
    
    // send key event
    bezManager.keyPressed(key);
    
    // show / hide guide
    if (key == OF_KEY_RETURN) {
        bezManager.toggleGuideVisible();
    }
    // save settings
    if (key == 's') {
        bezManager.saveSettings();
    }
    // load settings
    if (key == 'l') {
        bezManager.loadSettings();
    }
    if (key == 'v') {
        setTo1960s();
    } else if (key == 'b') {
        setTo2010s();
    }
    if(key == 'q'){
        fakeSpinnerNumber = posMod((fakeSpinnerNumber - 15), 3600);
        spinnerChanged(fakeSpinnerNumber);
    }
    if(key == 'w'){
        fakeSpinnerNumber = posMod((fakeSpinnerNumber + 15), 3600);
        spinnerChanged(fakeSpinnerNumber);
    }
    if (key == 'g'){
        bShowGui = !bShowGui;
    }
    if(key == 'h'){
        ard.sendDigital(4, ARD_HIGH);
    }
    if(key == 'j'){
        ard.sendDigital(4, ARD_LOW);
    }
}

void ofApp::keyReleased(int key){
}


//----------------------   HELPERS   --------------------------//


int ofApp::posMod(int x, int y){
    return ((x % y) + y) % y;
}

float ofApp::averageOfList(deque<int> list){
    if(list.size() == 0) return 0.0;
    
    int sum = 0;
    for(int i=0; i < list.size(); i++) { sum += list.at(i); }
    float average = float(sum) / float(list.size());
    return average;
}


//----------------------   ARDUINO   --------------------------//


void ofApp::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    float analogValue = ard.getAnalog(pinNum);
    int newSpinnerNumber = int(ofMap(analogValue, 0.0, 735.0, 0.0, 3599.0, true));
    spinnerChanged(newSpinnerNumber);
    encoderVal = "analog pin: " + ofToString(pinNum) + " = " + ofToString(newSpinnerNumber);

}


void ofApp::digitalPinChanged(const int & pinNum) {
    // do something with the digital input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    ofLog() << "pin num: " << pinNum << " value: "<< ofToString(ard.getDigital(pinNum));
    if(pinNum == 2){
        buttonOneState = ard.getDigital(pinNum);
    }
    else if(pinNum == 3){
        buttonTwoState = ard.getDigital(pinNum);
    }
}


//----------------------   OTHER   --------------------------//


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    // send drag event
    bezManager.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    // send press event
    bezManager.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
