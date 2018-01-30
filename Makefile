CC=g++
ROOTCFLAGS=$(shell root-config --cflags)
ROOTCLIBS=$(shell root-config --libs)
CFLAGS=-O2 -Wall

.PHONY: all clean build debug
.IGNORE: clean
.DEFAULT_GOAL:=all


HEADERFILES=Analyse.h Histos.h main.h v1751raw.h
SRCFILES=Analyse.cc main.cc

EXEC=analyser.exe

all: $(EXEC)

$(EXEC): $(HEADERFILES) $(SRCFILES)
	$(CC) $(ROOTCFLAGS) $(ROOTCLIBS) $(SRCFILES) -o $@
