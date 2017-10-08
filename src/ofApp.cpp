#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

    spinnerNumber = 0;
    frameNumberToShow = 0;
    frameNumberShown = 0;
    fakeSpinnerNumber = 0;
    loopFrameNumber = -1;
  
    ofSetFullscreen(false);
    
    vidBuffer.allocate(vidWidth, vidHeight);
    
    bezManager.setup(10); //WarpResolution
    bezManager.addFbo(&vidBuffer);
    bezManager.loadSettings();
    
    scheduleOfVideos.load("sched.xml");
    scheduleOfVideos.setTo("videos");
    for (int i =0; i <scheduleOfVideos.getNumChildren(); i++ ) {
        vector <string> loopFiles;
        string name = scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/NAME");
        int firstFrame_1960 = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/FIRST_FRAME_1960"));
        int endFrame_1960 = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/LAST_FRAME_1960"));
        int firstFrame_2010 = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/FIRST_FRAME_2010"));
        int endFrame_2010 = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/LAST_FRAME_2010"));
        string flag = scheduleOfVideos.getAttribute("GROUP[" + ofToString(i) +"]/NAME[@flag]");
        if(flag == "WT"){
            int trigWT_sound_1960 = firstFrame_1960;
            int trigWT_sound_2010 = firstFrame_2010;
        }
        else if(flag == "JUNCO"){
            int trigJUNCO_sound_1960 = firstFrame_1960;
            int trigJUNCO_sound_2010 = firstFrame_2010;
        }
        scheduleOfVideos.setTo("GROUP[" + ofToString(i) +"]/LOOPS");
          for(int j=0; j < scheduleOfVideos.getNumChildren(); j++){
            loopFiles.push_back(scheduleOfVideos.getValue("FILENAME[" + ofToString(j)+ "]"));
        }
        scheduleOfVideos.setTo("../../");
        Vid temp(name, firstFrame_1960, endFrame_1960, firstFrame_2010, endFrame_2010, loopFiles);
        allVids.push_back(temp);
    }
    
    // hey Sam this should work just as well as long as long as the files are labeled
    // with the correct number of leading zeros in linux
  
    // formatting 1960 scrub level image filenames
    for (int i = 0; i < 3600; i++){
      ofImage temp;
      string leadingZeros = "";
      if (i+1 < 10) { leadingZeros = "000"; }
      else if (i+1 < 100) { leadingZeros = "00"; }
      else if (i+1 < 1000) { leadingZeros = "0"; }
      string imageURL = "./videos/1960_scrubLevel/1960_" + leadingZeros + ofToString(i+1) + ".jpg";
      fullScene_1960.push_back(imageURL);
    }
  
    decorativeFrame.load("./Frame.png");
    
    // VARIABLES FOR SENSING SPINNING //
    isSpinMode = true;
    encoderVal = "nothing yet";
    lastSensorValue = 0;
    diffList.assign(9, 0);
    
    // LOAD IN SOUNDS //
    ambientSound.load("sounds/ambient.mp3");
    ambientSound.setLoop(true); 
    WTSounds.load("sounds/WT_chirp.mp3");
    JuncoSounds.load("sounds/JUNCO_Chirp.mp3");
    
    // ARDUINO
    ard.connect("tty.usbmodem1411", 57600);
    // listen for EInitialized notification. this indicates that
    // the arduino is ready to receive commands and it is safe to
    // call setupArduino()
    ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
    // to do: set up one to be on for start up.
    buttonOneState = false;
    buttonTwoState = false;
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

//--------------------------------------------------------------
void ofApp::update(){
    
    spinDistance = getSpinDistance(frameNumberShown, spinnerNumber, 3600);
    updateDiffQueue(spinDistance);
    
    if (averageDiff == 0.0 && isSpinMode){
        // stop the spin mode
        // trigger ambient sound to stop
        ambientSound.setPaused(true);
        isSpinMode = false;
    }
    else if (abs(averageDiff) > 0.0 && !isSpinMode){
        ambientSound.play();
        ambientSound.setPaused(false);
        isSpinMode = true;
        // frameNumberShown = loopFrameNumber;
    }
    
    // Figure out which frame to show
    calculateFrameToShow();

    // Display frame rate
    ofSetWindowTitle(ofToString(ofGetFrameRate()));

    // update the arduino, get any data or messages.
    ard.update();
}

void ofApp::calculateFrameToShow() {
    if(isSpinMode){
        // Spinning
        int adjustedSpinnerNumber = spinnerNumber;
        int frameAdjustment = averageDiff;
        if (averageDiff < 50 && averageDiff >= 0) {
            frameAdjustment = 1;
        } if (averageDiff > -50 && averageDiff < 0) {
            frameAdjustment = -1;
        }
        if (spinDistance >= 0) {
            // go forwards
            if (adjustedSpinnerNumber < frameNumberShown){
                adjustedSpinnerNumber += 3600;
            }
            if (frameNumberShown + frameAdjustment < adjustedSpinnerNumber) {
                frameNumberToShow = posMod(int(frameNumberShown + frameAdjustment),3600);
            } else {
                frameNumberToShow = posMod(spinnerNumber, 3600);
            }
            
            // see if you should let the spin catch the loop frame still
            if (frameNumberToShow < loopFrameNumber) {
                frameNumberToShow = loopFrameNumber;
            } else if (loopFrameNumber != -1) {
                loopFrameNumber = -1;
            }
        } else {
            // go backwards
            
            if (adjustedSpinnerNumber > frameNumberShown){
                adjustedSpinnerNumber -= 3600;
            }
            if (frameNumberShown - frameAdjustment > adjustedSpinnerNumber ) {
                frameNumberToShow = posMod(int(frameNumberShown - frameAdjustment), 3600);
            } else {
                frameNumberToShow = posMod(spinnerNumber, 3600);
            }
            
            // turn loop off
            if (loopFrameNumber != -1) {
                loopFrameNumber = -1;
                frameNumberShown = loopFrameNumber;
            }
        }
        
        // loopFrameNumber = -1;
    } else {
        // If In Loop Mode
        // Tell vids what frame is currently shown
        // And see which is active
        for(int i = 0; i < allVids.size(); i++) {
            allVids.at(i).update(frameNumberShown);
            if (allVids.at(i).isCurrentlyPlaying) {
                debugInfo = allVids.at(i).debugInfo_;
                loopFrameNumber = allVids.at(i).calculateFrameToShow();
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofSetColor(255, 255, 255);

    vidBuffer.begin();
      ofClear(0, 0, 0, 0);
    
    if (loopFrameNumber >= 0 && loopFrameNumber < 3600) {
        ofImage loopFrameToShow;
        loopFrameToShow.load(fullScene_1960.at(loopFrameNumber));
        loopFrameToShow.draw(0, 0);
        for(int i = 0; i < allVids.size(); i++) {
            if (allVids.at(i).isCurrentlyPlaying) {
                allVids.at(i).drawVid();
            }
        }
    } else {
        frameToShow.load(fullScene_1960.at(frameNumberToShow));
        frameToShow.draw(0, 0);
        frameNumberShown = frameNumberToShow;
    }

    if (showDecorativeFrame) {
      decorativeFrame.draw(0, 0);
    }
  
    vidBuffer.end();
    bezManager.draw();
    
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(encoderVal, 10, 20);
    ofDrawBitmapString(isSpinMode ? "SPIN MODE" : "LOOP MODE", 200, 20);
    ofDrawBitmapString("Frame # shown: " + ofToString(frameNumberShown), 10, 40);
    ofDrawBitmapString("Spinner #: " + ofToString(spinnerNumber), 200, 40);
    ofDrawBitmapString("Average Diff" + ofToString(averageDiff), 400, 40);

    ofDrawBitmapString("month = " + ofToString(mons[(int)trunc((frameNumberShown / 3600.0) * 12)]), 10, 60);
    ofDrawBitmapString("year = 1960", 300, 60);
    ofDrawBitmapString(debugInfo, 10, 80);
}


//--------------------------------------------------------------
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
    if(key == 'q'){
        fakeSpinnerNumber = posMod((fakeSpinnerNumber - 25), 3600);
        spinnerChanged(fakeSpinnerNumber);
    }
    if(key == 'w'){
        fakeSpinnerNumber = posMod((fakeSpinnerNumber + 25), 3600);
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

void ofApp::drawDebugMasks() {
    ofSetColor(255);
    int camW = vidHeight/3;
    int camH = vidWidth/3;
    int previewW = camW/2, previewH = camH/2, labelOffset = 10;
    drawCheckerboard(camW, camH, previewW, previewH, 5);    
}

//--------------------------------------------------------------
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

int ofApp::posMod(int x, int y){
    return ((x % y) + y) % y;
}

void ofApp::updateDiffQueue(int value) {
    diffList.push_front(abs(value));
    averageDiff = averageOfList(diffList);
    diffList.pop_back();
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

void ofApp::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    float analogValue = ard.getAnalog(pinNum);
    int newSpinnerNumber = int(ofMap(analogValue, 0.0, 735.0, 0.0, 3599.0, true));
    spinnerChanged(newSpinnerNumber);
    encoderVal = "analog pin: " + ofToString(pinNum) + " = " + ofToString(newSpinnerNumber);

}

void ofApp::spinnerChanged(const int newSpinnerNumber){
    int threshold = 20;
    if (abs(newSpinnerNumber - spinnerNumber) > threshold) {
        spinnerNumber = newSpinnerNumber;
        encoderVal = ", spin dist = " + ofToString(spinDistance);
    }
   
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

float ofApp::averageOfList(deque<int> list){
    if(list.size() == 0) return 0.0;

    int sum = 0;
    for(int i=0; i < list.size(); i++) { sum += list.at(i); }
    float average = float(sum) / float(list.size());
    return average;
}

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
