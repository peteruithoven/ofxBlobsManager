/*
 *  BlobsManager.cpp
 *
 *  Created by Peter Uithoven on 5/6/11.
 */

#import "ofxBlobsManager.h"


ofxBlobsManager::ofxBlobsManager()
{
	maxMergeDis = 100; 
	normalizePercentage = 1;	
	
	enableMinDetectedTimeFilter = true;
	minDetectedTime = 1000;
	
	enableUndetectedBlobs = true;
	maxUndetectedTime = 1000;
	
	giveLowestPossibleIDs = false;
	maxNumBlobs = 9999;

	sequentialID = 0;
}
bool sortBlobsOnDis(ofxStoredBlobVO * blob1, ofxStoredBlobVO * blob2)
{
	return (blob1->dis < blob2->dis); 
}
void ofxBlobsManager::update(vector<ofxCvBlob> newBlobs)
{
	//cout << "BlobsManager::update" << endl;
	int numNewBlobs = newBlobs.size();
	
	int currentTime = ofGetElapsedTimeMillis();
	
	//cout << "  loop new blobs (" << numNewBlobs << ")" << endl;
	for( int i = 0; i < numNewBlobs; i++ ) 
	{
		ofxCvBlob newBlob = newBlobs.at(i);
		
		// find new blob in blobs
		vector<ofxStoredBlobVO*> closeBlobs = findCloseBlobs(&newBlob,&blobs);
		
		bool foundInStoredBlobs = closeBlobs.size() > 0;
		if(foundInStoredBlobs)
		{
			// update stored blob
			ofxStoredBlobVO* closestBlob = closeBlobs.at(0);
			//cout << "      found matching stored blob: " << closestBlob->id << endl;
			closestBlob->update(newBlob);
			if(normalizePercentage < 1)
			{
				closestBlob->centroid.x = closestBlob->centroid.x*(1-normalizePercentage) + newBlob.centroid.x*normalizePercentage;
				closestBlob->centroid.y = closestBlob->centroid.y*(1-normalizePercentage) + newBlob.centroid.y*normalizePercentage;
			}
			closestBlob->lastDetectedTime = currentTime;
		}
		else
		{
			if(enableMinDetectedTimeFilter)
			{
				// find new blob in candidates
				vector<ofxStoredBlobVO*> closeCandidateBlobs = findCloseBlobs(&newBlob,&candidateBlobs);
				bool foundInCandidateBlobs = closeCandidateBlobs.size() > 0;
				if(foundInCandidateBlobs)
				{
					// update candidate
					ofxStoredBlobVO* closestCandidateBlob = closeCandidateBlobs.at(0);
					//cout << "      found matching candidate blob: " << closestCandidateBlob->id << endl;
					closestCandidateBlob->update(newBlob);
					closestCandidateBlob->lastDetectedTime = currentTime;
				}
				else
				{
					// store the new candidate blob
					// we make a ofxStoredBlobVO out of the ofxCvBlob so we can store a id for example
					ofxStoredBlobVO * newCandidateBlob = new ofxStoredBlobVO(newBlob);
					//if(giveSequentialID)
						//newCandidateBlob->id = sequentialID;
					newCandidateBlob->iniDetectedTime = currentTime;
					newCandidateBlob->lastDetectedTime = currentTime;
					candidateBlobs.push_back(newCandidateBlob);
					//cout << "    new blob candidate: " << newCandidateBlob << endl;
					//cout << "        x: " << newCandidateBlob->centroid.x << ", y: " << newCandidateBlob->centroid.y << endl;
				}
			}
			else 
			{
				// store the new blob
				// we make a ofxStoredBlobVO out of the ofxCvBlob so we can store a id for example
				ofxStoredBlobVO * newStoredBlob = new ofxStoredBlobVO(newBlob);
				if(!giveLowestPossibleIDs)
				{
					newStoredBlob->id = sequentialID;
					sequentialID++;	
				}
				newStoredBlob->iniDetectedTime = currentTime;
				newStoredBlob->lastDetectedTime = currentTime;
				blobs.push_back(newStoredBlob);
				//cout << "    new blob: " << newStoredBlob << endl;
				//cout << "        x: " << newStoredBlob->centroid.x << ", y: " << newStoredBlob->centroid.y << endl;
			}

		}
		
	}
	
	if(enableMinDetectedTimeFilter)
	{
		//cout << "  loop candidate blobs (" << candidateBlobs.size() << ") (check undetected and detected time)" << endl; 
		for( int i = 0; i < candidateBlobs.size(); i++ ) 
		{
			ofxStoredBlobVO * candidateBlob = candidateBlobs.at(i);
			int undetectedTime = currentTime-candidateBlob->lastDetectedTime;
			int detectionTime = candidateBlob->lastDetectedTime-candidateBlob->iniDetectedTime;
			//cout << "    candidateBlob: " << candidateBlob << " detectionTime: " << detectionTime << " undetectedTime: " << undetectedTime << endl; 
			int maxUndetectedTime = (enableUndetectedBlobs)? this->maxUndetectedTime : 0;
			if(undetectedTime > maxUndetectedTime)
			{
				//cout << "      to long undetected" << endl;
				removeBlob(candidateBlob,&candidateBlobs,true);
				i--;
			}
			else if(detectionTime > minDetectedTime)
			{
				//cout << "      long enough detected, move to blobs" << endl;
				removeBlob(candidateBlob,&candidateBlobs,false);
				blobs.push_back(candidateBlob);
				i--;
				if(!giveLowestPossibleIDs)
				{
					candidateBlob->id = sequentialID;
					sequentialID++;	
				}
			}
		}
	}
	
	//cout << "  loop stored blobs (" << blobs.size() << ") (check time since last detection)" << endl; 
	for( int i = 0; i < blobs.size(); i++ ) 
	{
		ofxStoredBlobVO * blob = blobs.at(i);
		int undetectedTime = currentTime-blob->lastDetectedTime;
		//cout << "    blob: " << blob->id << " undetectedTime: " << undetectedTime << endl; 
		int maxUndetectedTime = (enableUndetectedBlobs)? this->maxUndetectedTime : 0;
		//cout << "    this->maxUndetectedTime: " << this->maxUndetectedTime << endl;
		//cout << "    local maxUndetectedTime: " << maxUndetectedTime << endl;
		if(undetectedTime > maxUndetectedTime)
		{
			removeBlob(blob,&blobs,true);
			i--;
		}
	}
	
	// give lowest possible id's 
	if(giveLowestPossibleIDs)
	{
		//cout << "  loop stored blobs (" << blobs.size() << ") (find lowest id)" << endl; 
		for( int i = 0; i < blobs.size(); i++ ) 
		{
			ofxStoredBlobVO * blob = blobs.at(i);
			if(blob->id == -1)
			{
				int lowestID = 0;
				while(hasBlob(lowestID)) 
				{
					lowestID++;
				}
				blob->id = lowestID;
				//cout << "    lowestID: " << lowestID << endl;
				if(blob->id > maxBlobs)
				{
					removeBlob(blob,&blobs,true);
					i--;
				}
			}
		}
	}
	
	//cout << "  loop resulting stored blobs (" << blobs.size() << ")" << endl; 
	for( int i = 0; i < blobs.size(); i++ ) 
	{
		ofxStoredBlobVO * blob = blobs.at(i);
		//cout << "    blob: " << blob->id << " x: " << blob->centroid.x << " y: " << blob->centroid.y << endl;	
	}
}

vector<ofxStoredBlobVO*> ofxBlobsManager::findCloseBlobs(ofxCvBlob * newBlob,vector<ofxStoredBlobVO*> * blobs)
{
	//cout << "ofxBlobsManager::findCloseBlobs" << endl;
	// find closest blobs, to see if it is the same blob as a stored blob.
	int numBlobs = blobs->size();
	//cout << "  loop stored (candidate) blobs (" << numBlobs << ")" << endl;
	vector<ofxStoredBlobVO*> closeBlobs;
	for( int j = 0; j < numBlobs; j++ ) 
	{
		ofxStoredBlobVO * blob = blobs->at(j);
		blob->dis = blob->centroid.distance(newBlob->centroid);
		
		//cout << "      " << blob->id << ": dis: " << blob->dis << endl;
		if(blob->dis < maxMergeDis)
			closeBlobs.push_back(blob);
	}
	if(closeBlobs.size() > 0)
		sort (closeBlobs.begin(), closeBlobs.end(), &sortBlobsOnDis);
	
	return closeBlobs;
}

bool ofxBlobsManager::hasBlob(int blobID)
{
	for( int i = 0; i < blobs.size(); i++ ) 
	{
		ofxStoredBlobVO * blob = blobs.at(i);
		if(blob->id == blobID)
			return true;
	}
	return false;
}
void ofxBlobsManager::removeBlob(ofxStoredBlobVO * targetBlob, vector<ofxStoredBlobVO*> * blobs, bool deleteBlob)
{
	//cout <<  "BlobsManager::removeBlob blob: " << targetBlob << " id: " << targetBlob->id << " delete: " << deleteBlob << endl;
	vector <ofxStoredBlobVO*>::iterator itr;
	for (itr = blobs->begin(); itr != blobs->end(); ++itr) {
		ofxStoredBlobVO * blob = *itr;
		if(blob == targetBlob)
		{
			if(deleteBlob)
				delete blob;
			blobs->erase(itr);
			break;
		}
	}
}

void ofxBlobsManager::debugDraw(int baseX, int baseY, int inputWidth, int inputHeight, int displayWidth, int displayHeight)
{
	float scaleX = float(displayWidth)/float(inputWidth);
	float scaleY = float(displayHeight)/float(inputHeight);
	
	ofEnableAlphaBlending();
	//ofSetHexColor(0x0036B7);
	int numBlobs = blobs.size();
	for( int i = 0; i < numBlobs; i++ ) 
	{
		ofxStoredBlobVO * blob = blobs.at(i);
		
		int x = baseX+blob->centroid.x*scaleX;
		int y = baseY+blob->centroid.y*scaleY;
		
		ofNoFill();
		ofSetColor(0, 50, 150,255);
		ofCircle(x, y, 10);
		
		ofFill();
		ofSetColor(255, 255, 255,125);
		ofCircle(x, y, 10);

		ofSetColor(0, 0, 50,255);
		
		if(blob->id >= 10)
			x -= 4;
		ofDrawBitmapString(ofToString(blob->id),x-4,y+5);
	}
	
}