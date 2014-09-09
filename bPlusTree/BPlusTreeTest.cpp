#include "BPTPage.h"
#include "BPlusTreeC.h"
#include "BPlusTreeUI.h"

using namespace BPlusTreeNS;

BPlusTreeC * myBPTObj;
sysIndex * propertyObj;
bufferManager* bufManager;
BPTPage *bufManObj;

void TEST1_initializeBPlusTree() {
	UImenu();
}

void TEST2_createInternalNode() {
	/*
	 internalNode *n1, *n2, *n3, *n4;
	 myBPTObj->createInternalNode(&n1);
	 myBPTObj->createInternalNode(&n2);
	 myBPTObj->createInternalNode(&n3);
	 myBPTObj->createInternalNode(&n4);
	 */
}

int main() {

	myBPTObj = new BPlusTreeC;
	bufManObj = new BPTPage;
	//======================================================================================================================

	char* databaseName = (char*) malloc(DB_NAME);
	strncpy(databaseName, "sreenivasa.db", DB_NAME);

	db * testDB = (db*) malloc(sizeof(database));
	testDB->dbIndex = 1;
	strncpy(testDB->dbName, databaseName, DB_NAME);

	free(databaseName);

	fstream f(testDB->dbName, ios::in | ios::out | ios::binary);
	if (!f.is_open()) {
		cout << "Error in opening file";
		return -1;
	}

	//=======================================================================================================================
	// For my testing, property obj is getting populated in this function !
	// i.e. you get to know the type of database index that you need to create
	propertyObj = (sysIndex*) malloc(sizeof(sysIndex));
	TEST1_initializeBPlusTree();
	//=======================================================================================================================

	myBPTObj->initBPlusTree(testDB, propertyObj); // the rest of the fields get initialized here


	//=======================================================================================================================
	// For my Testing, all the tuples get generated here
	cout << endl << "Enter the values for the records ";

	char choice = 'y';

	while (choice == 'y') {
		char * generatedKey = (char *) malloc(myBPTObj->getKeyUnitSize());
		tupleSubMenu(propertyObj->compositeKeyParts, generatedKey);

		//-------------------------------------------------------------------------------------------------
		block b;
		b.pageNo = 99;
		b.slotNo = 99;

		myBPTObj->insert_key_and_record(generatedKey, b); // if the root is not initialized, it will return false.
		free(generatedKey);
		//--------------------------------------------------------------------------------------------------
		cout << "Do you wish to enter more records ( y or n ) ";
		cin >> choice;

		if (choice == 'y') {
			char * generatedKey = (char *) malloc(myBPTObj->getKeyUnitSize());
			tupleSubMenu(propertyObj->compositeKeyParts, generatedKey);
			free(generatedKey);
		}
		printKey(propertyObj->compositeKeyParts, generatedKey);
	}

	//=======================================================================================================================


	free(testDB);
	free(propertyObj);
	return 0;
}
