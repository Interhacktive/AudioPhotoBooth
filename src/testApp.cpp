#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	canon.start();
	canon.addPictureTakenListener(this, &testApp::onPictureTaken);
    
    snapCounter = 0;
	bSnapshot = false;
	cooper.loadFont("verdana.ttf", 10, true, true);
    big.loadFont("verdana.ttf", 50, true, true);
    
	phase = 0;
	memset(snapString, 0, 255);		// clear the string by setting all chars to 0

    ofBackground(0);
    
     
     ofSetVerticalSync(true);
     ofSetCircleResolution(80);
     ofBackground(54, 54, 54);	
     
     // 0 output channels, 
     // 2 input channels
     // 44100 samples per second
     // 256 samples per buffer
     // 4 num buffers (latency)
     
     soundStream.listDevices();
     
     //if you want to set a different device id 
     //soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
     
     int bufferSize = 256;
     
     
     left.assign(bufferSize, 0.0);
     right.assign(bufferSize, 0.0);
     volHistory.assign(400, 0.0);
     
     bufferCounter	= 0;
     drawCounter		= 0;
     smoothedVol     = 0.0;
     scaledVol		= 0.0;
     
     soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
     

}
void testApp::onPictureTaken(roxlu::CanonPictureEvent& ev) {
	cout << ev.getFilePath() << endl;
}
//--------------------------------------------------------------
void testApp::update(){
	if(!canon.isLiveViewActive() && canon.isSessionOpen()) {
		canon.startLiveView();
        
	}
    
    
     //lets scale the vol up to a 0-1 range 
     scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
     
     //lets record the volume into an array
     volHistory.push_back( scaledVol );
     
     //if we are bigger the the size we want to record - lets drop the oldest value
     if( volHistory.size() >= 400 ){
         volHistory.erase(volHistory.begin(), volHistory.begin()+1);
     }
     
     
}

//--------------------------------------------------------------
void testApp::draw(){
	canon.drawLiveView();
    
    
    if (bSnapshot == true){
		// grab a rectangle at 200,200, width and height of 300,180
		img.grabScreen(0,0,1056,700);
        
		string fileName = "snapshot_"+ofToString(10000+snapCounter)+".png";
		img.saveImage(fileName);
		sprintf(snapString, "saved %s", fileName.c_str());
		snapCounter++;
		bSnapshot = false;
	}
    
	//ofDrawBitmapString(snapString, 1075,200);
    ofSetColor(255);
    cooper.drawString(snapString, 1075, 200);
    
	ofSetHexColor(0xFFFFFF);
	if(snapCounter > 0) {
        img.draw(1056,0,300,180);
	}

    /*
     ofSetColor(225);
     ofDrawBitmapString("AUDIO INPUT EXAMPLE", 32, 32);
     ofDrawBitmapString("press 's' to unpause the audio\n'e' to pause the audio", 31, 92);
     
     ofNoFill();
     
     // draw the left channel:
     ofPushStyle();
     ofPushMatrix();
     ofTranslate(32, 170, 0);
     
     ofSetColor(225);
     ofDrawBitmapString("Left Channel", 4, 18);
     
     ofSetLineWidth(1);	
     ofRect(0, 0, 512, 200);
     
     ofSetColor(245, 58, 135);
     ofSetLineWidth(3);
     
     ofBeginShape();
     for (int i = 0; i < left.size(); i++){
     ofVertex(i*2, 100 -left[i]*180.0f);
     }
     ofEndShape(false);
     
     ofPopMatrix();
     ofPopStyle();
     
     // draw the right channel:
     ofPushStyle();
     ofPushMatrix();
     ofTranslate(32, 370, 0);
     
     ofSetColor(225);
     ofDrawBitmapString("Right Channel", 4, 18);
     
     ofSetLineWidth(1);	
     ofRect(0, 0, 512, 200);
     
     ofSetColor(245, 58, 135);
     ofSetLineWidth(3);
     
     ofBeginShape();
     for (int i = 0; i < right.size(); i++){
     ofVertex(i*2, 100 -right[i]*180.0f);
     }
     ofEndShape(false);
     
     ofPopMatrix();
     ofPopStyle();
     */
     // draw the average volume:
     ofPushStyle();
     ofPushMatrix();
     ofTranslate(1056, 170, 0);
     
     //ofSetColor(225);
     //ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(scaledVol * 100.0, 0), 4, 18);
    // ofRect(0, 0, 400, 400);
     
     ofSetColor(245, 58, 135);
     ofFill();		
     ofCircle(200, 400, scaledVol * 190.0f);
     
     //lets draw the volume history as a graph
     ofBeginShape();
     for (int i = 0; i < volHistory.size(); i++){
     if( i == 0 ) ofVertex(i, 400);
     
     ofVertex(i, 400 - volHistory[i] * 70);
     
     if( i == volHistory.size() -1 ) ofVertex(i, 400);
     }
     ofEndShape(false);		
     
     ofPopMatrix();
     ofPopStyle();
     
     drawCounter++;
     
     /*ofSetColor(225);
     string reportString = "buffers received: "+ofToString(bufferCounter)+"\ndraw routines called: "+ofToString(drawCounter)+"\nticks: " + ofToString(soundStream.getTickCount());
     ofDrawBitmapString(reportString, 32, 589);
     */
    
    string shoutText = "Make a LOUD sound to take picture!!!";
    // ofDrawBitmapString(shoutText, 20, 720);
    ofSetColor(255);
    big.drawString(shoutText, 20, 800);
    
    if(scaledVol > 0.75){
        bSnapshot = true;
        
    }
}


 //--------------------------------------------------------------
 void testApp::audioIn(float * input, int bufferSize, int nChannels){	
 
 float curVol = 0.0;
 
 // samples are "interleaved"
 int numCounted = 0;	
 
 //lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
 for (int i = 0; i < bufferSize; i++){
 left[i]		= input[i*2]*0.5;
 right[i]	= input[i*2+1]*0.5;
 
 curVol += left[i] * left[i];
 curVol += right[i] * right[i];
 numCounted+=2;
 }
 
 //this is how we get the mean of rms :) 
 curVol /= (float)numCounted;
 
 // this is how we get the root of rms :) 
 curVol = sqrt( curVol );
 
 smoothedVol *= 0.93;
 smoothedVol += 0.07 * curVol;
 
 bufferCounter++;
 
 }
 

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	/*
    if(key == ' ') {
		canon.takePicture();
        canon.endLiveView();
        
	}
    if(key == 'd'){
        
      //  canon.live_pixels 
    
    }
     */
    
    if (key == 'x'){
        
		bSnapshot = true;
	}
    
    /*
    if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}
     */
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}