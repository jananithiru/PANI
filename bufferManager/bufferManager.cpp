/*
 * bufferManager.cpp
 *
 *  Created on: 17-Sep-2011
 *      Author: amrita
 */

#include "bufferManager.h"

Utils utils;

bufferManager::bufferManager(long memsize) {
	// TODO Auto-generated constructor stub
	this->bPool=new bufferPool(memsize);
}
bufferManager::~bufferManager() {
	// TODO Auto-generated destructor stub
}
//void bufferManager::allocate(long memsize) {
//	// TODO Auto-generated constructor stub
//	bPool=new bufferPool(memsize);
//	//bPool=b;
//}
void bufferManager::writePage(db* databaseId, Page* page)
{
	Frame* frameToWrite;
	//check for frame containing the page number
	long frameNumber=this->bPool->getFrameWithPageNumber(page->pageNumber);


	#ifdef DEBUG
	//utils.myprintf("[writePage]:frame number containing page %d",frameNumber);
	#endif
	if(frameNumber != -1)
		{frameToWrite=this->bPool->getFrameNumber(frameNumber);

		}//get free frame and write onto it
	else {
			frameToWrite = this->bPool->getFreeFrame();
			if (frameToWrite == NULL)
			{
				//None of the frames are free, apply replacement
				frameToWrite = applyReplacement();
			}

		}
	//After getting the frame to be written, change the variables

		frameToWrite->dbIndex=databaseId;
		frameToWrite->dirty=true;
		frameToWrite->refCount++;
		memcpy(&(frameToWrite->pageData[0]),page,PAGE_SIZE);
		cout<<"Page written";
		//for testing
//		Page* page1 = (Page*)frameToWrite->pageData;
		//update pagemap
		this->bPool->pageMap[frameToWrite->frameNumber]=page->pageNumber;

}
Page* bufferManager::readPage(db* databaseId, long pageNumber)
{
	Frame* frameToRead;
	Page* page=(Page*)malloc(PAGE_SIZE);
	//check for frame containing the page number
	long frameNumber=this->bPool->getFrameWithPageNumber(pageNumber);

	if(frameNumber != -1)
	{
		frameToRead=this->bPool->getFrameNumber(frameNumber);
		if(frameToRead->dirty==true)
		{
			//what should be done
		}


		page=(Page*)frameToRead->pageData;


		frameToRead->refCount++;
	}
	//get a free frame for loading
	else {
				frameToRead = this->bPool->getFreeFrame();
				if (frameToRead == NULL)
				{
					//None of the frames are free, apply replacement
					frameToRead = applyReplacement();
				}

	//load page data from file onto the frame
				page=readFile(databaseId,pageNumber);
				if(page!=NULL)
				{
					//update refcount by 1

					frameToRead->dbIndex=databaseId;
					frameToRead->dirty=false;
					unsigned char* pageChar=(unsigned char*)page;
					pageChar=&(frameToRead->pageData[1]);
					frameToRead->refCount++;
				}
	}
	return page;
}
void bufferManager::writeFile(db* database,Page* page)
{
	char * pagedata = (char*)page;
	cout<<"Page "<<page->pageNumber<<"is written to file"<<endl;
	database->dbFP->seekg(page->pageNumber*PAGE_SIZE,ios::beg);
	database->dbFP->write(pagedata,PAGE_SIZE);
}
Page* bufferManager::readFile(db* database,long pageNumber)
{
	char * pagedata=(char*)malloc(PAGE_SIZE);
	database->dbFP->seekg(pageNumber*PAGE_SIZE,ios::beg);

	database->dbFP->read(pagedata,PAGE_SIZE);
	Page* page=(Page*)pagedata;
	if (page->pageNumber!=pageNumber)
		return NULL;
	else
		return page;

}
Frame* bufferManager::applyReplacement()
{
	Frame* frameReplaced;
	//additional check to find if all frames are full, else return first free frame(remove after testing)

	//compare refcount of all frames and least refcount in FIFO order is pushed
	int iterateFrame=0;
	int flag=false;
	int refCount=0;
	Frame* currFrame=this->bPool->getFrameNumber(0);
	refCount=currFrame->refCount;
	frameReplaced=currFrame;
	cout<<"Applying replacement"<<endl;
		for(iterateFrame=0;iterateFrame < this->bPool->frameCount;iterateFrame++)
		{
			currFrame=this->bPool->getFrameNumber(iterateFrame);

					if(currFrame->refCount <= refCount)
						{
						frameReplaced=currFrame;
						flag=true;
						break;
						}
		}
	//write back the current contents of the frame onto file call writefile

		if (frameReplaced->dirty==true)
		{

			this->bPool->printMap();
			Page* pageReplaced=(Page*)(frameReplaced->pageData);


			writeFile(frameReplaced->dbIndex,pageReplaced);
			//free(pageData);
		}
	//initialise and return this frame
			frameReplaced->dirty=false;
			frameReplaced->refCount=0;
			frameReplaced->dbIndex=NULL;
			frameReplaced->pageData[1]='\0';
			return(frameReplaced);
}
void bufferManager::commit()
{
	cout<<"committing___________________________"<<endl;
	long iterFrame;
	for(iterFrame=0;iterFrame<this->bPool->frameCount;iterFrame++)
		{
		Frame* currFrame=this->bPool->getFrameNumber(iterFrame);
				if(currFrame->dbIndex!=NULL)
				{
					Page* page=(Page*)currFrame->pageData;

					writeFile(currFrame->dbIndex,page);
				}
		}
}
void bufferManager::hexdump(void *pAddressIn, long  lSize)
{
 char szBuf[100];
 long lIndent = 1;
 long lOutLen, lIndex, lIndex2, lOutLen2;
 long lRelPos;
 struct { char *pData; unsigned long lSize; } buf;
 unsigned char *pTmp,ucTmp;
 unsigned char *pAddress = (unsigned char *)pAddressIn;

   buf.pData   = (char *)pAddress;
   buf.lSize   = lSize;

   while (buf.lSize > 0)
   {
      pTmp     = (unsigned char *)buf.pData;
      lOutLen  = (int)buf.lSize;
      if (lOutLen > 16)
          lOutLen = 16;

      // create a 64-character formatted output line:
      sprintf(szBuf, " >                            "
                     "                      "
                     "    %08lX", pTmp-pAddress);
      lOutLen2 = lOutLen;

      for(lIndex = 1+lIndent, lIndex2 = 53-15+lIndent, lRelPos = 0;
          lOutLen2;
          lOutLen2--, lIndex += 2, lIndex2++
         )
      {
         ucTmp = *pTmp++;

         sprintf(szBuf + lIndex, "%02X ", (unsigned short)ucTmp);
         if(!isprint(ucTmp))  ucTmp = '.'; // nonprintable char
         szBuf[lIndex2] = ucTmp;

         if (!(++lRelPos & 3))     // extra blank after 4 bytes
         {  lIndex++; szBuf[lIndex+2] = ' '; }
      }

      if (!(lRelPos & 3)) lIndex--;

      szBuf[lIndex  ]   = '<';
      szBuf[lIndex+1]   = ' ';

      printf("%s\n", szBuf);

      buf.pData   += lOutLen;
      buf.lSize   -= lOutLen;
   }
}
