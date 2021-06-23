## Calibration Execution Makefile

#defines targets all and clean
.PHONY = all clean

#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#
#	$^ The names of all the prerequisites, with spaces between them.
#	$< The name of the first prerequisite.
#	$@ The file name of the target of the rule

# define the compiler to use
CC = g++

# compile-time flags
#CFLAGS =-Wall -g -fsanitize=address -static-libasan -fsanitize=leak -fno-omit-frame-pointer
#CFLAGS =-g -fsanitize=address -static-libasan -fsanitize=leak -fno-omit-frame-pointer
CFLAGS =-g 
	# -g provides for debugging with gdb
	# -Wall provides extra errors and warnings from compilation
	# -fsanitize=leak catches memory leaks, out of bound array access, and use after free errors

UTIL_DIR =./src/Util/
SSH_DIR =./src/SSH/
MAIN_DIR =./src/

# define any include directories for header files other than /usr/include
# -I means include directory
INCLUDES = -I $(UTIL_DIR) -I $(SSH_DIR) -I ./

# libraries to link into executable:
LIBS =-l ssh -l m

# define the source .cpp files
SSH_SRC = $(wildcard src/SSH/*.cpp)
UTIL_SRC = $(wildcard src/Util/*.cpp)
MAIN_SRC = $(wildcard src/*.cpp)
PARAMETERS_DEF_FILE = ./Parameters.cpp
SRCS = $(SSH_SRC) $(UTIL_SRC) $(MAIN_SRC)

UTIL_INCLUDES = 	-I /src/Util/FileH.h \
					-I /src/Util/network.h \
					-I /src/Util/StringH.h \

# define the name of the executable file 
MAIN = transfer

OBJ_DIR := obj
BIN_DIR := bin
SRC_DIR := src

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of the rule (a .c file)
#  $@: the name of the target of the rule (a .o file) 
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

SSH_OBJ = $(SSH_SRC:.cpp=.o)
UTIL_OBJ = $(UTIL_SRC:.cpp=.o)
MAIN_OBJ = $(MAIN_SRC:.cpp=.o) 
PARAMETERS_OBJ = $(PARAMETERS_DEF_FILE:.cpp=.o) 
OBJS = $(SSH_OBJ) $(UTIL_OBJ) $(MAIN_OBJ) $(PARAMETERS_OBJ)

OBJECT_NAMES = $(notdir $(OBJS))
OBJ = $(addprefix $(OBJ_DIR)/, $(OBJECT_NAMES) )

red:=$(shell tput setaf 1)
bold:=$(shell tput bold)
yellow:=$(shell tput setaf 3)
reset:=$(shell tput sgr0)

#OBJ := $(patsubst %,$(OBJ_DIR)%,$(OBJ1))
all: target

# the $(OBJ_DIR) and $(BIN_DIR) creates those directories if they dont already exist
target: object_files
	@cp $(MAIN) $(BIN_DIR)/$(MAIN)

object_files: $(OBJ_DIR) $(BIN_DIR) util_module ssh_module parameters main_exec 
	@echo "$(yellow) Linking.... $(reset)"
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJ) $(LIBS)
	@echo '-------------------------------------------------------------------'
	@echo 'done.'
	@echo 'created executable  "$(MAIN)"'
	@echo 'to run:  $(red) ./$(MAIN)$(reset)'
	@echo '** $(bold)$(red)DO NOT USE SUDO $(reset) to run this executable ** -it will look in the wrong directory for ssh keys'

# for all the below rules, the object files are created in the current working directory.
# so the mv command after the compile line moves the object files to the $(OBJ_DIR)
main_exec:
	@echo "$(yellow) Main Executable $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(E3648_INCLUDE) -c $(MAIN_SRC) $(PARAMETERS_DEF_FILE) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

util_module:
	@echo "$(yellow) Util Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(UTIL_SRC) $(PARAMETERS_DEF_FILE) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

ssh_module: 
	@echo "$(yellow) SSH Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(SSH_SRC) $(UTIL_INCLUDES) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

parameters:
	@echo "$(yellow) Parameters Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(PARAMETERS_DEF_FILE) $(UTIL_INCLUDES) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

$(OBJ_DIR):
	@mkdir -p $@

$(BIN_DIR):
	@mkdir -p $@
clean:
	$(RM) *.o *~ $(MAIN)
	$(RM) $(OBJ_DIR)/*.o
	$(RM) $(BIN_DIR)/*










