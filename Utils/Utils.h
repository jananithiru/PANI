/*
 * Utils.h
 *
 *  Created on: 24-Sep-2011
 *      Author: amrita
 */

#ifndef UTILS_H_
#define UTILS_H_
#include "../globals/globals.h"
//logging messages
//#define myprintf(const char* Format, ... ) myprintf(__FILE__,__LINE__,const char* Format, ... )
class Utils {
public:
	Utils();
	virtual ~Utils();
	void myprintf(const char *location,const char* Format, ... );
	bool str2int (int &num, char const *str);
	void trace(...);
};

#endif /* UTILS_H_ */
