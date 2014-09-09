#include "dbModule.h"
//extern int TABLE_ID;
dbModule::dbModule()
{
}


bool dbModule::createNewDB(char dbName[DB_NAME], long dbSize)
{

	//this should be deleted later
	bufferManager bufMan_db(FRAME_SIZE * 2);

	//TODO: wat is the extension??
	fstream fileDB(dbName,ios::out);
	fileDB.open(dbName,ios::in|ios::out|ios::binary);

	if(!fileDB.is_open())
	{
		cout<<"Error in opening file";
		//utils.myprintf(AT, "Error in opening database file");
		return 0;
	}
	//	truncate(dbName, dbSize);

	/*
	 * populate the database structure
	 */
	db* myDatabase=(db*)malloc(sizeof(LONG_SIZE + LONG_SIZE + strlen(dbName) + 1));

	myDatabase->dbIndex=1;
	myDatabase->dbName=dbName;
	myDatabase->dbFP=&fileDB;
	//myDatabase->dbFP = fp;

	/*
	 * populate dbheader
	 */
	dbHeader *DBHeader = (dbHeader*)malloc(sizeof(dbHeader));
	DBHeader->freeList = 7;


	Page* DBHeaderPage = (Page*)malloc(PAGE_SIZE);
	DBHeaderPage->priority = 0;
	DBHeaderPage->pageNumber = 0;
	DBHeaderPage->nextPage = 0;
	DBHeaderPage->CFS = 0; // after SI ? 16 bytes

	/*
	 * create a dp page for systab
	 */
	Page* sysTablePage = (Page*)malloc(PAGE_SIZE);
	sysTablePage->priority = 1;
	sysTablePage->pageNumber = 1;
	sysTablePage->nextPage = 0;
	sysTablePage->CFS = 0;

	/*
	 * dir page size will be 8192 - sizeof(pageHeader) -12
	 * size of each dirEntryList will be 12 bytes
	 */

	dirPage* sysTableDP = (dirPage*)malloc(sizeof(dirPage));
	sysTableDP->noOfCurrentDEs = 0;
	sysTableDP->ptrToNextDP = 0;
	for(int i = 0; i < DP_MAX_NOS_DES; i++)
	{
		sysTableDP->dirEntryList[i].dbPage = -1;
		sysTableDP->dirEntryList[i].TFS = 0;
	}

	//TODO: print the sysTablePage->pageData[0] and see if data is present

	memcpy(&(sysTablePage->pageData[0]),sysTableDP, PAGEDATA_SIZE);


	bufMan_db.writePage(myDatabase, sysTablePage);
	//sysTablePage and sysTableDp work is done here
	free(sysTableDP);
	free(sysTablePage);
	DBHeader->sysTablePage = 1; //pagenumber of systable

	/*
	 * create a dp page for sysCol
	 */
	Page* sysColPage = (Page*)malloc(PAGE_SIZE);
	sysColPage->priority = 1;
	sysColPage->pageNumber = 2;
	sysColPage->nextPage = 0;
	sysColPage->CFS = 0;

	/*
	 * dir page size will be 8192 - sizeof(pageHeader) -12
	 * size of each dirEntryList will be 12 bytes
	 */

	dirPage* sysColDP = (dirPage*)malloc(sizeof(dirPage));// = createDPPage();
	sysColDP->noOfCurrentDEs = 0;
	sysColDP->ptrToNextDP = 0;
	for(int i = 0; i < DP_MAX_NOS_DES; i++)
	{
		sysColDP->dirEntryList[i].dbPage = -1;
		sysColDP->dirEntryList[i].TFS = 0;
	}

	memcpy(&(sysColPage->pageData[0]), sysColDP, PAGEDATA_SIZE);

	bufMan_db.writePage(myDatabase, sysColPage);
	//sysColPage and sysColDp work is done here
	free(sysColDP);
	free(sysColPage);
	DBHeader->sysColPage = 2; //pagenumber of sysCol

	/*
	 * create a dp page for sysIndex
	 */
	Page* sysIndexPage = (Page*)malloc(PAGE_SIZE); // check func call
	sysIndexPage->priority = 1;
	sysIndexPage->pageNumber = 3;
	sysIndexPage->nextPage = 0;
	sysIndexPage->CFS = 0;

	/*
	 * dir page size will be 8192 - sizeof(pageHeader) -12
	 * size of each dirEntryList will be 12 bytes
	 */

	dirPage* sysIndexDP = (dirPage*)malloc(sizeof(dirPage));// = createDPPage();
	sysIndexDP->noOfCurrentDEs = 0;
	sysIndexDP->ptrToNextDP = 0;
	for(int i = 0; i < DP_MAX_NOS_DES; i++)
	{
		sysIndexDP->dirEntryList[i].dbPage = -1;
		sysIndexDP->dirEntryList[i].TFS = 0;
	}

	memcpy(&(sysIndexPage->pageData[0]),sysIndexDP, PAGEDATA_SIZE);

	bufMan_db.writePage(myDatabase, sysIndexPage);
	//sysIndexPage and sysIndexDp work is done here
	free(sysIndexDP);
	free(sysIndexPage);
	DBHeader->sysIndexPage = 3; //pagenumber of sysIndex

	memcpy(&(DBHeaderPage->pageData[0]), DBHeader, LONG_SIZE * 4); //freelist + systablePage + sysColPage + sysIndexPage
	bufMan_db.writePage(myDatabase, DBHeaderPage);

	/*****************************************************************************************************************
	 *****************************************************************************************************************/
	/*
	 * create a db page
	 * create a slot for each of these sys table
	 * entries and fill them
	 */

	/*
	 * noOfentries = (pagedata_size - sizeof(slotCount)) / (sizeof(all the columns) + sizeof(slotEntry))
	 * noOfentries of slot entries have to be malloced
	 */
	int noOfEntries = (PAGEDATA_SIZE - INT_SIZE) / ((INT_SIZE + LONG_SIZE + TABLE_NAME) + sizeof(slotEntry));

	char tableName[TABLE_NAME] = "sysTable";
	int sizeOfSysTable = INT_SIZE + LONG_SIZE + TABLE_NAME;
	sysTable* sysTableForSysTable = (sysTable*)malloc(sizeOfSysTable);
	sysTableForSysTable->tableId = TABLE_ID + 1;
	sysTableForSysTable->tablePage = 2; //dp of systable

	sysTableForSysTable->tableName = (char*)malloc(TABLE_NAME);
	memcpy(sysTableForSysTable->tableName, tableName, TABLE_NAME);
	SYSTABLE_COUNT = SYSTABLE_COUNT + 1;

	tuple *sysTableTuple = (tuple*)malloc(sizeOfSysTable);
	memcpy(&(sysTableTuple->recData[0]), sysTableForSysTable, sizeOfSysTable);//memcpy

	//sysTableForSysTable work is done here
	free(sysTableForSysTable);

	sysTableTuple->recLen = sizeOfSysTable;
	sysTableTuple->ptrToTableHeaderPage = -1;

	Page* sysTableDBPage = (Page*)malloc(PAGE_SIZE);   // Seg fault here
	sysTableDBPage->nextPage = -1;
	sysTableDBPage->pageNumber = 4;
	sysTableDBPage->priority = 2;


	dbPage* sysTableDB = (dbPage*)malloc(PAGEDATA_SIZE);
	sysTableDB->backPtrToDPPage = 1;
	sysTableDB->ptrToTableHeader = -1;

	int sizeOfSysTableDBTuple = sizeOfSysTable + INT_SIZE + INT_SIZE; //data+tupleid+reclen
	memcpy(&(sysTableDB->pageData[0]), sysTableTuple, sizeOfSysTableDBTuple);
	free(sysTableTuple);

	slotEntry *slotEntryForSysTable = (slotEntry*)malloc(INT_SIZE + INT_SIZE);
	slotEntryForSysTable->slotOffset = 0;
	slotEntryForSysTable->slotSize = sizeOfSysTable;

	/*
	 * for all the slotEntries, memory is malloced now only.
	 */
	slotDirectory *slotDirForSysTable = (slotDirectory*)malloc(INT_SIZE + noOfEntries * (INT_SIZE + INT_SIZE)); //slotcount + noOfentries(slot entry)
	slotDirForSysTable->slotCount = 0;

	memcpy(&(slotDirForSysTable->slotList[slotDirForSysTable->slotCount]), slotEntryForSysTable, INT_SIZE + INT_SIZE); //slotOffset + slotSize
	slotDirForSysTable->slotCount = 1;
	free(slotEntryForSysTable);

	/*
	 * in the dbPage, first slot dir + slot entry is written at = PAGEDATA_SIZE - (slotCount+slotSize+slotOffset)
	 */

	//memcpy(&(sysTableDB->pageData[PAGEDATA_SIZE - (INT_SIZE + INT_SIZE + INT_SIZE)]), slotDirForSysTable, (3*INT_SIZE)); //slotCount + slotEntry
	sysTableDBPage->CFS = INT_SIZE + INT_SIZE + sizeOfSysTableDBTuple; //backPtrToDpPage + ptrToHeaderPage

	/*****************************************************************************************************************/

	/*
	 * create a slot for each of these in sys table
	 * entries and fill them
	 */
	int sizeOfSysCol = INT_SIZE + LONG_SIZE + TABLE_NAME;//tableId + tablePage

	char tName[TABLE_NAME] = "sysCol";
	sysTable* sysTableForSysCol = (sysTable*)malloc(sizeOfSysCol);
	sysTableForSysCol->tableId = TABLE_ID + 1;
	sysTableForSysCol->tableName = (char*)malloc(TABLE_NAME);
	memcpy(sysTableForSysCol->tableName, tName, TABLE_NAME);
	sysTableForSysCol->tablePage = 2; //systable dp
	SYSTABLE_COUNT = SYSTABLE_COUNT + 1;

	tuple *sysColumnTuple = (tuple*)malloc(sizeOfSysCol + (INT_SIZE + LONG_SIZE)); //recLen + ptrToTableHeader
	sysColumnTuple->recLen = sizeOfSysCol;
	sysColumnTuple->ptrToTableHeaderPage = -1;

	memcpy(&(sysColumnTuple->recData[0]), sysTableForSysCol, sizeOfSysCol);//memcpy
	free(sysTableForSysCol);

	int sizeOfSysColumnDBTuple = sizeOfSysCol + INT_SIZE + INT_SIZE; //data+tupleid+reclen
	memcpy(&(sysTableDB->pageData[sysTableDBPage->CFS]), sysColumnTuple, sizeOfSysColumnDBTuple);
	free(sysColumnTuple);

	slotEntry *slotEntryForSysColumn = (slotEntry*)malloc(sizeof(slotEntry));
	slotEntryForSysColumn->slotOffset = sysTableDBPage->CFS;
	slotEntryForSysColumn->slotSize = sizeOfSysCol;

	memcpy(&(slotDirForSysTable->slotList[slotDirForSysTable->slotCount]), slotEntryForSysColumn, INT_SIZE + INT_SIZE); //slotsize+slotoffset
	slotDirForSysTable->slotCount += 1;
	sysTableDBPage->CFS += sizeOfSysCol;

	/*****************************************************************************************************************/

	/*
	 * create a db page
	 * create a slot for each of these sys table
	 * entries and fill them
	 */
	char tabName[TABLE_NAME] = "sysIndex";
	int sizeOfSysIndex = INT_SIZE + LONG_SIZE + TABLE_NAME;//tableId + tablePage
	sysTable* sysTableForSysIndex = (sysTable*)malloc(sizeOfSysIndex);
	sysTableForSysIndex->tableId = TABLE_ID + 1;
	sysTableForSysIndex->tableName = (char*)malloc(TABLE_NAME);
	memcpy(sysTableForSysIndex->tableName, tabName, TABLE_NAME);
	sysTableForSysIndex->tablePage = 2;//systable dp
	SYSTABLE_COUNT = SYSTABLE_COUNT + 1;

	tuple *sysIndexTuple = (tuple*)malloc(sizeof(sizeOfSysIndex + INT_SIZE + LONG_SIZE)); //recLen + ptrToTableHeader
	//int sizeOfSysIndex = 4 + 8 + strlen(sysTableForSysIndex->tableName); //tableId + tablePage + sizeof("sysTab")
	sysIndexTuple->recLen = sizeOfSysIndex;
	sysIndexTuple->ptrToTableHeaderPage = -1;

	memcpy(&(sysIndexTuple->recData[0]), sysTableForSysIndex, sizeOfSysIndex);//memcpy
	free(sysTableForSysIndex);

	int sizeOfSysIndexDBTuple = sizeOfSysIndex + INT_SIZE + INT_SIZE; //data+tupleid+reclen
	memcpy(&(sysTableDB->pageData[sysTableDBPage->CFS]), sysIndexTuple, sizeOfSysIndexDBTuple);
	free(sysIndexTuple);

	slotEntry *slotEntryForSysIndex = (slotEntry*)malloc(sizeof(slotEntry));
	slotEntryForSysIndex->slotOffset = sysTableDBPage->CFS;
	slotEntryForSysIndex->slotSize = sizeOfSysIndex;

	memcpy(&(slotDirForSysTable->slotList[slotDirForSysTable->slotCount]), slotEntryForSysColumn, INT_SIZE + INT_SIZE); //slotsize+slotoffset
	slotDirForSysTable->slotCount += 1;
	sysTableDBPage->CFS += sizeOfSysCol;

	/*
	 * now, the slotDir is written to the db page
	 * 1 slot Count + 3 slotEntries are written
	 */
	memcpy(&(sysTableDB->pageData[PAGEDATA_SIZE - (INT_SIZE + 3*(INT_SIZE + INT_SIZE))]), slotDirForSysTable, (INT_SIZE + 3*(INT_SIZE + INT_SIZE)));

	/*
	 * write the db page to the page
	 */
	memcpy(&(sysTableDBPage->pageData[0]), sysTableDB, PAGEDATA_SIZE);

	free(sysTableDBPage);

	/*****************************************************************************************************************
	 *****************************************************************************************************************/

	/*
	 * create a syscol db page
	 * make entries for systable, syscol and sysindex in this dbpage
	 */

	/*
	 * noOfentries = (pagedata_size - sizeof(slotCount)) / (sizeof(all the columns) + sizeof(slotEntry))
	 * noOfentries of slot entries have to be malloced
	 */

	int noOfEntriesForSysCol = (PAGEDATA_SIZE - INT_SIZE) / (INT_SIZE + INT_SIZE + (TABLE_NAME + INT_SIZE + INT_SIZE + INT_SIZE +
			BOOL_SIZE + BOOL_SIZE + BOOL_SIZE + VOID_POINTER_SIZE + LONG_SIZE) + sizeof(slotEntry));


	Page* sysColumnDBPage = (Page*)malloc(PAGE_SIZE);
	sysColumnDBPage->CFS = 0;
	sysColumnDBPage->nextPage = -1;
	sysColumnDBPage->pageNumber = 6;
	sysColumnDBPage->priority = 2;

	dbPage* sysColumnDB = (dbPage*)malloc(PAGEDATA_SIZE);
	sysColumnDB->backPtrToDPPage = 6;
	sysColumnDB->ptrToTableHeader = -1;

	/******************************************************************************************************************/

	/*
	 * first enter systable
	 */

	int sizeOfSysCols = INT_SIZE + INT_SIZE + 3 * (TABLE_NAME + INT_SIZE + INT_SIZE + INT_SIZE + BOOL_SIZE + BOOL_SIZE + BOOL_SIZE + VOID_POINTER_SIZE + LONG_SIZE);
	sysCols* sysColsForSysTable = (sysCols*)malloc(sizeof(sizeOfSysCols));
	sysColsForSysTable->noOfAttributes = 3;
	sysColsForSysTable->tableId = TABLE_ID + 1;

	char colName1[TABLE_NAME] = "tableId";
	sysColsForSysTable->columnValues[0].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysTable->columnValues[0].colName, colName1, TABLE_NAME);

	sysColsForSysTable->columnValues[0].colType = INTEGER;
	sysColsForSysTable->columnValues[0].defaultValue = NULL;
	sysColsForSysTable->columnValues[0].hasDefaultValue = false;
	sysColsForSysTable->columnValues[0].isIndexAttribute = false;
	sysColsForSysTable->columnValues[0].isNull = false;
	sysColsForSysTable->columnValues[0].precision = 0;
	sysColsForSysTable->columnValues[0].sizeOfAttribute = INT_SIZE;
	sysColsForSysTable->columnValues[0].tablePage = 3; //syscol dp page

	char colName2[TABLE_NAME] = "tablePage";
	sysColsForSysTable->columnValues[1].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysTable->columnValues[1].colName, colName2, TABLE_NAME);
	sysColsForSysTable->columnValues[1].colType = LONG;
	sysColsForSysTable->columnValues[1].defaultValue = NULL;
	sysColsForSysTable->columnValues[1].hasDefaultValue = false;
	sysColsForSysTable->columnValues[1].isIndexAttribute = false;
	sysColsForSysTable->columnValues[1].isNull = false;
	sysColsForSysTable->columnValues[1].precision = 0;
	sysColsForSysTable->columnValues[1].sizeOfAttribute = LONG_SIZE;
	sysColsForSysTable->columnValues[1].tablePage = 3; //syscol dp page


	char colName3[TABLE_NAME] = "tableName";
	sysColsForSysTable->columnValues[2].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysTable->columnValues[2].colName, colName3, TABLE_NAME);
	sysColsForSysTable->columnValues[2].colType = CHAR;
	sysColsForSysTable->columnValues[2].defaultValue = NULL;
	sysColsForSysTable->columnValues[2].hasDefaultValue = false;
	sysColsForSysTable->columnValues[2].isIndexAttribute = false;
	sysColsForSysTable->columnValues[2].isNull = false;
	sysColsForSysTable->columnValues[2].precision = 0;
	sysColsForSysTable->columnValues[2].sizeOfAttribute = INT_SIZE;
	sysColsForSysTable->columnValues[2].tablePage = 3; //syscol dp page

	int sysColForSysTableTupleSize = INT_SIZE + INT_SIZE + sizeof(sysColsForSysTable->columnValues[0]) + sizeof(sysColsForSysTable->columnValues[1])
		                                																								 + sizeof(sysColsForSysTable->columnValues[2]);

	tuple* sysColForSysTableTuple = (tuple*)malloc(sysColForSysTableTupleSize);
	sysColForSysTableTuple->ptrToTableHeaderPage = -1;

	sysColForSysTableTuple->recLen = sysColForSysTableTupleSize;
	memcpy(&(sysColForSysTableTuple->recData[0]), sysColsForSysTable, sysColForSysTableTupleSize);
	free(sysColsForSysTable->columnValues[0].colName);
	free(sysColsForSysTable->columnValues[1].colName);
	free(sysColsForSysTable->columnValues[2].colName);
	free(sysColsForSysTable);

	memcpy(&(sysColumnDB->pageData[0]), sysColForSysTableTuple, sysColForSysTableTupleSize);
	free(sysColForSysTableTuple);

	sysColumnDBPage->CFS = sysColForSysTableTupleSize;

	slotEntry* slotEntryForSysColsForSysTable = (slotEntry*)malloc(sizeof(slotEntry));
	slotEntryForSysColsForSysTable->slotOffset = sysColumnDBPage->CFS;
	slotEntryForSysColsForSysTable->slotSize = sysColForSysTableTupleSize;

	slotDirectory* slotDirForSysCols = (slotDirectory*)malloc(INT_SIZE + noOfEntriesForSysCol * (INT_SIZE + INT_SIZE)); //slotcount + noOfentries(slot entry)
	slotDirForSysCols->slotCount = 0;
	memcpy(&(slotDirForSysCols->slotList[slotDirForSysCols->slotCount]), slotEntryForSysColsForSysTable, (INT_SIZE + INT_SIZE));
	slotDirForSysCols->slotCount += 1;
	free(slotEntryForSysColsForSysTable);

	//memcpy(&(sysColumnDB->pageData[8159 - (INT_SIZE + INT_SIZE)]), slotDirForSysCols, 12);

	/******************************************************************************************************************/

	/*
	 * next enter syscol
	 */
	int sizeOfSysColsForSysCols = INT_SIZE + INT_SIZE + 11 * (TABLE_NAME + INT_SIZE + INT_SIZE + INT_SIZE + BOOL_SIZE + BOOL_SIZE + BOOL_SIZE + VOID_POINTER_SIZE + LONG_SIZE);

	sysCols* sysColsForSysCols = (sysCols*)malloc(sizeOfSysColsForSysCols);
	sysColsForSysCols->noOfAttributes = 11;
	sysColsForSysCols->tableId = TABLE_ID + 1;

	char colName4[TABLE_NAME] = "tableId";
	sysColsForSysCols->columnValues[0].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysCols->columnValues[0].colName, colName4, TABLE_NAME);
	sysColsForSysCols->columnValues[0].colType = INTEGER;
	sysColsForSysCols->columnValues[0].defaultValue = NULL;
	sysColsForSysCols->columnValues[0].hasDefaultValue = false;
	sysColsForSysCols->columnValues[0].isIndexAttribute = false;
	sysColsForSysCols->columnValues[0].isNull = false;
	sysColsForSysCols->columnValues[0].precision = 0;
	sysColsForSysCols->columnValues[0].sizeOfAttribute = INT_SIZE;
	sysColsForSysCols->columnValues[0].tablePage = 3; //syscol dp page

	char colName26[TABLE_NAME] = "noOfAttributes";
	sysColsForSysCols->columnValues[1].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysCols->columnValues[1].colName, colName26, TABLE_NAME);
	sysColsForSysCols->columnValues[1].colType = INTEGER;
	sysColsForSysCols->columnValues[1].defaultValue = NULL;
	sysColsForSysCols->columnValues[1].hasDefaultValue = false;
	sysColsForSysCols->columnValues[1].isIndexAttribute = false;
	sysColsForSysCols->columnValues[1].isNull = false;
	sysColsForSysCols->columnValues[1].precision = 0;
	sysColsForSysCols->columnValues[1].sizeOfAttribute = INT_SIZE;
	sysColsForSysCols->columnValues[1].tablePage = 3; //syscol dp page

	char colName5[TABLE_NAME] = "colVal.colName";
	sysColsForSysCols->columnValues[2].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysCols->columnValues[2].colName, colName5, TABLE_NAME);
	sysColsForSysCols->columnValues[2].colType = CHAR;
	sysColsForSysCols->columnValues[2].defaultValue = NULL;
	sysColsForSysCols->columnValues[2].hasDefaultValue = false;
	sysColsForSysCols->columnValues[2].isIndexAttribute = false;
	sysColsForSysCols->columnValues[2].isNull = false;
	sysColsForSysCols->columnValues[2].precision = 0;
	sysColsForSysCols->columnValues[2].sizeOfAttribute = INT_SIZE;
	sysColsForSysCols->columnValues[2].tablePage = 3; //syscol dp page

	char colName6[TABLE_NAME] = "colVal.colType";
	sysColsForSysCols->columnValues[3].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysCols->columnValues[3].colName, colName6, TABLE_NAME);
	sysColsForSysCols->columnValues[3].colType = INTEGER;
	sysColsForSysCols->columnValues[3].defaultValue = NULL;
	sysColsForSysCols->columnValues[3].hasDefaultValue = false;
	sysColsForSysCols->columnValues[3].isIndexAttribute = false;
	sysColsForSysCols->columnValues[3].isNull = false;
	sysColsForSysCols->columnValues[3].precision = 0;
	sysColsForSysCols->columnValues[3].sizeOfAttribute = INT_SIZE;
	sysColsForSysCols->columnValues[3].tablePage = 3; //syscol dp page

	char colName7[TABLE_NAME] = "colVal.defaultValue";
	sysColsForSysCols->columnValues[4].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysCols->columnValues[4].colName, colName7, TABLE_NAME);
	sysColsForSysCols->columnValues[4].colType = VOID;
	sysColsForSysCols->columnValues[4].defaultValue = NULL;
	sysColsForSysCols->columnValues[4].hasDefaultValue = false;
	sysColsForSysCols->columnValues[4].isIndexAttribute = false;
	sysColsForSysCols->columnValues[4].isNull = false;
	sysColsForSysCols->columnValues[4].precision = 0;
	sysColsForSysCols->columnValues[4].sizeOfAttribute = INT_SIZE;
	sysColsForSysCols->columnValues[4].tablePage = 3; //syscol dp page

	char colName8[TABLE_NAME] = "colVal.hasDefVal";
	sysColsForSysCols->columnValues[5].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysCols->columnValues[5].colName, colName8, TABLE_NAME);
	sysColsForSysCols->columnValues[5].colType = BOOL;
	sysColsForSysCols->columnValues[5].defaultValue = NULL;
	sysColsForSysCols->columnValues[5].hasDefaultValue = false;
	sysColsForSysCols->columnValues[5].isIndexAttribute = false;
	sysColsForSysCols->columnValues[5].isNull = false;
	sysColsForSysCols->columnValues[5].precision = 0;
	sysColsForSysCols->columnValues[5].sizeOfAttribute = BOOL_SIZE;
	sysColsForSysCols->columnValues[5].tablePage = 3; //syscol dp page

	char colName9[TABLE_NAME] = "colVal.isIndexAttr";
	sysColsForSysCols->columnValues[6].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysCols->columnValues[6].colName, colName9, TABLE_NAME);
	sysColsForSysCols->columnValues[6].colType = BOOL;
	sysColsForSysCols->columnValues[6].defaultValue = NULL;
	sysColsForSysCols->columnValues[6].hasDefaultValue = false;
	sysColsForSysCols->columnValues[6].isIndexAttribute = false;
	sysColsForSysCols->columnValues[6].isNull = false;
	sysColsForSysCols->columnValues[6].precision = 0;
	sysColsForSysCols->columnValues[6].sizeOfAttribute = BOOL_SIZE;
	sysColsForSysCols->columnValues[6].tablePage = 3; //syscol dp page

	char colName10[TABLE_NAME] = "colVal.isNull";
	sysColsForSysCols->columnValues[7].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysCols->columnValues[7].colName, colName10, TABLE_NAME);
	sysColsForSysCols->columnValues[7].colType = BOOL;
	sysColsForSysCols->columnValues[7].defaultValue = NULL;
	sysColsForSysCols->columnValues[7].hasDefaultValue = false;
	sysColsForSysCols->columnValues[7].isIndexAttribute = false;
	sysColsForSysCols->columnValues[7].isNull = false;
	sysColsForSysCols->columnValues[7].precision = 0;
	sysColsForSysCols->columnValues[7].sizeOfAttribute = BOOL_SIZE;
	sysColsForSysCols->columnValues[7].tablePage = 3; //syscol dp page

	char colName11[TABLE_NAME] = "colVal.precision";
	sysColsForSysCols->columnValues[8].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysCols->columnValues[8].colName, colName11, TABLE_NAME);
	sysColsForSysCols->columnValues[8].colType = INTEGER;
	sysColsForSysCols->columnValues[8].defaultValue = NULL;
	sysColsForSysCols->columnValues[8].hasDefaultValue = false;
	sysColsForSysCols->columnValues[8].isIndexAttribute = false;
	sysColsForSysCols->columnValues[8].isNull = false;
	sysColsForSysCols->columnValues[8].precision = 0;
	sysColsForSysCols->columnValues[8].sizeOfAttribute = INT_SIZE;
	sysColsForSysCols->columnValues[8].tablePage = 3; //syscol dp page

	char colName12[TABLE_NAME] = "colVal.sizeOfAttr";
	sysColsForSysCols->columnValues[9].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysCols->columnValues[9].colName, colName12, TABLE_NAME);
	sysColsForSysCols->columnValues[9].colType = INTEGER;
	sysColsForSysCols->columnValues[9].defaultValue = NULL;
	sysColsForSysCols->columnValues[9].hasDefaultValue = false;
	sysColsForSysCols->columnValues[9].isIndexAttribute = false;
	sysColsForSysCols->columnValues[9].isNull = false;
	sysColsForSysCols->columnValues[9].precision = 0;
	sysColsForSysCols->columnValues[9].sizeOfAttribute = INT_SIZE;
	sysColsForSysCols->columnValues[9].tablePage = 3; //syscol dp page

	char colName13[TABLE_NAME] = "colVal.tablePage";
	sysColsForSysCols->columnValues[10].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysCols->columnValues[10].colName, colName13, TABLE_NAME);
	sysColsForSysCols->columnValues[10].colType = LONG;
	sysColsForSysCols->columnValues[10].defaultValue = NULL;
	sysColsForSysCols->columnValues[10].hasDefaultValue = false;
	sysColsForSysCols->columnValues[10].isIndexAttribute = false;
	sysColsForSysCols->columnValues[10].isNull = false;
	sysColsForSysCols->columnValues[10].precision = 0;
	sysColsForSysCols->columnValues[10].sizeOfAttribute = LONG_SIZE;
	sysColsForSysCols->columnValues[10].tablePage = 3; //syscol dp page

	tuple* sysColsForSysColsTuple = (tuple*)malloc(sizeof(tuple));
	sysColsForSysColsTuple->ptrToTableHeaderPage = -1;
	int sysColsForSysColsTupleSize = 0;

	for(int ii = 0; ii < 11; ii++)
	{
		sysColsForSysColsTupleSize += sysColsForSysColsTupleSize + sizeof(sysColsForSysCols->columnValues[ii]);
	}
	sysColsForSysColsTuple->recLen = sysColsForSysColsTupleSize; //char* + void*
	memcpy(&(sysColsForSysColsTuple->recData), sysColsForSysCols, sysColsForSysColsTupleSize);
	for(int jj = 0; jj < 11; jj++)
	{
		free(sysColsForSysCols->columnValues[jj].colName);
	}
	free(sysColsForSysCols);

	memcpy(&(sysColumnDB->pageData[sysColumnDBPage->CFS]), sysColsForSysColsTuple, sysColsForSysColsTupleSize);
	free(sysColsForSysColsTuple);
	sysColumnDBPage->CFS += sysColsForSysColsTupleSize;

	slotEntry* seForSysColsForSysCols = (slotEntry*)malloc(sizeof(slotEntry));
	seForSysColsForSysCols->slotOffset = sysColumnDBPage->CFS;
	seForSysColsForSysCols->slotSize = sysColsForSysColsTupleSize;

	memcpy(&(slotDirForSysCols->slotList[slotDirForSysCols->slotCount]), seForSysColsForSysCols, sysColsForSysColsTupleSize);
	free(seForSysColsForSysCols);
	slotDirForSysCols->slotCount += 1;

	//TODO: put this last memcpy(&(sysColumnDB->pageData[8159 - 2 * (INT_SIZE + INT_SIZE)]), slotDirForSysColsForSysTable, 12);

	/******************************************************************************************************************/

	/*
	 * next enter syscol
	 */

	int sizeOfSysColsForSysIndex = INT_SIZE + INT_SIZE + 12 * (TABLE_NAME + INT_SIZE + INT_SIZE + INT_SIZE
			+ BOOL_SIZE + BOOL_SIZE + BOOL_SIZE + VOID_POINTER_SIZE + LONG_SIZE);

	sysCols* sysColsForSysIndex = (sysCols*)malloc(sizeOfSysColsForSysIndex);
	sysColsForSysIndex->noOfAttributes = 12;
	sysColsForSysIndex->tableId = TABLE_ID + 1;

	char colName14[TABLE_NAME] = "indexID";
	sysColsForSysIndex->columnValues[0].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysIndex->columnValues[0].colName, colName14, TABLE_NAME);
	sysColsForSysIndex->columnValues[0].colType = INTEGER;
	sysColsForSysIndex->columnValues[0].defaultValue = NULL;
	sysColsForSysIndex->columnValues[0].hasDefaultValue = false;
	sysColsForSysIndex->columnValues[0].isIndexAttribute = false;
	sysColsForSysIndex->columnValues[0].isNull = false;
	sysColsForSysIndex->columnValues[0].precision = 0;
	sysColsForSysIndex->columnValues[0].sizeOfAttribute = INT_SIZE;
	sysColsForSysIndex->columnValues[0].tablePage = 3; //sysdp page num

	char colName15[TABLE_NAME] = "tableID";
	sysColsForSysIndex->columnValues[1].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysIndex->columnValues[1].colName, colName15, TABLE_NAME);
	sysColsForSysIndex->columnValues[1].colType = INTEGER;
	sysColsForSysIndex->columnValues[1].defaultValue = NULL;
	sysColsForSysIndex->columnValues[1].hasDefaultValue = false;
	sysColsForSysIndex->columnValues[1].isIndexAttribute = false;
	sysColsForSysIndex->columnValues[1].isNull = false;
	sysColsForSysIndex->columnValues[1].precision = 0;
	sysColsForSysIndex->columnValues[1].sizeOfAttribute = INT_SIZE;
	sysColsForSysIndex->columnValues[1].tablePage = 3; //sysdp page num

	char colName16[TABLE_NAME] = "tableDirPage";
	sysColsForSysIndex->columnValues[2].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysIndex->columnValues[2].colName, colName16, TABLE_NAME);
	sysColsForSysIndex->columnValues[2].colType = LONG;
	sysColsForSysIndex->columnValues[2].defaultValue = NULL;
	sysColsForSysIndex->columnValues[2].hasDefaultValue = false;
	sysColsForSysIndex->columnValues[2].isIndexAttribute = false;
	sysColsForSysIndex->columnValues[2].isNull = false;
	sysColsForSysIndex->columnValues[2].precision = 0;
	sysColsForSysIndex->columnValues[2].sizeOfAttribute = LONG_SIZE;
	sysColsForSysIndex->columnValues[2].tablePage = 3; //sysdp page num

	char colName17[TABLE_NAME] = "colCountInKey";
	sysColsForSysIndex->columnValues[3].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysIndex->columnValues[3].colName, colName17, TABLE_NAME);
	sysColsForSysIndex->columnValues[3].colType = INTEGER;
	sysColsForSysIndex->columnValues[3].defaultValue = NULL;
	sysColsForSysIndex->columnValues[3].hasDefaultValue = false;
	sysColsForSysIndex->columnValues[3].isIndexAttribute = false;
	sysColsForSysIndex->columnValues[3].isNull = false;
	sysColsForSysIndex->columnValues[3].precision = 0;
	sysColsForSysIndex->columnValues[3].sizeOfAttribute = INT_SIZE;
	sysColsForSysIndex->columnValues[3].tablePage = 3; //sysdp page num

	char colName18[TABLE_NAME] = "colType";
	sysColsForSysIndex->columnValues[4].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysIndex->columnValues[4].colName, colName18, TABLE_NAME);
	sysColsForSysIndex->columnValues[4].colType = CHAR;
	sysColsForSysIndex->columnValues[4].defaultValue = NULL;
	sysColsForSysIndex->columnValues[4].hasDefaultValue = false;
	sysColsForSysIndex->columnValues[4].isIndexAttribute = false;
	sysColsForSysIndex->columnValues[4].isNull = false;
	sysColsForSysIndex->columnValues[4].precision = 0;
	sysColsForSysIndex->columnValues[4].sizeOfAttribute = CHAR_SIZE;
	sysColsForSysIndex->columnValues[4].tablePage = 3; //sysdp page num

	char colName19[TABLE_NAME] = "part1";
	sysColsForSysIndex->columnValues[6].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysIndex->columnValues[6].colName, colName19, TABLE_NAME);
	sysColsForSysIndex->columnValues[5].colType = CHAR;
	sysColsForSysIndex->columnValues[5].defaultValue = NULL;
	sysColsForSysIndex->columnValues[5].hasDefaultValue = false;
	sysColsForSysIndex->columnValues[5].isIndexAttribute = false;
	sysColsForSysIndex->columnValues[5].isNull = false;
	sysColsForSysIndex->columnValues[5].precision = 0;
	sysColsForSysIndex->columnValues[5].sizeOfAttribute = INT_SIZE;
	sysColsForSysIndex->columnValues[5].tablePage = 3; //sysdp page num

	char colName20[TABLE_NAME] = "part2";
	sysColsForSysIndex->columnValues[6].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysIndex->columnValues[6].colName, colName20, TABLE_NAME);
	sysColsForSysIndex->columnValues[6].colType = CHAR;
	sysColsForSysIndex->columnValues[6].defaultValue = NULL;
	sysColsForSysIndex->columnValues[6].hasDefaultValue = false;
	sysColsForSysIndex->columnValues[6].isIndexAttribute = false;
	sysColsForSysIndex->columnValues[6].isNull = false;
	sysColsForSysIndex->columnValues[6].precision = 0;
	sysColsForSysIndex->columnValues[6].sizeOfAttribute = INT_SIZE;
	sysColsForSysIndex->columnValues[6].tablePage = 3; //sysdp page num

	char colName21[TABLE_NAME] = "part3";
	sysColsForSysIndex->columnValues[7].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysIndex->columnValues[7].colName, colName21, TABLE_NAME);
	sysColsForSysIndex->columnValues[7].colType = CHAR;
	sysColsForSysIndex->columnValues[7].defaultValue = NULL;
	sysColsForSysIndex->columnValues[7].hasDefaultValue = false;
	sysColsForSysIndex->columnValues[7].isIndexAttribute = false;
	sysColsForSysIndex->columnValues[7].isNull = false;
	sysColsForSysIndex->columnValues[7].precision = 0;
	sysColsForSysIndex->columnValues[7].sizeOfAttribute = INT_SIZE;
	sysColsForSysIndex->columnValues[7].tablePage = 3; //sysdp page num

	char colName22[TABLE_NAME] = "part4";
	sysColsForSysIndex->columnValues[8].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysIndex->columnValues[8].colName, colName22, TABLE_NAME);
	sysColsForSysIndex->columnValues[8].colType = CHAR;
	sysColsForSysIndex->columnValues[8].defaultValue = NULL;
	sysColsForSysIndex->columnValues[8].hasDefaultValue = false;
	sysColsForSysIndex->columnValues[8].isIndexAttribute = false;
	sysColsForSysIndex->columnValues[8].isNull = false;
	sysColsForSysIndex->columnValues[8].precision = 0;
	sysColsForSysIndex->columnValues[8].sizeOfAttribute = INT_SIZE;
	sysColsForSysIndex->columnValues[8].tablePage = 3; //sysdp page num

	char colName23[TABLE_NAME] = "part5";
	sysColsForSysIndex->columnValues[9].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysIndex->columnValues[9].colName, colName23, TABLE_NAME);
	sysColsForSysIndex->columnValues[9].colType = CHAR;
	sysColsForSysIndex->columnValues[9].defaultValue = NULL;
	sysColsForSysIndex->columnValues[9].hasDefaultValue = false;
	sysColsForSysIndex->columnValues[9].isIndexAttribute = false;
	sysColsForSysIndex->columnValues[9].isNull = false;
	sysColsForSysIndex->columnValues[9].precision = 0;
	sysColsForSysIndex->columnValues[9].sizeOfAttribute = INT_SIZE;
	sysColsForSysIndex->columnValues[9].tablePage = 3; //sysdp page num

	char colName24[TABLE_NAME] = "ptrToRootOfTree";
	sysColsForSysIndex->columnValues[10].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysIndex->columnValues[10].colName, colName24, TABLE_NAME);
	sysColsForSysIndex->columnValues[10].colType = LONG;
	sysColsForSysIndex->columnValues[10].defaultValue = NULL;
	sysColsForSysIndex->columnValues[10].hasDefaultValue = false;
	sysColsForSysIndex->columnValues[10].isIndexAttribute = false;
	sysColsForSysIndex->columnValues[10].isNull = false;
	sysColsForSysIndex->columnValues[10].precision = 0;
	sysColsForSysIndex->columnValues[10].sizeOfAttribute = LONG_SIZE;
	sysColsForSysIndex->columnValues[10].tablePage = 3; //sysdp page num

	char colName25[TABLE_NAME] = "isAscOrDesc";
	sysColsForSysIndex->columnValues[11].colName = (char*)malloc(TABLE_NAME);
	memcpy(sysColsForSysIndex->columnValues[11].colName, colName25, TABLE_NAME);

	sysColsForSysIndex->columnValues[11].colName = "isAscOrDesc";
	sysColsForSysIndex->columnValues[11].colType = INTEGER;
	sysColsForSysIndex->columnValues[11].defaultValue = NULL;
	sysColsForSysIndex->columnValues[11].hasDefaultValue = false;
	sysColsForSysIndex->columnValues[11].isIndexAttribute = false;
	sysColsForSysIndex->columnValues[11].isNull = false;
	sysColsForSysIndex->columnValues[11].precision = 0;
	sysColsForSysIndex->columnValues[11].sizeOfAttribute = INT_SIZE;
	sysColsForSysIndex->columnValues[11].tablePage = 3; //sysdp page num


	tuple* sysColsForSysIndexTuple = (tuple*)malloc(sizeof(tuple));
	sysColsForSysIndexTuple->ptrToTableHeaderPage = -1;
	int sysColsForSysIndexTupleSize = 0;

	for(int iii = 0; iii < 11; iii++)
	{
		sysColsForSysIndexTupleSize += sysColsForSysIndexTupleSize + sizeof(sysColsForSysIndex->columnValues[iii]);
	}
	sysColsForSysIndexTuple->recLen = sysColsForSysIndexTupleSize + 24; //char* + void* for 12 fields
	memcpy(&(sysColsForSysIndexTuple->recData), sysColsForSysIndex, sysColsForSysIndexTupleSize);
	for(int kk = 0; kk < 11; kk++)
	{
		free(sysColsForSysIndex->columnValues[kk].colName);
	}
	free(sysColsForSysIndex);

	memcpy(&(sysColumnDB->pageData[sysColumnDBPage->CFS]), sysColsForSysIndexTuple, sysColsForSysIndexTupleSize);
	free(sysColsForSysIndexTuple);
	sysColumnDBPage->CFS += sysColsForSysIndexTupleSize;

	slotEntry* seForSysColsForSysIndex = (slotEntry*)malloc(sizeof(slotEntry*));
	seForSysColsForSysIndex->slotOffset = sysColumnDBPage->CFS;
	seForSysColsForSysIndex->slotSize = sysColsForSysIndexTupleSize;

	memcpy(&(slotDirForSysCols->slotList[slotDirForSysCols->slotCount]), seForSysColsForSysIndex, sysColsForSysIndexTupleSize);
	free(seForSysColsForSysIndex);
	slotDirForSysCols->slotCount += 1;

	memcpy(&(sysColumnDB->pageData[PAGEDATA_SIZE - (INT_SIZE + 3*(INT_SIZE + INT_SIZE))]), slotDirForSysCols, (INT_SIZE + 3*(INT_SIZE + INT_SIZE)));

	memcpy(&(sysColumnDBPage->pageData[0]), sysColumnDB, PAGEDATA_SIZE);

	/******************************************************************************************************************/

	//finally the dbheader is written
	memcpy(&(DBHeaderPage->pageData[0]), DBHeader, 16);
	free(DBHeader);

	//close the fstream
	fileDB.close();

}

bool dbModule::useDatabase(db* database)
{

	/*
	 * open the file again as it is closed once it is created
	 * check if a file with dbName exists, only then open it
	 * if it does not exist, then such a database was not created
	 */
	fstream fileDB(database->dbName,ios::out);
	if(access(database->dbName, F_OK) != -1)
	{

		fileDB.open(database->dbName,ios::in|ios::out|ios::binary);
	}
	else
	{
		cout << "Database with that name was not created" << endl;
		return FAILURE;
	}
	/*
	 * TODO: check if the file is already opened
	 */
	if(database->dbFP->is_open())
	{
		return FAILURE;
	}

	return SUCCESS;
}



int main()
{
	dbModule db;
	//db.dbModule();
	cout << "hiosdf";
	db.createNewDB("Pani", 100*1024*1024);
	//db.useDatabase("Pani");

}


