#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    frameShown = 0;
  
    ofSetFullscreen(false);

    vidBuffer.allocate(vidWidth, vidHeight);
    
    bezManager.setup(10); //WarpResolution
    bezManager.addFbo(&vidBuffer);
    bezManager.loadSettings();
    
    scheduleOfVideos.load("sched.xml");
    scheduleOfVideos.setTo("videos");
  
    for(int i =0; i <scheduleOfVideos.getNumChildren(); i++ ){
        vector <string> loopFiles;
      
        string name = scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/NAME");
        int firstFrame_1960 = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/FIRST_FRAME_1960"));
        int lastFrame_1960 = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/LAST_FRAME_1960"));
      
        scheduleOfVideos.setTo("GROUP[" + ofToString(i) +"]/LOOPS");
          for(int j=0; j < scheduleOfVideos.getNumChildren(); j++){
            loopFiles.push_back(scheduleOfVideos.getValue("FILENAME[" + ofToString(j)+ "]"));
        }
        scheduleOfVideos.setTo("../../");
      
        Vid temp;
        temp.setup( name, firstFrame_1960, lastFrame_1960, loopFiles);
        allVids.push_back(temp);
    }
  
    // formatting 1960 scrub level image filenames
    for (int i = 0; i < 3600; i++){
      ofImage temp;
      string leadingZeros = "";
      if (i < 10) {
        leadingZeros = "000";
      } else if (i < 100) {
        leadingZeros = "00";
      } else if (i < 1000) {
        leadingZeros = "0";
      }
      string imageURL = "./videos/1960_scrubLevel/1960_" + leadingZeros + ofToString(i+1) + ".jpg";
      fullScene_1960.push_back(imageURL);
    }
  
    ard.connect("tty.usbmodem1421", 57600);
    // listen for EInitialized notification. this indicates that
    // the arduino is ready to receive commands and it is safe to
    // call setupArduino()
    ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
    
    encoderVal = "nothing yet";
  
    lastSensorValue = -20;
    
    decorativeFrame.load("./Frame.png");
    
    isSpinMode = false;
    lastValue = 0;
    diffCount = 0;
    diffList.assign(5, 0);
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

    ofAddListener(ard.EAnalogPinChanged, this, &ofApp::analogPinChanged);
}

//--------------------------------------------------------------
void ofApp::update(){
    if(!isSpinMode){
      diffList.push_front(0);
      diffList.pop_back();
      allVids.at(0).frame = frameShown;
      for(int i =0; i < allVids.size(); i++){
          allVids.at(i).update();
      }
    }
  
    ofSetWindowTitle(ofToString(ofGetFrameRate()));

    // update the arduino, get any data or messages.
    ard.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofSetColor(255, 255, 255);
  
  
    //if(!isSpinMode) {
    //  ofClear(255, 0, 0);
    //} else {
    //  ofClear(0, 255, 0);
    //}

  
    vidBuffer.begin();
    ofClear(0, 0, 0, 0);
  
    string debugInfo = "";
    if(!isSpinMode){
      // draw the currently active loop
      for(int i =0; i < allVids.size(); i++){
        if((allVids.at(i).isCurrentlyPlaying)){
            allVids.at(i).draw();
            debugInfo = allVids.at(i).debugInfo;
        }
      }
    }
    else {
      ofImage temp;
      temp.load(fullScene_1960.at(frameShown));
      temp.draw(0, 0);
    }
    
    if (showDecorativeFrame) {
        decorativeFrame.draw(0, 0);
    }
  
    vidBuffer.end();
    bezManager.draw();
    
    
    ofSetColor(0, 0, 255);
    ofDrawBitmapString(encoderVal, 10, 20);
    ofDrawBitmapString(frameShown, 10, 40);
    ofDrawBitmapString("month = " + ofToString(mons[(int)trunc((frameShown / 3600.0) * 12)]), 10, 60);
    ofDrawBitmapString("day = FIGURE OUT MATH " + ofToString(frameShown % 30), 150, 60);
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
        isSpinMode = true;
        frameShown -= 5;
        if(frameShown < 0){
            frameShown = 3599;
        }
    }
   if(key == 'w'){
        isSpinMode = true;
        frameShown += 5;
        if(frameShown >= 3600){
         frameShown = 0;
        }
    }
    if (key == 'g'){
        bShowGui = !bShowGui;
    }
}

void ofApp::keyReleased(int key){
   if(key == 'q'){
    isSpinMode = false;
  }
  if(key == 'w'){
    isSpinMode = false;
  } if (key == 'p') {
    isSpinMode = !isSpinMode;
  }
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

void ofApp::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    
    int result = ard.getAnalog(pinNum);
   
    int absDiff = abs(result - lastSensorValue);
    diffList.push_front(absDiff);
    diffList.pop_back();
    float averageDiff = averageOfList(diffList);
    
    
    if(averageDiff > 1){
        // In update, I added 0s to diffList if spin mode = false (not moving)
        // seems to be working well and we can go a little slower
        diffCount++;
        isSpinMode = true;
        frameShown = int(ofMap(result, 0, 735, 0, 3599, true));
        lastSensorValue = result;
    }
    else if(averageDiff < 1){
      isSpinMode = false;
    }

    
    encoderVal = "analog pin: " + ofToString(pinNum) + " = " + ofToString(result);
}


float ofApp::averageOfList(deque<int> list){
    int sum = 0;
    
    for(int i=0; i < list.size(); i++){
        sum += list.at(i);
    }
    float average = sum / list.size();
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
