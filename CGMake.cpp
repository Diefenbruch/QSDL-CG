/****************************************************************************
 * $RCSfile: CGMake.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/02/24 09:21:25 $
 * $Revision: 1.12 $
 ***************************************************************************/

#ifdef USE_PRAGMA
#pragma interface
#endif

/****************************************************************************
 * Include-Anweisungen
 ****************************************************************************/

#include <DS/DSString.h>
#include <DS/DSSystem.h>
#include <DS/DSBlock.h>
#include <DS/DSProcess.h>
#include <DS/DSProcedure.h>
#include <DS/DSMachine.h>

#include "CGFile.h"
#include "CGMake.h"

#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Konstanten
 ****************************************************************************/

/***************************************************************************
 * CGMake(): Konstruktor
 *            -> sys: Name des Systems
 *            Seiteneffekte: keine
 ***************************************************************************/

CGMake::CGMake(void)
{
}

/***************************************************************************
 * ~CGMake(): Destruktor
 *             Seiteneffekte: keine
 ***************************************************************************/

CGMake::~CGMake(void)
{
}

/***************************************************************************
 * CGMakefile(): erzeugt und schreibt das Makefile
 *              -> writer
 *              Ergebnis: DS_OK oder DS_ERROR
 *              Seiteneffekte: keine
 ***************************************************************************/

DSResult CGMake::CGMakefile(CGWriter *writer)
{
  DSResult result;
//  DSCardinal i = 0;
  DSString *system_name;
  DSString filename(FILE_MAKE);
  DSSystem *system;
  
  DSString str;

  CGFile makefile(writer->CGGetOutputDir(), &filename);

  system = writer->CGGetSystem();
  assert(system);

  system_name = system->GetName();
  assert(system_name);

  if (makefile.CGFileOpen() != DS_OK)
  {
    writer->cgCout << "Can't open " << makefile.CGFilePath();
    writer->cgCout << makefile.CGFileName() << std::endl;
    return DS_ERROR;
  }
  writer->CGSetFilePointer(&makefile);

  InsertString(makefile, "#", 0, CG_WITH_EOL);
  InsertString(makefile, "# Makefile for the QSDL simulator", 0, CG_NO_EOL);
  str.Format(" '%s'", system_name->GetString());
  InsertString(makefile, str, 0, CG_WITH_EOL);
  InsertString(makefile,
               "#\n"
               "# Authors: Marc Diefenbruch (md@informatik.uni-essen.de)\n"
               "#          Axel Hirche (hirche@informatik.uni-essen.de)\n"
               "#", 0, CG_WITH_EOL);
  InsertString(makefile,
               "# (C) 1995-98 University of Essen\n"
               "#\n"
               "# All rights reserved\n"
               "#", 0, CG_WITH_EOL);
  InsertString(makefile,
               "# Supported Operations:\n"
               "#", 0, CG_WITH_EOL);
  InsertString(makefile, "# -) make [all]            : Create simulator",
               0, CG_WITH_EOL);
  InsertString(makefile, "# -) make depend           : Create dependencies",
               0, CG_WITH_EOL);
  InsertString(makefile, "# -) make clean            : Delete object files and executable",
               0, CG_WITH_EOL);
  InsertString(makefile, "# -) make veryclean        : make clean + delete Log and MSC files",
               0, CG_WITH_EOL);
  InsertString(makefile, "# -) make find-error       : Debug",
               0, CG_WITH_EOL);
  InsertString(makefile, "# -) make tags             : Create source browser database",
               0, CG_WITH_EOL);
  InsertString(makefile, "#",
               0, CG_WITH_EOL);
  InsertLine(makefile);


  InsertString(makefile, "###########################\n"
                         "# 0. Preliminaries:       #\n"
                         "###########################", 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "SCRIPT = \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\tcase `uname -s` in \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\tLinux)\tcase `uname -m` in \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\t\t\ti?86)\techo ix86-linux;; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\t\t\tx86_64)\techo x86_64-linux;; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\t\t\tarm*)\techo arm-linux;; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\t\t\tsparc)\techo sparc-linux;; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\t\t\t*)\t\techo unknown-linux;; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\t\tesac;; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\tSunOS)\techo `uname -p`-SunOS;; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t*)\t\techo unknown-unknown;; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\tesac", 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "QUEST_HOSTTYPE := $(shell $(SCRIPT))", 0, CG_WITH_EOL);
  InsertLine(makefile);

  /***********************/
  /* Makefiledirektiven: */
  /***********************/
  InsertString(makefile, "###########################\n"
                         "# 1. Makefile directives: #\n"
                         "###########################", 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, ".SILENT:", 0, CG_WITH_EOL);
  InsertString(makefile, ".PHONY: clean all", 0, CG_WITH_EOL);
  str.Format(".SUFFIXES: %s %s %s %s %s\n",
             SUFFIX_C, SUFFIX_C_INC, SUFFIX_H, SUFFIX_H_INC, SUFFIX_O);
  InsertString(makefile, str, 0, CG_WITH_EOL);

  /*************************/
  /* Pfade und Dateinamen: */
  /*************************/
  InsertString(makefile, "########################\n"
                         "# 2. Files and pathes: #\n"
                         "########################", 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "OBJBASEDIR = obj", 0, CG_WITH_EOL);
  InsertString(makefile, "OBJDIR = $(OBJBASEDIR)/$(QUEST_HOSTTYPE)", 0, CG_WITH_EOL);
  str.Format("OUTPUT = $(OBJDIR)/%s", system_name->GetString());
  InsertString(makefile, str, 0, CG_WITH_EOL);
  InsertString(makefile, "LOGFILE = $(USER).$(QUEST_HOSTTYPE).make.log", 0, CG_WITH_EOL);
  InsertString(makefile, "DEPFILE = .depend.$(QUEST_HOSTTYPE)", 0, CG_WITH_EOL);
  InsertString(makefile, "INCDIR = $(QUEST_INC_DIR)", 0, CG_WITH_EOL);
  InsertString(makefile, "LIBDIR = $(QUEST_LIB_DIR)", 0, CG_WITH_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(makefile, "X11_INCDIR = $(QUEST_X11_INC_DIR)", 0, CG_WITH_EOL);
    InsertString(makefile, "X11_LIBDIR = $(QUEST_X11_LIB_DIR)", 0, CG_WITH_EOL);
  }
  InsertLine(makefile);

  /*********************************************/
  /* Programme Kommandos und Kommandooptionen: */
  /*********************************************/
  InsertString(makefile, "###########################################\n"
                         "# 3. Tools,", 0, CG_NO_EOL);
  InsertString(makefile, " commands and command options: #\n"
                         "###########################################\n",
               0, CG_WITH_EOL);
  InsertString(makefile, "EDITOR = $(QUEST_EDITOR)", 0, CG_WITH_EOL);
  InsertString(makefile, "TAGS = ctags -t", 0, CG_WITH_EOL);
  InsertString(makefile, "C++ = $(QUEST_CC)", 0, CG_WITH_EOL);
#ifdef _PRODUCTION_RELEASE
  InsertString(makefile, "MAKE_DEPEND = $(C++) -MM", 0, CG_WITH_EOL);
#else
  InsertString(makefile, "MAKE_DEPEND = $(C++) -M", 0, CG_WITH_EOL);
#endif
  InsertString(makefile, "MKDIR = mkdir", 0, CG_WITH_EOL);
  InsertString(makefile, "MV = mv -f", 0, CG_WITH_EOL);
  InsertString(makefile, "SED = sed", 0, CG_WITH_EOL);
  InsertString(makefile, "TOUCH = touch", 0, CG_WITH_EOL);
  InsertString(makefile, "DIFF = cpp-diff", 0, CG_WITH_EOL);
  InsertLine(makefile);

  /**************************************/
  /* Compilierungs- und Linkeroptionen: */
  /**************************************/
  InsertString(makefile, "###################################\n"
                         "# 4. Compiler and linker options: #\n"
                         "###################################\n",
               0, CG_WITH_EOL);
//  str.Format("DEFINES = $(QUEST_THREAD_TYPE)");
  str.Format("DEFINES = ");
  if (writer->CGDebug())
  {
    str += " ";
    str += CG_DEBUG_DEFINES;
	}

  InsertString(makefile, str, 0, CG_WITH_EOL);

  InsertString(makefile, "INCLUDES = -I$(QUEST_ADDITIONAL_INC_DIR) -I$(INCDIR)",
               0, CG_NO_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(makefile, " -I$(X11_INCDIR)",
                 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(makefile, "",
                 0, CG_WITH_EOL);
  }
  InsertString(makefile, "CFLAGS = $(QUEST_CC_FLAGS)", 0, CG_WITH_EOL);
  InsertString(makefile, "CDEBUG = $(QUEST_CC_DEBUG)", 0, CG_WITH_EOL);
  InsertString(makefile, "LFLAGS = $(QUEST_LD_FLAGS)", 0, CG_WITH_EOL);
  str = "LIBS = -L$(LIBDIR)";
  
  if (writer->CGSupportPEV())
  {
    str += " -L$(X11_LIBDIR) ";
    str += CG_STANDARD_PEV_LIBS;
  }
  str += " ";
  str += CG_STANDARD_SCL_LIB;
  if (system->GetAtomList())
  {
    str += " ";
    str += CG_STANDARD_TL_LIB;
  }
  str += " ";
  str += CG_STANDARD_THREAD_LIB;
  str += " ";
  str += CG_STANDARD_LIBS;
  if (writer->CGSupportPEV())
  {
    str += " ";
    str += CG_STANDARD_X11_LIB;
  }
  str += " ";
  str += "$(QUEST_SOCKET_LIBS)";
  if (writer->CGDebug())
  {
    str += " -l";
    str += CG_DEBUG_LIB;
  }
  InsertString(makefile, str, 0, CG_WITH_EOL);
  InsertLine(makefile);

  /******************************/
  /* Quelldateien des Projekts: */
  /******************************/
  InsertString(makefile, "#####################################\n"
                         "# 5. Source files of the simulator: #\n"
                         "#####################################\n",
               0, CG_WITH_EOL);
  str.Format("SOURCES = %s%s", FILE_MAIN, SUFFIX_C);
  InsertString(makefile, str, 0, CG_NO_EOL);
  InsertString(makefile, "\\", 0, CG_WITH_EOL);
  filename = system->GetIdentifier();
  str.Format("          %s%s", filename.GetString(), SUFFIX_C);
  InsertString(makefile, str, 0, CG_NO_EOL);
  
  result = CGWriteBlockFiles(writer, system->GetBlockList(), SUFFIX_C);
  
  if (writer->CGCreateTemplatefile())
  {
    InsertString(makefile, "\\", 0, CG_WITH_EOL);
    str.Format("          %s%s", FILE_TEMPLATES, SUFFIX_C);
    InsertString(makefile, str, 0, CG_NO_EOL);
  }
  if (writer->CGDebug())
  {
    InsertString(makefile, "\\", 0, CG_WITH_EOL);
    str.Format("          %s%s", CG_DEBUG_FILE, SUFFIX_C);
    InsertString(makefile, str, 0, CG_WITH_EOL);
  }
  else
  {
    InsertLine(makefile);
  }
  InsertLine(makefile);

  InsertString(makefile, "HEADERS = ", 0, CG_NO_EOL);
  filename = system->GetIdentifier();
  str.Format("%s%s", filename.GetString(), SUFFIX_H);
  InsertString(makefile, str, 0, CG_NO_EOL);
  
  result = CGWriteBlockFiles(writer, system->GetBlockList(), SUFFIX_H);
  InsertLine(makefile);
  InsertLine(makefile);

  /*******************************/
  /* Objektdateien des Projekts: */
  /*******************************/

  InsertString(makefile, "#####################################\n"
                         "# 6. Object files of the simulator: #\n"
                         "#####################################\n",
               0, CG_WITH_EOL);
  str.Format("OBJS = $(addprefix $(OBJDIR)/, $(SOURCES:%s=%s))", SUFFIX_C, SUFFIX_O);
  InsertString(makefile, str, 0, CG_WITH_EOL);
  InsertLine(makefile);

  /*******************/
  /* Makefileregeln: */
  /*******************/
  InsertString(makefile, "######################\n"
                         "# 7. Makefileregeln: #\n"
                         "######################\n",
               0, CG_WITH_EOL);

  InsertString(makefile, "default: clean-rubbish $(OBJDIR) $(OUTPUT)", 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "$(OUTPUT): $(OBJS)", 0, CG_NO_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(makefile, " $(LIBDIR)", 0, CG_NO_EOL);
    InsertString(makefile, CG_PATH_SEPARATOR_STRING, 0, CG_NO_EOL);
    InsertString(makefile, CG_STANDARD_PEV_LIBS_LONG, 0, CG_NO_EOL);
  }
  InsertString(makefile, " $(LIBDIR)", 0, CG_NO_EOL);
  InsertString(makefile, CG_PATH_SEPARATOR_STRING, 0, CG_NO_EOL);
  InsertString(makefile, CG_STANDARD_SCL_LIB_LONG, 0, CG_NO_EOL);
  if (system->GetAtomList())
  {
    InsertString(makefile, " $(LIBDIR)", 0, CG_NO_EOL);
    InsertString(makefile, CG_PATH_SEPARATOR_STRING, 0, CG_NO_EOL);
    InsertString(makefile, CG_STANDARD_TL_LIB_LONG, 0, CG_WITH_EOL);
  }
  else
  {
    InsertLine(makefile);
  }
  InsertString(makefile, "\t@echo Linking $(OUTPUT)...",  0, CG_WITH_EOL);
  InsertString(makefile, "\t$(C++) $(LFLAGS) $(OUTPUT) $(OBJS) $(LIBS)",
               0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "all: $(OUTPUT)", 0, CG_WITH_EOL);
  InsertLine(makefile);
  str.Format("$(OBJDIR)/%%%s: %%%s $(DEPFILE)", SUFFIX_O, SUFFIX_C);
  InsertString(makefile, str, 0, CG_WITH_EOL);
  InsertString(makefile, "\t@echo Compiling $<...", 0, CG_WITH_EOL);
  InsertString(makefile, "\t$(C++) $(CDEBUG) $(CFLAGS) $(DEFINES) $(INCLUDES)"
                         " $< -o $@ 2>> $(LOGFILE)", 0, CG_WITH_EOL);
  InsertLine(makefile);
  
  // Rule for making *.cpp from *.cpp.new:
  str.Format("%%%s: %%%s", SUFFIX_C, SUFFIX_C_INC);
  InsertString(makefile, str, 0, CG_WITH_EOL);
  InsertString(makefile, "\t@if [ -f $< ]; then  \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\tif [ \\( -f $@ \\) ] && $(DIFF) $< $@; then \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\techo Removing unchanged $<...; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\t$(RM) $<; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\telse\\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\techo Moving changed or new $< to $@...; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\t$(MV) $< $@; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\tfi \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\tfi", 0, CG_WITH_EOL);
  InsertLine(makefile);

  // Rule for making *.h from *.h.new:
  str.Format("%%%s: %%%s", SUFFIX_H, SUFFIX_H_INC);
  InsertString(makefile, str, 0, CG_WITH_EOL);
  InsertString(makefile, "\t@if [ -f $< ]; then  \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\tif [ \\( -f $@ \\) ] && $(DIFF) $< $@; then \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\techo Removing unchanged $<...; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\t$(RM) $<; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\telse\\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\techo Moving changed or new $< to $@...; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t\t$(MV) $< $@; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\tfi \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\tfi", 0, CG_WITH_EOL);
  InsertLine(makefile);

  // Empty rule for catching nonexistent *.new files:
  str.Format("%%%s:", SUFFIX_INC);
  InsertString(makefile, str,0, CG_WITH_EOL);
  InsertLine(makefile);
  
  InsertString(makefile, "$(OBJBASEDIR):", 0, CG_WITH_EOL);
  InsertString(makefile, "\t@if [ ! \\( -d $(OBJBASEDIR) \\) ]; then \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\techo Creating $(OBJBASEDIR) ...; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t$(MKDIR) $(OBJBASEDIR); fi", 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "$(OBJDIR): $(OBJBASEDIR)", 0, CG_WITH_EOL);
  InsertString(makefile, "\t@if [ ! \\( -d $(OBJDIR) \\) ]; then \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\techo Creating $(OBJDIR) ...; \\", 0, CG_WITH_EOL);
  InsertString(makefile, "\t\t$(MKDIR) $(OBJDIR); fi", 0, CG_WITH_EOL);
  InsertLine(makefile);

  InsertString(makefile, "clean-log:", 0, CG_WITH_EOL);
  str.Format("\t-$(RM) *.log *%s *%s", SUFFIX_MSC_TRACE, SUFFIX_PEV_REPORT);
  InsertString(makefile, str, 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "clean-rubbish:", 0, CG_WITH_EOL);
  InsertString(makefile, "\t-$(RM) core *.bak *.log *~ $(LOGFILE)", 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "clean-objects:", 0, CG_WITH_EOL);
  InsertString(makefile, "\t-$(RM) $(OBJDIR)/*.o $(OUTPUT) *.o", 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "clean: clean-rubbish clean-objects", 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "veryclean: clean clean-log", 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "find-error fe:", 0, CG_WITH_EOL);
  InsertString(makefile, "\t$(EDITOR) -p $(LOGFILE)", 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "tags: $(SOURCES)", 0, CG_WITH_EOL);
  InsertString(makefile, "\t-$(TAGS) $(SOURCES)", 0, CG_WITH_EOL);
  InsertLine(makefile);
  InsertString(makefile, "$(DEPFILE) depend: $(SOURCES) $(HEADERS)", 0, CG_WITH_EOL);
  InsertString(makefile, "\t@echo Building dependency file $(DEPFILE) ...",  0, CG_WITH_EOL);
  str.Format("\t$(MAKE_DEPEND) $(DEFINES) $(SOURCES) $(INCLUDES) > $(DEPFILE)");
  InsertString(makefile, str, 0, CG_WITH_EOL);
  InsertString(makefile, "\t$(SED) 's/^\\(.*\\)\\.o/\\$$(OBJDIR)\\/\\1\\.o/g' $(DEPFILE) > $(DEPFILE).sed",
               0, CG_WITH_EOL);
  InsertString(makefile, "\t$(MV) $(DEPFILE).sed $(DEPFILE)", 0, CG_WITH_EOL);
  InsertLine(makefile);
  
  
  
  // The minus before include suppresses "file not found" if dependency file not yet existent.
  InsertString(makefile, "-include $(DEPFILE)", 0, CG_WITH_EOL);

  return DS_OK;
}


/****************************************************************************
 * CGWriteBlockFiles(): erzeugt und fuellt <block>.h, <block.cc>
 *                      Ergebnis: CG_OK, falls alles geklappt hat,sonst enthaelt
 *                      CG_RESULT den aufgetretenen Fehler
 *                      Seiteneffekte: Anlegen und Fuellen der Blockfiles
 ****************************************************************************/

DSResult CGMake::CGWriteBlockFiles(CGWriter *writer, DSBlockKeyList *block_list,
                                   const char * suffix)
{
  DSResult result;
  DSResult status;
  DSBlock *block;
  DSBlockSubstructure *substructure;
  DSBlock *save_pointer_blk;
  DSString filename;
  DSString str;
  CGFile *out;
//  CGPos pos;

  out = writer->CGGetFilePointer();
  assert(out);
//  pos = writer->CGGetIndentPos();

  if (block_list == NULL)
    return DS_OK;

  /******** Erzeugen der Blockdateien ****************************************/

  for (status = block_list->MoveFirst();
       status == DS_OK;
       status = block_list->MoveNext())
  {
    block = block_list->GetCurrentData();
    assert(block);

    save_pointer_blk = block;

    InsertString(*out, "\\", 0, CG_WITH_EOL);
    filename = block->GetIdentifier();
    str.Format("          %s%s", filename.GetString(), suffix);
    InsertString(*out, str, 0, CG_NO_EOL);

    substructure = block->GetBlockSubstructure();
    
    if (substructure)
    {
      InsertString(*out, "\\", 0, CG_WITH_EOL);
      filename = substructure->GetIdentifier();
      str.Format("          %s%s", filename.GetString(), suffix);
      InsertString(*out, str, 0, CG_NO_EOL);
    }

    result = CGWriteProcessFiles(writer, block->GetProcessList(), suffix);
    if (result != DS_OK) return result;

    result = CGWriteProcedureFiles(writer, block->GetProcedureList(), suffix);
    if (result != DS_OK) return result;

    result = CGWriteMachineFiles(writer, block->GetMachineList(), suffix);
    if (result != DS_OK) return result;

    // Rekursiv weitersuchen...
    if (block->GetBlockSubstructure())
    {
      CGWriteBlockFiles(writer, block->GetBlockSubstructure()->GetBlockList(), suffix);
    }

    assert(block_list->GotoElement(save_pointer_blk) == DS_OK);
                                              /* Blockzeiger wieder auf    */
                                              /* alten Wert setzen         */

  }

  return DS_OK;
}


/****************************************************************************
 * CGWriteProcessFiles(): erzeugt und fuellt <process>.h, <process.cc>
 *                        Ergebnis: CG_OK, falls alles geklappt hat,sonst enthaelt
 *                        CG_RESULT den aufgetretenen Fehler
 *                        Seiteneffekte: Anlegen und Fuellen der Prozessfiles
 ****************************************************************************/

DSResult CGMake::CGWriteProcessFiles(CGWriter *writer, DSProcessKeyList *process_list,
                                     const char * suffix)
{
  DSResult result;
  DSResult status;
  DSProcess *process;
  DSProcess *save_pointer_pcs;
  DSString filename;
  DSString str;
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  if (process_list == NULL)
    return DS_OK;

  /******** Erzeugen der Prozessdateien  *************************************/

  for (status = process_list->MoveFirst();
       status == DS_OK;
       status = process_list->MoveNext())
  {
    process = process_list->GetCurrentData(); 
    assert(process);

    save_pointer_pcs = process;

    InsertString(*out, "\\", 0, CG_WITH_EOL);
    filename = process->GetIdentifier();
    str.Format("          %s%s", filename.GetString(), suffix);
    InsertString(*out, str, 0, CG_NO_EOL);

    result = CGWriteProcedureFiles(writer, process->GetProcedureList(), suffix);
    if (result != DS_OK) return result;

    assert(process_list->GotoElement(save_pointer_pcs) == DS_OK);
                                              /* Prozesszeiger wieder auf  */
                                              /* alten Wert setzen         */
  }

  return DS_OK;
}


/****************************************************************************
 * CGWriteProcedureFiles(): erzeugt und fuellt <procedure>.h, <process.cc>
 *                        Ergebnis: CG_OK, falls alles geklappt hat,sonst enthaelt
 *                        CG_RESULT den aufgetretenen Fehler
 *                        Seiteneffekte: Anlegen und Fuellen der Prozessfiles
 ****************************************************************************/

DSResult CGMake::CGWriteProcedureFiles(CGWriter *writer, DSProcedureKeyList *procedure_list,
                                       const char * suffix)
{
  DSResult result;
  DSResult status;
  DSProcedure *procedure;
  DSProcedure *save_pointer_pcd;
  DSString filename;
  DSString str;
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  if (procedure_list == NULL)
    return DS_OK;

  /******** Erzeugen der Prozessdateien  *************************************/

  for (status = procedure_list->MoveFirst();
       status == DS_OK;
       status = procedure_list->MoveNext())
  {
    procedure = procedure_list->GetCurrentData();
    assert(procedure);

    save_pointer_pcd = procedure;

    if (procedure->GetIsComplex())
    {
      InsertString(*out, "\\", 0, CG_WITH_EOL);
      filename = procedure->GetIdentifier();
      str.Format("          %s%s", filename.GetString(),
                 suffix);
      InsertString(*out, str, 0, CG_NO_EOL);
    }

    result = CGWriteProcedureFiles(writer, procedure->GetProcedureList(), suffix);
    if (result != DS_OK) return result;

    assert(procedure_list->GotoElement(save_pointer_pcd) == DS_OK);
                                              /* Prozedurzeiger wieder auf  */
                                              /* alten Wert setzen         */
  }

  return DS_OK;
}

/****************************************************************************
 * CGWriteMachineFiles(): erzeugt und fuellt <machine>.h, <machine.cc>
 *                        Ergebnis: CG_OK, falls alles geklappt hat,sonst enthaelt
 *                        CG_RESULT den aufgetretenen Fehler
 *                        Seiteneffekte: Anlegen und Fuellen der Maschinenfiles
 ****************************************************************************/

DSResult CGMake::CGWriteMachineFiles(CGWriter *writer, DSMachineKeyList *machine_list,
                                     const char * suffix)
{
  DSResult result;
  DSResult status;
  DSMachine *machine;
  DSMachine *save_pointer_mach;
  DSString filename;
  DSString str;
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  if (machine_list == NULL)
    return DS_OK;

  /******** Erzeugen der Prozessdateien  *************************************/

  for (status = machine_list->MoveFirst();
       status == DS_OK;
       status = machine_list->MoveNext())
  {
    machine = machine_list->GetCurrentData();
    assert(machine);

    save_pointer_mach = machine;

    InsertString(*out, "\\", 0, CG_WITH_EOL);
    filename = machine->GetIdentifier();
    str.Format("          %s%s", filename.GetString(), suffix);
    InsertString(*out, str, 0, CG_NO_EOL);

    assert(machine_list->GotoElement(save_pointer_mach) == DS_OK);
                                              /* Maschinenzeiger wieder auf  */
                                              /* alten Wert setzen         */
  }

  return DS_OK;
}
