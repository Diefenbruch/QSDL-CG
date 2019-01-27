/****************************************************************************
 *                 Modul: $RCSfile: CGBlockSubstructure.cpp,v $
 *
 * $Author: md $
 * $Date: 1997/10/30 09:56:14 $
 * $Revision: 1.3 $
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
#include "CGBlockSubstructure.h"
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

#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGBlockSubstructure::CGBlockSubstructure(DSObject *father,
                                         DSString* sys_name) :
  DSBlockSubstructure(father, sys_name)
{
}

CGBlockSubstructure::CGBlockSubstructure(DSObject *father,
                                         DSString& sys_name) :
  DSBlockSubstructure(father, sys_name)
{
}

CGBlockSubstructure::CGBlockSubstructure(DSObject *father,
                                         const char * sys_name) :
  DSBlockSubstructure(father, sys_name)
{
}

/****************************************************************************
 * Destruktoren
 ****************************************************************************/

CGBlockSubstructure::~CGBlockSubstructure(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGBlockSubstructure::New(DSObject *father) const
{
  (void)father;

  return new CGBlockSubstructure;
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer ein BlockSubstructure 
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGBlockSubstructure::Write(DSWriter *writer, DSCardinal what) const
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
  }
  return DS_OK;
}
    

/****************************************************************************
 * WriteInitialize(): statische Initialisierungsfkt. der Substructure schreiben
 *                    -> writer
 *                    Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                    Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/
 
DSResult CGBlockSubstructure::WriteInitialize(CGWriter *writer) const
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
 * WriteFinish(): statische Beendigungsfkt. der Substructure schreiben
 *                    -> writer
 *                    Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                    Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/

DSResult CGBlockSubstructure::WriteFinish(CGWriter *writer) const
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
 * WriteHeader(): Anlegen und Fuellen des sys_*.h-files
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des h-Files
 ****************************************************************************/

DSResult CGBlockSubstructure::WriteHeader(CGWriter *writer) const
{
  DSResult result;
  DSString str;
  DSString filename;
  DSObject *father;

  father = GetParent();
  assert(father);

  filename = GetIdentifier();
  filename += SUFFIX_H;

  writer->CGSetIndentPos(0);

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
  father->Write(writer, CG_WRITE_INCLUDE);
  InsertLine(h_file);
  if (GetSensorList())
  {
    InsertIncludeSCL(h_file, "SCSensorSignal.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCSensorRequest.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCSensorProcess.h", 0, CG_WITH_EOL);
  }
  InsertLine(h_file);
  InsertHead(h_file, "SDL-BlockSubstructure-Klasse", 0);
  InsertString(h_file, "class ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  InsertString(h_file, " : public ", 0, CG_NO_EOL);
  GetParent()->Write(writer, CG_WRITE_IDENTIFIER);
  InsertLine(h_file);
  InsertString(h_file, "{", 0, CG_WITH_EOL); 
  InsertString(h_file, "public:", 2, CG_WITH_EOL); 
  
  InsertHead(h_file, "Initialisierungsfunktion der BlockSubstructure (statisch)", 4);
  InsertString(h_file, "static SCBoolean ", 4, CG_NO_EOL);
  InsertString(h_file, "Initialize(void);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file, "Beendigungsfunktion der BlockSubstructure (statisch)", 4);
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
    InsertHead(h_file, "Benutzerdefinierte SDL-Datentypen auf BlockSubstructure-Ebene",
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
    InsertHead(h_file, "Prototypen fuer die Prozeduren auf BlockSubstructure-Ebene", 4);
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
    InsertHead(h_file, "IDs der Signaltypen auf BlockSubstructure-Ebene", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_SIGNAL, CG_WRITE_ENUM);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    if (HasSignalParams())
    {
      InsertHead(h_file, "Strukturen fuer die Signalparameter auf BlockSubstructure-Ebene", 4);
      writer->CGSetIndentPos(4);
      result = Run(writer, DS_SIGNAL, CG_WRITE_PARAM_STRUCT);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);
    
      InsertHead(h_file, "Variablen fuer die Signalparameter auf BlockSubstructure-Ebene", 4);
      writer->CGSetIndentPos(4);
      result = Run(writer, DS_SIGNAL, CG_WRITE_PARAM);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);
    }

    InsertHead(h_file, "Signaltypen auf BlockSubstructure-Ebene (statisch)", 4);
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
               "IDs der Maschienendienste auf BlockSubstructure-Ebene", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_MACHINESERVICE, 
                 CG_WRITE_ENUM);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    InsertHead(h_file, "Maschinendienste auf BlockSubstructure-Ebene (statisch)", 4);
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
    InsertHead(h_file, "Sensoren der BlockSubstructure", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_SENSOR, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }
  
  // diese Methoden muessen auch dann generiert werden,
  // wenn das BlockSubstructure selbst keine Sensoren enthaelt,
  // da sonst Block-Sensoren nicht gespeichert werden:
  InsertHead(h_file, "Save-, Restore- und Size-Methoden", 4);
  InsertString(h_file, "static SCBoolean StaticSave(SCMem &);", 4, CG_WITH_EOL);
  InsertString(h_file, "static SCBoolean StaticRestore(SCMem &);", 4, CG_WITH_EOL);
  InsertString(h_file, "static void StaticSize(SCSize *);", 4, CG_WITH_EOL);
  InsertLine(h_file);
  
  InsertString(h_file, "};", 0, CG_WITH_EOL);
  InsertLine(h_file);

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
 
DSResult CGBlockSubstructure::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  DSString str;
  DSString filename;

  filename = GetIdentifier();
  filename += SUFFIX_C;

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

  InsertCopyright(c_file);
  InsertHead(c_file, "Includes", 0);
  Write(writer, CG_WRITE_INCLUDE);
  result = Run(writer, DS_BLOCK, CG_WRITE_INCLUDE);
  if (result != DS_OK)
  {
    return DS_ERROR;
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
               "Signaltypen der SDL-Signale auf BlockSubstructureebene (statisch)", 0);
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
    InsertHead(c_file, "Servicetypen der QSDL-Dienste auf BlockSubstructure-Ebene (statisch)", 0);
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
    InsertHead(c_file, "Konstanten (Synonyme) auf BlockSubstructure-Ebene (statisch)", 0);
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
    InsertHead(c_file, "Initialisierung der substructureweiten Sensoren", 0);
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
             "Initialisierungsfunktion der SDL-BlockSubstructure-Klasse (statisch)",
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
             "Beendigungsfunktion der SDL-BlockSubstructure-Klasse (statisch)",
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

  /****** Methoden zum Backtracking der BlockSubstructuresensoren ********/

//  if (GetSensorList()) // sonst werden Block-Sensoren nicht gespeichert!
  {
    InsertHead(c_file, "Save-Methode zur Sicherung der BlockSubstructure-Sensoren", 0);
    result = WriteSaveImplementation(writer);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
    InsertHead(c_file, "Restore-Methode zur Restaurierung der BlockSubstructure-Sensoren", 0);
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
  
  return DS_OK;
}

/****************************************************************************
 * WriteInclude(): Schreiben des Includes fuer das BlockSubstructure 
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/
 
DSResult CGBlockSubstructure::WriteInclude(CGWriter *writer) const
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
 * WriteIdentifier(): Schreiben des BlockSubstructure-Namens (mit Prefix)
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGBlockSubstructure::WriteIdentifier(CGWriter *writer) const
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
 * GetIdentifier(): Liefert den vollen Namen des BlockSubstructures (mit Prefix)
 *                      Ergebnis: Referenz auf DSString-Objekt
 *                      Seiteneffekte: keine
 ****************************************************************************/
 
DSString & CGBlockSubstructure::GetIdentifier(void) const
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
      std::cerr << "Unexpected type of parent object for CGBlockSubstructure !" << std::endl;
      assert(DS_FALSE);
    }
    father = father->GetParent();
  }
  
  *full_name = PREFIX_BLOCKSUBSTRUCTURE + *full_name;

  assert(GetName());    
  *full_name += *GetName();
  
  return *full_name;    
}


/****************************************************************************
 * WriteInitializeImplementation(): Generiert die Initialisierungsfunktion
 *                                  eines Blocks.
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGBlockSubstructure::WriteInitializeImplementation(CGWriter *writer) const
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

  InsertString(*out, "if (SCScheduler::IsVerbose()) std::cout << \"Initializing substructure ",
               2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "...\" << std::endl;", 0, CG_WITH_EOL);
  InsertLine(*out);

  if (GetSortList())
  {
    InsertHead(*out, "Datentpyen auf BlockSubstructure-Ebene initialisieren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SORT, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
    InsertString(*out, "if (!result)", 2, CG_WITH_EOL);
    InsertString(*out, "{", 2, CG_WITH_EOL);
    InsertString(*out, "std::cerr << std::endl << \"BlockSubstructure initialization failed!\" << std::endl;",
                 4, CG_WITH_EOL);
    InsertString(*out, "return false;", 4, CG_WITH_EOL);
    InsertString(*out, "}", 2, CG_WITH_EOL);
    InsertLine(*out);
  }

  if (DSSignalComponent::GetSignalList())
  {
    InsertHead(*out,
               "Signaltypen der SDL-Signale auf BlockSubstructure-Ebene initialisieren:", 2);
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
    InsertHead(*out, "Servicetypen der QSDL-Dienste auf BlockSubstructure-Ebene initialisieren:", 2);
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
    InsertHead(*out, "Sensoren auf BlockSubstructure-Ebene initialisieren:", 2);
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
  InsertString(*out, "std::cerr << std::endl << \"BlockSubstructure initialization failed!\" << std::endl;",
               4, CG_WITH_EOL);
  InsertString(*out, "SCScheduler::Stop();", 4, CG_WITH_EOL);
  InsertString(*out, "}", 2, CG_WITH_EOL);

  InsertLine(*out);
  InsertString(*out, "return result;", 2, CG_WITH_EOL);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  InsertLine(*out);

  return DS_OK;
}


/****************************************************************************
 * WriteFinishImplementation(): Generiert die Beendigungsfunktion
 *                              des BlockSubstructures.
 *                              -> writer
 *                              Ergebnis: DS_OK, falls alles geklappt hat,
 *                                        sonst DS_ERROR
 *                              Seiteneffekte: Anlegen und Fuellen des .cc-Files
 ****************************************************************************/
 
DSResult CGBlockSubstructure::WriteFinishImplementation(CGWriter *writer) const
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
               "Servicetypen der QSDL-Dienste auf BlockSubstructureebene zerstoeren:", 2);
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
               "Sensoren auf BlockSubstructureebene zerstoeren:", 2);
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

  InsertString(*out, "}", 0, CG_WITH_EOL);
  
  return DS_OK;
}

/****************************************************************************
 * WriteSaveImplementation(): Generiert die Save-Methode zum Speichern
 *                            der BlockSubstructure-Sensoren
 *                            -> writer
 *                            Ergebnis: DS_OK, falls alles geklappt hat,
 *                                      sonst DS_ERROR
 *                            Seiteneffekte: Anlegen und Fuellen der .h und
 *                                           .cc-Files
 ****************************************************************************/
 
DSResult CGBlockSubstructure::WriteSaveImplementation(CGWriter *writer) const
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
 
DSResult CGBlockSubstructure::WriteRestoreImplementation(CGWriter *writer) const
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
 
DSResult CGBlockSubstructure::WriteSizeImplementation(CGWriter *writer) const
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

DSResult CGBlockSubstructure::WriteSave(CGWriter *writer) const
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

DSResult CGBlockSubstructure::WriteRestore(CGWriter *writer) const
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

DSResult CGBlockSubstructure::WriteSize(CGWriter *writer) const
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
