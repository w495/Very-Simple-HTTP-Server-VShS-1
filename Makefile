CC := g++
DEFINES := -DDEBUG -DTEST
CFLAGS := -c -ansi -Wall -pedantic $(DEFINES)
LDFLAGS:= -lrt
# -lrt is required for linux (shm_open is in real time libs).
SRCDIR := ./src
SRC := main.cpp options.cpp server.cpp util.cpp
SOURCES := $(addprefix $(SRCDIR)/,$(SRC))
OBJDIR := ./obj
OBJ := $(SRC:.cpp=.o)
OBJECTS := $(addprefix $(OBJDIR)/,$(OBJ))
EXECUTABLE :=server

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o  $@

$(OBJECTS): | $(OBJDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/options.o: $(SRCDIR)/options.cpp
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/server.o: $(SRCDIR)/server.cpp
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/util.o: $(SRCDIR)/util.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS) 

clean-all:
	rm -f $(EXECUTABLE)
	rm -rf $(OBJDIR) 
