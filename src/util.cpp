#include "util.hpp"

/*
	Debug function in oldschool C Style.
*/

void
error(const char *msg)
{
	perror(msg);
	exit(1);
}


/*
	Debug function in oldschool C Style.
	Use:
		_debug("some info %s, %d", some_info, errno);
*/
void _debug(const char* string, ...){
	va_list(args);
	va_start(args, string);
	vfprintf(stderr, string, args);
	va_end(args);
}


/*
	Creates C-string in with format.
	
	It uses malloc --- in this case, 
	don't worry about outmem pointers in char*.
*/

char* ssprintf(const char* fmt, ...)
{
	int symbols;
	size_t size = 256;
	char *res;
	va_list ap;
	if ((res = (char* )malloc (sizeof(char) * size)) == NULL){
		perror("ERROR malloc failure");
		return NULL;
	}
	while (1) {
	/* Try to print in the allocated space. */
		memset(res, 0, size);
		va_start(ap, fmt);
		symbols = vsnprintf (res, size, fmt, ap);
		va_end(ap);
		if (symbols > -1 && (size_t )symbols < size)
			return res;
		/* Else try again with more space. */
		if (symbols > -1){    					/* glibc 2.1 */
			size = symbols + 1; 				/* precisely what is needed */
		}
		else{           							/* glibc 2.0 */
			size *= 2;  						/* twice the old size */
		}
		if ((res = (char* )realloc(res, sizeof(char) * size)) == NULL)	{
			perror("ERROR realloc  failure");
			return NULL;
		}
   }
}

/* Returns rawtime
*/

time_t getTime()
{
	time_t rawtime;
	time ( &rawtime );
	return rawtime;
}


/* Returns formated string with current time.
	
	return string in format "hh:mm:ss".
*/

char* getFormatedTime(time_t rawtime)
{
	struct tm * timeinfo;
	char* buffer = (char *)malloc(sizeof(char)* 18);// XXXX.XX.XX XX:XX\0
	timeinfo = localtime ( &rawtime );
	strftime(buffer,18,"%X", timeinfo);
	return buffer;
}
