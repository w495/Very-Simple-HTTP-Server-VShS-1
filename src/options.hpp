#ifndef __SERVER_OPTIONS__
#define __SERVER_OPTIONS__

#include <cstdlib>
#include <cstdio>

extern "C" {
#include <getopt.h>
}


#include "util.hpp"


# define PORT 7070 
# define LOGFILENAME "log.txt" 

#define ABOUT " This is simple HTTP server.\n"								\
" It receives requests from client and sends them the html page:\n"			\
" \tReceives (<number_of_requests>)\n"										\
" \t<http header from client>\n"											\
" It can serve several clients in one moment.\n"							\
" More than that, server writes logfile in format\n"						\
" \t<number of requests> [<connection time?] -> [<answer time>]:\n"			\
" \t<http header from client>\n"											\
"\n"

#define USAGE " USAGE: \n"													\
"     server [-p<PORT>|--port=<PORT>][-l<LOG>|--log=<LOG>][-h|--help]\n"	\
"       -p<PORT> or --port=<PORT> sets the server port .\n"					\
"                                  It works on 127.0.0.1:PORT. \n"			\
"                                  Default PORT is 7070. \n"				\
"                                  PORT ought to be in (0, 65535], \n"		\
"                                  otherwise, it uses default one. \n"		\
"       -l<LOG> or --log=<LOG>    sets the name of logfile.\n"				\
"                                  Default PORT is './log.txt'.\n"			\
"       -h or --help              shows this message.\n"					\
"\n"

#define HELP ABOUT USAGE

class Options{
  public:

	/*! \brief Creates server options from cmd arguments.
	*/
	
	Options(int argc, char *argv[]);
	int getPort();
	char* getLogfilename();
	
	/* DATA (*_options) */
	static const char* short_options; 
	static const struct option long_options [];
	
  private: 
  /* DATA */
	int _port;
	char* _logfilename;
};

#endif /* __SERVER_OPTIONS__ */