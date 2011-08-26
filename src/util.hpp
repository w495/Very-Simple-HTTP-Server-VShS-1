#ifndef __SERVER_UTIL__
#define __SERVER_UTIL__

#include<cerrno>
#include<cstdlib>
#include<cstdio>
#include<cstdarg>
#include<cstring>
#include<ctime>

extern "C" {
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h> 
}


/*! \brief Debug function in oldschool C Style. */

void error(const char *msg);

/*! \brief Debug function in oldschool C Style.

	Use: _debug("some info %s, %d", some_info, errno);
*/
void _debug(const char* string, ...);

/*! \brief Creates C-string in with format.
	
	It uses malloc --- in this case, 
	don't worry about outmem pointers in char*.
	
	\return allocated C-string.
*/

char* ssprintf(const char* fmt, ...);

/*! \brief Returns rawtime
*/

time_t getTime();

/*! \brief Returns formated string with current time.
	
	\return string in format "hh:mm:ss".
*/

char* getFormatedTime(time_t rawtime);



#endif /* __SERVER_UTIL__ */