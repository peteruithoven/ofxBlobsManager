/*
 *  BlobsManager.cpp
 *
 *  Created by Peter Uithoven on 5/6/11.
 */

#import "ofxBlobsManager.h"


ofxBlobsManager::ofxBlobsManager()
{
	maxMergeDis = 50; 
	maxUndetectedTime = 5000;
	sequentialID = 0;
	giveSequentialID = false;
	maxBlobs = 9999;
	normalizePercentage = 1;
}
bool sortBlobsOnDis(ofxStoredBlobVO * blob1, ofxStoredBlobVO * blob2)
{
	return (blob1->dis < blob2->dis); 
}
void ofxBlobsManager::update(vector<ofxCvBlob> newBlobs)
{
	//cout << "BlobsManager::update" << endl;
	int numNewBlobs = newBlobs.size();
	
	//cout << "  loop new blobs (" << numNewBlobs << ")" << endl;
	
	for( int i = 0; i < numNewBlobs; i++ ) 
	{
		ofxCvBlob newBlob = newBlobs.at(i);
		
		// find closest blobs, to see if it is the same blob as a stored blob.
		int numBlobs = blobs.size();
		//cout << "    loop prev blobs (" << numBlobs << ")" << endl;
		vector<ofxStoredBlobVO*> closeBlobs;
		for( int j = 0; j < numBlobs; j++ ) 
		{
			ofxStoredBlobVO * storedBlob = blobs.at(j);
			storedBlob->dis = storedBlob->centroid.distance(newBlob.centroid);
			
			//cout << "      " << storedBlob->id << ": dis: " << storedBlob->dis << endl;
			if(storedBlob->dis < maxMergeDis)
				closeBlobs.push_back(storedBlob);
		}
		if(closeBlobs.size() > 0)
		{
			sort (closeBlobs.begin(), closeBlobs.end(), &sortBlobsOnDis);
			ofxStoredBlobVO* closestBlob = closeBlobs.at(0);
			//cout << "      closestBlob: " << closestBlob->id << endl;
			
			if(normalizePercentage < 1)
			{
				newBlob.centroid.x = closestBlob->centroid.x*(1-normalizePercentage) + newBlob.centroid.x*normalizePercentage;
				newBlob.centroid.y = closestBlob->centroid.y*(1-normalizePercentage) + newBlob.centroid.y*normalizePercentage;
			}
			
			closestBlob->centroid.x = newBlob.centroid.x;
			closestBlob->centroid.y = newBlob.centroid.y;
			
			closestBlob->lastDetectedTime = ofGetElapsedTimeMillis();
		}
		else
		{
			// store the new blob
			// we make a ofxStoredBlobVO out of the ofxCvBlob so we can store a id for example
			ofxStoredBlobVO * newStoredBlob = new ofxStoredBlobVO(newBlob);
			if(giveSequentialID)
				newStoredBlob->id = sequentialID;
			newStoredBlob->lastDetectedTime = ofGetElapsedTimeMillis();
			blobs.push_back(newStoredBlob);
			//cout << "      new blob: " << newStoredBlob->id << endl;
			//cout << "        x: " << newStoredBlob->centroid.x << ", y: " << newStoredBlob->centroid.y << endl;
			if(giveSequentialID)	  
				sequentialID++;	
		}
		
	}
	
	//cout << "  loop stored blobs (" << blobs.size() << ") (check age)" << endl; 
	for( int i = 0; i < blobs.size(); i++ ) 
	{
		ofxStoredBlobVO * blob = blobs.at(i);
		int elapsedTime = ofGetElapsedTimeMillis()-blob->lastDetectedTime;
		//cout << "    blob: " << blob->id << " elapsedTime: " << elapsedTime << endl; 
		//cout << "    blob: " << blob->id << " x: " << blob->centroid.x << " y: " << blob->centroid.y << endl;	
		if(elapsedTime > maxUndetectedTime)
		{
			removeBlob(blob->id);
			//delete blob;
			i--;
		}
	}
	
	// find name
	if(!giveSequentialID)
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
					removeBlob(blob->id);
					i--;
				}
			}
		}
	}
	
	//cout << "  loop re-id'd blobs (" << blobs.size() << ")" << endl; 
	for( int i = 0; i < blobs.size(); i++ ) 
	{
		ofxStoredBlobVO * blob = blobs.at(i);
		//cout << "    blob: " << blob->id << " x: " << blob->centroid.x << " y: " << blob->centroid.y << endl;	
	}
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
void ofxBlobsManager::removeBlob(int blobID)
{
	//cout <<  "BlobsManager::removeCandidate blobID: " << blobID << endl;

	vector <ofxStoredBlobVO*>::iterator itr;
	for (itr = blobs.begin(); itr != blobs.end(); ++itr) {
		ofxStoredBlobVO * blobVO = *itr;
		//cout <<  "\n  trackedObject->id: " << trackedObject->id;
		if(blobVO->id == blobID)
		{
			delete blobVO;
			blobs.erase(itr);
			break;
		}
	}
}

void ofxBlobsManager::debugDraw(int baseX, int baseY, int inputWidth, int inputHeight, int displayWidth, int displayHeight)
{
	float scaleX = float(displayWidth)/float(inputWidth);
	float scaleY = float(displayHeight)/float(inputHeight);
	
	//cout <<  "BlobsManager::debugDraw x: " << baseX << " y: " << baseY << " scaleX " << endl;
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
		ofDrawBitmapString(ofToString(blob->id),x-4,y+5);
	}
	
}