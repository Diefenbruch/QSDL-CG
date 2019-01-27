/****************************************************************************
 *                 Modul: $RCSfile: CGBlock.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/02/09 11:09:56 $
 * $Revision: 1.6 $
 *
 * Aufgabe: Dieses Modul enthaelt die Funktionen, mit denen der C++-Code aus
 *          der Datenstruktur generiert wird
 * Funktionen: CGWriteBlockH(): Oberfunktion der Codegenerierung fuer Bloecke
 *             CGWriteBlockCC(): Oberfunktion der Codegenerierung fuer Bloecke 
 *             CGWriteProcessEnum(): Erzeugen eines enum-Typs fuer alle im
 *                                   Block enthaltenen Prozesse 
 *             CGWriteProcessParamStruct(): Erzeugen von entsprechenden
 *                                          Structs fuer Prozesse mit Prozedur-
 *                                          parametern 
 ****************************************************************************/

#ifdef USE_PRAGMA
#pragma interface
#endif

/****************************************************************************
 * Konstanten
 ****************************************************************************/

/****************************************************************************
 * Include-Anweisungen
 ****************************************************************************/

#include <DS/DSString.h>

#include "CGFile.h"
#include "CGBlock.h"
#include "CGSystem.h"

#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGBlock::CGBlock(DSObject* father, DSString* block_name) : DSBlock(father, block_name)
{
}
CGBlock::CGBlock(DSObject* father, DSString& block_name) : DSBlock(father, block_name)
{
}
CGBlock::CGBlock(DSObject* father, const char* block_name) : DSBlock(father, block_name)
{
}

/****************************************************************************
 * Destruktoren
 ****************************************************************************/

CGBlock::~CGBlock(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGBlock::New(DSObject *father) const
{
  return new CGBlock(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder falls fehlerhaft DS_ERROR 
 *          Seiteneffekte: Aufruf der entsprechenden Generierungsfunktionen 
 ****************************************************************************/
 
DSResult CGBlock::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
  switch (what)
  {
    case CG_WRITE_H:
      result = WriteHeader((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_CC:
      result = WriteImplementation((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_IDENTIFIER:
      result = WriteIdentifier((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_INCLUDE:
      result = WriteInclude((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_INIT:
      result = WriteInitialize((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_FINISH:
      result = WriteFinish((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_SAVE:
      result = WriteSave((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_RESTORE:
      result = WriteRestore((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_SIZE:
      result = WriteSize((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;

    default:
      assert(DS_FALSE);
      return DS_ERROR;
      break;
  }
  return DS_OK;     
}

/****************************************************************************
 * WriteInclude(): Schreiben einer Include-Zeile fuer den Block
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/

DSResult CGBlock::WriteInclude(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "#include \"", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, SUFFIX_H, 0, CG_NO_EOL);
  InsertString(*out, "\"", 0, CG_WITH_EOL);

  return DS_OK;
}


/****************************************************************************
 * WriteProcessIncludes(): Schreiben einer Include-Zeile fuer jeden Prozess
 *                         im Block
 *                         -> writer
 *                         Ergebnis: DS_OK, falls alles geklappt hat,
 *                                   sonst DS_ERROR
 *                         Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/

DSResult CGBlock::WriteProcessIncludes(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  
  out = writer->CGGetFilePointer();
  assert(out);

  if (GetProcessList())
  {
    result = Run(writer, DS_PROCESS, CG_WRITE_INCLUDE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }
  if (GetBlockSubstructure())
  {
    DSBlock *blk_ptr;

    for (blk_ptr = GetBlockSubstructure()->GetFirstBlock();
         blk_ptr;
         blk_ptr = GetBlockSubstructure()->GetNextBlock())
    {
      ((CGBlock *)blk_ptr)->WriteProcessIncludes(writer);
    }
  }

  return DS_OK;
}


/****************************************************************************
 * WriteMachineIncludes(): Schreiben einer Include-Zeile fuer den Block
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/

DSResult CGBlock::WriteMachineIncludes(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  if (GetMachineList())
  {
    result = Run(writer, DS_MACHINE, CG_WRITE_INCLUDE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }

  return DS_OK;
}


/****************************************************************************
 * WriteBlockIncludes(): Schreiben einer Include-Zeile fuer jeden Block
 *                       -> writer
 *                       Ergebnis: DS_OK, falls alles geklappt hat,
 *                                 sonst DS_ERROR
 *                       Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/

DSResult CGBlock::WriteBlockIncludes(CGWriter *writer) const
{
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  WriteInclude(writer);

  if (GetBlockSubstructure())
  {
    DSBlock *blk_ptr;

    for (blk_ptr = GetBlockSubstructure()->GetFirstBlock();
         blk_ptr;
         blk_ptr = GetBlockSubstructure()->GetNextBlock())
    {
      ((CGBlock *)blk_ptr)->WriteBlockIncludes(writer);
    }
  }

  return DS_OK;
}

/****************************************************************************
 * WriteInitialize(): statische Initialisierungsfkt. des Blocks schreiben
 *                    -> writer
 *                    Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                    Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/

DSResult CGBlock::WriteInitialize(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "result = result & ", 2, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "::Initialize();", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteFinish(): statische Beendigungsfkt. des Blocks schreiben
 *                    -> writer
 *                    Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                    Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/

DSResult CGBlock::WriteFinish(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  WriteIdentifier(writer);
  InsertString(*out, "::Finish();", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteHeader(): Anlegen und Fuellen des blk_*.h-Files
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/
 
DSResult CGBlock::WriteHeader(CGWriter *writer) const
{
  DSResult result;
  DSString filename;
  DSSystemRef system;
  DSObject *father;

  father = GetParent();
  assert(father);

  system = GetThisSystem();
  assert (system);

  filename = GetIdentifier();
  if (writer->CGIncrementalCodeGeneration())
  {
    filename += SUFFIX_H_INC;
  }
  else
  {
    filename += SUFFIX_H;
  }

  CGFile h_file(writer->CGGetOutputDir(),
                &filename,
                writer->CGPrettyPrinting());

  if (!h_file.CGFileOpen())
  {
    writer->cgCout << "Can't open " << h_file.CGFilePath() 
                   << h_file.CGFileName() << std::endl;
    return DS_ERROR;
  }

  filename = GetIdentifier();
  filename += SUFFIX_H;

  writer->CGSetFilePointer(&h_file);
  writer->CGSetIndentPos(4);

  InsertCopyright(h_file);

  filename.ToUpper(); 

  InsertString(h_file, "#ifndef _", 0, CG_NO_EOL); 
  InsertString(h_file, filename, 0, CG_NO_EOL);
  InsertString(h_file,"_" , 0, CG_WITH_EOL);
  InsertString(h_file, "#define _", 0, CG_NO_EOL); 
  InsertString(h_file, filename, 0, CG_NO_EOL);
  InsertString(h_file, "_", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file, "Includes", 0);
  father->Write(writer, CG_WRITE_INCLUDE);
  InsertLine(h_file);
  InsertIncludeSCL(h_file, "SCProcessType.h", 0, CG_WITH_EOL);
  if (GetMachineList() != NULL)
  {
    InsertIncludeSCL(h_file, "SCMachineFCFS.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCMachineFCFSPP.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCMachineFCFSPNP.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCMachineIS.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCMachinePS.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCMachineLCFSPP.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCMachineRandom.h", 0, CG_WITH_EOL);
  }
  if (GetSensorList())
  {
    InsertIncludeSCL(h_file, "SCSensorSignal.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCSensorRequest.h", 0, CG_WITH_EOL);
    if (GetProcessList())
    {
      InsertIncludeSCL(h_file, "SCSensorProcess.h", 0, CG_WITH_EOL);
    }
    if (GetMachineList())
    {
      InsertIncludeSCL(h_file, "SCSensorMachine.h", 0, CG_WITH_EOL);
    }
  }
  InsertLine(h_file);

  InsertHead(h_file, "SDL-Blockklasse", 0);
  InsertString(h_file, "class ", 0, CG_NO_EOL); 
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  InsertString(h_file, " : protected ", 0, CG_NO_EOL);
  father->Write(writer,  CG_WRITE_IDENTIFIER);
  InsertLine(h_file);
  InsertString(h_file, "{", 0, CG_WITH_EOL); 
  
  InsertString(h_file, "public:", 2, CG_WITH_EOL); 

  InsertHead(h_file, "Initialisierungsfunktion (statisch)", 4);
  InsertString(h_file, "static SCBoolean ", 4, CG_NO_EOL); 
  InsertString(h_file, "Initialize(void);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file, "Beendigungsfunktion (statisch)", 4);
  InsertString(h_file, "static void ", 4, CG_NO_EOL); 
  InsertString(h_file, "Finish(void);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  /******************** Prozesse *****************************************/
  
  if (GetProcessList())
  {
    InsertHead(h_file, "Prozesstypen (statisch)", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_PROCESS, CG_WRITE_TYPE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }

  /******************** Maschinen *****************************************/
  
  if (GetMachineList())
  {
    InsertHead(h_file, "Maschinen (statisch)", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_MACHINE, CG_WRITE_DEFINITION);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    
    InsertLine(h_file);
  }

  InsertHead(h_file, "Konstruktor", 4);
  writer->CGSetIndentPos(4);
  WriteIdentifier(writer);
  InsertString(h_file, "(void);", 0, CG_WITH_EOL); 
  InsertLine(h_file);

  InsertHead(h_file, "Destruktor", 4);
  InsertString(h_file, "~", 4, CG_NO_EOL); 
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(4);
  InsertString(h_file, "(void);", 0, CG_WITH_EOL); 
  InsertLine(h_file);

  /******************** Sorten *****************************************/
  
  if (GetSortList())
  {
    InsertHead(h_file, "Benutzerdefinierte SDL-Datentypen auf Blockebene",
               4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_SORT, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }

  /******** Prozesse *******************************************************/

  if (GetProcessList())
  {
    InsertHead(h_file, "IDs der Prozesstypen", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_PROCESS, CG_WRITE_ENUM);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    if (HasFormalParams())
    {
      InsertHead(h_file, "Strukturen fuer die formalen Parameter der Prozesse", 4);
      result = Run(writer, DS_PROCESS, CG_WRITE_PARAM_STRUCT);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);

      InsertHead(h_file, "Variablen fuer die formalen Parameter der Prozesse", 4);
      result = Run(writer, DS_PROCESS, CG_WRITE_PARAM);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);
    }
  }
  
  /******** Maschinen *******************************************************/

  if (GetMachineList())
  {
    InsertHead(h_file,
               "IDs der Maschinen", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_MACHINE, CG_WRITE_ENUM);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }

  /******** Prozeduren *******************************************************/
  
  if (GetProcedureList())
  {
    InsertHead(h_file, "Prototypen fuer die Prozeduren auf Blockebene", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_PROCEDURE, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }

  /*************** Signale *******************************************/

  if (DSSignalComponent::GetSignalList())
  {
    InsertHead(h_file, "IDs der Signaltypen auf Blockebene", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_SIGNAL, CG_WRITE_ENUM);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    if (HasSignalParams())
    {
      InsertHead(h_file, "Strukturen fuer die Signalparameter auf Blockebene", 4);
      result = Run(writer, DS_SIGNAL, CG_WRITE_PARAM_STRUCT);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);

      InsertHead(h_file, "Variablen fuer die Signalparameter auf Blockebene", 4);
      result = Run(writer, DS_SIGNAL, CG_WRITE_PARAM);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);
    }

    InsertHead(h_file, "Signaltypen auf Blockebene (statisch)", 4);
    result = Run(writer, DS_SIGNAL, CG_WRITE_TYPE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }

  /******** Services *******************************************************/

  if (GetMachineServiceList())
  {
    InsertHead(h_file,
               "IDs der Maschienendienste auf Blockebene", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_MACHINESERVICE, 
                 CG_WRITE_ENUM);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    InsertHead(h_file, "Maschinendienste auf Blockebene (statisch)", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_MACHINESERVICE, 
                 CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }

  if (GetMachineList())
  {
    InsertHead(h_file,
               "Dienstspezifische Geschwindigkeiten der Maschinen (statisch)", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_MACHINE, CG_WRITE_SPEED_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }

  /******** Sensoren *************************************************/

  if (GetSensorList())
  {
    InsertHead(h_file, "Sensoren des Blocks", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_SENSOR, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
    InsertHead(h_file, "Save-, Restore- und Size-Methoden", 4);
    InsertString(h_file, "static SCBoolean StaticSave(SCMem &);", 4, CG_WITH_EOL);
    InsertString(h_file, "static SCBoolean StaticRestore(SCMem &);", 4, CG_WITH_EOL);
    InsertString(h_file, "static void StaticSize(SCSize *);", 4, CG_WITH_EOL);
    InsertLine(h_file);
  }

  InsertString(h_file, "};", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertString(h_file, "#endif", 0, CG_WITH_EOL);
  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): Anlegen und Fuellen der blk_*.cc-Files
 *                        -> writer
 *                        Ergebnis: DS_OK, falls alles geklappt hat, sonst 
 *                                  DS_ERROR
 *                        Seiteneffekte: Anlegen und Fuellen des .cc-Files
 ****************************************************************************/
 
DSResult CGBlock::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  DSString filename;
  DSString  str;

  filename = GetIdentifier();
  if (writer->CGIncrementalCodeGeneration())
  {
    filename += SUFFIX_C_INC;
  }
  else
  {
    filename += SUFFIX_C;
  }

  CGFile c_file(writer->CGGetOutputDir(),
                &filename,
                writer->CGPrettyPrinting());

  if (!c_file.CGFileOpen())                
  {
    writer->cgCout << "Can't open " << c_file.CGFilePath() 
                   << c_file.CGFileName() << std::endl;
    return DS_ERROR;
  }

  filename = GetIdentifier();
  filename += SUFFIX_C;

  writer->CGSetFilePointer(&c_file);
  writer->CGSetIndentPos(0);

  InsertCopyright(c_file);
  InsertHead(c_file, "Includes", 0);
  Write(writer, CG_WRITE_INCLUDE);
  writer->CGSetIndentPos(0);
  result = Run(writer, DS_PROCESS, CG_WRITE_INCLUDE);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  result = Run(writer, DS_MACHINE, CG_WRITE_INCLUDE);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  if (GetBlockSubstructure())
  {
    result = GetBlockSubstructure()->Write(writer, CG_WRITE_INCLUDE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }
  InsertLine(c_file);

  if (writer->CGDebug())
  {
    str.Format("#include <%s>", CG_DEBUG_INCLUDE);
    InsertString(c_file, str, 0, CG_WITH_EOL);
    InsertLine(c_file);
  }
  InsertIncludeSCL(c_file, "SCScheduler.h", 0, CG_WITH_EOL);
  InsertLine(c_file);
  
  /****** Schreiben der Signaltypen fuer Tracing-Funktionen ******************/

  if (DSSignalComponent::GetSignalList())
  {
    InsertHead(c_file,
               "Signaltypen der SDL-Signale auf Blockebene (statisch)",
               0);
    result = Run(writer, DS_SIGNAL, CG_WRITE_TYPE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /********** Initialisierung der statischen Konstanten  **********************/

  if (GetSortList())
  {
    InsertHead(c_file, "Konstanten (Synonyme) auf Blockebene (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_SORT, CG_WRITE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /******** jetzt Anlegen der Requests und Initialisieren der Maschinen ******/
    
  if (GetMachineServiceList())
  {
    InsertHead(c_file,
               "Diensttypen der QSDL-Dienste auf Blockebene (statisch)",
               0);
    writer->CGSetIndentPos(0);

    result = Run(writer, DS_MACHINESERVICE, 
                 CG_WRITE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  if (GetMachineList())
  {
    InsertHead(c_file,
               "Initialisieren der Maschinen auf Blockebene (statisch)",
               0);
    writer->CGSetIndentPos(0);

    result = Run(writer, DS_MACHINE, 
                 CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  if (GetProcessList())
  {
    InsertHead(c_file,
               "Initialisierung der Prozesstypen und der Prozess-IDs (statisch)",
               0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_PROCESS, CG_WRITE_TYPE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);      
  }

  /********** Sensoren initialisieren ***************************************/

  if (GetSensorList())
  {
    InsertHead(c_file, "Initialisierung der blockweiten Sensoren", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_SENSOR, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /********* Initialisierungsfunktion ****************************************/

  InsertHead(c_file,
             "Initialisierungsfunktion der SDL-Blockklasse (statisch)",
             0);
  writer->CGSetIndentPos(0);
  result = WriteInitializeImplementation(writer);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(c_file);
   
  /********* Beendigungsfunktion ****************************************/

  InsertHead(c_file,
             "Beendigungsfunktion der SDL-Blockklasse (statisch)",
             0);
  writer->CGSetIndentPos(0);
  result = WriteFinishImplementation(writer);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(c_file);
   
  /********** Konstruktor ****************************************/

  writer->CGSetIndentPos(4);
  result = WriteConstructor(writer);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }

  /********** Destruktor ****************************************/

  writer->CGSetIndentPos(4);
  result = WriteDestructor(writer);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  
  writer->CGSetIndentPos(0);

  /********** Prozeduren ****************************************/

  if (GetProcedureList())
  {
    InsertHead(c_file, "Lokale Prozeduren", 0);
    result = Run(writer, DS_PROCEDURE, CG_WRITE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }

  /****** Methoden zum Backtracking der Systemsensoren ********/

  if (GetSensorList())
  {
    InsertHead(c_file, "Save-Methode zur Sicherung der Block-Sensoren", 0);
    result = WriteSaveImplementation(writer);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
    InsertHead(c_file, "Restore-Methode zur Restaurierung der Block-Sensoren", 0);
    result = WriteRestoreImplementation(writer);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
    InsertHead(c_file, "Size-Methode liefert benoetigten Speicherplatz fuer die Sensoren", 0);
    result = WriteSizeImplementation(writer);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }

  return DS_OK;
}


/****************************************************************************
 * GetIdentifier(): Liefert den vollen Namen des Systems (mit Prefix)
 *                      Ergebnis: Referenz auf DSString-Objekt
 *                      Seiteneffekte: keine
 ****************************************************************************/
 
DSString & CGBlock::GetIdentifier(void) const
{
  DSString * full_name;
  DSObject * father;
  
  full_name = new DSString;
  assert(full_name);

  father = GetParent();
  
  while (father)
  {
    switch(father->GetType())
    {
      case DS_SYSTEM:
        break;
      case DS_BLOCK:
        *full_name = *((DSBlock *)father)->GetName() + "_" + *full_name;
        break;
      case DS_BLOCKSUBSTRUCTURE:
        *full_name = *((DSBlockSubstructure *)father)->GetName() + "_" + *full_name;
        break;
    default:
      std::cerr << "Unexpected type of parent object for CGBlock !" << std::endl;
      assert(DS_FALSE);
    }
    father = father->GetParent();
  }
  
  *full_name = PREFIX_BLOCK + *full_name;

  assert(GetName());    
  *full_name += *GetName();
  
  return *full_name;    
}


/****************************************************************************
 * WriteIdentifier(): Schreiben des vollen System-Namens (mit Prefix)
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGBlock::WriteIdentifier(CGWriter *writer) const
{  
  DSString full_name;
  DSResult result;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  full_name = GetIdentifier();
  InsertString(*out, full_name, pos, CG_NO_EOL);

  return DS_OK;
}


/****************************************************************************
 * WriteConstructor(): Schreiben des Block-Konstruktors
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGBlock::WriteConstructor(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();
  
  InsertHead(*out, "Konstruktor des Blocks", 0);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  InsertString(*out, "::", 0, CG_NO_EOL);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  InsertLine(*out);
  
  return DS_OK;
}


/****************************************************************************
 * WriteDestructor(): Schreiben des Block-Destruktors
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGBlock::WriteDestructor(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;
  CGPos pos;
  
  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();
  
  InsertHead(*out, "Destruktor des Blocks", 0);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  InsertString(*out, "::~", 0, CG_NO_EOL);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  InsertLine(*out);
  
  return DS_OK;
}


/****************************************************************************
 * WriteInitializeImplementation(): Generiert die Initialisierungsfunktion
 *                                  eines Blocks.
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGBlock::WriteInitializeImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "::Initialize(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  InsertString(*out, "SCBoolean result = true;", 2, CG_WITH_EOL);
  InsertLine(*out);
  
  InsertString(*out, "if (SCScheduler::IsVerbose())", 2, CG_WITH_EOL);
  InsertString(*out, "{", 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Initializing block ", 4, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "...\" << std::endl;", 0, CG_WITH_EOL);
  InsertString(*out, "}", 2, CG_WITH_EOL);
  InsertLine(*out);

  if (GetSortList())
  {
    InsertHead(*out, "Datentypen auf Blockebene initialisieren:" , 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SORT, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (DSSignalComponent::GetSignalList())
  {
    InsertHead(*out,
               "Signaltypen der SDL-Signale auf Blockebene initialisieren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SIGNAL, CG_WRITE_TYPE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetMachineServiceList() != NULL)
  {
    InsertHead(*out, "Servicetypen der QSDL-Dienste auf Blockebene initialisieren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_MACHINESERVICE,
                 CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  } 

  if (GetProcessList())
  {
    InsertHead(*out,
               "Prozesstypen initialisieren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_PROCESS, CG_WRITE_TYPE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetSensorList())
  {
    InsertHead(*out, "Sensoren auf Blockebene initialisieren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SENSOR,
                 CG_WRITE_CREATE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  } 

  /********** Prozesse initialisieren ****************************************/

  if (GetProcessList())
  {
    InsertHead(*out,
               "Statische Initialisierungsfunktionen der Prozesse aufrufen:",
               2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_PROCESS, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetMachineList())
  {
    InsertHead(*out,
               "Initialisieren der Maschinengeschwindigkeiten:",
               2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_MACHINE, CG_WRITE_SPEED_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);

    InsertHead(*out,
               "Erzeugen der Maschinen:",
               2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_MACHINE, CG_WRITE_CREATE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);

    InsertHead(*out,
               "Schedulen der Maschinen:",
               2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_MACHINE, CG_WRITE_SCHEDULE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
  
  if (GetBlockSubstructure())
  {
    InsertHead(*out,
               "Initialisieren der Substructure:",
               2);
    GetBlockSubstructure()->Write(writer, CG_WRITE_INIT);
  }
  
  InsertString(*out, "return result;", 2, CG_WITH_EOL);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  
  return DS_OK;
}


/****************************************************************************
 * WriteFinishImplementation(): Generiert die Beendigungsfunktion
 *                              eines Blocks.
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGBlock::WriteFinishImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "void ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, "Finish(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  
  if (DSSignalComponent::GetSignalList())
  {
    InsertHead(*out,
               "Signaltypen der SDL-Signale auf Blockebene zerstoeren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SIGNAL, CG_WRITE_TYPE_FINISH);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetMachineServiceList())
  {
    InsertHead(*out,
               "Servicetypen der QSDL-Dienste auf Blockebene zerstoeren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_MACHINESERVICE, CG_WRITE_FINISH);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetProcessList())
  {
    InsertHead(*out,
               "Prozesstypen zerstoeren:",
               2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_PROCESS, CG_WRITE_TYPE_FINISH);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetSensorList())
  {
    InsertHead(*out,
               "Sensoren auf Blockebene zerstoeren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SENSOR, CG_WRITE_DESTROY);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetProcessList())
  {
    InsertHead(*out,
               "Statische Beendigungsfunktionen der Prozesse aufrufen:",
               2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_PROCESS, CG_WRITE_FINISH);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }

  if (GetBlockSubstructure())
  {
    InsertHead(*out,
               "Statische Beendigungsfunktion der Substructure aufrufen:",
               2);
    writer->CGSetIndentPos(2);
    result = GetBlockSubstructure()->Write(writer, CG_WRITE_FINISH);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }
  
  InsertString(*out, "}", 0, CG_WITH_EOL);
  
  return DS_OK;
}

/****************************************************************************
 * WriteSaveImplementation(): Generiert die Save-Methode zum Speichern der
 *                            Block-Sensoren
 *                            -> writer
 *                            Ergebnis: DS_OK, falls alles geklappt hat,
                                        sonst DS_ERROR
 *                            Seiteneffekte: Anlegen und Fuellen der .h und
 *                                           .cc-Files
 ****************************************************************************/
 
DSResult CGBlock::WriteSaveImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, "StaticSave(SCMem &save_area)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  writer->CGSetIndentPos(2);
  result = Run(writer, DS_SENSOR, CG_WRITE_SAVE);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(*out);
  InsertString(*out, "return true;", 2, CG_WITH_EOL);
  writer->CGSetIndentPos(0);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  
  return DS_OK;
}

/****************************************************************************
 * WriteRestoreImplementation(): Generiert die Restore-Methode zum Restau-
 *                               rieren der Block-Sensoren
 *                               -> writer
 *                               Ergebnis: DS_OK, falls alles geklappt hat,
 *                                         sonst DS_ERROR
 *                               Seiteneffekte: Anlegen und Fuellen der .h
 *                                              und .cc-Files
 ****************************************************************************/
 
DSResult CGBlock::WriteRestoreImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, "StaticRestore(SCMem &save_area)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  writer->CGSetIndentPos(2);
  result = Run(writer, DS_SENSOR, CG_WRITE_RESTORE);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(*out);
  InsertString(*out, "return true;", 2, CG_WITH_EOL);
  writer->CGSetIndentPos(0);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  
  return DS_OK;
}

/****************************************************************************
 * WriteSizeImplementation(): Generiert Methode zur Berechnung den Speicher-
 *                            platz fuer die Senoren des Blocks
 *                            -> writer
 *                            Ergebnis: DS_OK, falls alles geklappt hat,
 *                                      sonst DS_ERROR
 *                            Seiteneffekte: Anlegen und Fuellen des .cc-Files
 ****************************************************************************/
 
DSResult CGBlock::WriteSizeImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "void ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, "StaticSize(SCSize *cur_size)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  writer->CGSetIndentPos(2);
  result = Run(writer, DS_SENSOR, CG_WRITE_SIZE);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertString(*out, "}", 0, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGBlock::WriteSave(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  if (GetSensorList())
  {
    WriteIdentifier(writer);
    InsertString(*out, "::StaticSave(save_area);", 0, CG_WITH_EOL);
  }

  return DS_OK;
}

DSResult CGBlock::WriteRestore(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  if (GetSensorList())
  {
    WriteIdentifier(writer);
    InsertString(*out, "::StaticRestore(save_area);", 0, CG_WITH_EOL);
  }

  return DS_OK;
}

DSResult CGBlock::WriteSize(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  if (GetSensorList())
  {
    WriteIdentifier(writer);
    InsertString(*out, "::StaticSize(cur_size);", 0, CG_WITH_EOL);
  }

  return DS_OK;
}
