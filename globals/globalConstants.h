/*
 * globalConstants.h
 *
 *  Created on: 15-Sep-2011
 *      Author: prat
 */

#ifndef GLOBALCONSTANTS_H_
#define GLOBALCONSTANTS_H_

#include "globalHeaders.h"
#include "pageSize.h"

// known data types
#define LONG_SIZE 4
#define INT_SIZE 4
#define SHORT_SIZE 2
#define MAX_VARCHAR 100
#define CHAR_SIZE 1
#define BOOL_SIZE 1
#define VOID_POINTER_SIZE 4


//our datatypes
#define FRAME_SIZE (PAGE_SIZE + LONG_SIZE + LONG_SIZE + POINTER_SIZE + CHAR_SIZE + BOOL_SIZE)//8192+14-1=8205 where 14 is size of frameStruct
#define PAGE_SIZE 8192
#define TABLE_NAME 20
#define DB_NAME 20
#define MAX_NOS_COLUMNS 5

// J: BplusTree
#define NUM_OF_DATATYPES 4
#define PAGEDATA_SIZE ((((PAGE_SIZE - INT_SIZE) - LONG_SIZE) - LONG_SIZE) - INT_SIZE) // priority, page number, next page, CFS)
#define POINTER_SIZE LONG_SIZE
#define MAX_NO_OF_COLS_INDEXED 5


#define LESSER 55
#define GREATER 66
#define EQUAL 77
#define UNASSIGNED -1


#define INTEGER 0
#define FLOAT 1
#define CHAR 2
#define VARCHAR 3
#define LONG 4
#define VOID 5
#define BOOL 6

#define LEAF true
#define INODE false

// no. of DEs
#define PAGE_PRIORITY INT_SIZE
#define PAGE_NUMBER LONG_SIZE
#define PAGE_NEXT_POINTER LONG_SIZE
#define PAGE_CFS INT_SIZE
#define PAGE_DATA 1
#define DP_NUM_CURRENT_DES INT_SIZE
#define DP_NEXT_POINTER LONG_SIZE
#define DP_MAX_NOS_DES (( PAGEDATA_SIZE - INT_SIZE - LONG_SIZE ) / ( LONG_SIZE + INT_SIZE))// (pagesize - priority - pageNumber - nextPage - CFS - pageData - noOfCurrentDEs - ptrToNextDP ) / (TFS + pointerToDBPage)( 8192 - 4 - 8 - 8 - 4 - 1 - 4 - 8 ) / 12

#define SUCCESS 1
#define FAILURE 0



// debug flag
#define DEBUG false
#endif /* GLOBALCONSTANTS_H_ */
