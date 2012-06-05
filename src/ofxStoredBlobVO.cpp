/*
 *  ofxStoredBlobVO.cpp
 *
 *  Created by Peter Uithoven on 5/6/11.
 */

#include "ofxStoredBlobVO.h"


ofxStoredBlobVO::ofxStoredBlobVO(ofxCvBlob& newBlob)
{
	// a quick "shallow" copy 
	update(newBlob);
	
	id = -1;
	lastDetectedTime = 0;
}

void ofxStoredBlobVO::update(ofxCvBlob& newBlob)
{
	// a quick "shallow" update 
	area = newBlob.area;
	length = newBlob.length;
	boundingRect = newBlob.boundingRect;
	centroid = newBlob.centroid;
	hole = newBlob.hole;
	pts = newBlob.pts;
	nPts = newBlob.nPts;
}