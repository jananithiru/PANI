/*
 * BPlusTreeC.h
 *
 *  Created on: 20-Nov-2011
 *      Author: janani
 */

/*Sept 13th 2011
 * Author: Janani
 *
 * LEAF NODE STRUCT
 _____________________________________\\_______________________________________
 | _______________ _____________       \\   _______________ _______________   |
 | |Key| RID ptr | |Key| RID ptr |		\\ 	|Key| RID ptr | |Key| RID ptr |   |
 | |___|_________| |___|_________|	 	 \\ |___|_________| |___|_________|   |
 |________________________________________\\ _________________________________|

 where,
 _______________
 |Key| RID ptr |
 |___|_________|		LUnit

 *
 *
 *
 ----------------------------------------------------------------------------------------------
 *
 *
 * INTERNAL NODE STRUCT
 ____________________\\______________________
 | _______________ ___\\___________________  |
 | ||Key||Key||Key||   \\  ||Key||Key||Key|| |
 | ||___||___||___||____\\ ||___||___||___|| |
 |_______________________\\ _________________|

 where,
 ______
 ||Key|
 ||___|		IUnit

 *
 *
 */

/*
 myBPTObj->setRootPageNumber(5);
 myBPTObj->attributeCount;
 myBPTObj->compositeKeyParts;
 myBPTObj->fanoutFactor;
 myBPTObj->globPosition;
 myBPTObj->iUnitSize;
 myBPTObj->isAscOrDesc;
 myBPTObj->keyUnitSize;
 myBPTObj->lUnitSize;
 myBPTObj->leafFanoutFactor;
 myBPTObj->rootPageNumber;
 myBPTObj->tableDirPage;
 */
/*propertyObj->compositeKeyParts;
 propertyObj->indexID;
 propertyObj->isAsc;
 propertyObj->ptrToPageContainingRootofTree;
 propertyObj->tableDirPage;
 propertyObj->tableID;
 */

#ifndef BPLUSTREEC_H_
#define BPLUSTREEC_H_

#include <string.h>
#include "../globals/globals.h"
#include "../bufferManager/bufferManager.h"
#include "../Utils/Utils.h"
//#include "../dbEngine/dbModule.h"

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------My B Plus Tree NameSpace with associated structures ---------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

namespace BPlusTreeNS {

typedef struct IUnitStruct {
	int ptr;
	char key[1];
} iUnit;

typedef struct LUnitStruct {
	int pageNo;
	int slotNo;
	char key[1];
} lUnit;

typedef struct LeafNodeStruct {
	bool isLeaf;
	int level;
	int numOfRecords;
	int prevLNodePtr;
	int nextLNodePtr;
	//struct LUnitStruct lunit[1]; // Implying you can have a variable number of such lunits within a LNode
	char lunit[1];
} leafNode;

typedef struct InternalNodeStruct {
	bool isLeaf; // false
	int level;
	int numOfTrees;
	int numOfKeys;
	int prevINodePtr;
	//struct IUnitStruct iunit[1];
	char iunit[1];
} internalNode;

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------My B Plus Tree Class Definition ---------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
class BPlusTreeC {

private:

	int rootPageNumber;
	int globPosition; // position to insert
	int tableDirPage;
	bool isAsc;
	int fanoutFactor;
	int leafFanoutFactor;
	int attributeCount;
	int iUnitSize;
	int keyUnitSize;
	int lUnitSize;
public:
	Page * rootNodePage;
	keyPart compositeKeyParts[MAX_NO_OF_COLS_INDEXED];
	db databaseObj;

	/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
public:
	// Constructors & Destructors
	BPlusTreeC();
	virtual ~BPlusTreeC();

	// getters and setters

	int getRootPageNumber() const {
		return this->rootPageNumber;
	}

	void setRootPageNumber(int a) {
		this->rootPageNumber = a;
	}

	int getTableDirPage() const {
		return this->tableDirPage;
	}

	void setTableDirPage(int a) {
		this->tableDirPage = a;
	}

	bool getIsAsc() const {
		return this->isAsc;
	}

	void setIsAsc(bool a) {
		this->isAsc = a;
	}

	int getAttributeCount() const {
		return this->attributeCount;
	}

	void setAttributeCount(int i) {
		this->attributeCount = i;
	}

	int getIUnitSize() const {
		return this->iUnitSize;
	}

	void setIUnitSize() {
		if (this->keyUnitSize == 0) {
			setKeyUnitSize();
		}
		this->iUnitSize = LONG_SIZE + this->keyUnitSize;
		cout << endl << "The Internal Node's Iunit size is " << "\t"
				<< this->iUnitSize;
	}

	int getLUnitSize() const {
		return this->lUnitSize;
	}

	void setLUnitSize() {
		if (this->keyUnitSize == 0) {
			setKeyUnitSize();
		}

		this->lUnitSize = LONG_SIZE + INT_SIZE + this->keyUnitSize;
		cout << endl << "The Leaf Node's Lunit size is " << "\t"
				<< this->lUnitSize;
	}

	int getKeyUnitSize() const {
		return this->keyUnitSize;
	}

	void setKeyUnitSize() {
		int count = this->getAttributeCount();
		for (int i = 0; i < count; i++)
			this->keyUnitSize += this->compositeKeyParts[i].attributeSize;

		cout << endl << "The key unit size is " << "\t" << this->keyUnitSize;

	}

	int getFanoutFactor() const {
		return this->fanoutFactor;
	}

	void setFanoutFactor() {
		// Page data size (priority, nextpage, )  - the node header (typeflag, level, numoftrees, numofkeys, prevInode) - sibling pointer divided by the size of the key and the pointer

		this->fanoutFactor = ((PAGEDATA_SIZE - BOOL_SIZE - INT_SIZE - INT_SIZE
				- INT_SIZE - LONG_SIZE - LONG_SIZE) / ((LONG_SIZE
				+ this->keyUnitSize)));
		cout << endl << "Fanout for a Node" << "\t" << this->getFanoutFactor();
	}

	int getLeafFanoutFactor() const {
		return this->leafFanoutFactor;
	}

	void setLeafFanoutFactor() {

		// Page data size - the node header(typeflag, level, numOfRecords, prev, next) - divided by the size of the key and the pointer

		this->leafFanoutFactor = ((PAGEDATA_SIZE - BOOL_SIZE - INT_SIZE
				- INT_SIZE - LONG_SIZE - LONG_SIZE) / ((LONG_SIZE + INT_SIZE
				+ this->keyUnitSize)));
		cout << endl << "Fanout for a LEAF Node" << "\t"
				<< this->getLeafFanoutFactor();
	}

	/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
	// Strict BPlusTree functions
public:

	void initBPlusTree(database* databaseObj, sysIndex* propertyObj);
	void delTree();

	void createInternalNode(internalNode ** n);

	bool insert_key_and_record(char* key, block ridPtr); /*note this is not a pointer, though it is called one*/

	leafNode* find_leaf_to_insert_in(char* generatedKey);

	leafNode* insert_into_leaf_with_ample_space(leafNode* leaf,char * generatedkey,block blockPtr);

	bool insert_into_leaf_after_splitting(leafNode* leaf, char * generatedkey, block blockPtr) ;

	bool delete_key_from_tree(char* key);

	bool update_key_in_tree(char* key, block newRIDPtr);

	void printInternalNode(internalNode *n);

	void printLeafNode(leafNode *ln );

	int createLeafNode(leafNode ** n);

	int keyCompare(char* searchKey, char* nodeKey, keyPart* keyInfo);

	int extract_type_from_page(Page* temp_page);
	leafNode* extract_leaf_from_page(Page* temp_page);
	internalNode* extract_inode_from_page(Page* temp_page);


	/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

};//end of BPlusTreeClass

} // end of BPlusTreeNS
#endif /* BPLUSTREEC_H_ */
//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

// Read more !!

/**
 *
 *
 *
 *\
 *\
 *\
 *\
 *\      public:
 //int add( const ElemType &elem );
 //void removeAt( int index );
 //void removeAt( int index, ElemType &removed );
 //void clear();
 //bool find( const KeyT &key, int &index ) const;
 //bool hasChilds() const;
 //bool isEmpty() const;
 //bool isFull() const;
 //int count() const;


 *\
 *\///
 /// Add new key:data pair
 bool add( const KeyT &key, const DataT &data );

 ///
 /// Remove pair by key
 bool remove( const KeyT &key );

 ///
 /// Remove pair by key and return associated data
 bool remove( const KeyT &key, DataT &data );

 ///
 /// Find data by key, or return false if key was not found
 bool find( const KeyT &key, DataT &data );

 ///
 /// Find all specified data
 template< class TChecker, class TContainer >
 bool search( TContainer &retList, const KeyT &startKey,
 bool preciseSearch, TChecker &condition );

 ///
 /// Get all specified in container data
 template< class TContainer >
 bool getAll( TContainer &retList );

 ///
 /// Change data which belongs to the key is specified
 void changeData( const KeyT &key, const DataT &newData );

 ///
 /// Close BTree and release all occupied resources
 void close();



 int getMedian( NodeType *node, BTreeElement< KeyT, DataT > &elem );

 void splitNodeByKey( NodeType **fullNode, KeyT &key );
 bool splitNode( NodeType *node,
 BTreeElement< KeyT, DataT > &median, NodeType **nodeRight );

 NodeType* findNode( NodeType *node, const KeyT &key, int &retIndex,
 int &parentIndex, bool &found );

 bool rebalance( NodeType *node, int parentIndex );
 bool combine( NodeType *leftNode, NodeType *rightNode );
 bool pullOut( NodeType *node, int itemIndex );

 NodeType* rightMost( NodeType *subtree, KeyT &largestKey, DataT &largestData );
 NodeType* leftMost( NodeType *subtree, KeyT &smallestKey, DataT &smallestData );

 // Internal search methods
 template < class TChecker, class TContainer >
 bool allKeys( NodeType *node, TContainer &retList, int elemIndex,
 const KeyT &startKey, TChecker &condition );
 template < class TChecker, class TContainer >
 bool allKeys( NodeType *node, TContainer &retList, TChecker &condition );
 template< class TContainer >
 bool allKeys( NodeType *node, TContainer &retList );

 *
 *
 */

