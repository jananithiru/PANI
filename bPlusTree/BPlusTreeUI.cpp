/*
 * BPlusTreeUI.cpp
 *
 *  Created on: 22-Nov-2011
 *      Author: janani
 */

#include "BPlusTreeC.h"
using namespace BPlusTreeNS;

extern BPlusTreeC * myBPTObj;
extern sysIndex * propertyObj;

int subMenu(int a) {
	int attributeSize;
	switch (a) {
	case 0:
		return 4;
	case 1:
		return 4;
	case 2:
		cout << endl << "Enter the maximum size of the datatype :  ";
		cin >> attributeSize;
		/*if (isalpha(attributeSize))
			subMenu(5);*/
		return attributeSize;
	case 3:
		cout << endl << "Enter the maximum size of the datatype :  ";
		cin >> attributeSize;
		/*if (isalpha(attributeSize))
			subMenu(5);*/
		return attributeSize;
	default:
		cout << endl << "Invalid Choice! Please re-enter your choice.";
		cin >> attributeSize;
		/*if (isalpha(attributeSize))
			subMenu(5);*/
		subMenu(attributeSize);

	}
	return 0;
}

void UImenu() {

	propertyObj->indexID = 123; // globIndexCount++;
	propertyObj->tableID = 123;
	propertyObj->isAsc = true; // false if desc
	propertyObj->ptrToPageContainingRootofTree = 10;  // getFreePage();
	propertyObj->tableDirPage = 20;  // getTableDirPage();

	keyPart k[MAX_NO_OF_COLS_INDEXED];

	cout << endl
			<< "------------------------------------------------------------BPLUSTREE-------------------------------------------------";
	cout << endl
			<< "Index Creation: Mention the attribute names & attribute types ";
	cout << endl
			<< "-------------------------------------------------------------------------------------------------------"
			<< endl;
	cout
			<< "Enter the number of fields on which to create the BPlusTree on :  ";


	cin >> propertyObj->numOfAttributes;

	cout << endl << "Enter the names/types of fields!!";

	for (int count = 0; count < propertyObj->numOfAttributes; count++) {
		cout << endl << "Enter the NAME for column `" << count << "` :  ";
		cin >> k[count].attributeName;
		cout << endl << "Enter the DATA TYPE for column `" << count << "` :  ";
		cout << endl << " 0 - INTEGER \n 1 - FLOAT \n 2 - CHAR \n 3 - VARCHAR ";
		cout << endl << "Enter your choice :  ";
		cin >> k[count].attributeType;

		k[count].attributeSize = subMenu(k[count].attributeType);
	}

	//copying keyPart object
	memcpy(&propertyObj->compositeKeyParts, &k, (MAX_NO_OF_COLS_INDEXED
			* sizeof(keyPart)));



}// UI Menu

void printKey(keyPart* addrOfK, char* generatedKey)
{
	cout << endl
			<< "---------------------------------Entering print mode----------------------------------------------------------------------";
	int noOfBytesCopied = 0;
	char * te1, *te2;

	cout << endl << endl;

	for (int idx = 0; idx < propertyObj->numOfAttributes; idx++) {

		switch (addrOfK[idx].attributeType) {
		case 0:
			int t;
			memcpy(&t, &generatedKey[noOfBytesCopied],
					addrOfK[idx].attributeSize);
			noOfBytesCopied += addrOfK[idx].attributeSize;
			cout << "\t" << t;
			break;

		case 1:
			float te;
			memcpy(&te, &generatedKey[noOfBytesCopied],
					addrOfK[idx].attributeSize);
			noOfBytesCopied += addrOfK[idx].attributeSize;
			cout << "\t" << te;
			break;

		case 2:
			te1 = (char*) malloc(addrOfK[idx].attributeSize);
			memcpy(te1, &generatedKey[noOfBytesCopied],
					addrOfK[idx].attributeSize);
			noOfBytesCopied += addrOfK[idx].attributeSize;

			cout << "\t" << te1;

			free(te1); // SEE if you may need it while doing key comp
			break;

		case 3:
			te2 = (char*) malloc(addrOfK[idx].attributeSize);
			memcpy(te2, &generatedKey[noOfBytesCopied],
					addrOfK[idx].attributeSize);
			noOfBytesCopied += addrOfK[idx].attributeSize;

			cout << "\t" << te2;
			free(te2); // SEE if you may need it while doing key comp
			break;

		}
	}
	cout << endl
			<< "---------------------------------Quitting print mode----------------------------------------------------------------------";
	return;

}


void tupleSubMenu(keyPart* addrOfK, char* generatedKey) {

	int noOfBytesCopied = 0;

	char *temp2, *temp3;

	for (int idx = 0; idx < propertyObj->numOfAttributes; idx++) {

		int type = addrOfK[idx].attributeType;

		switch (type) {
		case 0:
			int tem;
			cout << "Enter the INTEGER value for the `"
					<< addrOfK[idx].attributeName << "` Column :  ";
			cin >> tem;
			memcpy(&generatedKey[noOfBytesCopied], &tem,
					addrOfK[idx].attributeSize);
			noOfBytesCopied += addrOfK[idx].attributeSize;
			break;

		case 1:
			float te;
			cout << "Enter the FLOAT value for the `"
					<< addrOfK[idx].attributeName << "` Column :  ";
			cin >> te;
			memcpy(&generatedKey[noOfBytesCopied], &te,
					addrOfK[idx].attributeSize);
			noOfBytesCopied += addrOfK[idx].attributeSize;
			break;

		case 2:
			temp2 = (char*) malloc(addrOfK[idx].attributeSize);
			cout << "Enter the STRING value for the `"
					<< addrOfK[idx].attributeName << "` Column :  ";
			cin >> temp2;
			fflush(stdin);
			memcpy(&generatedKey[noOfBytesCopied], temp2,
					addrOfK[idx].attributeSize);
			noOfBytesCopied += addrOfK[idx].attributeSize;
			free(temp2);
			break;

		case 3:
			temp3 = (char*) malloc(addrOfK[idx].attributeSize);

			cout << "Enter the VARCHAR value for the `"
					<< addrOfK[idx].attributeName << "` Column :  ";
			cin >> temp3;
			fflush(stdin);
			memcpy(&generatedKey[noOfBytesCopied], temp3,
					addrOfK[idx].attributeSize);
			noOfBytesCopied += addrOfK[idx].attributeSize;

			free(temp3);
			break;
		}

	} // end of for. This means that one full tuples should have generated

	return ;
}

