//
//  vids.cpp
//  manyVideos
//
//  Created by Caroline Record on 8/22/17.
//
//

#include "Vid.h"

string Vid::mons[12] = { "january","february","march", "april", "may", "june", "july", "august", "september","october","november","december" };
float Vid::percent =0;
bool Vid::isAfter = false;

void Vid::setup( int groupID, vector <string> filename, string dateAfter, string dateBefore, int isLoop, string alphaRange){
    
    vector<string> nums = ofSplitString(alphaRange, ",");
    alphaStartClip = ofToFloat(nums.at(0));
    alphaEndClip = ofToFloat(nums.at(1));
    
    isPNG = false;
    debugInfo = "group ID: " + ofToString(groupID) + "\n date range: " + dateBefore + "\n loop state: " + ofToString(isLoop) + "\n";
    
    for(int i =0; i< filename.size(); i++ ){
        
        vector<string> components = ofSplitString(filename.at(i), ".");
        
        if(components.size() > 1 ){
            if(components.at(1) == "png"){
                isPNG = true;
                pngImage.load("videos/"+ filename.at(i));
            }
        }
        else{
            ofVideoPlayer temp;
            string filen = "videos/"+ filename.at(i) +".mp4";
            //ofLog() << filen;
            temp.load(filen);
            videos.push_back(temp);
        }
    }
    
    //ofLog("loaded the videos. Size is: " + );
    
    // is the time range straddling the time seam for the 1960s
    isSeamBefore = false;
    // is the time range straddling the time seam for the 2000s
    isSeamAfter = false;
    
    vector<string> datePiecesBefore = ofSplitString( dateBefore , "-");
    if (datePiecesBefore.size() > 0){
        startPercentBefore = convertDate(datePiecesBefore.at(0));
        endPercentBefore = convertDate(datePiecesBefore.at(1));
        // the time range is sitting on the rift
        if(startPercentBefore > endPercentBefore){
            isSeamBefore = true;
        }
        debugInfo += "start: " + ofToString(startPercentBefore) + " end: " + ofToString(endPercentBefore);
    }
    else{
       // ofLog()<< "problem with parsing the format. Check before date on "+ fileN;
    }
    
    
    
    
    vector<string> datePiecesAfter = ofSplitString( dateAfter , "-");
    if (datePiecesAfter.size() > 0){
        startPercentAfter = convertDate(datePiecesAfter.at(0));
        endPercentAfter = convertDate(datePiecesAfter.at(1));
        //ofLog() << "start: " + ofToString(startPercentAfter) + " end: " + ofToString(endPercentAfter);
        if(startPercentAfter > endPercentAfter){
            isSeamAfter = true;
        }
    }
    else{
        //ofLog()<< "problem with parsing the format. Check After date on "+ fileN;
    }
    
    debugInfo += "\n is Seam: " + ofToString(isSeamAfter);

    loopingState = isLoop;
    
    isCurrentlyPlaying = false;
    
    currentlyPlayingIndex = -1;
    ofLog()<< debugInfo;
}

void Vid::update(){
    
    // currently looking at the 2000s
    if (isAfter){
        if(isSeamAfter){
            if((percent >= startPercentAfter) | (percent <= endPercentAfter)){
                if (!isCurrentlyPlaying){
                    // set up anything that needs to happen
                    isCurrentlyPlaying = true;
                    setupVideoBlock();
                }
                updateVideoBlock();
            }
            else{
                if(isCurrentlyPlaying){
                    isCurrentlyPlaying = false;
                    stopVideoBlock();
                }
            }
        }
        else{
            if ((percent >= startPercentAfter)&(percent <= endPercentAfter)){
                if (!isCurrentlyPlaying){
                    // set up anything that needs to happen
                    isCurrentlyPlaying = true;
                    setupVideoBlock();
                }
                updateVideoBlock();
            }
            else{
                if(isCurrentlyPlaying){
                    isCurrentlyPlaying = false;
                    stopVideoBlock();
                }
            }
        }
    }
    
    // looking at the 1960s
    else{
        
        // located on the seam of years
        if(isSeamBefore){
            if((percent >= startPercentBefore) | (percent <= endPercentBefore)){
                if (!isCurrentlyPlaying){
                    // set up anything that needs to happen
                    isCurrentlyPlaying = true;
                    setupVideoBlock();

                }
                updateVideoBlock();
            }
            else{
                if(isCurrentlyPlaying){
                    isCurrentlyPlaying = false;
                    stopVideoBlock();
                }
            }
        }
        else{
        if ((percent >= startPercentBefore)&(percent <= endPercentBefore)){
            if (!isCurrentlyPlaying){
                // set up anything that needs to happen
                isCurrentlyPlaying = true;
                setupVideoBlock();
                
            }
            updateVideoBlock();
        }
        else{
            if(isCurrentlyPlaying){
                isCurrentlyPlaying = false;
                stopVideoBlock();
            }
        }
        }
    }
}


/*
 0 - static, non looping use the position to dictate where in the video we are
 1 - Looping a video none at once and doing it evenly - can work for multiple or one
 2 - looping with a default video but other ones playing occasionally
 3 - looping but only occasionally- not too many at once.
*/

void Vid::setupVideoBlock(){
    if(!isPNG){
        // for now let's treat these the same
        if((loopingState == 0) | (loopingState == 1) | (loopingState == 2  )){
            currentlyPlayingIndex = 0;
            if(videos.size() == 1){
                videos.at(currentlyPlayingIndex).setPosition(0);
                videos.at(currentlyPlayingIndex).setLoopState(OF_LOOP_NORMAL);
                videos.at(currentlyPlayingIndex).play();
                videos.at(currentlyPlayingIndex).update();
            }
            else{
                
                videos.at(currentlyPlayingIndex).setLoopState(OF_LOOP_NONE);
                videos.at(currentlyPlayingIndex).play();
                videos.at(currentlyPlayingIndex).update();
            }
            currentlyPlayingIndexes.push_back(currentlyPlayingIndex);
            
        }
        // assuming more than one
        else if(loopingState == 3 ){
            
            isWait1 = false;
            isWait2 = false;
            // start two random ones
            randomIndex1 = chooseIndex(300);
            videos.at(randomIndex1).setPosition(0);
            videos.at(randomIndex1).setLoopState(OF_LOOP_NONE);
            videos.at(randomIndex1).play();
            videos.at(randomIndex1).update();
            
            randomIndex2 = chooseIndex(randomIndex1);
            videos.at(randomIndex2).setPosition(0);
            videos.at(randomIndex2).setLoopState(OF_LOOP_NONE);
            videos.at(randomIndex2).play();
            videos.at(randomIndex2).update();
            
            
        }
        else{
            ofLog()<< "loop state is out of range";
        }
    }
}

void Vid::updateVideoBlock(){
    if(!isPNG){
        
        // The first clip should play once and then we loop the default video.
        if(loopingState == 0 ){
            if((videos.size() == 1) | (currentlyPlayingIndex != 0)){
                videos.at(currentlyPlayingIndex).update();
            }
            else{
                if(videos.at(currentlyPlayingIndex).getIsMovieDone()){
                    if(currentlyPlayingIndex <  videos.size()-1){
                        currentlyPlayingIndex ++;
                    }
                    currentlyPlayingIndexes.clear();
                    currentlyPlayingIndexes.push_back(currentlyPlayingIndex);
                    videos.at(currentlyPlayingIndex).setLoopState(OF_LOOP_NORMAL);
                    videos.at(currentlyPlayingIndex).setPosition(0);
                    videos.at(currentlyPlayingIndex).play();
                }
                videos.at(currentlyPlayingIndex).update();
            }
        }
    if(loopingState == 1){
        // just loop it if it is that one video
        if(videos.size() == 1){
            videos.at(currentlyPlayingIndex).update();
        }
        else{
            if(videos.at(currentlyPlayingIndex).getIsMovieDone()){
                if(currentlyPlayingIndex <  videos.size()-1){
                    currentlyPlayingIndex ++;
                }
                else{
                    currentlyPlayingIndex= 0;
                }
                currentlyPlayingIndexes.clear();
                currentlyPlayingIndexes.push_back(currentlyPlayingIndex);
                videos.at(currentlyPlayingIndex).setLoopState(OF_LOOP_NONE);
                videos.at(currentlyPlayingIndex).setPosition(0);
                videos.at(currentlyPlayingIndex).play();
            }
            videos.at(currentlyPlayingIndex).update();
        }
    }
    else if(loopingState == 2 ){
            if(videos.at(currentlyPlayingIndex).getIsMovieDone()){
                float range = ofRandom(10);
                // .5 chance it will just stay on the default
                // else it just stays on the default
                if( range > 5){
                    // all good just start on the same one
                    currentlyPlayingIndex = ofMap(range, 5, 10, .5, videos.size()-.52);
                }
                currentlyPlayingIndexes.clear();
                currentlyPlayingIndexes.push_back(currentlyPlayingIndex);
                videos.at(currentlyPlayingIndex).setLoopState(OF_LOOP_NONE);
                videos.at(currentlyPlayingIndex).setPosition(0);
                videos.at(currentlyPlayingIndex).play();
            }
            videos.at(currentlyPlayingIndex).update();
    }
    
    else if(loopingState == 3 ){
        
        currentlyPlayingIndexes.clear();
        
        if((videos.at(randomIndex1).getIsMovieDone()) & !isWait1){
            
            // remove the index from the list.
            // Make the wait time
            // set it to wait
            isWait1 = true;
            waitTime1 = ofRandom(5000);
            startTime1 = ofGetElapsedTimeMillis();
        }
        
        else if((videos.at(randomIndex1).getIsMovieDone()) & isWait1){
            if ((ofGetElapsedTimeMillis() - startTime1) >= waitTime1){
                randomIndex1 = chooseIndex(randomIndex2);
                videos.at(randomIndex1).setPosition(0);
                videos.at(randomIndex1).setLoopState(OF_LOOP_NONE);
                videos.at(randomIndex1).play();
                videos.at(randomIndex1).update();
                isWait1 = false;
                currentlyPlayingIndexes.push_back(randomIndex1);
            }
        }
        else{
            videos.at(randomIndex1).update();
            currentlyPlayingIndexes.push_back(randomIndex1);
        }
        
        // second video
        if((videos.at(randomIndex2).getIsMovieDone()) & !isWait2){
            // Make the wait time
            // set it to wait
            isWait2 = true;
            waitTime2 = ofRandom(5000);
            startTime2 = ofGetElapsedTimeMillis();
            
            
        }
        else if((videos.at(randomIndex2).getIsMovieDone()) & isWait2){
            if ((ofGetElapsedTimeMillis() - startTime2) >= waitTime2){
                randomIndex2 = chooseIndex(randomIndex1);
                videos.at(randomIndex2).setPosition(0);
                videos.at(randomIndex2).setLoopState(OF_LOOP_NONE);
                videos.at(randomIndex2).play();
                videos.at(randomIndex2).update();
                currentlyPlayingIndexes.push_back(randomIndex2);
                isWait1 = false;
            }
        }
        else{
            videos.at(randomIndex2).update();
            currentlyPlayingIndexes.push_back(randomIndex2);
        }
        
    }
    }
}


// a touch of recursion to make sure it doesn't overlap. 
int Vid::chooseIndex(int lastIndex){
    int randIndex = round(ofRandom(0,videos.size()-.52));
    if (randIndex == lastIndex){
        chooseIndex(lastIndex);
    }
    return randIndex;
}


void Vid::draw(){

    
}




void Vid::stopVideoBlock(){
    for(int i =0; i< videos.size(); i++){
        videos.at(i).stop(); 
    }
}

void Vid::drawVideoBlock(){
    
}

float Vid::convertDate (string date){
    
    vector<string> datePieces = ofSplitString(date," ");
    if (datePieces.size() > 0){
        string month = datePieces.at(0);
        // convert to lower case in case that mistake is made
        transform( month.begin(), month.end(), month.begin(), ptr_fun <int, int> ( tolower ) );
        int monthToDays = -1;
        
        // find the matching month
        for(int i=0; i < 12; i++){
            if (mons[i] == month){
                monthToDays = i;
            }
        }
        // ofLog()<< monthToDays;
        if(monthToDays >= 0 ){
            
            float dayOfMonth = ofToFloat(datePieces.at(1));
            // no more re-mapping - keeping it simpler.
            /*
            if ((monthToDays ==0) | (monthToDays ==2)| (monthToDays ==4) |(monthToDays ==6)|(monthToDays ==7)|(monthToDays ==9)|(monthToDays ==11)){
                //ofLog() << "month with 31 days- I am going to remap it.";
                dayOfMonth = ofMap(dayOfMonth, 1,31,1,30);
            }
            else if (monthToDays == 1){
                //ofLog() << "it is feb. Need to remap it";
                dayOfMonth = ofMap(dayOfMonth, 1,28,1,30);
            }
             */
          
            monthToDays *= 30;
            float totalDays = monthToDays + dayOfMonth;
            float perc = ofMap(totalDays, 1, 360, 0, 1);
            //ofLog() << perc;
            return perc;
        }
        else {
            ofLog() << "The month didn't match up with the months listed. Check spelling on " +fileN ;
            return 0.0;
        }
    }
    else{
        ofLog() << "There weren't two words on date. Check syntax on " +fileN ;
        return 0.0;
    }
}

string Vid::percentageToMonth (float perc){
    string date;
    // find the month
    float scaledToDays = ofMap(perc, 0, 1, 1, 360);
    int monthIndex = int(scaledToDays/30);
    monthIndex = int(ofClamp(monthIndex, 0, 11));
    date = mons[monthIndex];
    // find the day
    float remainder = scaledToDays - monthIndex*30;
    
    // keeping it simpler
    /*
    if ((monthIndex ==0) | (monthIndex ==2)| (monthIndex ==4) |(monthIndex ==6)|(monthIndex ==7)|(monthIndex ==9)|(monthIndex ==11)){
        //ofLog() << "month with 31 days- I am going to remap it.";
        remainder = ofMap(remainder, 1,30,1,31);
    }
    else if (monthIndex == 1){
        //ofLog() << "it is feb. Need to remap it";
        remainder = ofMap(remainder, 1,30,1,28);
    }
    */
    
    int theDay = round(remainder);
    date += " "+ ofToString(theDay);
    return date;
}
