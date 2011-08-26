#include "server.hpp"
#include "options.hpp"

#include <cstdio>

int main (int argc, char *argv[]){
	Options options = Options(argc, argv);
	Server server = Server(options.getPort(), options.getLogfilename());
	server.run();
	
	return 0;
}
