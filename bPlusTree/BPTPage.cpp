/*
 * BPTPage.cpp
 *
 *  Created on: 18-Dec-2011
 *      Author: janani
 */

#include "BPTPage.h"
#include <deque>
#include <algorithm>
#include <iterator>
using namespace std;
deque<int> freeListQ;

BPTPage::BPTPage() {
	// TODO Auto-generated constructor stub

}

BPTPage::~BPTPage() {
}
// TODO Auto-generated destructor stub

int BPTPage::getFreePage() {
	for (int i = 10; i >= 0; i--)
		freeListQ.assign(100, i);
	int pageNo = freeListQ[0];
	cout << " The page number of the first free page in the list is : \t "
			<< freeListQ[0];
	freeListQ.pop_back();
	cout << " The next free page is : \t " << freeListQ[0];
	return pageNo;
}

Page* BPTPage::readDiskPage(db* database, long pageNumber) {

		cout <<  endl << "Reading page " << pageNumber << endl;

		char *charBuf = (char*) malloc(PAGE_SIZE);

		fstream in(database->dbName, ios::in | ios::binary);


		in.seekg(pageNumber * PAGE_SIZE, ios::beg);
		in.read(charBuf, PAGE_SIZE);

		Page *char_to_page_cast = reinterpret_cast<Page*>(charBuf);

		cout << endl<< in.gcount() << " bytes read\n" << endl;
		in.close();

		return (char_to_page_cast);
}

int BPTPage::writeDiskPage(db* database, Page* page) {

	cout << endl << "Writing page " << page->pageNumber << endl;

		ofstream out(database->dbName, ios::out | ios::binary);
		if (!out) {
			cout << "Cannot open file.";
			return 1;
		}

		char *page_to_char_cast = reinterpret_cast<char*>(page);
		out.seekp((page->pageNumber * PAGE_SIZE), ios::beg);
		out.write(page_to_char_cast, PAGE_SIZE);

		out.close();

		return SUCCESS;

}





