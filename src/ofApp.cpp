#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){


    setPosition = 0;
    
    ofSetFullscreen(false);
    
    // could be dyanmic, but whatever
    vidWidth = 1920;
    vidHeight = 1080;
    
    chromakey = new ofxChromaKeyShader(vidWidth, vidHeight);
    chromakey1 = new ofxChromaKeyShader(vidWidth, vidHeight);
    
    // maskee
    bg_image.load("bg.jpg");
    
    // GUI
    chromaGui.setDefaultHeight(18);
    chromaGui.setDefaultWidth(250);
    chromaGui.setup(chromakey->paramGp, "chromaSettings.xml");
    chromaGui.loadFromFile("chromaSettings.xml");
    chromaGui.setPosition(0, 0);
    
    
    drawAllVid.allocate(vidWidth, vidHeight);
    
    bezManager.setup(10); //WarpResolution
    bezManager.addFbo(&drawAllVid);
    bezManager.loadSettings();
    
    scheduleOfVideos.load("sched_New.xml");
    scheduleOfVideos.setTo("videos");
    ofLog()<< scheduleOfVideos.getNumChildren();
    
    for(int i =0; i <scheduleOfVideos.getNumChildren(); i++ ){
        vector <string> files;
        scheduleOfVideos.setTo("GROUP[" + ofToString(i) +"]/VIDS");
        for(int j=0; j < scheduleOfVideos.getNumChildren(); j++){
            files.push_back(scheduleOfVideos.getValue("FILENAME[" + ofToString(j)+ "]"));
        }
        scheduleOfVideos.setTo("../../");
        string dateAfter = scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/DATE_AFTER");
        string dateBefore = scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/DATE_BEFORE");
        int loopState = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/LOOP"));
        int groupID = ofToInt(scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/ID"));
        string alphaRange = scheduleOfVideos.getValue("GROUP[" + ofToString(i) +"]/ALPHA");
        Vid temp;
        temp.setup( groupID, files, dateAfter, dateBefore, loopState, alphaRange);
        allVids.push_back(temp);
    }
    
    
    ard.connect("tty.usbmodem1421", 57600);
    // listen for EInitialized notification. this indicates that
    // the arduino is ready to receive commands and it is safe to
    // call setupArduino()
    ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
    
    encoderVal = "nothing yet";
    dateOfYear = "no date yet";
    
    
    lastSensorValue = -20;
    
    isSpinMode = false;
    lastValue =0;
    diffCount = 0;
    diffList.assign(5, 0);
    
    spinLevelVid.load("WoodThrushOnly.mp4");
    spinLevelVid.setLoopState(OF_LOOP_NORMAL);
    
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
    //vidTwo.setPaused(true);
    
    if(allVids.size()>0){
        allVids.at(0).percent=setPosition;
    }
    
    if(!isSpinMode){
        for(int i =0; i <allVids.size(); i++){
            allVids.at(i).update();
        }
    }
    else{
        spinLevelVid.setPosition(setPosition);
        spinLevelVid.update();
    }
    
    ofBackground(0);
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    ofLog()<< ofToString(setPosition);
    
    // update the arduino, get any data or messages.
    ard.update();
    
    // convert knob rotation into day of the year
    if(allVids.size() > 0){
       dateOfYear = allVids.at(0).percentageToMonth(setPosition);
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(isSpinMode){
        ofClear(220,0,0);
    }
    else{
        ofClear(0,220,0);
    }
    

    drawAllVid.begin();
    ofClear(0,0,0,0);
    /*
    for(int i =0; i <allVids.size(); i++){
         allVids.at(i).draw();
     }
    */
    
    //allVids.at(0).video.draw(0,0);
  
    if(!isSpinMode){
    // draw all the currently active videos and PNGs
    for(int i =0; i <allVids.size(); i++){
        if((allVids.at(i).isCurrentlyPlaying)){
            
            if(allVids.at(i).isPNG){
                allVids.at(i).pngImage.draw(0,0);
            }
            else{
                for(int j=0; j < allVids.at(i).currentlyPlayingIndexes.size(); j++){
                    int indx = allVids.at(i).currentlyPlayingIndexes.at(j);
                    chromakey->endMaskClip.set(ofVec2f( allVids.at(i).alphaStartClip, allVids.at(i).alphaEndClip));
                    chromakey->updateChromakeyMask(allVids.at(i).videos.at(indx).getTexture(),
                                                   bg_image.getTexture());
                    chromakey->drawFinalMask(0, 0, vidWidth, vidHeight);
                    //chromakey->fbo_chroma.draw(0,0);
                }
            }
            // if videos are being play co-currently loop through them
        }
      }
    }
    else {
        spinLevelVid.draw(0, 0);
    }
   
    drawAllVid.end();
    bezManager.draw();
    
    
    
    
    ofSetColor(255, 0, 0);
    ofDrawBitmapString(encoderVal, 10, 20);
    ofDrawBitmapString(dateOfYear, 10, 30);
    ofDrawBitmapString(setPosition, 10, 40);
    ofSetColor(255);

    int count =0;
    for(int i =0; i <allVids.size(); i++){
        if((allVids.at(i).isCurrentlyPlaying)){
            ofDrawBitmapString(allVids.at(i).debugInfo, 10 + 200 * count, 50);
            count++;
        }
    }
    
    if(bShowGui) {
        chromaGui.draw();
        drawDebugMasks();
        // draw bg color's reference Rect
        if(bUpdateBgColor) {
            ofPushStyle();
            ofNoFill();
            ofSetLineWidth(3);
            ofSetColor(0);
            ofVec2f bgColorPos = chromakey->bgColorPos.get();
            ofRect(bgColorPos.x + camW/2, bgColorPos.y, chromakey->bgColorSize.get(), chromakey->bgColorSize.get());
            ofDrawBitmapString("bgColor", bgColorPos.x + camW/2, bgColorPos.y - 5);
            ofPopStyle();
        }
    }
   
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
        setPosition -= .05;
        if( setPosition < 0){
            setPosition = 1;
            
            //vidTwo.setPaused(true);
        }
    }
   if(key == 'w'){
        setPosition += .05;
        if( setPosition > 1){
            setPosition =0;
        }
    }
    if (key == 'a'){
        bUpdateBgColor = !bUpdateBgColor;
    }
    if (key == 'g'){
        bShowGui = !bShowGui;
    }
    
}

void ofApp::drawDebugMasks() {

    ofSetColor(255);
    int camW = vidHeight/3;
    int camH = vidWidth/3;
    
    int previewW = camW/2, previewH = camH/2, labelOffset = 10;
    
    chromakey->drawBaseMask(camW + previewW, 0, previewW, previewH);
    ofDrawBitmapStringHighlight("Base mask", camW + previewW, labelOffset, ofColor(0, 125), ofColor::yellowGreen);
    
    chromakey->drawDetailMask(camW + previewW, previewH, previewW, previewH);
    ofDrawBitmapStringHighlight("Detailed mask", camW + previewW, previewH + labelOffset, ofColor(0, 125), ofColor::yellowGreen);
    
    chromakey->drawChromaMask(previewW, camH, previewW, previewH);
    ofDrawBitmapStringHighlight("Chroma mask", previewW, camH + labelOffset, ofColor(0, 125), ofColor::yellowGreen);
				
    drawCheckerboard(camW, camH, previewW, previewH, 5);
    chromakey->drawFinalMask(camW, camH, previewW, previewH);
    ofDrawBitmapStringHighlight("Final mask", camW, camH + labelOffset, ofColor(0, 125), ofColor::yellowGreen);
    
    webcam.draw(camW + previewW, camH, previewW, previewH);
    ofDrawBitmapStringHighlight("RGB image", camW + previewW, camH + labelOffset, ofColor(0, 125), ofColor::yellowGreen);
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
    
    
    if(averageDiff > 3){
        diffCount++;
        isSpinMode = true;
    }
    else if(averageDiff < 2){
        isSpinMode =false;
    }
    
    /*
    if (absDiff > 3){
        diffCount++;
    }
   
    if(diffCount > 3){
        isSpinMode =true;
    }
    else{
        diffCount =0;
        isSpinMode =false;
    }
   */
    
    
    if(abs(result - lastSensorValue) > 3 ){
        setPosition = ofMap(result, 0, 735, 0, 1);
        lastSensorValue = result;
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
void ofApp::keyReleased(int key){

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
