#include "testApp.h"

void testApp::setup()
{
	ofBackground(50, 50, 50);
	
	kinect.init(false, false);  // disable infrared/rgb video iamge (faster fps)
	kinect.setVerbose(true);
	kinect.open();
	
	// start with the live kinect source
	kinectSource = &kinect;
	depthImage.allocate(kinect.width, kinect.height);
	blobsImage.allocate(kinect.width, kinect.height);
	
	nearThreshold = 255;
	farThreshold  = 225;
	blobsMinArea = 1500;
	blobsMaxArea = 15000;
	blobsNumMax = 10;
	blobsFindHoles = false;
	
	paused = false;
	record = false;
	playback = false;
	angle = 0;
	
	displayWidth = 400;
	displayHeight = 300;
	
	blobsManager.normalizePercentage = 0.7;
		
	ofAddListener(ofEvents.exit, this, &testApp::exit);
}

void testApp::update()
{
	kinectSource->update();
	
	// there is a new frame and we are connected
	if(kinectSource->isFrameNew() && !paused)
	{
		// record ?
		if(record && kinectRecorder.isOpened()) {
			kinectRecorder.newFrame(kinect.getRawDepthPixels(), kinect.getPixels());
		}
		
		if(playback) 
			blobsImage.setFromPixels(kinectPlayer.getDepthPixels(), kinect.width, kinect.height);
		else
			blobsImage.setFromPixels(kinectSource->getDepthPixels(), kinect.width, kinect.height);
		
		depthImage.setFromPixels(blobsImage.getPixels(), kinect.width, kinect.height);
		
		thresholdDepthRange(&blobsImage, nearThreshold, farThreshold);
		
		contourFinder.findContours(blobsImage, blobsMinArea, blobsMaxArea, blobsNumMax, blobsFindHoles);
		
		blobsManager.update(contourFinder.blobs);
	}
}

void testApp::draw()
{
	ofSetColor(255, 255, 255);
	if(playback) 
	{
		// draw from the player
		kinectPlayer.drawDepth(10, 10, displayWidth, displayHeight);
	} 
	else 
	{
		// draw from the live kinect
		depthImage.draw(10, 10, displayWidth, displayHeight);
	}
	blobsImage.draw(displayWidth+10*2, 10, displayWidth, displayHeight);
	contourFinder.draw(displayWidth+10*2, 10, displayWidth, displayHeight);
	blobsManager.debugDraw(displayWidth+10*2, 10, kinect.width, kinect.height, displayWidth, displayHeight);
	
	// draw recording/playback indicators
	ofPushMatrix();
	ofTranslate(25, 25);
	ofFill();
	if(record)
	{
		ofSetColor(255, 0, 0);
		ofCircle(0, 0, 10);
	}
	if(playback) 
	{
		ofSetColor(0, 255, 0);
		ofTriangle(-10, -10, -10, 10, 10, 0);
	}
	ofPopMatrix();
	
	ofSetColor(255, 255, 255);
	stringstream reportStream;
	reportStream 
	<< "set near threshold " << nearThreshold << " (press: + -)" << endl
	<< "set far threshold " << farThreshold << " (press: < >) " << endl 
	<< "num blobs found " << contourFinder.nBlobs
	<< ", fps: " << ofGetFrameRate() << endl
	<< "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
	<< "press r to record and p to playback. " << ((record)? "is recording." : "") << ((playback)? "is playing." : "") << endl
	<< "set tilt angle: " << angle << " degrees (press UP and DOWN)";
	ofDrawBitmapString(reportStream.str(),10,displayHeight+10*3);
}
void testApp::exit(ofEventArgs & args) {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	kinectPlayer.close();
	kinectRecorder.close();
}
void testApp::thresholdDepthRange(ofxCvGrayscaleImage * grayImage, int nearThreshold, int farThreshold)
{
	unsigned char * pix = grayImage->getPixels();
	
	int numPixels = grayImage->getWidth() * grayImage->getHeight();
	for(int i = 0; i < numPixels; i++) 
	{
		if(pix[i] < nearThreshold && pix[i] > farThreshold) 
		{
			pix[i] = 255;
		} 
		else 
		{
			pix[i] = 0;
		}
	}
	
	// update the cv images
	grayImage->flagImageChanged();
}
void testApp::startRecording() {
	
	// stop playback if running
	stopPlayback();
	
	kinectRecorder.init(ofToDataPath("recording.dat"));
	record = true;
}
void testApp::stopRecording() {
	kinectRecorder.close();
	record = false;
}
void testApp::startPlayback() {
	stopRecording();
	kinect.close();
	
	// set record file and source
	kinectPlayer.setup(ofToDataPath("recording.dat"), true);
	kinectPlayer.loop();
	kinectSource = &kinectPlayer;
	playback = true;
}
void testApp::stopPlayback() {
	kinectPlayer.close();
	kinect.open();
	kinectSource = &kinect;
	playback = false;
}
	

	
//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	switch(key)
	{
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':		
		case ',':		
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':		
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}
void testApp::keyReleased(int key)
{
	switch(key)
	{
		case 'q':
			if(!record)
				playback = !playback;
			if(record)
				startRecording();
			else
				stopRecording();
			break;
		case 'r':
			if(!playback)
				record = !record;
			if(playback)
				startPlayback();
			else
				stopPlayback();
			break;
		case 'p':
			paused = !paused;
			break;
		case keyUp:
			angle += 5;
	}
	
	
}
void testApp::mouseMoved(int x, int y ){
	
}
void testApp::mouseDragged(int x, int y, int button){

}
void testApp::mousePressed(int x, int y, int button){

}
void testApp::mouseReleased(int x, int y, int button){

}
void testApp::windowResized(int w, int h){

}
void testApp::gotMessage(ofMessage msg){

}
void testApp::dragEvent(ofDragInfo dragInfo){ 

}