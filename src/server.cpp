#include "server.hpp"

/* Creates Server instance. It listen 127.0.0.1:portno.
*
*	portno is number of target port.
*	logfilename is file in which we write data according to the task:
* 		<time> <client_string> <time> <answer>.
*/
Server::Server(int portno, const char* logfilename){

	_logfilename = logfilename;
	_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// see /etc/protocols for detales.
	
	struct sockaddr_in serv_addr;
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	
	_bind(serv_addr);
	_listen(CLIENT_MAX_NUMBER);
	_zombie_handling();
}

/* Runs Server instance. Listen socket in cycle and fork process, 
* 	if something was received. Calls: _accept(); _read(); fork(); _close().
*/

bool
Server::run()
{
	Shared<size_t> counter_1 = Shared<size_t>();
	if(not counter_1.isValid()){
		perror("Cannot create objects in counter_1 memory. Sorry.");
		return false;
	}
	counter_1 = 0;
	size_t  counter_2 = 0;

	
	
	for(;;){
		if(_accept()){
			pid_t pid = fork();
			if (-1 == pid){   
			  perror("Can't fork");
			  return false;
			}
			if (0 == pid){
					time_t accept_time; time(&accept_time);
					
					char* client_string = _sread_header();
					if(!client_string)	_exit(0);
					counter_1 += 1; // Shared<T>& operator += (const T &new_t)
					counter_2 += 1;

					/**
						We use it cause getValue can
						return another value next moment;
					**/
					
					size_t c1v = counter_1.getValue();
					
					char* html = ssprintf(
					"<html><head><title>%s</title></head>"
					"<body>"
						"Request (%lu):\n"
						"<pre style=\"color:red;\">%s</pre>"
					"</body></html>\n\n",
					"Request", c1v, client_string);					
#ifdef TEST
					/** Any long operation **/
					sleep(10);
#endif /* TEST */

					_sprintf("HTTP/1.1 200 OK\n"
					"Content-Type: text\\html\n"
					"Content-Length: %d\n"
					"\n%s\n",
					strlen(html), html);
					
					free(html);
					html = NULL;
					
					time_t exit_time; time(&exit_time);
					
					FILE* logfile;
					logfile = fopen(_logfilename, "a");
					if(NULL == logfile){
						fprintf(stderr, "Cannot to open logfile\n");
					}

					char* faccept_time = getFormatedTime(accept_time);
					char* fexit_time = getFormatedTime(exit_time);
					
					fprintf(logfile, "%2.2lu [%s] -> [%s]:\n%s\n\n",
							c1v, faccept_time,
							fexit_time, client_string);

					free(faccept_time); faccept_time = NULL;
					free(fexit_time); 	fexit_time = NULL;
					
					fflush(logfile);
					fclose(logfile);

					free(client_string); client_string = NULL;
					
				_exit(0);
			}
			
			_close();
		}
#ifdef TEST
		fprintf(stderr, "\ncounter_1 = %lu\n",  counter_1.getValue());
		fprintf(stderr, "\ncounter_2 = %lu\n",  counter_2); // is 0;
		fflush(stderr);
#endif /* TEST */
	}
}

/* ==========================================================================
		Server::Server(int portno, const char* logfilename){
 ========================================================================== */

/* Creates socket with error handling. */
void
Server::_socket(int family, int type, int protocol)
{
	_listen_socket = socket(family, type, protocol);
	if(0 > _listen_socket){
		error("socket error");
	}
}

/* Binds the socket with error handling. */
void
Server::_bind(struct sockaddr_in &serv_addr)
{
	int on = 1;
	int n;
	/* Make server socket reusable.
		
		int setsockopt(int sockfd, int level, int optname,
			const void *optval, socklen_t optlen);
		
		getsockopt()  and setsockopt() manipulate options for the socket
		referred to by the file descriptor sockfd.  Options may exist
		at multiple protocol levels; they are always present
		at the uppermost socket level.
		
    */

	n = setsockopt(_listen_socket, SOL_SOCKET, SO_REUSEADDR,
		(char *)&on, sizeof(on));
	if (0 > n){
		error("setsockopt error");
	}
	
	n = bind(_listen_socket, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr));
	if (0 > n) 
		error("bind error");
}	


/* Listens the socket. <Size> clients may be connected. */
void
Server::_listen(int size)
{
	listen(_listen_socket, size);
}


void Server::_zombie_handling(){
	/** Zombie handling **/

	/* 	POSIX.1-2001 allows a parent process to elect for the kernel
		to automatically reap child processes that terminate by setting
		the disposition of SIGCHLD to SIG_IGN or by setting the SA_NOCLDWAIT
		flag for the SIGCHLD signal; Linux 2.6 kernels adhere to this behavior,
		and FreeBSD supports both of these methods since version 5.0
	*/

    struct sigaction sa;
    sigaction(SIGCHLD, NULL, &sa);
    sa.sa_handler = SIG_IGN;
    sigaction(SIGCHLD, &sa, NULL);
}

void
Server::stop()
{
	close(_listen_socket);
}

/* ==========================================================================
		bool Server::run()
 ========================================================================== */

/* 	Accepts connections with error handling. 
	Creates joint (data) socket. 			*/
bool
Server::_accept()
{
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	clilen = sizeof(cli_addr);
	_data_socket = accept(_listen_socket, (struct sockaddr *)
		&cli_addr, &clilen);
	if (_data_socket < 0)
		return false;
	return true;
}

/*  Closes joint (data) socket. */	
void
Server::_close()
{
	close(_data_socket);
}

/* ==========================================================================
		I/O  functions for sockets.
 ========================================================================== */

/* Reads from socket while this is possible. */

char *
Server::_sread_header()
{
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	
	/** If we will use fdopen(_data_socket, "r"),
			we also should use fscanf.
		May be in some cases it'll be better.
		For example if we want 
			to parse input_string with anoher way:
			
			char char_1, char_2, char_3; 
			char rest_string[256];
			fscanf(_data_socket_stream, "%c%c%c%s",
				char_1, char_2, char_3, rest_string);
	**/
	
	short symbols = -1;
	long times = -1;
	
	long char_size = 0;
	long memo_size = sizeof(char)*BUFFER_SIZE;
	
	char *res;
	if ((res = (char* )malloc(memo_size)) == NULL){
		perror("ERROR malloc failure");
		return NULL;
	}
	memset(res, 0, memo_size);

	
	for(;;){

		/** In this we use dynamic string for input_srings bigger then 
			BUFFER_SIZE - 1. Be carefull with very big stings, 
			they may not work correctly. 
		**/
		symbols = read(_data_socket, buffer, BUFFER_SIZE - 1);

		if (0 > symbols) {	
			perror("Error reading from socket");
			free(res);
			res = NULL;
			return res;
		}
		
		char_size += symbols;
#ifdef DEBUG		
		fprintf(stderr, "\n\nchar_size =  %li\n\n", char_size );
#endif /*DEBUG*/		
		if(char_size >= memo_size){
			memo_size += sizeof(char) * (BUFFER_SIZE);
			if ((res = (char* )realloc (res, memo_size)) == NULL)	{
				fprintf(stderr,
						"ERROR realloc failure: memo_size = %li", memo_size );
				free(res);
				res = NULL;
				return res;
			}
		}
		
		/** TODO use memcpy **/
		res = strncat(res, buffer, symbols);

		if(BUFFER_SIZE - 1 > symbols){ // REQUEST
			break;
		}

		symbols = -1;
		memset(buffer, 0, BUFFER_SIZE);
		times += 1;
	}
#ifdef DEBUG
	fprintf(stderr, "%s", res);
	fflush(stderr);
#endif /*DEBUG*/
	return res;
}

/* Dummy write with error handling. */

void
Server::_swrite(const char * buff)
{
	int n = write(_data_socket, buff, strlen(buff));
	if (0 > n)
		perror("ERROR writing to socket");
}


void
Server::_sprintf(const char* fmt, ...)
{
	int symbols;
	size_t size = BUFFER_SIZE;
	char *p = NULL;
	va_list ap;
	if ((p = (char* )malloc (sizeof(char) * size)) == NULL){
		perror("ERROR malloc failure");
		if(p) free(p);
		p = NULL;
		return;
	}

	while (1) {
	/* Try to print in the allocated space. */
		memset(p, 0, size);
		va_start(ap, fmt);
		symbols = vsnprintf (p, size, fmt, ap);
		va_end(ap);
		if (symbols > -1 && (size_t )symbols < size)
			break;
		/* Else try again with more space. */
		if (symbols > -1){    					/* glibc 2.1 */
			size = symbols + 1; 				/* precisely what is needed */
		}
		else{           						/* glibc 2.0 */
			size *= 2;  						/* twice the old size */
		}

		if ((p = (char* )realloc(p, sizeof(char) * size)) == NULL)	{
			perror("ERROR realloc  failure");
			return;
		}
	}
	int status = write(_data_socket, p, (size_t)symbols);
	if (0 > status)
		perror("ERROR writing to socket");
	
	if(p) free(p);
	p = NULL;
}

#ifdef __SERVER_MAIN__
int main(int argc, char* agrv[], char* env[]){
	Server server = Server(7070);
	bool runed = server.run();
	if(runed)
		server.stop();
	return 0;
}
#endif /* __SERVER_MAIN__ */

