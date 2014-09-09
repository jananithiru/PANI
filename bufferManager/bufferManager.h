/*
 * bufferManager.h
 *
 *  Created on: 17-Sep-2011
 *      Author: amrita
 */

#ifndef BUFFERMANAGER_H_
#define BUFFERMANAGER_H_
#include "bufferPool.h"

class bufferManager {

public:
	bufferPool* bPool;

	bufferManager(long memsize);
	virtual ~bufferManager();

//	void allocate(long memsize);
	void writePage(db* database, Page* page);
	Page* readPage(db* database, long pageNum);
	void hexdump(void *pAddressIn, long  lSize);
	void writeFile(db* database,Page* page);
	Page* readFile(db* database,long pageNum);
	Frame* applyReplacement();
	void commit();
	//Database related

	//Page related
	//void* readPage(long pageNumber,int dbIndex);
};

#endif /* BUFFERMANAGER_H_ */
