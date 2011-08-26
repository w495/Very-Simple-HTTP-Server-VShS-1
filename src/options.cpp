#include "options.hpp"

const char* Options::short_options = "hp:l:";

const struct option Options::long_options [] = {
	{"help",no_argument,NULL,'h'},
	{"port",required_argument,NULL,'p'},
	{"log",required_argument,NULL,'l'},
};


/* Creates server options from cmd arguments.
	[-p<PORT>|--port=<PORT>][-l<LOG>|--log=<LOG>][-h|--help]
		-p<PORT> or --port=<PORT> sets the server port.
									It works on 127.0.0.1:PORT.
									Default PORT is 7070.
		-l<LOG> or --log=<LOG>    sets the name of logfile.
									Default PORT is './log.txt'.
		-h or --help              shows this help.
*/

Options::Options(int argc, char *argv[])
{	
	/** Default paramets **/
	_port = PORT;
	_logfilename = ssprintf(LOGFILENAME);
	
	/** No arguments **/
	if(1 == argc) 
		return;
	
	int answ = 0;
	int option_index = -1;
	
	bool has_option = false;
	
	while (EOF != (answ = getopt_long(argc, argv, 
		short_options, long_options, &option_index))){
		switch(answ){
			case 'h': { /** -h | --help **/	
				fputs(HELP, stderr); 
				has_option = true;
				exit(1); // break;
				
			};
			case 'p': { /** -p | --port **/	
				if (NULL != optarg){
					_port = atoi(optarg);
					if ((65535 < _port) or (0 >= _port)){
						_port = PORT; /* defult port */
						fprintf(stderr, "Wrong port number. "
							"Required (0, 65535]. " "We will use %d.\n", _port);
					}
				}
				has_option = true;
				break; 
			};
			case 'l': { /** -p | --port **/	
				if (NULL != optarg){
					_logfilename = ssprintf("%s\0", optarg);
				}
				has_option = true;
				break;
			};
			case '?': /** any another **/ 	
			default: {
				fprintf(stderr, "found unknown option\n\n");
				fputs(USAGE, stderr); 
				has_option = true;
				exit(1); // break;
			}
		}
	}
	if(false == has_option){
		fprintf(stderr, "found unknown argument\n\n");
		fputs(USAGE, stderr); 
		exit(1); 
	}
}

int Options::getPort()
{
	return _port;
}

char* Options::getLogfilename()
{
	return _logfilename;
}

#ifdef __SERVER_OPTIONS_MAIN__
int main(int argc, char* agrv[], char* env[]){
	Options options = Options(argc, argv);
	printf("Port = %d\n", options.getPort());
	printf("Logfilename = %s\n", options.getLogfilename().c_str());
	return 0;
};
#endif /* __SERVER_OPTIONS_MAIN__ */
