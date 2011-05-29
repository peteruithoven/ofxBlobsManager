/*
 *  BlobsManager.h
 *  A manager for openCV blobs. By remembering blobs and comparing them 
 *  it will identify blobs, even though they are moving. 
 *  It's basicly a lot of things you need to do when working with blobs. 
 *  
 *  Created by Peter Uithoven on 5/6/11.
 */

#ifndef _ofxBlobsManager
#define _ofxBlobsManager

#include "ofMain.h"
#include "ofxStoredBlobVO.h"
#include "ofxOpenCv.h"

class ofxBlobsManager
{
public:

	vector<ofxStoredBlobVO*> blobs;
	//the closest stored blob that are withing this distance, will be merged into new found blob 
	int maxMergeDis; 
	int maxUndetectedTime;
	// when giving sequential id's he will give a new, higher id to every new found blob
	bool giveSequentialID;
	int maxBlobs;
	float normalizePercentage;

	ofxBlobsManager();
	void update(vector<ofxCvBlob> newCVBlobs);
	void debugDraw(int baseX, int baseY, int inputWidth, int inputHeight, int displayWidth, int displayHeight);

	bool hasBlob(int blobID);
	void removeBlob(int blobID);
private:
	int sequentialID;
		
};

#endif