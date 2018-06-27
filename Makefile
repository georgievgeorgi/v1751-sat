CC=g++
ROOTCFLAGS=$(shell root-config --cflags)
ROOTCLIBS=$(shell root-config --libs)
CFLAGS=-O0 -g -Wall

.PHONY: all clean build debug analyser.exe
.IGNORE: clean
.DEFAULT_GOAL:=all


HEADERFILES=Analyse.h Histos.h main.h v1751raw.h
SRCFILES=Analyse.cc main.cc

EXEC=analyser.exe

all: $(EXEC)

$(EXEC): $(HEADERFILES) $(SRCFILES)
	$(CC) $(ROOTCFLAGS) $(ROOTCLIBS) $(SRCFILES) $(CFLAGS) -o $@
