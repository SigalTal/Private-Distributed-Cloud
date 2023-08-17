
# make clean/all CONFIG=Debug/Release ARCH=linux64/raspi ECHO=0/1

# the parent dir of "arch" dir
ROOT_DIR = ../../../../..

PROJ_SRCS = $(wildcard *.cpp)
#PROJ_SRCS = aaa.c bbb.cpp

# list of directories containing header files
#PROJ_INC_DIR = $(ROOT_DIR)/include
PROJ_INC_DIR = ../../include ../../../framework/include

# the executable file
PROJ_OUT = atlas_ThreadPool_main_Test.out

# list of ADDITIONAL compiler flags (prepended with - sign)
#PROJ_CFLAGS = -Wunused-but-set-variable
PROJ_CFLAGS =
PROJ_CPPFLAGS =-std=c++11

#PROJ_MACROS = _BSD_SOURCE
PROJ_MACROS =

# additional object files (created by other projects)
PROJ_ADD_OBJS = nbd_comm.o select.obj function.obj ThreadPool.obj semaphore.obj

# additional libraries
PROJ_LIBS =

# linker flags
PROJ_LDFLAGS =

# list of projects (makefiles) this project depends on
PROJ_DEPENDS  = ../../../framework/src/function/Makefile ../../../framework/src/rector/Makefile ../nbd/Makefile ../../../framework/src/ThreadPool/Makefile ../../../framework/src/semaphore/Makefile ../../../framework/src/singleton/Makefile

include $(ROOT_DIR)/make/Project.mak

