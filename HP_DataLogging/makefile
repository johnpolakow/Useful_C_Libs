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
CFLAGS =-g -fsanitize=address -static-libasan -fsanitize=leak -fno-omit-frame-pointer
	# -g provides for debugging with gdb
	# -Wall provides extra errors and warnings from compilation
	# -fsanitize=leak catches memory leaks, out of bound array access, and use after free errors

UTIL_DIR =./src/Util/
MAIN_DIR =./src/
E3648A_DIR = ./src/E3648A
HP34401_DIR = ./src/HP34401/

# define any include directories for header files other than /usr/include
# -I means include directory
INCLUDES = -I $(HP34401_DIR) -I $(E3648A_DIR) -I $(UTIL_DIR) -I $(MAIN_DIR) -I ./

# libraries to link into executable:
# link in libbcm2835.so and libm.so: (PI GPIO library and math library)
#LIBS = -l bcm2835 -l m
LIBS =-l m

# define the source .cpp files
UTIL_SRC = $(wildcard src/Util/*.cpp)
MAIN_SRC = $(wildcard src/*.cpp)
HP34401_SRC = $(wildcard src/HP34401/*.cpp)
E3648_SRC = $(wildcard src/E3648A/*.cpp)
PARAMETERS_DEF_FILE = ./Cal_Parameters.cpp
SRCS = $(PI_SRC) $(UTIL_SRC) $(MAIN_SRC) $(HP34401_SRC) $(E3648_SRC)

UTIL_INCLUDES = 	-I /src/Util/Struct_Defs.h \
					-I /src/Util/TStamp.h \
					-I /src/Util/TimeH.h \
					-I /src/Util/StringH.h \
					-I /src/Util/Data.h \
					-I ./

HP_INCLUDES = 		-I /src/Util/StringH.h \
					-I /src/Util/Data.h \
					-I ./

E3648_INCLUDES = 	-I /src/Util/StringH.h \
					-I /src/Util/Data.h \
					-I ./


# define the name of the executable file 
MAIN = datalog

OBJ_DIR := obj
BIN_DIR := bin
SRC_DIR := src

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of the rule (a .c file)
#  $@: the name of the target of the rule (a .o file) 
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

UTIL_OBJ = $(UTIL_SRC:.cpp=.o)
MAIN_OBJ = $(MAIN_SRC:.cpp=.o) 
HP34401_OBJ = $(HP34401_SRC:.cpp=.o) 
E3648_OBJ = $(E3648_SRC:.cpp=.o) 

PARAMETERS_OBJ = $(PARAMETERS_DEF_FILE:.cpp=.o) 
OBJS = $(UTIL_OBJ) $(MAIN_OBJ) $(PARAMETERS_OBJ) $(HP34401_OBJ) $(E3648_OBJ)

OBJECT_NAMES = $(notdir $(OBJS))
OBJ = $(addprefix $(OBJ_DIR)/, $(OBJECT_NAMES) )

ccred=$(shell echo -e "\033[0;31m")
ccend=$(shell echo -e "\033[0m")

red:=$(shell tput setaf 1)
bold:=$(shell tput bold)
yellow:=$(shell tput setaf 3)
reset:=$(shell tput sgr0)

#OBJ := $(patsubst %,$(OBJ_DIR)%,$(OBJ1))
all: target

# the $(OBJ_DIR) and $(BIN_DIR) creates those directories if they dont already exist
target: object_files
	@cp $(MAIN) $(BIN_DIR)/$(MAIN)

object_files: $(OBJ_DIR) $(BIN_DIR) parameters util_module hp_module e3648_module cal_main 
	@echo "$(yellow) Linking.... $(reset)"
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJ) $(LIBS)
	@echo '-------------------------------------------------------------------'
	@echo 'done.'
	@echo 'created executable  "$(bold)$(red)$(MAIN)$(reset)"'
	@echo 'to run, enter command:  "$(bold)$(red)sudo ./$(MAIN)$(reset)"'

# for all the below rules, the object files are created in the current working directory.
# so the mv command after the compile line moves the object files to the $(OBJ_DIR)
cal_main:
	@echo "$(yellow) Cal Main Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(E3648_INCLUDE) -c $(MAIN_SRC) $(PARAMETERS_DEF_FILE) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

util_module:
	@echo "$(yellow) Util Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(UTIL_SRC) $(PARAMETERS_DEF_FILE) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

hp_module:
	@echo "$(yellow) HP34401 Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(HP34401_SRC) $(PARAMETERS_DEF_FILE) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""


e3648_module:
	@echo "$(yellow) E3648 Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(E3648_INCLUDES) -c $(E3648_SRC) $(PARAMETERS_DEF_FILE) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

parameters:
	@echo "$(yellow) Parameters Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(PARAMETERS_DEF_FILE) $(LIBS)
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










