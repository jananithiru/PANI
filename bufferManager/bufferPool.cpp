/*
 * bufferPool.cpp
 *
 *  Created on: 17-Sep-2011
 *      Author: amrita
 */

#include "bufferPool.h"

bufferPool::bufferPool(long memSize)
{
	int iterFrame;
	Frame* newFrame;

	this->frameCount=memSize/FRAME_SIZE;
	this->bufferSize=memSize;
	//allocate bufferPool memory
	this->buffer =(void*) malloc(this->bufferSize);
	cout<<"buffer size"<<this->bufferSize;
	//initialise the frames
	//currentFrame=getFirstFrame();
	for(iterFrame=0;iterFrame<this->frameCount;iterFrame++)
	{
		newFrame = getFrameNumber(iterFrame);
		Frame* currentFrame=(Frame*)malloc(FRAME_SIZE);
	currentFrame->dirty=false;
	currentFrame->frameNumber=iterFrame;
	currentFrame->refCount=0;
	currentFrame->dbIndex=NULL;
	currentFrame->pageData[0]='\0';
	memcpy(newFrame,currentFrame,FRAME_SIZE);
	free(currentFrame);


	//initialise the map
	this->pageMap[iterFrame]=-1;

	}


}

bufferPool::~bufferPool()
{
	free(this->buffer);

}
Frame* bufferPool::getFirstFrame()
{
	Frame* currentFrame=(Frame*)this->buffer;
	return currentFrame;
}
long bufferPool::getFrameWithPageNumber(long pageNumber)
{
	bool found = false;
	    map<long,long>::iterator iterMap = pageMap.begin();
	    while(iterMap != pageMap.end())
	    {
	        found = (iterMap->second == pageNumber);
	        if(found)
	        	return iterMap->first;
	        ++iterMap;
	    }
	    return -1;
}
void bufferPool::printMap()
{
	cout<<"MAP data"<<endl;
	map<long,long>::iterator iterMap ;
	    for ( iterMap=pageMap.begin() ; iterMap != pageMap.end(); iterMap++ )
	            cout << (*iterMap).first << " => " << (*iterMap).second << endl;
}
Frame* bufferPool::getPrevFrame()
{
	Frame* currentFrame=(Frame*)this->buffer;
	return currentFrame;
}
Frame* bufferPool::getFrameNumber(long frameNumber){

	long offset=(frameNumber)*FRAME_SIZE;
	//void* currFrame=malloc(FRAME_SIZE);
	//memcpy(currFrame,static_cast<char *>(this->buffer) + offset,FRAME_SIZE);
	void* currFrame=static_cast<char *>(this->buffer) + offset;
	Frame* currentFrame= (Frame*)currFrame;
	return currentFrame;
}
Frame* bufferPool::getFreeFrame()
{
	int iterateFrame=0;

	for(;iterateFrame < frameCount;iterateFrame++)
	{
		Frame* currFrame=getFrameNumber(iterateFrame);
		if(currFrame->dbIndex==NULL)
		{	cout<<"Getting free frames\n";
			return currFrame;
		}
	}
	return NULL;
}
void bufferPool::getFramedata()
{
	int iterateFrame=0;

		for(;iterateFrame < frameCount;iterateFrame++)
		{
			Frame* myFrame=getFrameNumber(iterateFrame);
			if(myFrame->pageData[0]!='\0')
			{
				cout<<"frame "<<myFrame->frameNumber;
				Page* pages=(Page*)myFrame->pageData;
			cout<<"has page "<<pages->pageNumber<<endl;
			}
			else cout<<"frame "<<myFrame->frameNumber <<"is empty"<<endl;
		}
}
