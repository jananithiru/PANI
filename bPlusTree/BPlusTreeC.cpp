/** BPlusTreeC.cpp
 *
 *  Created on: 20-Nov-2011
 *      Author: janani
 */

#include "BPlusTreeC.h"
#include "BPTPage.h"
#include "BPTPage.h"
//#include "../bufferManager/bufferManager.h"
extern sysIndex* propertyObj;
extern bufferManager* bufManager;
extern BPTPage* bufManObj;
//------------------------------------------------------------------------------------------------------------------------------------------------------
namespace BPlusTreeNS {

BPlusTreeC::BPlusTreeC() {
	// TODO Auto-generated constructor stub
}
//------------------------------------------------------------------------------------------------------------------------------------------------------
BPlusTreeC::~BPlusTreeC() {
	// TODO Auto-generated destructor stub
}
void BPlusTreeC::printInternalNode(internalNode *n) {

	cout << endl << "---------------------------------------------------------";

	cout << endl << "Printing out INTERNAL Node contents";
	cout << endl << "level" << "\t" << n->isLeaf;
	cout << endl << "level" << "\t" << n->level;
	cout << endl << "num of trees " << "\t" << n->numOfTrees;
	cout << endl << "num of keys " << "\t" << n->numOfKeys;
	cout << endl << "prev pointer " << "\t" << n->prevINodePtr;

	cout << endl;

	iUnit *ptr;

	int iNodeJump = this->getIUnitSize();
	int fan = this->getFanoutFactor();

	for (int i = 0; i < fan; i++) {

		ptr = &(n->iunit[0]) + (i * iNodeJump);

		cout << "||Page:";
		cout << ptr->ptr;
		cout << "|";
		cout << ptr->key;
		cout << "|| ";
	}
	cout << endl << "---------------------------------------------------------";

}

void BPlusTreeC::printLeafNode(leafNode *ln) {

	cout << endl << "---------------------------------------------------------";
	cout << endl << "Printing out LEAF Node contents";
	cout << endl << "Leaf Node " << ln->isLeaf;
	cout << endl << "level" << "\t" << ln->level;
	cout << endl << "num of records " << "\t" << ln->numOfRecords;
	cout << endl << "prev pointer " << "\t" << ln->prevLNodePtr;
	cout << endl << "prev pointer " << "\t" << ln->nextLNodePtr;

	cout << endl;

	int leafJump = this->getLUnitSize();
	int leafFanout = this->getLeafFanoutFactor();
	int i;
	lUnit * ptr;

	lUnit* char_to_lUnit_cast = reinterpret_cast<lUnit*>(&ln->lunit[0]);

	for (i = 0; i < leafFanout; i++) {
		ptr = (&(ln->lunit[0]) + (i * leafJump));

		cout << "||Page:";
		cout << ptr->ridPtr.pageNo;
		cout << "|";
		cout << ptr->ridPtr.slotNo;
		cout << "|";
		cout << ptr->key;
		cout << "|| ";
	}
	cout << endl << "---------------------------------------------------------";

}
//------------------------------------------------------------------------------------------------------------------------------------------------------
void BPlusTreeC::initBPlusTree(db* dbObj, sysIndex* propertyObj) {

	this->setAttributeCount(propertyObj->numOfAttributes);
	this->setRootPageNumber(propertyObj->ptrToPageContainingRootofTree);
	this->setTableDirPage(propertyObj->tableDirPage);
	memcpy(&(this->databaseObj), (dbObj), sizeof(dbObj));
	memcpy(&(this->compositeKeyParts), &(propertyObj->compositeKeyParts),
			(MAX_NO_OF_COLS_INDEXED * sizeof(keyPart)));

	this->setKeyUnitSize(); // always after you have copied composite key parts, else it wont refelct
	this->setFanoutFactor();
	this->setIUnitSize();
	this->setLeafFanoutFactor();
	this->setLUnitSize();

	this->rootNodePage = (Page*) malloc(PAGE_SIZE); // never free root page until end
	this->rootNodePage->CFS = ((PAGE_SIZE) - (PAGEDATA_SIZE));
	this->rootNodePage->pageNumber = 0;
	this->rootNodePage->priority = -1;
	this->rootNodePage->nextPage = -1;

	leafNode* root;

	if (createLeafNode(&root) == SUCCESS) // creates a node will all values set to default, rename it to identify it as a root
	{
		root->isLeaf = true;
		root->level = 0;
		root->nextLNodePtr = 0;
		root->prevLNodePtr = 0;
		root->numOfRecords = 0;

		//char* c = (char*) root;

		char* page_to_char_cast = reinterpret_cast<char *>(root);

		memcpy(&(this->rootNodePage->pageData[0]),page_to_char_cast, PAGEDATA_SIZE);

		cout << endl << "Printing Root Node Contents";
		this->printLeafNode(root);

		// ------------------------------------------------------------------------------
		// The Disk Read and Write Process
		int freePageNo = 3;//bufManObj->getFreePage();

		if (bufManObj->writeDiskPage(&this->databaseObj, this->rootNodePage)
				== SUCCESS) {

			Page * ppp2 = bufManObj->readDiskPage(&this->databaseObj,
					this->rootNodePage->pageNumber);
			cout << endl << ppp2->pageNumber;
			cout << endl << ppp2->nextPage;
			cout << endl << ppp2->CFS;
			//cout << endl << ppp2->pageData;

			leafNode* char_to_leaf_cast = reinterpret_cast<leafNode *>(&ppp2->pageData[0]);

			leafNode * print_leaf = (leafNode*) malloc(PAGEDATA_SIZE);

			memcpy(print_leaf, char_to_leaf_cast, PAGEDATA_SIZE);
			this->printLeafNode(print_leaf);
			free(print_leaf);
		}
	}
} // initBtree

//----------------------------------------------------------------------------------------------------------------------------------------------------

void BPlusTreeC::createInternalNode(internalNode ** n) {

	int iUnitSizeJump = this->getIUnitSize();
	int fan = this->getFanoutFactor();

	*n = (internalNode*) malloc(PAGEDATA_SIZE);

	(*n)->isLeaf = false;
	(*n)->level = -1; // root
	(*n)->numOfKeys = -1;//this->getFanoutFactor() - 1;
	(*n)->numOfTrees = -1; //this->getFanoutFactor();
	(*n)->prevINodePtr = -1;

	for (int i = 0; i < fan; i = i + 1) {
		iUnit* iu2 = (iUnit*) malloc(iUnitSizeJump);
		iu2->ptr = i;
		strcpy(&(iu2->key[0]), "Nil");
		char *iu_to_char_cast = (char*) iu2;
		memcpy(&((*n)->iunit[0]) + (i * iUnitSizeJump), iu_to_char_cast, iUnitSizeJump);
		free(iu2);
	}

	//this->printInternalNode(*n);


}
//----------------------------------------------------------------------------------------------------------------------------------------------------
int BPlusTreeC::createLeafNode(leafNode ** n) {

	int leafJump = this->getLUnitSize();
	int leafFan = this->getLeafFanoutFactor();

	*n = (leafNode*) malloc(PAGEDATA_SIZE);

	cout << endl << "Creating a leaf node";

	(*n)->isLeaf = true;
	(*n)->level = -1;
	(*n)->nextLNodePtr = -1;
	(*n)->prevLNodePtr = -1;
	(*n)->numOfRecords = -1;

	for (int i = 0; i < leafFan; i++) {
		lUnit* lu = (lUnit*) malloc(leafJump);
		lu->ridPtr.pageNo = i;
		lu->ridPtr.slotNo = -i;
		strcpy(lu->key, "Nil");
		char *lu_to_char_cast = reinterpret_cast<char*>(lu);
		memcpy(&((*n)->lunit[0]) + (i * leafJump), lu_to_char_cast, leafJump); // copying as a string
		free(lu);
	}

	return SUCCESS;
	// do a read to see if you have the object if you want
	//bufManObj->readPage(&this->databaseObj,this->rootNodePage);
	// ------------------------------------------------------------------------------
	//this->printLeafNode(*n);

}
//----------------------------------------------------------------------------------------------------------------------------------------------------
bool BPlusTreeC::insert_key_and_record(char * generatedKey, block blkPtr) {

	/*
	 if(find(key))
	 return false; //insert failed because of duplicate key
	 */

	// TODO: Deal with duplicate values as well

	if (this->rootNodePage == NULL) {
		return false;
	}

	leafNode *leaf = this->find_leaf_to_insert_in(generatedKey);


	/* Case: leaf has room for key and pointer.	   */

	if (leaf->numOfRecords < leafFanoutFactor - 1) // why -1 here ??  because it should split the node after the (order+1)th node comes
		{
			lUnit *ptr;
			int lUnitSize = this->getLUnitSize();
		// Insert into leaf if there are empty slots
			// no need of -1 here cos it will insert in the ith place exactly.
			//e.g. if num of rec = 5, in the array it will be at 4th place, so new record goes in 5th place

			ptr = &(leaf->lunit[0]) + (leaf->numOfRecords * lUnitSize); // similar to (leaf->iunit[i].ptr)

			strcpy(ptr->key, generatedKey); // leaf->keys[leaf->num_keys] = key;
			memcpy(&ptr->ridPtr.pageNo, &blkPtr.pageNo, LONG_SIZE); //leaf->pointers[leaf->num_keys] = blkPtr;
			leaf->numOfRecords++;


			leafNode * leaf_to_insert_into = insert_into_leaf_with_ample_space(leaf, generatedKey, blkPtr);

			return SUCCESS;
			//return this->rootNodePage;
		}

	  /* Case:  leaf must be split.	   */

	  return insert_into_leaf_after_splitting(leaf, generatedKey, blkPtr);
	// acc. to me, this function is called always, that should nt happen only if the above condition fails it should be called
	//	insert_into_leaf_after_splitting_leaf_node(leaf, generatedKey, blkPtr);
}


int BPlusTreeC::keyCompare(char* searchKey, char* generatedKey,
		keyPart* keyInfo) {

	int noOfBytesCopied = 0;

	for (int idx = 0; idx < propertyObj->numOfAttributes; idx++) {

		int type = keyInfo[idx].attributeType;

		switch (type) {
		case INTEGER:
			int searchInt, nodeInt;
			memcpy(&searchInt, &searchKey[noOfBytesCopied],
					keyInfo[idx].attributeSize);
			memcpy(&nodeInt, &generatedKey[noOfBytesCopied],
					keyInfo[idx].attributeSize);

			noOfBytesCopied += keyInfo[idx].attributeSize;

			if (searchInt < nodeInt)
				return LESSER;
			else if (searchInt > nodeInt)
				return GREATER;
			else
				return EQUAL;
			break;

		case FLOAT:
			float searchFloat, nodeFloat;
			memcpy(&searchFloat, &searchKey[noOfBytesCopied],
					keyInfo[idx].attributeSize);
			memcpy(&nodeFloat, &generatedKey[noOfBytesCopied],
					keyInfo[idx].attributeSize);

			noOfBytesCopied += keyInfo[idx].attributeSize;

			if (searchFloat < nodeFloat)
				return LESSER;
			else if (searchFloat > nodeFloat)
				return GREATER;
			else
				return EQUAL;
			break;

		case CHAR:

			char *searchChar, *nodeChar;
			int returnValue;

			searchChar = (char*) malloc(keyInfo[idx].attributeSize);
			nodeChar = (char*) malloc(keyInfo[idx].attributeSize);

			memcpy(&searchChar[0], &searchKey[noOfBytesCopied],
					keyInfo[idx].attributeSize);
			memcpy(&nodeChar[0], &generatedKey[noOfBytesCopied],
					keyInfo[idx].attributeSize);

			noOfBytesCopied += keyInfo[idx].attributeSize;

			returnValue = strncmp(searchChar, nodeChar,
					keyInfo[idx].attributeSize);

			free(searchChar);
			free(nodeChar);

			if (returnValue < 0)
				return LESSER;
			else if (returnValue > 0)
				return GREATER;
			else
				return EQUAL;

			break;

		case VARCHAR:

			char *searchVarChar, *nodeVarChar;
			int returnVal;

			searchVarChar = (char*) malloc(keyInfo[idx].attributeSize);
			nodeVarChar = (char*) malloc(keyInfo[idx].attributeSize);

			memcpy(&searchVarChar[0], &searchKey[noOfBytesCopied],
					keyInfo[idx].attributeSize);
			memcpy(&nodeVarChar[0], &generatedKey[noOfBytesCopied],
					keyInfo[idx].attributeSize);

			noOfBytesCopied += keyInfo[idx].attributeSize;

			returnVal = strncmp(searchVarChar, nodeVarChar,
					keyInfo[idx].attributeSize);

			free(searchVarChar);
			free(nodeVarChar);

			if (returnVal < 1)
				return LESSER;
			else if (returnVal > 1)
				return GREATER;
			else
				return EQUAL;

			break;
		}

	} // end of for. This means that one full tuples should have generated
	return FAILURE;
}

leafNode* BPlusTreeC::insert_into_leaf_with_ample_space(leafNode *leaf,
		char * generatedKey, block blockPtr) {

	/* initially, unit1, unit2, unit3, unit4  .....
	 unit1, unit2, KEY_2B_Inserted, unit3, unit4  ......
	 or more generally, left_unit , key_to_be_inserted, right_unit*/

	// Linear Search

	// int index = linearSearch(leaf->keys,leaf->num_keys);
	int leafUnitSize = this->getLUnitSize();
	int index; // returns the location where the new key has to be inserted
	lUnit *ptr;

	for (index = 0; index < leaf->numOfRecords; index++) {
		ptr = &(leaf->lunit[0]) + (index * leafUnitSize); // this is the array indexing part
		if ((keyCompare(generatedKey, ptr->key, this->compositeKeyParts) < 0)
				|| (keyCompare(generatedKey, ptr->key, this->compositeKeyParts)
						== 0))
			break;
	} // end of for index
	// return index;

	int keySize = this->getKeyUnitSize();

	// Next 4 lines copies the contents of the node where the new key has to be inserted
	char* temp_key1 = (char*) malloc(keySize);
	strncpy(temp_key1, ptr->key, keySize);

	block temp_block1;
	memcpy(&temp_block1, &ptr->ridPtr, sizeof(block));

	// Copies the new key and record into the location which was just temped
	strncpy(ptr->key, generatedKey, keySize);
	memcpy(&ptr->ridPtr, &blockPtr, sizeof(block));

	char* temp_key2 = (char*) malloc(keySize);
	block temp_block2;

	int i;
	lUnit *newLeafPtr;

	for (i = index + 1; i < leaf->numOfRecords; i++) {
		newLeafPtr = &(leaf->lunit[0]) + (i * leafUnitSize); // this is the array indexing part

		// copies the right units contents into temp2
		strncpy(temp_key2, newLeafPtr->key, keySize);
		memcpy(&temp_block2, &newLeafPtr->ridPtr, sizeof(block));

		// copies the contents of temp_key1, contents present in the location where the new node was inserted
		strncpy(newLeafPtr->key, temp_key1, keySize);
		memcpy(&newLeafPtr->ridPtr, &temp_block1, sizeof(block));

		// copies this into the temp1 for future swapping
		strncpy(temp_key1, temp_key2, keySize);
		memcpy(&temp_block1, &temp_block2, sizeof(block));

	}

	/*
	 *
	 *
	 *
	 lUnit * extraPtr = &(leaf->lunit[0]) + ((this->getLeafFanoutFactor() - 1)
	 * leafUnitSize); // this is the array indexing part
	 block extra;
	 memcpy(&extra, &extraPtr->ridPtr, sizeof(block));

	 Page* newLeafPage;
	 //newLeafPage= getFreePage();
	 //memcpy(&newLeafPage->pageData,&new_leaf_node,PAGEDATA_SIZE);

	 leafNode * new_leaf_node;
	 createLeafNode(&new_leaf_node);

	 int split = (this->getFanoutFactor() / 2);

	 int tempKeys = leaf->numOfRecords;

	 // setting the number of keys to the order in the nodes
	 leaf->numOfRecords = split;

	 leaf->nextLNodePtr = newLeafPage->pageNumber;

	 for (i = split, j = 0; i < tempKeys; i++, j++) {
	 new_leaf->keys[j] = leaf->keys[i];
	 new_leaf->pointers[j] = leaf->pointers[i];
	 new_leaf->num_keys++;
	 }
	 new_leaf->keys[j] = temp_key2;
	 new_leaf->pointers[j] = temp_ptr2;
	 new_leaf->pointers[j + 1] = extra;
	 insert_into_parent(leaf, new_leaf->keys[0], new_leaf);

	 */
}

bool BPlusTreeC::insert_into_leaf_after_splitting(leafNode* leaf, char * generatedkey, block blockPtr) {


	Page* newLeafPage = (Page*) malloc (PAGE_SIZE);
//	newLeafPage->priority = LEAF_PAGE_PRIORITY;
//	newLeafPage->pageNumber = getFreePage();
//	newLeafPage->nextPage = UNASSIGNED;

	//memcpy(&newLeafPage->pageData,&new_leaf_node,PAGEDATA_SIZE);

	leafNode * new_leaf_node;
	createLeafNode(&new_leaf_node);

	int leafFan = this->getLeafFanoutFactor();
	int leafUnitSize = this->getLUnitSize();
	int keyUnitSize = this->getKeyUnitSize();

	//make_temp_array()
	char* temp_keys = (char*)malloc(leaf->numOfRecords*keyUnitSize);
	if (temp_keys == NULL)
	{
			cout << "Temporary keys array creation failed";
			return FAILURE;
	}
	//make_temp_pointers(){
	block temp_blockPointers[leafFan];
}
	/*if (temp_pointers == NULL) {
		cout << "Temporary pointers array.Temporary keys array creation failed";
		return FAILURE;
	}

	int insertion_index = 0;

	lUnit * ptr, *p, *ipk, *ip;
	char * jpk, *jp;


	 while (insertion_index < leafFan - 1 && (keyCompare(generatedKey, ptr->key,
	 leaf, this->compositeKeyParts) < 0)) {
	 ptr = &(leaf->lunit[0]) + (inserted_index * leafUnitSize); // this is the array indexing part
	 insertion_index++;
	 }


	for (i = 0, j = 0; i < leaf->numOfRecords; i++, j++) {

		if (j == insertion_index)
			j++;

		ipk = &(leaf->lunit[0]) + (i * leafUnitSize);
		jpk = &(temp_keys[0]) + (j * keyUnitSize);

		memcpy(jpk, &ipk->key, keyUnitSize);

		jp = &(temp_pointers[0]) + (j * sizeof(block));

		memcpy(jp, &ipk->ridPtr, sizeof(block));
	}


	 temp_keys[insertion_index] = key;
	 temp_pointers[insertion_index] = pointer;

	 leaf->num_keys = 0;

	 split = cut(order - 1);

	 for (i = 0; i < split; i++) {
	 leaf->pointers[i] = temp_pointers[i];
	 leaf->keys[i] = temp_keys[i];
	 leaf->num_keys++;

}

for (i = split, j = 0; i < order; i++, j++) {
 new_leaf->pointers[j] = temp_pointers[i];
 new_leaf->keys[j] = temp_keys[i];
 new_leaf->num_keys++;
 }

 free(temp_pointers);
 free(temp_keys);


 new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
 leaf->pointers[order - 1] = new_leaf;

 for (i = leaf->num_keys; i < order - 1; i++)
 leaf->pointers[i] = NULL;
 for (i = new_leaf->num_keys; i < order - 1; i++)
 new_leaf->pointers[i] = NULL;

 new_leaf->parent = leaf->parent;
 new_key = new_leaf->keys[0];

 return insert_into_parent(root, leaf, new_key, new_leaf); }
 */

int BPlusTreeC::extract_type_from_page(Page* temp_page) {
	int type;
	memcpy(&type, &(this->rootNodePage->pageData[0]), 1);
	return type;
}

leafNode* BPlusTreeC::extract_leaf_from_page(Page* temp_Page) {
	leafNode* temp_leaf = (leafNode *) malloc(PAGEDATA_SIZE);
	memcpy(&temp_leaf, &(temp_Page->pageData[0]), PAGEDATA_SIZE);
	return temp_leaf;
}
internalNode* BPlusTreeC::extract_inode_from_page(Page* temp_page) {
	internalNode* temp_inode = (internalNode *) malloc(PAGEDATA_SIZE);
	memcpy(&temp_inode, &(temp_page->pageData[0]), PAGEDATA_SIZE);
	return temp_inode;
}

leafNode* BPlusTreeC::find_leaf_to_insert_in(char* generatedKey) {

	if (!this->rootNodePage)
		return NULL;

	Page* temp_page = (Page*) malloc(PAGE_SIZE);
	Page* temp_page_ptr;
	leafNode* temp_leaf;
	internalNode* temp_inode;

	temp_page_ptr = this->rootNodePage; // temp is assigned to root page
	int pageNo;


	while (temp_page_ptr) {
		/*Case i. When the root page comes, check for the type of node inside*/

		//if(temp_page->pageNumber == this->rootNodePage->pageNumber)

		temp_page = bufManObj->readDiskPage(&this->databaseObj,temp_page->pageNumber);

		bool type = this->extract_type_from_page(temp_page);

		if (type == LEAF) {

			temp_leaf = this->extract_leaf_from_page(temp_page);
			//if(temp_leaf->numOfRecords == 0)
				//printf ("\nEmpty tree.\n");
			//	free(temp_page);  // but if you do this, how will you know what are the page headers of this node such as page number /?
			return temp_leaf;
		}

		if (type == INODE) {

			temp_inode = extract_inode_from_page(temp_page);

			int i;
			iUnit *ptr, *keyPtr;
			int iuSize = this->getIUnitSize();

			char* nodeKey = (char*)malloc(iuSize);

			for (i = temp_inode->numOfKeys; i > 0; i--) { // temp->num_keys is the total number of keys

				ptr = &(temp_inode->iunit[0]) + (i * iuSize); // similar to (temp_inode->iunit[i].ptr)
				keyPtr = &(temp_inode->iunit[0]) + ((i - 1) * iuSize); // similar to (temp_inode->iunit[i-1].key)

				memcpy(nodeKey,&keyPtr->key,iuSize);

				if ((keyCompare(generatedKey, nodeKey, this->compositeKeyParts) == GREATER) || keyCompare(generatedKey,nodeKey,this->compositeKeyParts) == EQUAL )// temp->keys[i-1] ; -1 bcos its an array index
					continue ;
				else
					break;
			} // eofor

			pageNo = ptr->ptr;
			temp_page = bufManObj->readDiskPage(&this->databaseObj,pageNo);
		} // eoif inode check
	}// end of while(inode_temp_ptr);

	free(temp_page);
	free(temp_leaf);
	free(temp_inode);

	return FAILURE;

}
//------------------------------------------------------------------------------------------------------------------------------------------------------
}// end of Namespace

