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

	//the closest stored blob that are withing this distance, will be merged into new found blob 
	int maxMergeDis; 
	// the maximum time a blob can go undetected before the blob manager will remove the blob
	int maxUndetectedTime;
	// the minimum time a blob has to be detected before the blob manager will add the blob 
	// (last time detected - first time detection) 
	// not necessarily continuously (as long as undetected doesn't cross maxUndetectedTime)  
	int minDetectedTime;
	// normally it will every new blob a sequentially higher id, 
	// but with giveLowestPossibleIDs enabled it will try to give the lowest id's available. 
	bool giveLowestPossibleIDs;
	// the max amount of blobs when doing non sequential id's
	int maxBlobs;
	// by what percentage the blob movement will be normalized
	float normalizePercentage;
	
	vector<ofxStoredBlobVO*> blobs;

	
	ofxBlobsManager();
	void update(vector<ofxCvBlob> newCVBlobs);
	void debugDraw(int baseX, int baseY, int inputWidth, int inputHeight, int displayWidth, int displayHeight);
	bool hasBlob(int blobID);
	void removeBlob(ofxStoredBlobVO * targetBlob, vector<ofxStoredBlobVO*> * blobs, bool deleteBlob);
	
private:
	int sequentialID;
	vector<ofxStoredBlobVO*> candidateBlobs;
	
	vector<ofxStoredBlobVO*> findCloseBlobs(ofxCvBlob * newBlob,vector<ofxStoredBlobVO*> * blobs);
};

#endif