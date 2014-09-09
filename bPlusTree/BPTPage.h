/*
 * BPTPage.h
 *
 *  Created on: 18-Dec-2011
 *      Author: janani
 */

#ifndef BPTPAGE_H_
#define BPTPAGE_H_
#include "../bPlusTree/BPlusTreeC.h"
#include "../globals/globals.h"
#include "../Utils/Utils.h"

class BPTPage {
public:
	BPTPage();

	int getFreePage();
	int writeDiskPage(db* database, Page* page);
	Page* readDiskPage(db* database, long pageNum);

	virtual ~BPTPage();
};

#endif /* BPTPAGE_H_ */
