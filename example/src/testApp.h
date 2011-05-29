#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxBlobsManager.h"

class testApp : public ofBaseApp{

public:
	
	ofxKinect 			kinect;
	ofxKinectRecorder 	kinectRecorder;
	ofxKinectPlayer 	kinectPlayer;
	/// used to switch between the live kinect and the recording player
	ofxBase3DVideo* 	kinectSource;

	ofxCvGrayscaleImage depthImage;
	ofxCvGrayscaleImage blobsImage;
	
	ofxCvContourFinder 	contourFinder;
	ofxBlobsManager		blobsManager;
	
	int 				nearThreshold;
	int					farThreshold;
	int					blobsNumMax;
	bool				blobsFindHoles;
	int					blobsMinArea;
	int					blobsMaxArea;
	
	int					angle;
	bool 				record;
	bool 				playback;
	bool				paused;
	
	int					displayWidth;
	int					displayHeight;
	
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void exit(ofEventArgs & args);
	
	void thresholdDepthRange(ofxCvGrayscaleImage * grayImage, int nearThreshold, int farThreshold);
	void startRecording();
	void stopRecording();
	void startPlayback();
	void stopPlayback();
};
