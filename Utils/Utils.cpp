/*
 * Utils.cpp
 *
 *  Created on: 24-Sep-2011
 *      Author: amrita
 */

#include "Utils.h"
#include <stdarg.h>
Utils::Utils() {
	// TODO Auto-generated constructor stub

}

Utils::~Utils() {
	// TODO Auto-generated destructor stub
}

void Utils::myprintf(const char *location,const char* format, ... ){
	va_list args;
	FILE *fp2=fopen("Mydblog.txt","ab+");
	fprintf(fp2,"%s ",location);
	    va_start( args, format );
	    vfprintf( fp2, format, args );
	    va_end( args );
	    fprintf( fp2, "\n" );
	    fclose(fp2);
}

bool Utils::str2int (int &num, char const *str) {
	 char              c;
	   stringstream ss(str);
	    ss >> num;
	    if (ss.fail() || ss.get(c)) {
	        // not an integer
	        return false;
	    }
	    return true;
}
