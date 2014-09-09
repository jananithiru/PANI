/*
 * bufferManagerTest.cpp
 *
 *  Created on: 18-Sep-2011
 *      Author: amrita
 */

#include "bufferManagerTest.h"
struct KEY
	{
	char keydata[1];
	} *k;

bufferManagerTest::bufferManagerTest() {
	// TODO Auto-generated constructor stub

}

bufferManagerTest::~bufferManagerTest() {
	// TODO Auto-generated destructor stub

}

int main()
{
	Utils utils;
	int bufChoice,noFrames,pageNo,frameNo;
	char* pageData;
    cout<<"Enter the number of frames in the buffer:: ";
    cin>>noFrames;
	bufferManager BM(FRAME_SIZE * noFrames);
	Page* readPage;
	Page *writePage;
	Frame* myFrame;
	try{
		fstream fileDB;
		char* databaseName="Mydb.db";
		db* database=(db*)malloc(POINTER_SIZE);
	    database->dbIndex=1;
	    database->dbName=databaseName;
	    fileDB.open("Mydb.db",ios::in|ios::out|ios::binary);

	    if(!fileDB.is_open())
	    	{
	    	cout<<"Error in opening file";
	    	return -1;
	    	}
	    database->dbFP=&fileDB;

//	    BM.allocate(FRAME_SIZE * noFrames);
	    cout<<"\n Buffer allocated with "<<noFrames<<" frames.";
	do
	{
	printf("\n\n\t Buffer Manager Testing Module \n");
	        printf("\t ----------------------------- \n");
	        printf("\n\n");
	        printf("\t 1. Read Page: \n");
	        printf("\t 2. Write Page: \n");
	        printf("\t 3. Show Frame: \n");
	        printf("\t 4. Print Hash Map: \n");
	        printf("\t 5. Show frame data: \n");
	        printf("\t 6. Commit All: \n");
	        printf("\t 7. Release Buffer and Exit: \n");
	        printf("\t ----------------------------- \n\n");

	        printf("Enter your option(1-6):: ");
	        scanf("%d",&bufChoice);
	        if ((bufChoice<0)||(bufChoice>7))
	        	cout<<"Wrong Choice. Please enter again"<<endl;
	        else{
	switch(bufChoice)
	{
	case 1:cout<<"Enter the page number:: ";
		cin>>pageNo;
		readPage=BM.readPage(database,pageNo);
		if(readPage!=NULL)
		{
		cout<<"Page number  Page Data"<<endl;
		cout<<readPage->pageNumber;//<<" "<<readPage->pageData<<endl;
		cout<<"PAGE DATA HEX DUMP\n";
		BM.hexdump(&(readPage->pageData[0]),PAGE_SIZE);
		}
		else cout<<"Page not found";
		break;
	case 2:

		cout<<"Enter the page number:: ";
		cin>>pageNo;
		cout<<"Enter the page data"<<endl;
		pageData=(char*)malloc(PAGEDATA_SIZE);
		cin>>pageData;
		writePage = (Page*)malloc(PAGE_SIZE);
		//memcpy(&(writePage->pageNumber),&pageNo,LONG_SIZE);
		writePage->pageNumber=pageNo;
		writePage->CFS=PAGEDATA_SIZE;
		writePage->nextPage=NULL;
		writePage->priority=1;
		memcpy(&(writePage->pageData[0]),pageData,PAGEDATA_SIZE);
		free(pageData);

		BM.writePage(database,writePage);


		free(writePage);

		break;
	case 3:BM.bPool->getFramedata();
			break;
	case 4: BM.bPool->printMap();
			break;
	case 5:
		cout<<"Enter the frame number to be read"<<endl;
		cin>>frameNo;
		if(frameNo<=BM.bPool->frameCount)
		{
		myFrame=BM.bPool->getFrameNumber(frameNo);
		cout<<myFrame->frameNumber<<" "<<myFrame->dbIndex->dbName<<endl;
		cout<<"Printing dump"<<endl;
		BM.hexdump(&(myFrame->pageData[0]),PAGE_SIZE);
		}
		else cout<<"Frame number doesn't exist"<<endl;
		break;
	case 6:
		cout<<"Committing.........."<<endl;
		BM.commit();
		break;
	case 7:
		cout<<"Releasing buffer"<<endl;

		break;
	}
	        }
	}while((bufChoice!=7));
	free(database);
	}catch(std::exception e){   std::cout << "Exception: " << e.what() << std::endl;}
return 0;
}
/*int main()
{
Utils utils;
time_t currtime = time(0);
bufferManager BM(FRAME_SIZE * 2);
cout<<"Number of frames created"<<BM.bPool->frameCount;
//utils.myprintf(AT, "LOG STARTED AT %s ",ctime(&currtime));

//try{
//	fstream fileDB;
//	char* databaseName="Mydb.db";
//	db* database=(db*)malloc(sizeof(db*));
//    database->dbIndex=1;
//    database->dbName=databaseName;
//    fileDB.open("Mydb.db",ios::in|ios::out|ios::binary);
//
//    if(!fileDB.is_open())
//    	{
//    	cout<<"Error in opening file";
//    	//utils.myprintf(AT, "Error in opening database file");
//    	return 0;
//    	}
//    database->dbFP=&fileDB;
////
////
//    char* data1="123amrita1987";
//    char* data2="amrita2";
//    char* data3="amrita3";
//    char* data4="amrita4";
//
//    BM.bPool->printMap();
//
//
//    ////utils.myprintf(AT, "Testing write page");
//    /************data1******/
//    Page* page1=(Page*)malloc(PAGE_SIZE);
//        page1->CFS=PAGE_SIZE;
//        //strcpy(page->pageData,data1);
//        k = (struct KEY *) malloc (15);
//        	//int rollno =123;
//        	char* name = "Janani";
//        	int length= strlen(name);
//        	int year =2000;
//        	memcpy(&(k->keydata[0]),&length,4);
//        	memcpy(&(k->keydata[4]),name,6);
//        	memcpy(&(k->keydata[10]),&year,4);
//        memcpy(&(page1->pageData[0]),k,PAGE_DATA_SIZE);
//        page1->pageNumber=1;
//        page1->priority=0;
//        BM.writePage(database,page1);
//
//        cout<<"data1 written"<<endl;
//        BM.bPool->printMap();
//    /************data2******/
//        Page* page2=(Page*)malloc(PAGE_SIZE);
//               page2->CFS=PAGE_SIZE;
//
//               page2->pageData=data2;
//               page2->pageNumber=20;
//               page2->priority=0;
//               BM.writePage(database,page2);
//               cout<<"data2 written"<<endl;
//               ////utils.myprintf(AT, "data2 written at page %f", page->pageNumber);
//               BM.bPool->printMap();
//    /************data3******/
//               Page* page3=(Page*)malloc(PAGE_SIZE);
//     page3->CFS=PAGE_SIZE;
//
//     page3->pageData=data3;
//     page3->pageNumber=25;
//     page3->priority=0;
//     BM.writePage(database,page3);
//     cout<<"data3 written"<<endl;
//     ////utils.myprintf(AT, "data3 written at page %f", page->pageNumber);
//     BM.bPool->printMap();
//    /************data4******/
//     Page* page4=(Page*)malloc(PAGE_SIZE);
//    page4->CFS=PAGE_SIZE;
//
//    page4->pageData=data4;
//    page4->pageNumber=26;
//    page4->priority=0;
//    BM.writePage(database,page4);
//    cout<<"data4 written"<<endl;
//    ////utils.myprintf(AT, "data4 written at page %f", page->pageNumber);
//    BM.bPool->printMap();
//
//          /***********read Page****************************/
////        BM.commit();
//    BM.bPool->getFramedata();
//  //  Page* page5=(Page*)malloc(PAGE_SIZE);
//    Page* page5=BM.readPage(database,1);
//
//
//   cout<<"Read Page+++++++++++++++++++++++++++++++++++++++++++"<<endl;
//   if(page5!=NULL)
//	    cout<<"number"<<page5->pageNumber<<endl;
//   else cout<<"Page not found"<<endl;
//
//   int length2= (int)page5->pageData[0];
//   //memcpy(length2,&(page5->pageData[0]),4);
//   char* myName=(char*)malloc(length);
//   strncpy(myName,&(page5->pageData[4]),length2);
//   cout<<"Name: "<<myName;
//   free(myName);
//   int year2;//=(int)page5->pageData[10];
//   //memcpy(year2,&(page5->pageData[10]),4);
//   memcpy(&year2,&(page5->pageData[0])+10,4);
//   cout<<"Year"<<year2;
//   fileDB.close();
//   free(page1);
////   free(page2);
////   free(page3);
////   free(page4);
//
//
//   free(database);
//
//}catch(std::exception e){   std::cout << "Exception: " << e.what() << std::endl;}

///**change to integer**/
//int number;
//const char* testStr="000123";
//if(utils.str2int(number,testStr))
//	cout<<"Number is "<<number<<endl;
///**change to double**/
//
//const char* testStr2="000123.33288";
//double d1;
//d1 = strtod(testStr2,NULL);
//cout<<"Double number "<<d1<<endl;
///**change to double**/
//float number2;
//const char* testStr3="000123.33228";
//number2 = strtol(testStr3,NULL,10);
//cout<<"Long number "<<d1<<endl;

//dbModule db;
	//db.dbModule();
//	db.createNewDB("Pani", 100*1024*1024);
//	db.useDatabase("Pani");







//utils.myprintf(AT, "LOG ENDED AT %s ",ctime(&currtime));

//}
