/*
 * bufferPool.h
 *
 *  Created on: 17-Sep-2011
 *      Author: amrita
 */

#ifndef BUFFERPOOL_H_
#define BUFFERPOOL_H_
#include "../globals/globals.h"
#include "../Utils/Utils.h"

class bufferPool {

public:
	long frameCount;
		long bufferSize;
	bufferPool(long memSize);
	//bufferPool(int noOfDatabases);//TBD
	virtual ~bufferPool();
	void* buffer;
	map<long, long> pageMap;
	//frame related
		Frame* getFirstFrame();
		long getFrameWithPageNumber(long pageNumber);
		Frame* getPrevFrame();
		Frame* getFrameNumber(long frameNumber);
		Frame* getFreeFrame();
		void printMap();
		void getFramedata();

};

#endif /* BUFFERPOOL_H_ */
