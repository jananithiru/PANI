/*
 * dbModule.h
 *
 *  Created on: 14-Sep-2011
 *      Author: prat
 * 		Modified: Amrita
 */

#ifndef DBMODULE_H_
#define DBMODULE_H_
#include "../globals/globals.h"
#include "../Utils/Utils.h"
#include "../bufferManager/bufferPool.h"
#include "../bufferManager/bufferManager.h"
//global variable
int TABLE_ID = 0;

class dbModule{

	char* dbName;

	//bufferManager bufMan_db(FRAME_SIZE * 2);

	/*
	 * class variables. Same variables will be used.
	 * these are made class level assuming only one object will be
	 * created for a database.
	 */
	sysTable* systemTable;
	int SYSTABLE_COUNT;
	// prattie, include whatever you want here. 

	//

	/* Functions */

public:
	db *myDatabase;


	// implemented functions
	bool createNewDB(char dbName[DB_NAME], long int dbSize);
	dbModule();
	Page* getNewPagePointer();
	dirPage* createDPPage();
	bool useDatabase(db* database);
	void createTable(db *databaseName, char* tableName, vector<colMegaStructure> colVector);
	bool insertRecord(db *databaseName, char* tableName, vector<colMegaStructure> columnsVector, char* record);
	int giveNextEmptySlotEntry(dbPage* dbPg, int tableSlotSize);
	resultSet* select(db *databaseName, char* tableName);
	bool update(db *db, char* tableName, int primaryKey);
	bool deleteRecord(db *db, char* tableName, int primaryKey);

	//yet to implement functions

	void setPagePointer(long page,Page* ptr);

	void unPinPage(long pageNo);
	void pinPage(long pageNo);

	dbModule(int dpHeaderFile); //Done
	int getTotalNumRecords(int pagePtr);
	//bool deleteRecord(int pagePtr,const rid &rid);
	bool getRecord(long pagePtr,const rid &rid,unsigned char *(&tuplePtr)/*fill this*/, int &tupleLength/*fill this*/);
	int linearScan(int file,vector<tuple> &resultset);

	int createDpForTable();
	bool deletePagesFromDp(int pagePtr);
	void NOTMain_dbModule();
	int dbModule_verbose;

};


namespace dbModuleCoreUI{
int initDbCore();
};


#endif /* DBMODULE_H_ */
