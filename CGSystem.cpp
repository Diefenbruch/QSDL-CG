/****************************************************************************
 *                 Modul: $RCSfile: CGSystem.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/02/09 11:09:56 $
 * $Revision: 1.12 $
 *
 * Aufgabe: Dieses Modul enthaelt die Funktionen, mit denen der C++-Code aus
 *          der Datenstruktur generiert wird
 * Funktionen:  
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
#include <DS/DSChannel.h>
#include <DS/DSChannel2RouteConnection.h>

#include "CGFile.h"
#include "CGSystem.h"
#include "CGBlock.h"
#include "CGProcess.h"
#include "CGState.h"
#include "CGInput.h"
#include "CGOutput.h"
#include "CGInputSignal.h"
#include "CGTransition.h"
#include "CGNextState.h"
#include "CGSortExtended.h"
#include "CGOperator.h"
#include "CGGenerator.h"
#include "CGSignal.h" // for CC template instantiation!

#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGSystem::CGSystem(DSString* sys_name) : DSSystem(sys_name)
{
  InitializeBasicSorts();
  InitializeQSDLSorts();
}

CGSystem::CGSystem(DSString& sys_name) : DSSystem(sys_name)
{
  InitializeBasicSorts();
  InitializeQSDLSorts();
}

CGSystem::CGSystem(const char * sys_name) : DSSystem(sys_name)
{
  InitializeBasicSorts();
  InitializeQSDLSorts();
}

/****************************************************************************
 * Destruktoren
 ****************************************************************************/

CGSystem::~CGSystem(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGSystem::New(DSObject *father) const
{
  (void)father;

  return new CGSystem;
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer ein System 
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGSystem::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  switch(what)
  {
    case CG_WRITE_H:
      result = WriteHeader((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;

    case CG_WRITE_CC:
      result = WriteImplementation((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;

    case CG_WRITE_INCLUDE:
      result = WriteInclude((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;

    case CG_WRITE_IDENTIFIER:
      result = WriteIdentifier((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;

    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }
  return DS_OK;
}


/****************************************************************************
 * WriteHeader(): Anlegen und Fuellen des sys_*.h-files
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des h-Files
 ****************************************************************************/
 
DSResult CGSystem::WriteHeader(CGWriter *writer) const
{
  DSResult result;
  DSString str;
  DSString filename;

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
    return DS_WRITE_ERROR;
  }
  writer->CGSetFilePointer(&h_file);

  filename = GetIdentifier();
  filename += SUFFIX_H;

  filename.ToUpper();
  
  InsertCopyright(h_file);
  InsertString(h_file, "#ifndef _", 0, CG_NO_EOL); 
  InsertString(h_file, filename, 0, CG_NO_EOL);
  InsertString(h_file,"_" , 0, CG_WITH_EOL);
  InsertString(h_file, "#define _", 0, CG_NO_EOL); 
  InsertString(h_file, filename, 0, CG_NO_EOL); 
  InsertString(h_file, "_", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file, "Includes", 0);
  InsertIncludeSCL(h_file, "SCSystem.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCDataType.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCSyntype.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCArray.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCString.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCSignalType.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCRequestType.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCCharstring.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCRandConst.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCRandEmpirical.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCRandErlang.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCRandNegExp.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCRandNormal.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCRandUniform.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCRandBernoulli.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCRandPoisson.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCRandUniLong.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCByte.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCMem.h", 0, CG_WITH_EOL);
  if (GetSensorList())
  {
    InsertIncludeSCL(h_file, "SCSensorSignal.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCSensorRequest.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCSensorProcess.h", 0, CG_WITH_EOL);
  }
  InsertLine(h_file);
  // man weiss ja nie was die User so in den
  // informal text schreiben; daher mal
  // die wichtigsten Header inkludieren:
  if (writer->CGGetWriteInformalText())
  {
    InsertString(h_file, "#include <stdio.h>", 0, CG_WITH_EOL);
    InsertString(h_file, "#include <stdlib.h>", 0, CG_WITH_EOL);
    InsertString(h_file, "#include <math.h>", 0, CG_WITH_EOL);
  }
  InsertStringComment(h_file, "#include <string.h>", "fuer memset", 0);
  InsertIncludeSCL(h_file, "SCStream.h", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertString(h_file, "#define MAX_PATH 128", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file, "SDL-Systemklasse", 0);
  InsertString(h_file, "class ", 0, CG_NO_EOL); 
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  InsertString(h_file, " : public SCSystem", 0, CG_WITH_EOL);
  InsertString(h_file, "{", 0, CG_WITH_EOL); 
  InsertString(h_file, "public:", 2, CG_WITH_EOL); 
  
  InsertHead(h_file, "Initialisierungsfunktion des Systems (statisch)", 4);
  InsertString(h_file, "static SCBoolean ", 4, CG_NO_EOL);
  InsertString(h_file, "Initialize(class SCIndet *, class SCSocket *, ", 0, CG_NO_EOL);
  InsertString(h_file, "SCBoolean, SCBoolean", 0, CG_NO_EOL);
  InsertString(h_file,
               ", SCBoolean, SCBoolean", 
               0, CG_NO_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(h_file, ", SCBoolean", 0, CG_NO_EOL);
  }
  InsertString(h_file,
               ", const char *", 
               0, CG_NO_EOL);
  InsertString(h_file,
               ", const char *, const SCNatural", 
               0, CG_NO_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(h_file,
                 ", const char *", 
                 0, CG_NO_EOL);
  }
  InsertString(h_file, ", SCReal);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file, "Beendigungsfunktion des Systems (statisch)", 4);
  InsertString(h_file, "static void ", 4, CG_NO_EOL);
  InsertString(h_file, "Finish(void);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file, "Konstruktor", 4);
  writer->CGSetIndentPos(4);
  WriteIdentifier(writer);
  InsertString(h_file, "(void);", 0, CG_WITH_EOL);
  InsertLine(h_file);
  InsertHead(h_file, "Destruktor", 4);
  InsertString(h_file, "~", 4, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  InsertString(h_file, "(void);", 0, CG_WITH_EOL);
  InsertLine(h_file);
  
  /******** Sorten *******************************************************/
  
  if (GetSortList())
  {
    InsertHead(h_file, "Benutzerdefinierte SDL-Datentypen auf Systemebene",
               4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_SORT, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }

    InsertLine(h_file);
  }

  /******** Prozeduren *******************************************************/
  
  if (GetProcedureList())
  {
    InsertHead(h_file, "Prototypen fuer die Prozeduren auf Systemebene", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_PROCEDURE, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }

  /**************** Signale ********************************/
  
  if (DSSignalComponent::GetSignalList())
  {
    InsertHead(h_file, "IDs der Signaltypen auf Systemebene", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_SIGNAL, CG_WRITE_ENUM);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    if (HasSignalParams())
    {
      InsertHead(h_file, "Strukturen fuer die Signalparameter auf Systemebene", 4);
      writer->CGSetIndentPos(4);
      result = Run(writer, DS_SIGNAL, CG_WRITE_PARAM_STRUCT);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);
    
      InsertHead(h_file, "Variablen fuer die Signalparameter auf Systemebene", 4);
      writer->CGSetIndentPos(4);
      result = Run(writer, DS_SIGNAL, CG_WRITE_PARAM);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);
    }

    InsertHead(h_file, "Signaltypen auf Systemebene (statisch)", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_SIGNAL, CG_WRITE_TYPE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }

  /*************** Services *******************************************/
  
  if (GetMachineServiceList())
  {
    InsertHead(h_file,
               "IDs der Maschienendienste auf Systemebene", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_MACHINESERVICE, 
                 CG_WRITE_ENUM);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    InsertHead(h_file, "Maschinendienste auf Systemebene (statisch)", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_MACHINESERVICE, 
                 CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    
  }

  /******** Sensoren *************************************************/

  if (GetSensorList())
  {
    InsertHead(h_file, "Sensoren des Systems", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_SENSOR, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }
  
  // diese Methoden muessen auch dann generiert werden,
  // wenn das System selbst keine Sensoren enthaelt,
  // da sonst Block-Sensoren nicht gespeichert werden:
  InsertHead(h_file, "Virtuelle Save-, Restore- und Size-Methoden", 4);
  InsertString(h_file, "SCBoolean SystemSave(SCMem &);", 4, CG_WITH_EOL);
  InsertString(h_file, "SCBoolean SystemRestore(SCMem &);", 4, CG_WITH_EOL);
  InsertString(h_file, "void SystemSize(SCSize *);", 4, CG_WITH_EOL);
  InsertLine(h_file);
  
  InsertString(h_file, "};", 0, CG_WITH_EOL);
  InsertLine(h_file);

  /******** Atome *************************************************/

  if (GetAtomList())
  {
    InsertHead(h_file, "Atome des Systems", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_ATOM, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }

  InsertString(h_file, "#endif", 0, CG_WITH_EOL);
  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): Anlegen und Fuellen des sys_*.cc-Files
 *                       -> writer
 *                       Ergebnis: DS_OK, falls alles geklappt hat, sonst 
 *                                 DS_ERROR
 *                      Seiteneffekte: Anlegen und Fuellen des .cc-Files
 ****************************************************************************/
 
DSResult CGSystem::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  DSString str;
  DSString filename;

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
    return DS_WRITE_ERROR;
  }
  writer->CGSetFilePointer(&c_file);

  filename = GetIdentifier();
  filename += SUFFIX_C;

  InsertCopyright(c_file);
  InsertHead(c_file, "Includes", 0);
  Write(writer, CG_WRITE_INCLUDE);
  result = Run(writer, DS_BLOCK, CG_WRITE_INCLUDE);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  if (GetAtomList())  // fuer Atom brauchen wir alle Process-H-Files und
  {                   // Maschinen-H-Files!
    WriteProcessIncludes(writer);
    WriteMachineIncludes(writer);
  }
  InsertLine(c_file);

  if (writer->CGDebug())
  {
    str.Format("#include <%s>", CG_DEBUG_INCLUDE);
    InsertString(c_file, str, 0, CG_WITH_EOL);
    InsertLine(c_file);
  }
      
  InsertStringComment(c_file, "#include <fstream>", "for MSC tracing", 0);
  InsertLine(c_file);

  InsertIncludeSCL(c_file, "SCScheduler.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(c_file, "SCSocket.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(c_file, "SCTraceControl.h", 0, CG_WITH_EOL);

  if (HasTimer(DS_TRUE))
  {
    InsertIncludeSCL(c_file, "SCTimerControl.h", 0, CG_WITH_EOL);
  }
  if (HasDelayedOutputs(DS_TRUE))
  {
    InsertIncludeSCL(c_file, "SCPath.h", 0, CG_WITH_EOL);
  }
  InsertIncludeSCL(c_file, "SCTraceEventMSC.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(c_file, "SCTraceSocket.h", 0, CG_WITH_EOL);
  if (HasSensors(DS_TRUE))
  {
    InsertIncludeSCL(c_file, "SCTraceSensor.h", 0, CG_WITH_EOL);
  }
  if (writer->CGSupportPEV())
  {
    InsertIncludePEV(c_file, "PEEventDispatcher.h", 0, CG_WITH_EOL);
  }
  InsertLine(c_file);

  /****** Schreiben der Signaltypen fuer Tracing-Funktionen ******************/

  if (DSSignalComponent::GetSignalList())
  {
    InsertHead(c_file,
               "Signaltypen der SDL-Signale auf Systemebene (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_SIGNAL, CG_WRITE_TYPE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /*************** jetzt die Requests *******************************************/
  
  if (GetMachineServiceList() != NULL)
  {
    InsertHead(c_file, "Servicetypen der QSDL-Dienste auf Systemebene (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_MACHINESERVICE,
                 CG_WRITE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  } 

  /********** Initialisierung der statischen Konstanten  **********************/
  
  if (GetSortList())
  {
    InsertHead(c_file, "Konstanten (Synonyme) auf Systemebene (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_SORT, CG_WRITE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }
  
  /********** Sensoren initialisieren ***************************************/

  if (GetSensorList())
  {
    InsertHead(c_file, "Initialisierung der systemweiten Sensoren", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_SENSOR, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /********** Initialisierungsfunktion ***************************************/

  InsertHead(c_file,
             "Initialisierungsfunktion der SDL-Systemklasse (statisch)",
             0);
  writer->CGSetIndentPos(0);
  result = WriteInitializeImplementation(writer);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(c_file);
  
  /********** Beendigungsfunktion ***************************************/

  InsertHead(c_file,
             "Beendigungsfunktion der SDL-Systemklasse (statisch)",
             0);
  writer->CGSetIndentPos(0);
  result = WriteFinishImplementation(writer);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(c_file);
  
  /****** Konstruktor **********************************/
  
  InsertHead(c_file, "Konstruktor", 0);  
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  InsertString(c_file, "::", 0, CG_NO_EOL);
  WriteIdentifier(writer);
  InsertString(c_file, "(void)", 0, CG_WITH_EOL);
  InsertString(c_file, "{", 0, CG_WITH_EOL);
  InsertString(c_file, "}", 0, CG_WITH_EOL);
  InsertLine(c_file);

  /****** Destruktor **********************************/
  
  InsertHead(c_file, "Destruktor", 0);  
  WriteIdentifier(writer);
  InsertString(c_file, "::~", 0, CG_NO_EOL);
  WriteIdentifier(writer);
  InsertString(c_file, "(void)", 0, CG_WITH_EOL);
  InsertString(c_file, "{", 0, CG_WITH_EOL);
  InsertString(c_file, "}", 0, CG_WITH_EOL);
  InsertLine(c_file);

  writer->CGSetIndentPos(0);

  /****** Prozeduren **********************************/
  
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

//  if (GetSensorList()) // sonst werden Block-Sensoren nicht gespeichert!
  {
    InsertHead(c_file, "Save-Methode zur Sicherung der System-Sensoren", 0);
    result = WriteSaveImplementation(writer);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
    InsertHead(c_file, "Restore-Methode zur Restaurierung der System-Sensoren", 0);
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
    InsertLine(c_file);
  }
  
  /******** Atome *************************************************/

  if (GetAtomList())
  {
    InsertHead(c_file, "Atomfunktionen des Systems", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_ATOM, CG_WRITE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }


  return DS_OK;
}

/****************************************************************************
 * WriteInclude(): Schreiben des Includes fuer das System 
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/
 
DSResult CGSystem::WriteInclude(CGWriter *writer) const
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

DSResult CGSystem::InitializeBasicSorts(void)
{
  DSSort         *sort;
  DSSortExtended *sort_extended;
  DSGenerator    *sort_generator;
  DSOperator     *op;
  DSSortRefList  *input_sort_ref_list;
  DSSortRef       input_sort;
  DSSortRef       output_sort_ref;

  ////////////////////////
  // Boolean:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_BOOLEAN);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->InsertLiteral(DS_SORT_LITERAL_NAME_FALSE);
  sort_extended->InsertLiteral(DS_SORT_LITERAL_NAME_TRUE);
  sort_extended->SetNeutralElement(DS_SORT_NEUTRAL_BOOLEAN);

  ////////////////////////
  // Real:
  ////////////////////////
  sort = new CGSort(this, DS_SORT_NAME_REAL);
  sort->SetIsPredefined(DS_TRUE);
  sort->SetNeutralElement(DS_SORT_NEUTRAL_REAL);

  ////////////////////////
  // Integer:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_INTEGER);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetNeutralElement(DS_SORT_NEUTRAL_INTEGER);

  // Float-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_FLOAT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Fix-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_REAL);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_FIX,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Natural:
  ////////////////////////

  sort_extended = new CGSortExtended(this, DS_SORT_NAME_NATURAL);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetNeutralElement(DS_SORT_NEUTRAL_NATURAL);

  // Float-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_NATURAL);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_FLOAT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Fix-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_REAL);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_NATURAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_FIX,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Character:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_CHARACTER);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetNeutralElement(DS_SORT_NEUTRAL_CHAR);

  // Num-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARACTER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_NUM,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Chr-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_CHARACTER);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_CHR,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Charstring:
  ////////////////////////
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);
  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARACTER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);
  
  sort_generator = new CGGenerator(this, DS_SORT_NAME_CHARSTRING,
                                   DS_GENERATOR_NAME_STRING,
                                   input_sort_ref_list);
  sort_generator->SetIsPredefined(DS_TRUE);
  sort_generator->SetIsComplex(DS_TRUE);
  sort_generator->InsertLiteral(DS_SORT_LITERAL_NAME_EMPTYSTRING);
  sort_generator->InsertLiteral(DS_SORT_LITERAL_NAME_EMPTYSTRING2);
  sort_generator->SetNeutralElement(DS_SORT_NEUTRAL_CHARSTRING);

  // MkString-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARACTER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  op = new CGOperator(sort_generator,
                      DS_SORT_OPERATOR_NAME_MKSTRING,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Length-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  op = new CGOperator(sort_generator,
                      DS_SORT_OPERATOR_NAME_LENGTH,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // First-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_CHARACTER);
  op = new CGOperator(sort_generator,
                      DS_SORT_OPERATOR_NAME_FIRST,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Last-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_CHARACTER);
  op = new CGOperator(sort_generator,
                      DS_SORT_OPERATOR_NAME_LAST,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // "//"-Funktion: (Stringverkettung)
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  op = new CGOperator(sort_generator,
                      DS_SORT_OPERATOR_NAME_CONCAT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Extract!-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_CHARACTER);
  op = new CGOperator(sort_generator,
                      DS_SORT_OPERATOR_NAME_EXTRACT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Modify!-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARACTER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  op = new CGOperator(sort_generator,
                      DS_SORT_OPERATOR_NAME_MODIFY,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Substring-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  op = new CGOperator(sort_generator,
                      DS_SORT_OPERATOR_NAME_SUBSTRING,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Duration:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_DURATION);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetNeutralElement(DS_SORT_NEUTRAL_DURATION);

  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_REAL);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_DURATION);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_DURATION,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Time:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_TIME);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetNeutralElement(DS_SORT_NEUTRAL_TIME);

  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_DURATION);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_TIME);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_TIME,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // PID:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_PID);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->InsertLiteral(DS_SORT_LITERAL_NAME_NULL);
  sort_extended->SetNeutralElement(DS_SORT_NEUTRAL_PID);

  ////////////////////////
  // Byte:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_BYTE);
  sort_extended->SetIsPredefined(DS_TRUE);

  // BAND-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_BAND,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // BOR-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_BOR,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // BXOR-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_BXOR,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // BNOT-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_BNOT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // BSHL-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_BSHL,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // BSHR-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_BSHR,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // BPLUS-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_BPLUS,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // BSUB-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_BSUB,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // BMUL-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_BMUL,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // BDIV-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_BDIV,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // BMOD-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_BMOD,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // I2B-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_I2B,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // B2I-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_B2I,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // B2C-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_CHARACTER);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_B2C,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // C2B-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARACTER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BYTE);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_C2B,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  return DS_OK;
}

DSResult CGSystem::InitializeQSDLSorts(void)
{
  DSSortExtended *sort_extended;
  DSOperator     *op;
  DSSortRefList  *input_sort_ref_list;
  DSSortRef       input_sort;
  DSSortRef       output_sort_ref;

  sort_extended = new CGSortExtended(this, DS_SORT_NAME_DISCIPLINE);
  sort_extended->InsertLiteral(DS_SORT_LITERAL_NAME_PS);
  sort_extended->InsertLiteral(DS_SORT_LITERAL_NAME_IS);
  sort_extended->InsertLiteral(DS_SORT_LITERAL_NAME_FCFS);
  sort_extended->InsertLiteral(DS_SORT_LITERAL_NAME_LCFSPP);
  sort_extended->InsertLiteral(DS_SORT_LITERAL_NAME_RANDOM);
  sort_extended->InsertLiteral(DS_SORT_LITERAL_NAME_FCFSPP);
  sort_extended->InsertLiteral(DS_SORT_LITERAL_NAME_FCFSPNP);

  ////////////////////////
  // Deterministische Verteilung:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_DETERMINISTIC);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetIsComplex(DS_TRUE);
  // Init-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_DETERMINISTIC); // Vert.-Var.
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_REAL);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_DETERMINISTIC);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_INIT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Sample-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_DETERMINISTIC);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SAMPLE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Empirische-Verteilung:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_EMPIRICAL);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetIsComplex(DS_TRUE);
  // Init-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_EMPIRICAL); // Vert.-Var.
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_CHARSTRING);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_EMPIRICAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_INIT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Sample-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_EMPIRICAL);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SAMPLE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Erlang-Verteilung:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_ERLANG);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetIsComplex(DS_TRUE);
  // Init-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_ERLANG); // Vert.-Var.
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_REAL);  // Mittelwert
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_REAL);  // Standardabweichung
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_ERLANG);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_INIT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Sample-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_ERLANG);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SAMPLE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////////////////
  // Negativ-Exponentielle-Verteilung:
  ////////////////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_NEGEXP);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetIsComplex(DS_TRUE);
  // Init-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_NEGEXP); // Vert.-Var.
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_REAL);    // Mittelwert
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_NEGEXP);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_INIT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Sample-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_NEGEXP);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SAMPLE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Normal-Verteilung:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_NORMAL);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetIsComplex(DS_TRUE);
  // Init-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_NORMAL); // Vert.-Var.
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_REAL);  // Mittelwert
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_REAL);  // Standardabweichung
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_NORMAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_INIT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Sample-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_NORMAL);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SAMPLE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Gleich-Verteilung:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_UNIFORM);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetIsComplex(DS_TRUE);
  // Init-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_UNIFORM); // Vert.-Var.
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_REAL);  // untere Grenze
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_REAL);  // obere Grenze
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_UNIFORM);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_INIT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Sample-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_UNIFORM);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SAMPLE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Bernoulli-Verteilung:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_BERNOULLI);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetIsComplex(DS_TRUE);
  // Init-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BERNOULLI); // Vert.-Var.
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_REAL); // Wahrscheinlichkeit f. TRUE
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BERNOULLI);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_INIT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Sample-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_BERNOULLI);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_BOOLEAN);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SAMPLE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Poisson-Verteilung:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_POISSON);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetIsComplex(DS_TRUE);
  // Init-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_POISSON); // Vert.-Var.
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);   // Mittelwert
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_POISSON);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_INIT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Sample-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_POISSON);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SAMPLE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  //////////////////////////////////
  // Gleich-Verteilung (ganzzahlig):
  //////////////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_RANDINT);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetIsComplex(DS_TRUE);
  // Init-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_RANDINT); // Vert.-Var.
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);  // untere Grenze
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_INTEGER);  // obere Grenze
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_RANDINT);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_INIT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Sample-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_RANDINT);
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_INTEGER);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SAMPLE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Counter-Sensor:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_COUNTER);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetIsComplex(DS_TRUE);

  // Sum-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_COUNTER); // Vert.-Var.
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_NATURAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SUM,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // SumInt-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_COUNTER); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_NATURAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SUMINT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // SumTime-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_COUNTER); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SUMTIME,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // SumTimeInt-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_COUNTER); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SUMTIMEINT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Frequency-Sensor:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_FREQUENCY);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetIsComplex(DS_TRUE);

  // Sum-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_FREQUENCY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_NATURAL); // sum of samples
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SUM,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // AbsFreq-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_FREQUENCY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_NATURAL); // Value
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_NATURAL); // absolute frequency
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_ABSFREQ,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // RelFreq-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_FREQUENCY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_NATURAL); // Value
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL); // relative frequency
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_RELFREQ,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // LastValue-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_FREQUENCY); // sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_NATURAL); // last updated value
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_LASTVALUE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // LastSample-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_FREQUENCY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_NATURAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_LASTSAMPLE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // CurrentValue-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_FREQUENCY); // sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_NATURAL); // last updated value
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_CURRENTVALUE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  ////////////////////////
  // Tally-Sensor:
  ////////////////////////
  sort_extended = new CGSortExtended(this, DS_SORT_NAME_TALLY);
  sort_extended->SetIsPredefined(DS_TRUE);
  sort_extended->SetIsComplex(DS_TRUE);

  // Num-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_NATURAL); // num of samples
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_NUM,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // NumInt-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_NATURAL); // num of samples
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_NUMINT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Minimum-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL); // minimum of samples
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_MINIMUM,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Maximum-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL); // maximum of samples
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_MAXIMUM,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Sum-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL); // sum of samples
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SUM,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // SumInt-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL); // sum of samples
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SUMINT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // SumSquare-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL); // sum of squares of samples
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_SUMSQUARE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Average-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL); // average of samples
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_AVERAGE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // AverageInt-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL); // average of samples
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_AVERAGEINT,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // Variance-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL); // variance of samples
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_VARIANCE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // StdDev-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL); // standard deviation
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_STDDEV,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // VarCoeff-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL); // standard deviation
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_VARCOEFF,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  // LastSample-Funktion:
  input_sort_ref_list = new DSSortRefList;
  assert(input_sort_ref_list);

  input_sort = this->GetSortRefByName(DS_SORT_NAME_TALLY); // Sensor
  assert(input_sort);
  input_sort_ref_list->Append(input_sort);

  output_sort_ref = this->GetSortRefByName(DS_SORT_NAME_REAL);
  op = new CGOperator(sort_extended,
                      DS_SORT_OPERATOR_NAME_LASTSAMPLE,
                      input_sort_ref_list,
                      output_sort_ref);
  assert(op);

  return DS_OK;
}


/****************************************************************************
 * Close(): Schliessen eines offenen Systems, falls Flag gesetzt
 *          -> verbose
 *          Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *          Seiteneffekte: Fuellen der Files
 ****************************************************************************/
 
DSResult CGSystem::Close(DSStream &dsCout, DSBoolean verbose)
{
  DSChannel2RouteConnection *c2rc;
  DSSignalRoute    *env_signal_route;
  DSChannel        *channel;
  DSSignalItemList *signal_item_list_from_env, *signal_item_list_to_env;
  CGProcess        *env_process;
  CGState          *state = NULL;
  CGInput          *input;
  CGOutput         *output;
  CGTransition     *start_transition, *passive_transition, *active_transition;
  CGNextState      *nextstate;
  DSString         *process_name;
  DSString         *signalroute_name;
  DSSignalRefList  *signal_ref_list;
  DSSignalRef       signal_ref;
  CGInputSignal    *input_signal;
  DSResult          status;

  for (channel = this->GetFirstChannel();
       channel;
       channel = this->GetNextChannel())
  {
    if (channel->GetBlock1Ref() == NULL) // Kanal vom Environment?
    {
      assert(channel->GetBlock2Ref() != NULL);

      // Env-Block anlegen falls dies noch nicht geschehen ist:
      if (env_block == NULL)
      {
        if (verbose)
        {
          dsCout << "  Creating Environment Block '" << DS_NAME_ENV_BLOCK <<
                  "'..." << std::endl;
        }

        env_block = new CGBlock(NULL, DS_NAME_ENV_BLOCK);
        assert(env_block);
        InsertBlock(env_block, DS_FALSE); // vorne anfuegen!
      }

      // Kanal umrouten:
      channel->SetBlock1Ref(env_block);

      // Signallisten merken:
      signal_item_list_from_env = channel->GetSignalList12();
      signal_item_list_to_env = channel->GetSignalList21();
    }
    else if (channel->GetBlock2Ref() == NULL)
    {
      assert(channel->GetBlock1Ref() != NULL);

      // Env-Block anlegen falls dies noch nicht geschehen ist:
      if (env_block == NULL)
      {
        if (verbose)
        {
          dsCout << "  Creating Environment Block '" << DS_NAME_ENV_BLOCK <<
                  "'..." << std::endl;
        }

        env_block = new CGBlock(NULL, DS_NAME_ENV_BLOCK);
        assert(env_block);
        InsertBlock(env_block, DS_FALSE); // vorne anfuegen!
      }

      // Kanal umrouten:
      channel->SetBlock2Ref(env_block);

      // Signallisten merken:
      signal_item_list_from_env = channel->GetSignalList21();
      signal_item_list_to_env = channel->GetSignalList12();
    }
    else
    {
      continue;
    }

    if (signal_item_list_from_env && signal_item_list_to_env)
    {
      if (verbose)
      {
        dsCout << "  Found bidirectional Channel from/to Environment: '"
             << channel->GetName()->GetString() << "'" << std::endl;
      }
    }
    else if (signal_item_list_from_env)
    {
      if (verbose)
      {
        dsCout << "  Found unidirectional Channel from Environment: '"
             << channel->GetName()->GetString() << "'" << std::endl;
      }
    }
    else if (signal_item_list_to_env)
    {
      if (verbose)
      {
        dsCout << "  Found unidirectional Channel to Environment: '"
             << channel->GetName()->GetString() << "'" << std::endl;
      }
    }
    else
    {
      continue;
    }

    // Environment Process anlegen:
    process_name = new DSString;
    process_name->Format("%s_%d",
                         DS_NAME_ENV_PROCESS,
                         env_block->GetProcessList() ?
                         env_block->GetProcessList()->NumberOfElements() + 1 :
                         1);
    if (verbose)
    {
      dsCout << "  Creating Environment Process '" << *process_name << "'..." << std::endl;
    }
    env_process = new CGProcess(env_block, *process_name);
    assert(env_process);

    // aktives + passives Environment (nur dann input * generieren!) ?
    if (signal_item_list_to_env && signal_item_list_from_env)
    {
      state = new CGState(env_process, DS_NAME_ENV_STATE);
      assert(state);

      // Fuer alle Signale die der Prozess bekommen kann Inputs erzeugen:
      signal_ref_list = new DSSignalRefList;
      assert(signal_ref_list);
      signal_item_list_to_env->GetSignals(signal_ref_list);

      for (status = signal_ref_list->MoveFirst();
           status == DS_OK;
           status = signal_ref_list->MoveNext())
      {
        signal_ref = signal_ref_list->GetCurrentData();
        assert(signal_ref);

        input = new CGInput(state);
        assert(input);

        input_signal = new CGInputSignal(input, signal_ref);
        assert(input_signal);

        // Leere Transition erzeugen:
        passive_transition = new CGTransition(input);
        assert(passive_transition);

        nextstate = new CGNextState(passive_transition, (DSString*)NULL, state);
        assert(nextstate);
      }
      delete signal_ref_list;

      input = new CGInput(state, DS_FALSE, DS_FALSE, DS_TRUE);  // 'input none' erzeugen!
      active_transition = new CGTransition(input);
      assert(active_transition);

      nextstate = new CGNextState(active_transition, (DSString*)NULL, state);
      assert(nextstate);
    }
    // rein passives Environment (dann input * generieren!) ?
    else if (signal_item_list_to_env)
    {
      state = new CGState(env_process, DS_NAME_ENV_STATE_PASSIVE);
      assert(state);

      // Fuer alle Signale die der Prozess bekommen kann Inputs erzeugen:
      signal_ref_list = new DSSignalRefList;
      assert(signal_ref_list);
      signal_item_list_to_env->GetSignals(signal_ref_list);
      
      for (status = signal_ref_list->MoveFirst();
           status == DS_OK;
           status = signal_ref_list->MoveNext())
      {
        signal_ref = signal_ref_list->GetCurrentData();
        assert(signal_ref);

        input = new CGInput(state);
        assert(input);
      
        input_signal = new CGInputSignal(input, signal_ref);
        assert(input_signal);

        passive_transition = new CGTransition(input);
        assert(passive_transition);

        nextstate = new CGNextState(passive_transition, (DSString*)NULL, state);
        assert(nextstate);
      }
      delete signal_ref_list;
    }
    // rein aktives Environment (dann input nones generieren!) ?
    else if (signal_item_list_from_env)
    {
      state = new CGState(env_process, DS_NAME_ENV_STATE_ACTIVE);
      assert(state);

      // Fuer alle Signale die der Kanal uebertragen kann Outputs erzeugen:
      signal_ref_list = new DSSignalRefList;
      assert(signal_ref_list);
      signal_item_list_from_env->GetSignals(signal_ref_list);
      
      for (status = signal_ref_list->MoveFirst();
           status == DS_OK;
           status = signal_ref_list->MoveNext())
      {
        signal_ref = signal_ref_list->GetCurrentData();
        assert(signal_ref);

        input = new CGInput(state, DS_FALSE, DS_FALSE, DS_TRUE);  // 'input none' erzeugen!
        active_transition = new CGTransition(input);
        assert(active_transition);

        output = new CGOutput(active_transition, (DSString *)NULL, signal_ref);
        assert(output);

        nextstate = new CGNextState(active_transition, (DSString*)NULL, state);
        assert(nextstate);

      }
      delete signal_ref_list;
    }

    start_transition = new CGTransition(env_process);
    assert(start_transition);
    nextstate = new CGNextState(start_transition, (DSString*)NULL, state);
    assert(nextstate);

    if (verbose)
    {
      dsCout << "  Creating Signalroute to connect Environment Process '"
           << *process_name << "'..." << std::endl;
    }

    signalroute_name = new DSString;
    signalroute_name->Format("%s_%d",
                             DS_NAME_ENV_SIGNAL_ROUTE,
                             env_block->GetProcessList() ?
                             env_block->GetProcessList()->NumberOfElements() + 1 :
                             1);
    env_signal_route = new DSSignalRoute(env_block, *signalroute_name,
                                         NULL, env_process);
    assert(env_signal_route);
    assert(env_signal_route->SetSignalList12(signal_item_list_to_env));
    assert(env_signal_route->SetSignalList21(signal_item_list_from_env));
    c2rc = new DSChannel2RouteConnection(env_block);
    assert(c2rc);
    assert(c2rc->InsertChannelRef(channel) == DS_OK);
    assert(c2rc->InsertSignalRouteRef(env_signal_route) == DS_OK);
  }
  return DS_OK;
}

/****************************************************************************
 * GetIdentifier(): Liefert den vollen Namen des Systems (mit Prefix)
 *                      Ergebnis: Referenz auf DSString-Objekt
 *                      Seiteneffekte: keine
 ****************************************************************************/
 
DSString & CGSystem::GetIdentifier(void) const
{
  DSString * full_name;

  full_name = new DSString;
  assert(full_name);
    
  *full_name += PREFIX_SYSTEM;
  *full_name += *GetName();
  
  return *full_name;    
}


/****************************************************************************
 * WriteIdentifier(): Schreiben des vollen System-Namens (mit Prefix)
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGSystem::WriteIdentifier(CGWriter *writer) const
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
 * WriteInitializeImplementation(): Generiert die Initialisierungsfunktion
 *                                  eines Blocks.
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGSystem::WriteInitializeImplementation(CGWriter *writer) const
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
  InsertString(*out, "::Initialize(SCIndet *run_type, SCSocket *sensor_socket, SCBoolean fairness, SCBoolean verbose", 0, CG_NO_EOL);
  InsertString(*out, ", SCBoolean msc_tracing", 0, CG_NO_EOL);
  InsertString(*out, ", SCBoolean socket_tracing", 0, CG_NO_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(*out, ", SCBoolean pev_tracing", 0, CG_NO_EOL);
  }
  InsertString(*out, ", const char *file_name", 0, CG_NO_EOL);
  InsertString(*out, ", const char *host_name", 0, CG_NO_EOL);
  InsertString(*out, ", const SCNatural port_number", 0, CG_NO_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(*out, ", const char *config_file_name",
                 0, CG_NO_EOL);
  }
  InsertString(*out, ", SCReal display_rate", 0, CG_NO_EOL);
  InsertString(*out, ")", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  InsertString(*out, "SCBoolean result = true;", 2, CG_WITH_EOL);
  InsertString(*out, "SCTrace *tracing;", 2, CG_WITH_EOL);
  InsertLine(*out);
  
  InsertString(*out, "if (verbose) std::cout << \"Initializing system ",
               2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "...\" << std::endl;", 0, CG_WITH_EOL);
  InsertLine(*out);

  if (GetSortList())
  {
    InsertHead(*out, "Datentpyen auf Systemebene initialisieren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SORT, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
    InsertString(*out, "if (!result)", 2, CG_WITH_EOL);
    InsertString(*out, "{", 2, CG_WITH_EOL);
    InsertString(*out, "std::cerr << std::endl << \"System initialization failed!\" << std::endl;",
                 4, CG_WITH_EOL);
    InsertString(*out, "return false;", 4, CG_WITH_EOL);
    InsertString(*out, "}", 2, CG_WITH_EOL);
    InsertLine(*out);
  }

  /***** Tracing initialisieren ******************************************/

  InsertHead(*out, "Tracing initialisieren:", 2);
  InsertString(*out, "SCTraceControl::Initialize();", 2, CG_WITH_EOL);
  InsertLine(*out);
  
  /***** Trace-Objekte anlegen *********************************************/
  
  InsertString(*out, "if (msc_tracing)", 2, CG_WITH_EOL);
  InsertString(*out, "{", 2, CG_WITH_EOL);
  InsertHead(*out, "MSC-Tracingobjekt erzeugen:", 4);
  InsertString(*out, "SCStream *out = new SCStream(", 4, CG_NO_EOL);
  InsertString(*out, "file_name);", 0, CG_WITH_EOL);
  InsertString(*out,
               "tracing = new SCTraceEventMSC(out, scfTraceAll, true, \"",
               4, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "\");", 0, CG_WITH_EOL);
  InsertString(*out, "assert(tracing);", 4, CG_WITH_EOL);
  InsertString(*out, "SCTraceControl::AddTrace(tracing);", 4, CG_WITH_EOL);
  InsertString(*out, "}", 2, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "if (socket_tracing)", 2, CG_WITH_EOL);
  InsertString(*out, "{", 2, CG_WITH_EOL);
  InsertHead(*out, "Socket-Tracingobjekt erzeugen:", 4);
  InsertString(*out,
               "tracing = new SCTraceSocket(scfTraceAll, host_name, port_number);",
               4, CG_WITH_EOL);
  InsertString(*out, "assert(tracing);", 4, CG_WITH_EOL);
  InsertString(*out, "if (!((SCTraceSocket *)tracing)->IsOpen())", 4, CG_WITH_EOL);
  InsertString(*out, "{", 4, CG_WITH_EOL);
  InsertString(*out, "delete tracing;", 6, CG_WITH_EOL);
  InsertString(*out, "}", 4, CG_WITH_EOL);
  InsertString(*out, "else", 4, CG_WITH_EOL);
  InsertString(*out, "{", 4, CG_WITH_EOL);
  InsertString(*out, "SCTraceControl::AddTrace(tracing);", 6, CG_WITH_EOL);
  InsertString(*out, "}", 4, CG_WITH_EOL);
  InsertString(*out, "}", 2, CG_WITH_EOL);
  InsertLine(*out);

  if (writer->CGSupportPEV())
  {
    InsertString(*out, "if (pev_tracing)", 2, CG_WITH_EOL);
    InsertString(*out, "{", 2, CG_WITH_EOL);

    InsertHead(*out, "Performance-Evaluation-Tracingobjekt erzeugen:", 4);
    InsertString(*out,
                 "tracing = new PEEventDispatcher(config_file_name, \"",
                 4, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, "\", display_rate);", 0, CG_WITH_EOL);
    InsertString(*out, "assert(tracing);", 4, CG_WITH_EOL);
    InsertString(*out, "SCTraceControl::AddTrace(tracing);", 4, CG_WITH_EOL);
    InsertString(*out, "}", 2, CG_WITH_EOL);
    InsertLine(*out);
  }

  if (HasSensors(DS_TRUE))
  {
    InsertHead(*out, "Sensor-Tracingobjekt erzeugen:", 2);
    InsertString(*out,
                 "tracing = new SCTraceSensor(scfTraceAll);",
                 2, CG_WITH_EOL);
    InsertString(*out, "assert(tracing);", 2, CG_WITH_EOL);
    InsertString(*out, "SCTraceControl::AddTrace(tracing);", 2, CG_WITH_EOL);
    InsertLine(*out);
  }

  /***** Scheduler initialisieren ******************************************/

  InsertHead(*out, "Scheduler initialisieren:", 2);
  InsertString(*out,
               "if (verbose) std::cout << \"Initializing scheduler...\" << std::endl;",
               2, CG_WITH_EOL);
  InsertString(*out, "result = SCScheduler::Initialize(run_type, ", 2, CG_NO_EOL);
  InsertString(*out, "sensor_socket, fairness, verbose, ", 0, CG_NO_EOL);
  InsertString(*out, "kSCDefaultTableSize, 1.0 / display_rate);", 0, CG_WITH_EOL);
  InsertString(*out, "if (!result)", 2, CG_WITH_EOL);
  InsertString(*out, "{", 2, CG_WITH_EOL);
  InsertString(*out, "std::cerr << std::endl << \"System initialization failed!\" << std::endl;",
               4, CG_WITH_EOL);
  InsertString(*out, "return false;", 4, CG_WITH_EOL);
  InsertString(*out, "}", 2, CG_WITH_EOL);
  InsertLine(*out);


  /****** Timer-Prozess initialisieren (wenn Timer vorhanden) ************/

  if (HasTimer(DS_TRUE))
  {
    InsertHead(*out,
               "Timer-Prozess erzeugen und schedulen:",
               2);
    InsertString(*out, CG_NAME_SCL_SCHEDULER, 2, CG_NO_EOL);
    InsertString(*out, "::Schedule(new SCTimerControl);", 0, CG_WITH_EOL);
    InsertLine(*out);
  }

  /****** Path-Prozess initialisieren (wenn Delayed Outputs vorhanden) ******/

  if (HasDelayedOutputs(DS_TRUE))
  {
    InsertHead(*out,
               "Path-Prozess erzeugen und schedulen:",
               2);
    InsertString(*out, CG_NAME_SCL_SCHEDULER, 2, CG_NO_EOL);
    InsertString(*out, "::Schedule(new SCPath);", 0, CG_WITH_EOL);
    InsertLine(*out);
  }

  if (DSSignalComponent::GetSignalList())
  {
    InsertHead(*out,
               "Signaltypen der SDL-Signale auf Systemebene initialisieren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SIGNAL, CG_WRITE_TYPE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetMachineServiceList())
  {
    InsertHead(*out, "Servicetypen der QSDL-Dienste auf Systemebene initialisieren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_MACHINESERVICE,
                 CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  } 

  if (GetSensorList())
  {
    InsertHead(*out, "Sensoren auf Systemebene initialisieren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SENSOR,
                 CG_WRITE_CREATE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  } 

  /****** Initialisierungsfunktion ************/

  InsertHead(*out,
             "Statische Initialisierungsfunktionen der Bloecke aufrufen:",
             2);
  writer->CGSetIndentPos(2);
  result = Run(writer, DS_BLOCK, CG_WRITE_INIT);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(*out);

  InsertString(*out, "if (!result)", 2, CG_WITH_EOL);
  InsertString(*out, "{", 2, CG_WITH_EOL);
  InsertString(*out, "std::cerr << std::endl << \"System initialization failed!\" << std::endl;",
               4, CG_WITH_EOL);
  InsertString(*out, "SCScheduler::Stop();", 4, CG_WITH_EOL);
  InsertString(*out, "}", 2, CG_WITH_EOL);

  InsertLine(*out);
  InsertHead(*out, "System in SCSystem eintragen:", 2);
  InsertString(*out, "SCSystem::SetSystem(new ", 2, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  InsertString(*out, ");", 0, CG_WITH_EOL);

  InsertLine(*out);
  InsertString(*out, "return result;", 2, CG_WITH_EOL);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  InsertLine(*out);

  return DS_OK;
}


/****************************************************************************
 * WriteFinishImplementation(): Generiert die Beendigungsfunktion
 *                              des Systems.
 *                              -> writer
 *                              Ergebnis: DS_OK, falls alles geklappt hat,
 *                                        sonst DS_ERROR
 *                              Seiteneffekte: Anlegen und Fuellen des .cc-Files
 ****************************************************************************/
 
DSResult CGSystem::WriteFinishImplementation(CGWriter *writer) const
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
               "Servicetypen der QSDL-Dienste auf Systemebene zerstoeren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_MACHINESERVICE, CG_WRITE_FINISH);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetSensorList())
  {
    InsertHead(*out,
               "Sensoren auf Systemebene zerstoeren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SENSOR, CG_WRITE_DESTROY);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetBlockList())
  {
    InsertHead(*out,
               "Statische Beendigungsfunktionen der Bloecke aufrufen:",
               2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_BLOCK, CG_WRITE_FINISH);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  InsertHead(*out, "System in SCSystem austragen:", 2);
  InsertString(*out, "delete SCSystem::GetSystem();", 2, CG_WITH_EOL);

  InsertString(*out, "}", 0, CG_WITH_EOL);
  
  return DS_OK;
}

/****************************************************************************
 * WriteSaveImplementation(): Generiert die virtuelle Save-Methode zum
 *                            Speichern der System-Sensoren
 *                            -> writer
 *                            Ergebnis: DS_OK, falls alles geklappt hat,
 *                                      sonst DS_ERROR
 *                            Seiteneffekte: Anlegen und Fuellen der .h und
 *                                           .cc-Files
 ****************************************************************************/
 
DSResult CGSystem::WriteSaveImplementation(CGWriter *writer) const
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
  InsertString(*out, "SystemSave(SCMem &save_area)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  writer->CGSetIndentPos(pos + 2);
  result = Run(writer, DS_SENSOR, CG_WRITE_SAVE);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }  
  // Savefunktionen der Bloecke aufrufen:
  result = Run(writer, DS_BLOCK, CG_WRITE_SAVE);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(*out);
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);
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
 
DSResult CGSystem::WriteRestoreImplementation(CGWriter *writer) const
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
  InsertString(*out, "SystemRestore(SCMem &save_area)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  writer->CGSetIndentPos(pos + 2);
  result = Run(writer, DS_SENSOR, CG_WRITE_RESTORE);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }  
  // Restaurierungsfunktionen der Bloecke aufrufen:
  result = Run(writer, DS_BLOCK, CG_WRITE_RESTORE);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(*out);
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  
  return DS_OK;
}

/****************************************************************************
 * WriteSizeImplementation(): Generiert Methode zur Berechnung den Speicher-
 *                            platz für die Senoren des Blocks
 *                            -> writer
 *                            Ergebnis: DS_OK, falls alles geklappt hat,
 *                                      sonst DS_ERROR
 *                            Seiteneffekte: Anlegen und Fuellen des .cc-Files
 ****************************************************************************/
 
DSResult CGSystem::WriteSizeImplementation(CGWriter *writer) const
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
  InsertString(*out, "SystemSize(SCSize *cur_size)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  writer->CGSetIndentPos(2);
  result = Run(writer, DS_SENSOR, CG_WRITE_SIZE);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }  
  // Size-Methoden der Bloecke aufrufen:
  result = Run(writer, DS_BLOCK, CG_WRITE_SIZE);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }  
  writer->CGSetIndentPos(2);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  
  return DS_OK;
}


/****************************************************************************
 * WriteProcessIncludes(): Schreiben einer Include-Zeile fuer jeden Prozess
 *                         im System
 *                         -> writer
 *                         Ergebnis: DS_OK, falls alles geklappt hat,
 *                                   sonst DS_ERROR
 *                         Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/
 
DSResult CGSystem::WriteProcessIncludes(CGWriter *writer) const
{
  DSBlock *blk_ptr;
  
  for (blk_ptr = GetFirstBlock();
       blk_ptr;
       blk_ptr = GetNextBlock())
  {
    ((CGBlock *)blk_ptr)->WriteProcessIncludes(writer);
  }
  
  return DS_OK;
}


/****************************************************************************
 * WriteMachineIncludes(): Schreiben einer Include-Zeile fuer jede Maschine
 *                         im System
 *                         -> writer
 *                         Ergebnis: DS_OK, falls alles geklappt hat,
 *                                   sonst DS_ERROR
 *                         Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/
 
DSResult CGSystem::WriteMachineIncludes(CGWriter *writer) const
{
  DSBlock *blk_ptr;
  
  for (blk_ptr = GetFirstBlock();
       blk_ptr;
       blk_ptr = GetNextBlock())
  {
    ((CGBlock *)blk_ptr)->WriteMachineIncludes(writer);
  }
  
  return DS_OK;
}


/****************************************************************************
 * WriteBlockIncludes(): Schreiben einer Include-Zeile fuer jede Maschine
 *                       im System
 *                       -> writer
 *                       Ergebnis: DS_OK, falls alles geklappt hat,
 *                                 sonst DS_ERROR
 *                       Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/
 
DSResult CGSystem::WriteBlockIncludes(CGWriter *writer) const
{
  DSBlock *blk_ptr;
  
  for (blk_ptr = GetFirstBlock();
       blk_ptr;
       blk_ptr = GetNextBlock())
  {
    ((CGBlock *)blk_ptr)->WriteBlockIncludes(writer);
  }
  
  return DS_OK;
}

