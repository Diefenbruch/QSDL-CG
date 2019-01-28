#
# Makefile fuer den QUEST C++-Codegenerator
#
#
# Author: Marc Diefenbruch
# $Date: 1998/02/19 16:07:31 $
#
# (C) 1995 Universitaet GH Essen
#
# Dieses Makefile stellt folgende Operationen zur Verfuegung:
#
# -) make [all]     : Generator erzeugen
# -) make checkout  : alle Projektdateien auschecken
# -) make checkout-lock: alle Projektdateien zum Aendern auschecken
# -) make depend    : alle Abhaengigkeiten zwischen Projektdateien feststellen
# -) make clean     : Objektdateien (*.o) und temporaere Dateien entfernen
# -) make veryclean : make clean + rcsclean + alle Postscriptdateien entfernen
# -) make git-pull  : pull sources from github
# -) make git-push  : push sources to github
# -) make git-add   : add changed sources to git staging
# -) make git-commit: commit staged sources to git HEAD
# -) make postscript: Postscriptdateien aus alle *.h Dateien erzeugen
# -) make print     : alle *.h Dateien ausdrucken
# -) make backup    : Backup von allen *.cpp *.h Dateien und dem Makefile
# -) make find-error: Compilierungsfehler anzeigen
# -) make tags      : Source-Browser-Datei erzeugen
#
# Der Aufbau dieses Makefiles gliedert sich wie folgt:
#
#  0. Globale Umschalter (z.Z zwischen GNU- und SUN-Tools)
#  1. Makefiledirektiven
#  2. Pfade und Dateinamen
#  3. Programme, Kommandos, Kommandooptionen
#  4. Compilierungs- und Linkeroptionen
#  5. Quelldateien des Projekts
#  6. Objektdateien des Projekts
#  7. Makefileregeln (Operationen, s.o.)
#


##########################
# Check host system type #
##########################

## Don't rely on $HOSTTYPE being set correctly!
## Try to figure it out ourselves:

## Prevent make from interpreting special characters:
SCRIPT := \
	case `uname -s` in\
    	Linux)	case `uname -m` in\
					i?86) echo ix86-linux;;\
					arm*) echo arm-linux;;\
					sparc) echo sparc-linux;;\
					*) echo unknown-linux;;\
				esac;;\
		SunOS)	echo `uname -p`-SunOS;;\
		*) echo unknown-unknown;;\
	esac

## Run script and store result in variable:
QUEST_HOSTTYPE := $(shell $(SCRIPT))



##########################
# 0. Globale Umschalter: #
##########################

ifeq ($(QUEST_HOSTTYPE), sparc-SunOS)

#TOOLS = SUN
TOOLS = GNU

else

TOOLS = GNU

endif

#PROFILING = yes
PROFILING = no

##########################
# 1. Makefiledirektiven: #
##########################

.SILENT:
                         # alle Make Operationen ohne Ausgaben

.PHONY:	clean all release
                         # Welche Operationen sollen gespraechig sein?

.SUFFIXES: .cpp .h .o
                         # verwendete Dateiendungen 

############################
# 2. Pfade und Dateinamen: #
############################

INCDIR = ../../include
                         # Verzeichnis der DS-Includedateien fuer die
                         # Benutzer der DS (Generator, ...)

LIBDIR = ../../lib/$(QUEST_HOSTTYPE)
                         # Verzeichnis der DS-Library fuer die
                         # Benutzer der DS (Generator, ...)
BINDIR = ../../bin/$(QUEST_HOSTTYPE)
                         # Verzeichnis in dem die Binaerdateien gespeichert
                         # werden sollen
OBJBASEDIR = obj
                         # Verzeichnis unter dem die Objektdateien aller Hosts
                         # gespeichert werden sollen
OBJDIR = $(OBJBASEDIR)/$(QUEST_HOSTTYPE)
                         # Verzeichnis in dem die Objektdateien gespeichert
                         # werden sollen
PSDIR = ps
                         # Directory in dem die Postscriptversionen der
                         # Dateien gespeichert werden sollen
LOGFILE = $(USER).$(QUEST_HOSTTYPE).make.log
                         # Protokollierungsdatei fuer den make-Vorgang
DEPFILE = .depend.$(QUEST_HOSTTYPE)
                         # Dependency-Datei (automatisch generiert)
OUTPUT = $(OBJDIR)/libCG.a
                         # Name des erzeugten Programms/Library
TEST_OUTPUT = $(OBJDIR)/CGTest
                         # Name des Testprogramms
BACKUP = cg
                         # Name des Backupfiles (ohne Endungen!)

##############################################
# 3. Programme, Kommandos, Kommandooptionen: #
##############################################

SHELL = /bin/sh
                         # Shell
LEX = flex
                         # Kommando zum Starten des lexikalischen Analysators
LEXFLAGS = -i
                         # Flags fuer Lexer-Generator
YACC = bison
                         # Kommando zum Starten des Parser-Generators
YACCFLAGS = -d -v
                         # Flags fuer Parser-Generator
TAR = tar cf
                         # Befehl zum Erzeugen eines Sourcecode-Archievs
UNTAR = tar xf
                         # Befehl zum Expandieren eines Sourcecode-Archievs
TAR_SUFFIX = tar
                         # Endung welche das tar-Kommando verwendet
COMPRESS = gzip -9
                         # Befehl zum Komprimieren von Dateien
UNCOMPRESS = gzip -d
                         # Befehl zum Dekomprimieren von Dateien
COMPRESS_SUFFIX = gz
                         # Endung welche das Komprimierungsprogramm verwendet
EDITOR = nano
                         # Name des verwendeten Editors

GIT = git                # git

TAGS = ctags -t
                         # Programm zum Erzeugen von Sourcebrowsing-Infos
                         # (wo ist eine bestimmte Funktion definiert?, etc)
MAKE_PS = a2ps
                         # Kommando zum Erzeugen von Postscript aus ASCII
MAKE_PS_FLAGS = -o
                         # Option fuer MAKE_PS
PS_SUFFIX = ps

PRINT = lp -d ps1
                         # Druckkommando

ifeq ($(TOOLS), GNU)     # GNU-Version ?

CC = gcc
                         # Name des C-Compilers
C++ = g++
                         # Name des C++-Compilers

AR = ar
                         # Name des Archivers (zum Erzeugen von Libraries)
ARFLAGS = r
                         # Flags fuer den Archiver
else                     # SUN-Version

CC = cc
                         # Name des C-Compilers
C++ = CC
                         # Name des C++-Compilers
AR = $(C++)
                         # Name des Archivers (zum Erzeugen von Libraries)
ARFLAGS = -xar -o
                         # Flags fuer den Archiver
endif

RCSCLEAN = rcsclean
                         # Befehl zum "Aufraeumen" des Projekts (Loeschen
                         # der nicht mit Lock Option "ausgecheckten"
                         # Dateien
RM = /bin/rm -f
                         # Befehl zum Loeschen von Dateien
MV = /bin/mv
                         # Befehl zum Verschieben von Dateien
CP = /bin/cp -p
                         # Befehl zum Kopieren von Dateien
CHMOD = /bin/chmod
                         # Befehl zum Aendern der Dateiattribute
TOUCH = touch
                         # Befehl zum Aktualisieren des Dateidatums
MKDIR = mkdir -p
                         # Befehl zum Erzeugen von Directories
ifeq ($(TOOLS), GNU)

MAKE_DEPEND = $(C++) -M

else

MAKE_DEPEND = $(C++) -xM

endif
                         # Befehl zum Erzeugen der Abhaengigkeiten
STRIP = strip
                         # Befehl zum entfernen von Symbolen aus Binaerfiles
SED = sed
                         # Name des Stream-Editors sed
COFLAGS += -M -q
                         # Flags for checkout program.

#########################################
# 4. Compilierungs- und Linkeroptionen: #
#########################################

ifeq ($(TOOLS), GNU)     # GNU-Version ?

DEFINES = 
#-DDMALLOC -DDEBUG -DDMALLOC_FUNC_CHECK
                         # Defines fuer die Compiler
DEFINES += -D_PRODUCTION_RELEASE
                         # setzen fuer Auslieferungsversion von QUEST
INCLUDES = -I. -I$(INCDIR)
                         # Include-Verzeichnisse fuer die Compiler
CDEBUG = -O3 -pipe
                         # Debug-/Optimierungsoptionen
CFLAGS = $(CDEBUG) -Wall -pedantic-errors
                         # Compiler-Flags
TFLAGS = -fno-implicit-templates
                         # Template-Flags

ifeq ($(PROFILING), yes) # Profiling einschalten ?

PFLAGS = -pg

endif

LIBS = -L$(LIBDIR) -lDS
                         # Libraries die zum Projekt gelinkt werden sollen

else                     # Sun-Version !

DEFINES = 
#-DDEBUG -DDMALLOC_FUNC_CHECK
                         # Defines fuer die Compiler
INCLUDES = -I$(INCDIR)
                         # Include-Verzeichnisse fuer die Compiler
CDEBUG = -O
                         # Debug-/Optimierungsoptionen
CFLAGS = $(CDEBUG) -sb
                         # Compiler-Flags
TFLAGS = 
                         # Template-Flags

ifeq ($(PROFILING), yes) # Profiling einschalten ?

PFLAGS = -xpg

endif

LIBS = -L$(LIBDIR) -lDS
                         # Libraries die zum Projekt gelinkt werden sollen

endif

#################################
# 5. Quelldateien des Projekts: #
#################################

HEADERS =\
  CGActualParam.h\
  CGAssert.h\
  CGAtom.h\
  CGBasicTypes.h\
  CGBlock.h\
  CGBlockSubstructure.h\
  CGCall.h\
  CGClear.h\
  CGCreate.h\
  CGDecision.h\
  CGExpression.h\
  CGEventNames.h\
  CGFile.h\
  CGFormalParam.h\
  CGGenerator.h\
  CGWriter.h\
  CGInput.h\
  CGInputSignal.h\
  CGInputTimerSignal.h\
  CGJoin.h\
  CGMachine.h\
  CGMachineService.h\
  CGMachineServiceSpeed.h\
  CGMain.h\
  CGMake.h\
  CGNextState.h\
  CGOperandConstant.h\
  CGOperandExpression.h\
  CGOperandLiteral.h\
  CGOperandMachineService.h\
  CGOperandOperator.h\
  CGOperandProcedureCall.h\
  CGOperandProcessAccess.h\
  CGOperandSensor.h\
  CGOperandSignal.h\
  CGOperandState.h\
  CGOperandSynonym.h\
  CGOperandVariableAccess.h\
  CGOperator.h\
  CGOutput.h\
  CGProcedure.h\
  CGProcess.h\
  CGProcessAccess.h\
  CGRequest.h\
  CGReset.h\
  CGReturn.h\
  CGSensor.h\
  CGSensorNames.h\
  CGSensorProcess.h\
  CGSensorMachineService.h\
  CGSensorSignal.h\
  CGSensorState.h\
  CGSet.h\
  CGSignal.h\
  CGSort.h\
  CGSortExtended.h\
  CGState.h\
  CGStruct.h\
  CGStop.h\
  CGSynonym.h\
  CGSyntype.h\
  CGSystem.h\
  CGTask.h\
  CGTimer.h\
  CGTransition.h\
  CGUpdate.h\
  CGVariable.h\
  CGVariableAccess.h\
  CGVariableAccessField.h\
  CGVariableAccessIndex.h

SRCS =\
  CGAssert.cpp\
  CGAtom.cpp\
  CGBlock.cpp\
  CGBlockSubstructure.cpp\
  CGCall.cpp\
  CGClear.cpp\
  CGCreate.cpp\
  CGDecision.cpp\
  CGExpression.cpp\
  CGFile.cpp\
  CGGenerator.cpp\
  CGInput.cpp\
  CGInputSignal.cpp\
  CGInputTimerSignal.cpp\
  CGJoin.cpp\
  CGMachine.cpp\
  CGMachineService.cpp\
  CGMachineServiceSpeed.cpp\
  CGMain.cpp\
  CGMake.cpp\
  CGNextState.cpp\
  CGOperandConstant.cpp\
  CGOperandExpression.cpp\
  CGOperandLiteral.cpp\
  CGOperandMachineService.cpp\
  CGOperandOperator.cpp\
  CGOperandProcedureCall.cpp\
  CGOperandProcessAccess.cpp\
  CGOperandSensor.cpp\
  CGOperandSignal.cpp\
  CGOperandState.cpp\
  CGOperandSynonym.cpp\
  CGOperandVariableAccess.cpp\
  CGOperator.cpp\
  CGOutput.cpp\
  CGProcedure.cpp\
  CGProcess.cpp\
  CGProcessAccess.cpp\
  CGRequest.cpp\
  CGReset.cpp\
  CGReturn.cpp\
  CGSensor.cpp\
  CGSensorProcess.cpp\
  CGSensorMachineService.cpp\
  CGSensorSignal.cpp\
  CGSensorState.cpp\
  CGSet.cpp\
  CGSignal.cpp\
  CGSort.cpp\
  CGSortExtended.cpp\
  CGState.cpp\
  CGStop.cpp\
  CGStruct.cpp\
  CGSynonym.cpp\
  CGSyntype.cpp\
  CGSystem.cpp\
  CGTask.cpp\
  CGTimer.cpp\
  CGTransition.cpp\
  CGUpdate.cpp\
  CGVariable.cpp\
  CGVariableAccess.cpp\
  CGVariableAccessField.cpp\
  CGVariableAccessIndex.cpp\
  CGWriter.cpp

TEST_SRCS =\
  CGTest.cpp

##################################
# 6. Objektdateien des Projekts: #
##################################

OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))
TEST_OBJS= $(addprefix $(OBJDIR)/, $(TEST_SRCS:.cpp=.o))

######################
# 7. Makefileregeln: #
######################

default: clean-rubbish $(OBJDIR) $(OUTPUT)

$(OUTPUT): $(OBJS)
	echo Constructing $(OUTPUT) ...
	$(AR) $(ARFLAGS) $(OUTPUT) $(OBJS) \
		2>> $(LOGFILE)

$(OBJDIR)/%.o: %.cpp
	@echo Compiling $< ...
	$(C++) -c $(CFLAGS) $(TFLAGS) $(PFLAGS) $(DEFINES) $(INCLUDES) $< -o $@ 2>> $(LOGFILE)

test: clean-rubbish $(OUTPUT) $(TEST_OBJS)
	@echo Linking $(TEST_OUTPUT) ...
	$(C++) $(CDEBUG) $(PFLAGS) -o $(TEST_OUTPUT)\
		$(TEST_OBJS)\
		$(OUTPUT)\
		$(LIBS)\
    2>> $(LOGFILE)

all: $(OUTPUT) test

$(OBJBASEDIR):
	@if [ ! \( -d $(OBJBASEDIR) \) ]; then \
		echo Creating $(OBJBASEDIR) ...; \
		$(MKDIR) $(OBJBASEDIR); fi

$(OBJDIR): $(OBJBASEDIR)
	@if [ ! \( -d $(OBJDIR) \) ]; then \
		echo Creating $(OBJDIR) ...; \
		$(MKDIR) $(OBJDIR); fi

$(LIBDIR): 
	@if [ ! \( -d $(LIBDIR) \) ]; then \
		echo Creating $(LIBDIR) ...; \
		$(MKDIR) $(LIBDIR); fi

$(BINDIR): 
	@if [ ! \( -d $(BINDIR) \) ]; then \
		echo Creating $(BINDIR) ...; \
		$(MKDIR) $(BINDIR); fi

$(PSDIR): 
	@if [ ! \( -d $(PSDIR) \) ]; then \
		echo Creating $(PSDIR) ...; \
		$(MKDIR) $(PSDIR); fi

$(INCDIR): 
	@if [ ! \( -d $(INCDIR) \) ]; then \
		echo Creating $(INCDIR) ...; \
		$(MKDIR) $(INCDIR); fi

$(INCDIR)/CG: $(INCDIR)
	@if [ ! \( -d $(INCDIR)/CG \) ]; then \
		echo Creating $(INCDIR)/CG ...; \
                $(MKDIR) $(INCDIR)/CG; fi

$(DEPFILE):
	$(TOUCH) $(DEPFILE)

install-lib: $(OUTPUT) $(LIBDIR)
	@echo Deleting old library from $(LIBDIR) ...
	-$(RM) $(LIBDIR)/$(OUTPUT)
	@echo Installing new library in $(LIBDIR) ...
	$(CP)  $(OUTPUT) $(LIBDIR)

install-includes: $(HEADERS) $(INCDIR)/CG
	@echo Deleting old include files from $(INCDIR)/CG ...
	-$(RM) $(INCDIR)/CG/*.h
	@echo Installing new include files in $(INCDIR)/CG ...
	for X in $(HEADERS); do \
		$(CP)  $${X} $(INCDIR)/CG;\
		$(CHMOD) 644 $(INCDIR)/CG/$${X}; done

install: install-includes install-lib

release: install

git-pull:
	@echo Pulling sources form github...
	$(GIT) pull

git-add:
	@echo Staging changed local sources...
	$(GIT) add -A

git-commit:
	@echo Committing changed local sources...
	$(GIT) commit

git-push:
	@echo Pushing sources to github...
	$(GIT) push

postscript: $(HEADERS) $(PSDIR)
	@for X in $(HEADERS); do \
		echo Generating $$X.$(PS_SUFFIX) from $$X ...; \
		$(MAKE_PS) $(MAKE_PS_FLAGS) $(PSDIR)/$$X.$(PS_SUFFIX) $$X; done

print: postscript
	$(PRINT) $(PSDIR)/*.$(PS_SUFFIX)
	-$(RM) *.$(PS_SUFFIX)

backup: $(SRCS) $(HEADERS) $(TEST_SRCS)
	-$(RM) $(BACKUP).$(TAR_SUFFIX) $(BACKUP).$(TAR_SUFFIX).$(COMPRESS_SUFFIX)
	$(TAR) $(BACKUP).$(TAR_SUFFIX) $(SRCS) $(HEADERS) $(TEST_SRCS) Makefile
	$(COMPRESS) $(BACKUP).$(TAR_SUFFIX)
	-$(MV) $(BACKUP).$(TAR_SUFFIX).$(COMPRESS_SUFFIX) $(BACKUP).taz

find-error fe:
	$(EDITOR) -p $(LOGFILE)

tags: $(SRCS)
	-$(TAGS) $(SRCS)

clean-rubbish:
	-$(RM) core* *.bak *~ $(LOGFILE)

clean-objects:
	-$(RM) $(OBJDIR)/*.o $(OUTPUT) $(TEST_OUTPUT) *.o

clean-rcs:
	-@$(RCSCLEAN) 2> /dev/null

clean: clean-rubbish clean-objects

veryclean: clean clean-rcs 
	-$(RM) $(PSDIR)/*.$(PS_SUFFIX) *.$(PS_SUFFIX) *.$(TAR_SUFFIX) *.$(COMPRESS_SUFFIX) *.taz *tags 2> /dev/null

checkout:
	-@$(CO) -q $(HEADERS)\
						 $(SRCS)

checkout-lock:
	-@$(CO) -l $(HEADERS)\
						 $(SRCS)\

replace-headers: $(HEADERS)
	for X in $(HEADERS); do \
		$(MV) $$X $$X.old; \
		$(SED) 's/Message/Signal/g' $$X.old >> $$X ; done

replace-sources: $(SRCS)
	for X in $(SRCS); do \
		$(MV) $$X $$X.old; \
		$(SED) 's/Message/Signal/g' $$X.old >> $$X ; done

depend: $(HEADERS) $(SRCS)
	@echo Building dependency file $(DEPFILE) ...
	$(MAKE_DEPEND) $(DEFINES) $(SRCS) $(INCLUDES) > $(DEPFILE)
	$(SED) 's/^\(.*\)\.o/\$$(OBJDIR)\/\1\.o/g' $(DEPFILE) > $(DEPFILE).sed
	$(MV) $(DEPFILE).sed $(DEPFILE)


-include $(DEPFILE)

