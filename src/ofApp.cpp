#include "ofApp.h"


//----------------------   SETUP   --------------------------//


void ofApp::setup() {
    // ofSetFrameRate(1);
    ofSetFullscreen(true);
    
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
    spinDistanceList.assign(10, 0);
    
    // Setup GUI //
    gui.setup(); // most of the time you don't need a name
    gui.setPosition(1500, 10);
    gui.add(minimumEncoderMovement.setup("minimumEncoderMovement", 0, 0, 40));
    gui.add(spinModeThreshold.setup("spinModeThreshold", 0.0, 0.0, 40.0));
    
    // Setup Arduino //
    ard.connect("tty.usbmodem1421", 57600);
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
        vector <string> loopKeys;
        vector <int> loopDelays;
        string stillLoop = "";
        string name = scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/NAME");
        int firstFrame = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/FIRST_FRAME"));
        int endFrame = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/LAST_FRAME"));
        string flag = scheduleOfVideos.getAttribute("GROUP[" + ofToString(i) +"]/NAME[@flag]");
        if(flag == "WT"){
            trigWT_sound_1960 = firstFrame;
        }
        else if(flag == "JUNCO"){
            trigJUNCO_sound_1960 = firstFrame;
        }
        scheduleOfVideos.setTo("GROUP[" + ofToString(i) +"]/LOOPS");
          for(int j=0; j < scheduleOfVideos.getNumChildren(); j++){
            string loopFlag = scheduleOfVideos.getAttribute("FILENAME[" + ofToString(j) +"][@flag]");
            if (loopFlag == "STILL") {
                stillLoop = scheduleOfVideos.getValue("FILENAME[" + ofToString(j)+ "]");
            } else {
                string filen = scheduleOfVideos.getValue("FILENAME[" + ofToString(j)+ "]");
                // add this loop to its vid
                loopKeys.push_back(filen);
                // load the loop if not already loaded
                if (loops.find(filen) == loops.end()) {
                    ofVideoPlayer loop;
                    loop.load("videos/"+filen);
                    loop.setLoopState(OF_LOOP_NONE);
                    loops.insert(std::pair<string,ofVideoPlayer>(filen, loop));
                }
                string delayFlag = scheduleOfVideos.getAttribute("FILENAME[" + ofToString(j) +"][@delay]");
                if (delayFlag == "") delayFlag = 15000;
                loopDelays.push_back(ofToInt(delayFlag));
            }
        }
        scheduleOfVideos.setTo("../../");
        Vid temp(name, firstFrame, endFrame, loopKeys, loopDelays, &loops, stillLoop, "1960");
        vids_1960.push_back(temp);
    }
    
    // Load In 2010s Vids To Custom Vid Class //
    scheduleOfVideos.load("2010_sched.xml");
    scheduleOfVideos.setTo("VIDEOS");
    for (int i =0; i <scheduleOfVideos.getNumChildren(); i++ ) {
        vector <string> loopKeys;
        vector <int> loopDelays;
        string stillLoop = "";
        string name = scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/NAME");
        int firstFrame = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/FIRST_FRAME"));
        int endFrame = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/LAST_FRAME"));
        string flag = scheduleOfVideos.getAttribute("GROUP[" + ofToString(i) +"]/NAME[@flag]");
        if(flag == "WT"){
            trigWT_sound_2010 = firstFrame;
        }
        else if(flag == "JUNCO"){
            trigJUNCO_sound_2010 = firstFrame;
        }
        scheduleOfVideos.setTo("GROUP[" + ofToString(i) +"]/LOOPS");
        for(int j=0; j < scheduleOfVideos.getNumChildren(); j++){
            string loopFlag = scheduleOfVideos.getAttribute("FILENAME[" + ofToString(j) +"][@flag]");
            if (loopFlag == "STILL") {
                stillLoop = scheduleOfVideos.getValue("FILENAME[" + ofToString(j)+ "]");
            } else {
                string filen = scheduleOfVideos.getValue("FILENAME[" + ofToString(j)+ "]");
                // add this loop to its vid
                loopKeys.push_back(filen);
                // load the loop if not already loaded
                if (loops.find(filen) == loops.end()) {
                    ofVideoPlayer loop;
                    loop.load("videos/"+filen);
                    loop.setLoopState(OF_LOOP_NONE);
                    loops.insert(std::pair<string,ofVideoPlayer>(filen, loop));
                }
                string delayFlag = scheduleOfVideos.getAttribute("FILENAME[" + ofToString(j) +"][@delay]");
                if (delayFlag == "") delayFlag = 15000;
                loopDelays.push_back(ofToInt(delayFlag));
            }
        }
        scheduleOfVideos.setTo("../../");
        Vid temp(name, firstFrame, endFrame, loopKeys, loopDelays, &loops, stillLoop, "2010");
        vids_2010.push_back(temp);
    }

    ofDirectory dir;
    dir.listDir("./videos/1960_scrubLevel/");
    //  this will put it in order as long as it has the leading zeros in linux
    dir.sort();
    for(int i =0; i < dir.size(); i++){
        images_1960.push_back(dir.getPath(i));
    }
    
    ofDirectory dir2;
    dir2.listDir("./videos/2010_scrubLevel/");
    //  this will put it in order as long as it has the leading zeros in linux
    dir2.sort();
    for(int i =0; i < dir2.size(); i++){
        images_2010.push_back(dir2.getPath(i));
    }
    

    // Load Sounds //
    ambientSound.load("sounds/ambientForest.mp3");
    ambientSound.setLoop(true); 
    WTSounds.load("sounds/WT_chirp.mp3");
    JuncoSounds.load("sounds/JUNCO_Chirp.mp3");
    transSound.load("sounds/swooshes/swishSound.wav");
    
    setTo2010s();
    
    // this is the time in millis that the white flash happens over
    fadeTime = 1000;
}

void ofApp::setTo1960s() {
    if (years != "1960s") {
        // stop what is currently playing and clear spinner stuff
        if (activeVids != nullptr) {
            activeVids->at(activeVidIndex).stopVideoBlock();
        }
        spinDistanceList.assign(10, 0);
        isSpinMode = true;
    
        activeVids = &vids_1960;
        activeImages = &images_1960;
        trigWT_sound = trigWT_sound_1960;
        trigJUNCO_sound= trigJUNCO_sound_1960;
        years = "1960s";
        isCurrently1960 = true;
    }
}

void ofApp::setTo2010s() {
    if (years != "2010s") {
        // stop what is currently playing and clear spinner stuff
        if (activeVids != nullptr) {
            activeVids->at(activeVidIndex).stopVideoBlock();
        }
        spinDistanceList.assign(10, 0);
        isSpinMode = true;

        activeVids = &vids_2010;
        activeImages = &images_2010;
        trigWT_sound = trigWT_sound_2010;
        trigJUNCO_sound= trigJUNCO_sound_2010;
        years = "2010s";
        isCurrently1960 = false;
    }
}


void ofApp::setupArduino(const int & version) {
    // remove listener because we don't need it anymore
    ofRemoveListener(ard.EInitialized, this, &ofApp::setupArduino);
    // it is now safe to send commands to the Arduino
    bSetupArduino = true;
    // print firmware name and version to the console
    ofLogNotice() << ard.getFirmwareName();
    ofLogNotice() << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion();
    
    // set pin A0 to analog input for the encoder
    ard.sendAnalogPinReporting(0, ARD_ANALOG);
    
    // the buttons state
    ard.sendDigitalPinMode(3, ARD_INPUT);
    ard.sendDigitalPinMode(4, ARD_INPUT);
    ofAddListener(ard.EDigitalPinChanged, this, &ofApp::digitalPinChanged);
    ofAddListener(ard.EAnalogPinChanged, this, &ofApp::analogPinChanged);
    
    // the relay
    ard.sendDigitalPinMode(2, ARD_OUTPUT);
}


//----------------------   UPDATE   --------------------------//


void ofApp::update(){
    
    // See which vid is in range
    for(int i = 0; i < activeVids->size(); i++) {
        if (activeVids->at(i).isInRange(frameShown)) {
             activeVidIndex = i;
        }
    }
    
    // Update Distance Between Spinner and Scrub Level Frame
    spinDistance = getSpinDistance(scrubLevelFrame, spinnerNumber, 3600);
    spinDistanceList.pop_back();
    spinDistanceList.push_front(spinDistance);
    averageSpinDistance = averageOfList(spinDistanceList);
    
    bool nowSpinMode;
    if (averageSpinDistance <= spinModeThreshold){
        nowSpinMode = false;
    }
    else if (abs(averageSpinDistance) > spinModeThreshold){
        if (averageSpinDistance > 0 && (getSpinDistance(spinnerNumber, loopLevelFrame, 3600) > 0)) {
            // we are spinning forward and the loop is ahead of the scrub vid
            nowSpinMode = false;
        } else {
            nowSpinMode = true;
        }
    }
    
    
    
    // Trigger sounds to stop or start, if spin mode changes
    if (nowSpinMode != isSpinMode) {
        if (nowSpinMode) {
            isScrubSoundFadeUp = true;
            isScrubSoundFadeDown = false;
            startSoundFade = ofGetElapsedTimeMillis();
            ambientSound.play();
            ambientSound.setVolume(0);
            ambientSound.setPaused(false);
            scrubLevelFrame = loopLevelFrame;
            activeVids->at(activeVidIndex).stopVideoBlock();
        } else {
            isScrubSoundFadeDown = true;
            isScrubSoundFadeUp = false;
            startSoundFade = ofGetElapsedTimeMillis();
            activeVids->at(activeVidIndex).setupTransition(scrubLevelFrame);
        }
        isSpinMode = nowSpinMode;
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
        int frameAdjustment = abs(averageSpinDistance) > abs(spinDistance) ? spinDistance : averageSpinDistance;
        
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
            if (adjustedSpinnerNumber > scrubLevelFrame){
                adjustedSpinnerNumber -= 3600;
            }
            if (frameShown + frameAdjustment > adjustedSpinnerNumber ) {
                scrubLevelFrame = posMod(int(frameShown + frameAdjustment), 3600);
            } else {
                scrubLevelFrame = posMod(spinnerNumber, 3600);
            }
        }
        // trigger the tweet sounds at the correct moments
        if( checkInRange(adjustedSpinnerNumber, trigWT_sound)  & (WTSounds.isPlaying() == false)){
            WTSounds.play();
        }
        if( checkInRange(adjustedSpinnerNumber, trigJUNCO_sound)  & (JuncoSounds.isPlaying() == false)){
            JuncoSounds.play();
        }
        
    } else if (!isSpinMode) {
        loopLevelFrame = activeVids->at(activeVidIndex).calculateFrameToShow();
    }
}


void ofApp::spinnerChanged(const int newSpinnerNumber){
    if (abs(newSpinnerNumber - spinnerNumber) > minimumEncoderMovement) {
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
        if(isScrubSoundFadeUp){
            int timeChange = ofGetElapsedTimeMillis() - startSoundFade;
            float volume = ofMap(timeChange, 0, fadeTime, 0, 1);
            ambientSound.setVolume(volume);
            if(volume >= 1){
                isScrubSoundFadeUp = false;
            }
        }
    }
    if(isScrubSoundFadeDown){
        int timeChange = ofGetElapsedTimeMillis() - startSoundFade;
        float volume = ofMap(timeChange, 0, fadeTime, 1, 0);
        ambientSound.setVolume(volume);
        if(volume <= 0){
            ambientSound.setPaused(true);
            isScrubSoundFadeDown = false;
        }
    }

    if (showDecorativeFrame) {
      decorativeFrame.draw(0, 0);
    }
  
    vidBuffer.end();
    bezManager.draw();
    if (bShowGui) {
        gui.draw();
    }
    
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(encoderVal, 10, 20);
    ofDrawBitmapString(isSpinMode ? "SPIN MODE" : "LOOP MODE", 200, 20);
    ofDrawBitmapString("Frame # shown: " + ofToString(frameShown), 10, 40);
    ofDrawBitmapString("Spinner #: " + ofToString(spinnerNumber), 200, 40);
    ofDrawBitmapString("Scrub Level Frame #: " + ofToString(scrubLevelFrame), 600, 40);
    ofDrawBitmapString("Loop #: " + ofToString(loopLevelFrame), 800, 40);
    ofDrawBitmapString("Spinner Dist: " + ofToString(spinDistance), 600, 60);
    ofDrawBitmapString("Average Spin Dist" + ofToString(averageSpinDistance), 400, 60);

    ofDrawBitmapString("month = " + ofToString(mons[(int)trunc((frameShown / 3600.0) * 12)]), 10, 80);
    ofDrawBitmapString("day = " + ofToString(((frameShown / 10) % 30) + 1), 150, 80);
    ofDrawBitmapString("years = " + years, 300, 80);
    ofDrawBitmapString(debugInfo, 10, 100);
    
    if(isTransitioning){
        int timeDiff = ofGetElapsedTimeMillis() - startTime;
        int transDur = 1000;
        if(timeDiff < transDur){
            int scaledValue;
            
            if(timeDiff<= transDur/2){
                isMidTrans = true;
                scaledValue = ofMap(timeDiff, 0, transDur/2, 0, 255);
            }
            else{
                if(isMidTrans){
                    // switch in over 
                    isMidTrans = false;
                    if(isCurrently1960){
                        setTo1960s();
                    }
                    else{
                        setTo2010s();
                    }
                }
                scaledValue = ofMap(timeDiff, transDur/2,transDur , 255, 0);
            }
            ofSetColor(255, 255, 255, scaledValue);
            ofDrawRectangle(0, 0, ofGetWidth(),ofGetHeight());
            ofSetColor(255, 255, 255);
        }
        else{
            isTransitioning = false;
        }
        int scaledValue = ofMap(mouseX, 0, ofGetWidth(), 0, 255);
        ofSetColor(255, 255, 255, 20);
        ofDrawRectangle(0, 0, ofGetWidth(),ofGetHeight());
        ofSetColor(255, 255, 255);
    }
    
    
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
        if(!isCurrently1960){
            isCurrently1960 = true;
            startTime = ofGetElapsedTimeMillis();
            isTransitioning = true;
            transSound.play();
        }
    } else if (key == 'b') {
        if (isCurrently1960){
            isCurrently1960 = false;
            startTime = ofGetElapsedTimeMillis();
            isTransitioning = true;
            transSound.play();
        }
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


bool ofApp::checkInRange(int value, int centerOfRange){
    int rgSz = 5;
    if((value < centerOfRange + rgSz) & (value > centerOfRange - rgSz)){
        return true;
    }
    else{
        return false;
    }
}


//----------------------   ARDUINO   --------------------------//


void ofApp::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    float analogValue = ard.getAnalog(pinNum);
    int newSpinnerNumber = int(ofMap(analogValue, 0.0, 735.0, 0.0, 3599.0, true));
    
    // till the encoder is hooked up
    //spinnerChanged(newSpinnerNumber);
    //encoderVal = "analog pin: " + ofToString(pinNum) + " = " + ofToString(newSpinnerNumber);

}


void ofApp::digitalPinChanged(const int & pinNum) {
    // do something with the digital input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    ofLog() << "pin num: " << pinNum << " value: "<< ofToString(ard.getDigital(pinNum));
    if(pinNum == 3){
        buttonOneState = ard.getDigital(pinNum);
        if(!isCurrently1960){
            ard.sendDigital(2, ARD_HIGH);
            isCurrently1960 = true;
            startTime = ofGetElapsedTimeMillis();
            isTransitioning = true;
            transSound.play();
        }
        
    }
    else if(pinNum == 4){
        buttonTwoState = ard.getDigital(pinNum);
        if(isCurrently1960){
            ard.sendDigital(2, ARD_LOW);
            isCurrently1960 = false;
            startTime = ofGetElapsedTimeMillis();
            isTransitioning = true;
            transSound.play();
        }
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
