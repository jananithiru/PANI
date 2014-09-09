/*
 * globalDefinitions.h
 *
 *  Created on: 15-Sep-2011
 *      Author: prat
 *      Modified By:Amy, Jan
 * 		
 */

#ifndef GLOBALDEFINITIONS_H_
#define GLOBALDEFINITIONS_H_

#include "globalConstants.h"
#include "globalHeaders.h"
//#define trace(...) trace(__FILE__, __LINE__, __VA_ARGS__)
//frame structure

typedef struct database{
	char* dbName;
	int dbIndex;
	fstream* dbFP;

}db;

typedef struct frameStruct
{
public:
	int frameNumber;
	bool dirty;
	db* dbIndex;
	int refCount;//0 is lowest priority
	unsigned char pageData[1];
}Frame;

//Page structure (should it be a class or struct??)
typedef struct pageStruct
{

	/*
	 * Priority = 0 for dbheader
	 * 1 for dpPage
	 * 2 for dbPage
	 */
	int priority;
	int pageNumber;
	int nextPage;//TBD if needed
	int CFS; //contiguous free space
	char pageData[1];
}Page;

struct charStr {
	int charSize;

};

typedef struct sysTable {
	char *tableName;
	int tableId;
	int tablePage; //pointer to the directory page

}sysTable;

typedef struct columnValuesStruct {
	char *colName;
	int colType; //get it from globalConstants.h
	int sizeOfAttribute;
	int precision; //for float data
	bool isNull;
	bool isIndexAttribute;
	bool hasDefaultValue;
	void *defaultValue;
	int tablePage; //pointer to the directory page
}columnVal;

// J: New Structure Added
typedef struct keyPartStruct {
	char attributeName[50];
	int attributeType;
	int attributeSize;
} keyPart;

// J: Updated Structure
typedef struct sysIndexStructure {
	int indexID;
	int tableID; // id of the table to which the index belongs
	int ptrToPageContainingRootofTree;
	int tableDirPage;
	int isAsc;
	int numOfAttributes;
	keyPart compositeKeyParts[MAX_NO_OF_COLS_INDEXED];
} sysIndex;

// J: Updated
typedef struct blockStruct {
	int pageNo;
	int slotNo;
} block;

//J: Updated
typedef struct tupleStruct {
	int tupleId;
	unsigned int recLen;
	int ptrToTableHeaderPage; //this is used only for systable, not for other pages
	unsigned char recData[1];
} tuple;

typedef struct colMega{
	char *colName;
	char *colType; //get it from globalConstants.h
	int sizeOfAttribute;
	int precision; //for float data
	bool isNull;
	bool isIndexAttribute;
	bool hasDefaultValue;
	void *defaultValue;
}colMegaStructure;


typedef struct databaseHeader {
	int freeList;
	int sysTablePage;
	int sysColPage;
	int sysIndexPage;
}dbHeader;

typedef struct directoryEntry {
	int dbPage;
	int TFS;
}dirEntry;

typedef struct slotDirectoryEntry {
	int slotOffset;
	int slotSize;
}slotEntry;

typedef struct slotDirectoryStruct {
	int slotCount;
	slotEntry* slotList[1];
}slotDirectory;

typedef struct databasePage {
	int backPtrToDPPage;
	int ptrToTableHeader;
	slotDirectory tailSlotDir; //this will be at the tail end
	unsigned char* pageData[1];
}dbPage;

typedef struct directoryPage {
	int noOfCurrentDEs;
	int ptrToNextDP;
	dirEntry dirEntryList[DP_MAX_NOS_DES];
}dirPage;


/*typedef struct resultSet {
	struct rid;
	short ppos;
}resultSet;*/

typedef struct columnStructure {
	char *type;
	int size; //size of each column : int = 4, varchar(30) = 30 .....
}colSturct;

typedef struct tableHeaderStructure {
	int pointerToFirstDP; //DP chain starts from here
	int total_count; //similar to surrogate key. Increments by 1
	colSturct *colValues[MAX_NOS_COLUMNS];
}tableHeader;


typedef struct resSet {
	int count;
	tuple *tuples[1000000]; //1 million
}resultSet;

typedef struct colStruct {
	char *name;
	char *type;
	int size;
	void *value;
}columnStruct;

#endif /* GLOBALDEFINITIONS_H_ */
