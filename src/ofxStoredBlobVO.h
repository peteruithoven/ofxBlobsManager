/*
 *  BlobVO.h
 *
 *  Created by Peter Uithoven on 5/6/11.
 */

#ifndef _ofxStoredBlobVO
#define _ofxStoredBlobVO

#include "ofMain.h"
#include "ofxOpenCv.h"

class ofxStoredBlobVO : public ofxCvBlob
{
	public:
		int id;
		int lastDetectedTime; 
		int dis; //used to sort on distance and to find the closest blob to merge with
	
		ofxStoredBlobVO(ofxCvBlob newBlob);
};

#endif
