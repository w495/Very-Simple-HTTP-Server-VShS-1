#ifndef __SERVER_ENGINE__
#define __SERVER_ENGINE__

#include<cerrno>
#include<cstdlib>
#include<cstdio>
#include<cstring>
#include<cstdarg>

extern "C" {
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h> 
}

#include "util.hpp"
#include "shared.hpp"

class Server{	

  public:
  
	/*! \brief Creates Server instance. It listen 127.0.0.1:portno.
	*
	*	\param portno number of target port.
	*	\param logfilename is file in which we write data according to the task:
	* 				<time> <client_string> <time> <answer>.
	*/
	Server(int portno, const char* logfilename="log.txt\0");

	/*! \brief Runs Server instance. 
	* 		Listen socket in cycle and fork process, if something was received.
	*	
	*	Calls: _accept(); _read(); fork(); _close();
	*/
	bool	run();
	
	/*! \brief Stops Server instance. Closes socket listened socket.
	*/
	void	stop();
	
	static const short CLIENT_MAX_NUMBER = 5;
	static const short SYMBOLS_MAX_NUMBER = 100;
	static const short BUFFER_SIZE = 1024;
	
  private:	
  
	/* Wrappers for standart unistd functions. */
	
  	bool	_accept();
	void	_bind(struct sockaddr_in &serv_addr);
	void 	_listen(int size);
	void	_socket(int family, int type, int protocol);
	void	_close();

	void	_zombie_handling();
	
	/* I/O  functions for sockets. */
	
	char *		_sread_header();
	void 		_swrite(const char * buff);


	void		_sprintf(const char* fmt, ...);

	
	/** DATA **/
	
  	/*  main socket */	int _listen_socket;
	/* joint socket */	int _data_socket;
	/*   "log.txt"  */ 	const char* _logfilename;

};

#endif /* __SERVER_ENGINE__ */