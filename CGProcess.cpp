/****************************************************************************
 *                 Modul: $RCSfile: CGProcess.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/02/09 11:09:56 $
 * $Revision: 1.8 $
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
#include <DS/DSSystem.h>
#include <DS/DSBlock.h>
#include <DS/DSVariable.h>
#include <DS/DSExpression.h>
#include <DS/DSTransition.h>

#include "CGBasicTypes.h"
#include "CGFile.h"
#include "CGSystem.h"
#include "CGProcess.h"
#include "CGSort.h"
#include "CGEventNames.h"
#include "CGProcedure.h"

#include <ctype.h>
#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Globale Variablen
 ****************************************************************************/

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGProcess::CGProcess(DSObject* father, DSString *process_name, DSExpression* exp1,
                     DSExpression* exp2, DSExpression* exp3) : 
                     DSProcess(father, process_name, exp1, exp2, exp3)
{
}
CGProcess::CGProcess(DSObject* father, DSString &process_name, DSExpression* exp1,
                     DSExpression* exp2, DSExpression* exp3) : 
                     DSProcess(father, process_name, exp1, exp2, exp3)
{
}
CGProcess::CGProcess(DSObject* father, const char *process_name, DSExpression* exp1,
                     DSExpression* exp2, DSExpression* exp3) : 
                     DSProcess(father, process_name, exp1, exp2, exp3)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGProcess::~CGProcess(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGProcess::New(DSObject *father) const
{
  return new CGProcess(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung, schreibt alle zu einem
 *                    Prozess gehoerenden Elemente raus
 *             -> writer
 *             -> what
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/

DSResult CGProcess::Write(DSWriter *writer, DSCardinal what) const
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
            
    case CG_WRITE_INIT:
      result = WriteInitialize((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_FINISH:
      result = WriteFinish((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_TYPE_H:
      result = WriteTypeHeader((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;    
      
    case CG_WRITE_TYPE_CC:
      result = WriteTypeImplementation((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_TYPE_INIT:
      result = WriteTypeInit((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_TYPE_FINISH:
      result = WriteTypeFinish((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_INCLUDE:
      result = WriteInclude((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_ENUM:
      result = WriteEnum((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_PARAM_STRUCT:
      result = WriteParamStruct((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_PARAM:
      result = WriteParam((CGWriter *)writer);
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
 * WriteHeader(): Oberfunktion der Generierung des h-Files fuer Prozesse
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen der .h-Files
 ****************************************************************************/

DSResult CGProcess::WriteHeader(CGWriter *writer) const
{
  DSResult result;
  DSString str;
  DSSystemRef system;
  DSBlockRef block;
  DSString filename;

  writer->CGSetLastRequestID(0);
  writer->CGSetLastCallID(0);

  block = GetThisBlock();
  assert(block);

  system = GetThisSystem();
  assert(system);

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
                &filename, writer->CGPrettyPrinting());

  if (!h_file.CGFileOpen())                
  {
    writer->cgCout << "Can't open " << h_file.CGFilePath() 
                   << h_file.CGFileName() << std::endl;
    return DS_ERROR;
  }
  writer->CGSetFilePointer(&h_file);

  filename = GetIdentifier();
  filename += SUFFIX_H;

  InsertCopyright(h_file);

  filename.ToUpper();
  
  InsertString(h_file, "#ifndef _", 0, CG_NO_EOL); 
  InsertString(h_file, filename, 0, CG_NO_EOL);
  InsertString(h_file,"_" , 0, CG_WITH_EOL);
  InsertString(h_file, "#define _", 0, CG_NO_EOL); 
  InsertString(h_file, filename, 0, CG_NO_EOL);
  InsertString(h_file,"_" , 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file, "Includes", 0);
  block->Write(writer, CG_WRITE_INCLUDE);
  InsertLine(h_file); 

  InsertIncludeSCL(h_file, "SCProcess.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCStateType.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCTimerType.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCProcedureType.h", 0, CG_WITH_EOL);
  if (GetSensorList())
  {
    InsertIncludeSCL(h_file, "SCTraceSensor.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCSensorProcess.h", 0, CG_WITH_EOL);
  }
  InsertLine(h_file); 

  /*
   * 1. Dataklasse:
   */

  str.Format("Prozessklasse des SDL-Prozesses '%s' (Data)",
             GetName()->GetString());
  InsertHead(h_file, str, 0);
  InsertString(h_file, "class ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file, "_data", 0, CG_NO_EOL);
  InsertString(h_file, " : ", 0, CG_NO_EOL);
  InsertString(h_file, "public ", 0, CG_NO_EOL);
  block->Write(writer, CG_WRITE_IDENTIFIER);
  InsertLine(h_file);
  InsertString(h_file, "{", 0, CG_WITH_EOL);
   
  InsertString(h_file, "public:", 2, CG_WITH_EOL); 
  InsertHead(h_file,
             "Initialisierungsfunktion des Prozesses (statisch)", 4);
  InsertString(h_file, "static SCBoolean ", 4, CG_NO_EOL);
  InsertString(h_file, "Initialize(void);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file,
             "Beendigungsfunktion des Prozesses (statisch)", 4);
  InsertString(h_file, "static void ", 4, CG_NO_EOL);
  InsertString(h_file, "Finish(void);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file, "Konstruktor", 4);
  writer->CGSetIndentPos(4);
  WriteIdentifier(writer);
  InsertString(h_file, "_data(void);", 0, CG_WITH_EOL); 
  InsertLine(h_file);

  InsertHead(h_file, "Destruktor", 4);
  InsertString(h_file, "~", 4, CG_NO_EOL); 
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(4);
  InsertString(h_file, "_data(void);", 0, CG_WITH_EOL); 
  InsertLine(h_file);

  /******************** Prozeduren ****************************************/
  
  if (GetProcedureList())
  {
    InsertHead(h_file, "Prozedurtypen (statisch)", 4);
    result = Run(writer, DS_PROCEDURE, CG_WRITE_TYPE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }

  /**************** Signale ********************************/
  
  if (DSSignalComponent::GetSignalList())
  {
    InsertHead(h_file, "IDs der Signaltypen auf Prozessebene", 4);
    result = Run(writer, DS_SIGNAL, CG_WRITE_ENUM);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    if (HasSignalParams())
    {
      InsertHead(h_file, "Strukturen fuer die Signalparameter auf Prozessebene", 4);
      writer->CGSetIndentPos(4);
      result = Run(writer, DS_SIGNAL, CG_WRITE_PARAM_STRUCT);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);

      InsertHead(h_file, "Variablen fuer die Signalparameter auf Prozessebene", 4);
      writer->CGSetIndentPos(4);
      result = Run(writer, DS_SIGNAL, CG_WRITE_PARAM);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);
    }

    InsertHead(h_file, "Signaltypen auf Prozessebene (statisch)", 4);
    result = Run(writer, DS_SIGNAL, CG_WRITE_TYPE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }

    InsertLine(h_file);
  }

  /******** Timer *********************************************************/

  if (GetTimerList())
  {
    InsertHead(h_file, "IDs der Timertypen auf Prozessebene", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_TIMER, CG_WRITE_ENUM);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    if (HasTimerParams())
    {
      InsertHead(h_file, "Strukturen fuer die Timerparameter", 4);
      writer->CGSetIndentPos(4);
      result = Run(writer, DS_TIMER, CG_WRITE_PARAM_STRUCT);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);

      InsertHead(h_file, "Variablen fuer die Timerparameter", 4);
      writer->CGSetIndentPos(4);
      result = Run(writer, DS_TIMER, CG_WRITE_PARAM);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);
    }

    InsertHead(h_file, "Timertypen auf Prozessebene (statisch)", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_TIMER, CG_WRITE_TYPE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }


  /********* Sorten ******************************************************/
  
  if (GetSortList())
  {
    InsertHead(h_file, "Benutzerdefinierte SDL-Datentypen auf Prozessebene",
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
    InsertHead(h_file, "IDs der Prozedurtypen", 4);
    result = Run(writer, DS_PROCEDURE, CG_WRITE_ENUM);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    InsertHead(h_file, "Strukturen fuer die formalen Parameter der Prozeduren", 4);
    result = Run(writer, DS_PROCEDURE, CG_WRITE_PARAM_STRUCT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    InsertHead(h_file, "Variablen fuer die formalen Parameter der Prozeduren", 4);
    result = Run(writer, DS_PROCEDURE, CG_WRITE_PARAM);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }

    InsertLine(h_file);
  }

  InsertString(h_file, "};", 0, CG_WITH_EOL);
  InsertLine(h_file);

  /*
   * 2. Instanzklasse:
   */

  str.Format("Prozessklasse des SDL-Prozesses '%s' (Instanz)",
             GetName()->GetString());
  InsertHead(h_file, str, 0);
  InsertString(h_file, "class ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file, " : public SCProcess, ", 0, CG_NO_EOL);
  InsertString(h_file, "public ", 0, CG_NO_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file, "_data", 0, CG_WITH_EOL);
  InsertString(h_file, "{", 0, CG_WITH_EOL);
   
  InsertString(h_file, "public:", 2, CG_WITH_EOL); 
  InsertHead(h_file,
             "Initialisierungsfunktion des Prozesses (statisch)", 4);
  InsertString(h_file, "static SCBoolean ", 4, CG_NO_EOL);
  InsertString(h_file, "Initialize(void);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file,
             "Beendigungsfunktion des Prozesses (statisch)", 4);
  InsertString(h_file, "static void ", 4, CG_NO_EOL);
  InsertString(h_file, "Finish(void);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  writer->CGSetIndentPos(4);

  /********* Konstruktoren der Prozessklasse **********************************/

  InsertHead(h_file, "Konstruktoren", 4);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file, "(const SCProcessID", 0, CG_NO_EOL); // AMH 28.5.97:
                                                            // inserted const
                                                            // before
                                                            // SCProcessID
  InsertString(h_file, ", const SCProcessType *", 0, CG_NO_EOL);
  if (GetFormalParamList())
  {
    InsertString(h_file, ", ", 0, CG_NO_EOL);
    InsertString(h_file, PREFIX_PROCESS_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(h_file, GetName(),
                 0, CG_NO_EOL);
    InsertString(h_file, "* = NULL", 0, 
                 CG_NO_EOL);
  }
  InsertString(h_file, ");", 0, CG_WITH_EOL); 
  writer->CGSetIndentPos(4);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file, "(SCMem &);", 0, CG_WITH_EOL); 
  InsertLine(h_file);

  /********* Destruktor der Prozessklasse **********************************/

  InsertHead(h_file, "Destruktor", 4);
  InsertString(h_file, "~", 4, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file, "(void);", 0, CG_WITH_EOL); 
  InsertLine(h_file);

  /******** Prozeduren *******************************************************/
  
  if (GetProcedureList())
  {
    DSProcedure *procedure;
    
    InsertHead(h_file, "Prototypen fuer die Prozeduren", 4);
    writer->CGSetIndentPos(4);
    
    for (procedure = GetFirstProcedure();
         procedure;
         procedure = GetNextProcedure())
    {
      if (!procedure->GetIsComplex())
      {
        result = procedure->Write(writer, CG_WRITE_H);
        if (result != DS_OK)
        {
          return DS_ERROR;
        }
      }
    }

    InsertLine(h_file);
  }
  
  /********* Save-/Restore-Funktion **********************************/

  InsertHead(h_file, "Virtuelle Save- & Restore-Methoden", 4);
  InsertString(h_file, "SCBoolean Save(SCMem &) const;", 4, CG_WITH_EOL); 
  InsertString(h_file, "SCBoolean Restore(SCMem &);", 4, CG_WITH_EOL); 
  InsertLine(h_file);
  InsertHead(h_file, "Virtuelle Size-Methode", 4);
  InsertString(h_file, "void Size(SCSize *) const;", 4, CG_WITH_EOL); 
  InsertLine(h_file);
  InsertHead(h_file, "Display-Methoden", 4);
  InsertString(h_file, "friend SCStream& operator<<(SCStream &, const ", 4, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file, " &);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  /********* Sensor-Update-Funktion **********************************/

  if (GetSensorList() || block->GetSensorList() || system->GetSensorList())
  {
    InsertHead(h_file, "SensorUpdate-Methode", 4);
    InsertString(h_file,
                 "void SensorUpdate(const SCInteger, const SCTraceSensorRole, const struct SCTraceSensorUpdate &);",
                 4, CG_WITH_EOL); 
    InsertLine(h_file);
  }

  /********* Body-Funktion **********************************/

  InsertHead(h_file, "Implementierungsmethode", 4);
  InsertString(h_file, "void Body(void);", 4, CG_WITH_EOL); 
  InsertLine(h_file);

//  InsertString(h_file, "private:", 2, CG_WITH_EOL); 

  /******** Variablen *************************************************/

  if (GetVariableList())
  {
    InsertHead(h_file, "Lokale Variablen des Prozesses", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_VARIABLE, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }
  
  /******** formale Parameter ********************************************/

  if (GetFormalParamList())
  {
    InsertHead(h_file, "Formale Parameter des Prozesses", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_FORMALPARAM, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }
  
  /******** Sensoren *************************************************/

  if (GetSensorList())
  {
    InsertHead(h_file, "Sensoren des Prozesses", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_SENSOR, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }
  
  /******** Zustaende ******************************************************/

  // folgendes auf keinen Fall mehr aktivieren,
  // da es die Speicherung der Zustaende  nach
  // eindeutigen IDs in der Hashtabelle von
  // SCStateType verhindert!!

//  if (writer->CGSupportPEV())
//  {
//    writer->CGSetLastStateID(0); // Zustaende bei jedem neuen Prozess
//                                 // wieder bei 0 anfangen lassen (gut fuer PEV!)
//  }

  InsertHead(h_file, "IDs der Zustandstypen des Prozesses", 4);
  writer->CGSetIndentPos(4);
  result = Run(writer, DS_STATE, CG_WRITE_ENUM);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  if (!GetStateList())  // nur Startzustand vorhanden ?
  {
    DSCardinal i;
    DSCardinal last_state_id = writer->CGGetLastStateID();

    InsertString(h_file, "enum", 4, CG_WITH_EOL);
    InsertString(h_file, "{", 4, CG_WITH_EOL);
    InsertString(h_file, PREFIX_STATE_TYPE_ID, 6, CG_NO_EOL);
    InsertString(h_file, "start = kSCStateTypeIDBase + ", 0, CG_NO_EOL);
    InsertCardinal(h_file, writer->CGGetLastStateID(), 0, CG_NO_EOL);

    for (i = 0;
         i < NumOfActions(DS_REQUEST, DS_FALSE);
         i++)
    {
      InsertString(h_file, ",", 0, CG_WITH_EOL);
      InsertString(h_file, PREFIX_STATE_TYPE_ID, 6, CG_NO_EOL);
      InsertString(h_file, "request_", 0, CG_NO_EOL);
      InsertCardinal(h_file, i, 0, CG_NO_EOL);
      last_state_id++;
		}
    for (i = 0;
         i < NumOfActions(DS_CALL, DS_FALSE);
         i++)
    {
      InsertString(h_file, ",", 0, CG_WITH_EOL);
      InsertString(h_file, PREFIX_STATE_TYPE_ID, 6, CG_NO_EOL);
      InsertString(h_file, "call_", 0, CG_NO_EOL);
      InsertCardinal(h_file, i, 0, CG_NO_EOL);
      last_state_id++;
		}
    InsertLine(h_file);
    InsertString(h_file, "};", 4, CG_WITH_EOL);
	}

  InsertLine(h_file);
  InsertHead(h_file, "Zustandstypen des Prozesses (statisch)", 4);

  // Startzustand:
  InsertString(h_file, "static SCStateType * ", 4, CG_NO_EOL);
  InsertString(h_file, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(h_file, "start", 0, CG_NO_EOL);
  InsertString(h_file, ";", 0, CG_WITH_EOL);

  if (GetStateList())
  {
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_STATE, CG_WRITE_TYPE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }
  InsertLine(h_file);

  /******** Input-Sets der Transitionen *************************************/

  if (GetStateList())
  {
    InsertHead(h_file, "Input-Sets der Transitionen des Prozesses (statisch)", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_STATE, CG_WRITE_INPUT_SET_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    if (HasSaves(DS_FALSE))
    {
      InsertHead(h_file, "Save-Sets der Zustaende des Prozesses (statisch)", 4);
      writer->CGSetIndentPos(4);
      result = Run(writer, DS_STATE, CG_WRITE_SAVE_SET_H);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);
    }
  }

  /******** Standardvariablen zur Prozesssteuerung **************************/

  InsertHead(h_file, "Standardvariablen zur Prozesssteuerung", 4);
  str.Format("SCInteger %scur_state;", PREFIX_VARIABLE);
  InsertString(h_file, str, 4, CG_WITH_EOL);
  str.Format("SCInteger %stransition_id;", PREFIX_VARIABLE);
  InsertString(h_file, str, 4, CG_WITH_EOL);
  str.Format("SCTransition *%stransition;", PREFIX_VARIABLE);
  InsertString(h_file, str, 4, CG_WITH_EOL);

  str.Format("SCSignalType *%ssignal_read;", PREFIX_VARIABLE);
  InsertString(h_file, str, 4, CG_WITH_EOL);
  InsertLine(h_file);

  /******** Friend-Deklarationen fuer alle komplexen Prozeduren *******/

  if (GetProcedureList())
  {
    InsertHead(h_file, "Friend-Deklarationen fuer Prozeduren", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_PROCEDURE, CG_WRITE_FRIEND);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }

  InsertString(h_file, "};", 0, CG_WITH_EOL);
  InsertLine(h_file);
  InsertString(h_file, "#endif", 0, CG_WITH_EOL);
  
  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): Oberfunktion der der Generierung des cc-Files 
 *                        fuer Prozesse
 *                       -> writer
 *                       Ergebnis: DS_OK, falls alles geklappt hat, sonst 
 *                       DS_ERROR
 *                       Seiteneffekte: Anlegen und Fuellen der .cc-Files
 ****************************************************************************/

DSResult CGProcess::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  DSBlockRef block;
  DSString str;
  DSString filename;
  DSStringKeyList *label_list;
  DSCardinal num_of_requests;
  DSCardinal num_of_calls;
  DSCardinal i;

  block = GetThisBlock();

  filename = GetIdentifier();
  if (writer->CGIncrementalCodeGeneration())
  {
    filename += SUFFIX_C_INC;
  }
  else
  {
    filename += SUFFIX_C;
  }

  CGFile c_file(writer->CGGetOutputDir(), &filename,
                writer->CGPrettyPrinting());

  if (!c_file.CGFileOpen())                
  {
    writer->cgCout << "Can't open " << c_file.CGFilePath() 
                   << c_file.CGFileName() << std::endl;
    return DS_ERROR;
  }
  writer->CGSetFilePointer(&c_file);

  filename = GetIdentifier();
  filename += SUFFIX_C;

  InsertCopyright(c_file);
  InsertHead(c_file, "Includes", 0);
  
  // alle Block-H-Files includieren (fuer Output notwendig)
  ((CGSystem *)writer->CGGetSystem())->WriteBlockIncludes(writer);

  // alle Process-H-Files des Blocks includieren (fuer Create notwendig)
  block->Run(writer, DS_PROCESS, CG_WRITE_INCLUDE);
  InsertLine(c_file);

  // alle Process-H-Files des Blocks includieren (fuer Sensorzugriff notwendig)
  block->Run(writer, DS_MACHINE, CG_WRITE_INCLUDE);
  InsertLine(c_file);

  // alle Procedur-H-Files includieren (fuer komplexe Prozeduren notwendig)
  if (GetProcedureList() && HasComplexProcedures(DS_FALSE))
  {  
    Run(writer, DS_PROCEDURE, CG_WRITE_INCLUDE);
    InsertLine(c_file);
  }
  
  if (writer->CGDebug())
  {
    str.Format("#include <%s>", CG_DEBUG_INCLUDE);
    InsertString(c_file, str, 0, CG_WITH_EOL);
    InsertLine(c_file);
  }
  InsertIncludeSCL(c_file, "SCScheduler.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(c_file, "SCTransition.h", 0, CG_WITH_EOL);
  InsertLine(c_file);

  /******* Schreiben der Prozedurtypen ***************************************/
  
  if (GetProcedureList() && HasComplexProcedures(DS_FALSE))
  {
    InsertHead(c_file,
               "Prozedurtypen der Prozesse (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_PROCEDURE, CG_WRITE_TYPE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /****** Schreiben der Signaltypen  *****************************************/

  if (DSSignalComponent::GetSignalList())
  {
    InsertHead(c_file,
               "Signaltypen der SDL-Signale auf Prozessebene (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_SIGNAL, CG_WRITE_TYPE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /******* Schreiben der Timertypen ******************************************/
  
  if (GetTimerList())
  {
    InsertHead(c_file,
               "Signaltypen der SDL-Timer auf Prozessebene (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_TIMER, CG_WRITE_TYPE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /********** Initialisierung der statischen Konstanten  **********************/
  
  if (GetSortList())
  {
    InsertHead(c_file, "Konstanten (Synonyme) auf Prozessebene (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_SORT, CG_WRITE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /***** Save-Sets der Zustaende ********************************************/

  if (GetStateList() && HasSaves(DS_FALSE))
  {
    InsertHead(c_file, "Save-Sets der Prozesszustaende (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_STATE, CG_WRITE_SAVE_SET_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /** Schreiben der State-Typen  ***************/

  InsertHead(c_file,
             "Typen der SDL-Zustaende auf Prozessebene (statisch)", 0);

  writer->CGSetIndentPos(0);

  // Startzustand:
  InsertString(c_file, "SCStateType * ", 0, CG_NO_EOL);
  Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(c_file, "::", 0, CG_NO_EOL);
  InsertString(c_file, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(c_file, "start", 0, CG_NO_EOL);
  InsertString(c_file, " = NULL;", 0, CG_WITH_EOL);

  if (GetStateList())
  {
    result = Run(writer, DS_STATE, CG_WRITE_TYPE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }
  InsertLine(c_file);

  /***** Input-Sets der Transitionen ********************************************/

  if (GetStateList())
  {
    InsertHead(c_file, "Input-Sets der Prozesstransitionen (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_STATE, CG_WRITE_INPUT_SET_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /***** Initialisierungsfunktion ********************************************/

  InsertHead(c_file, "Initialisierungsfunktion der SDL-Prozessklasse (statisch)", 0);
  writer->CGSetIndentPos(0);
  result = WriteInitializeImplementation(writer);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(c_file);

  /***** Beendigungsfunktion ********************************************/

  InsertHead(c_file, "Beendigungsfunktion der SDL-Prozessklasse (statisch)", 0);
  writer->CGSetIndentPos(0);
  result = WriteFinishImplementation(writer);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(c_file);

  /********* Konstruktoren der Prozessklasse **********************************/
  
  writer->CGSetIndentPos(4);
  result = WriteConstructor(writer);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }

  /********* Destruktor der Prozessklasse **********************************/
  
  writer->CGSetIndentPos(4);
  result = WriteDestructor(writer);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }

  /******** Prozeduren *******************************************************/
  
  if (GetProcedureList())
  {
    DSProcedure *procedure;
    
    InsertHead(c_file, "Lokale Prozeduren", 0);
    writer->CGSetIndentPos(0);

    for (procedure = GetFirstProcedure();
         procedure;
         procedure = GetNextProcedure())
    {
      if (!procedure->GetIsComplex())
      {
        result = procedure->Write(writer, CG_WRITE_CC);
        if (result != DS_OK)
        {
          return DS_ERROR;
        }
      }
    }
    InsertLine(c_file);
  }

  writer->CGSetLastRequestID(0); // auch Prozeduren veraendern diesen Zaehler!
  writer->CGSetLastCallID(0); // auch Prozeduren veraendern diesen Zaehler!

  /*************** Body-Funktion der Prozessklasse **************************/
    
  InsertHead(c_file, "Implementierungsfunktion des SDL-Prozesses", 0);
  InsertString(c_file, "void ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(c_file, "::Body(void)", 0, CG_WITH_EOL); 
  InsertString(c_file, "{", 0, CG_WITH_EOL); 
  
  /************ Hauptschleife ************************************************/

  InsertHead(c_file, "Meldung ausgeben:", 2);
  InsertString(c_file, "if (IsVerbose())", 2, CG_WITH_EOL);
  InsertString(c_file, "{", 2, CG_WITH_EOL);
  InsertString(c_file, "SCScheduler::outputStream << \"Executing \" "
             "<< *this << std::endl;", 4, CG_WITH_EOL);
  InsertString(c_file, "}", 2, CG_WITH_EOL);
  InsertLine(c_file);

  InsertHead(c_file, "Hauptschleife:", 2);
  InsertString(c_file, "nextstate:", 2, CG_WITH_EOL);
  str.Format("%stransition_id = kSCNoTransition;", PREFIX_VARIABLE);
  InsertString(c_file, str, 2, CG_WITH_EOL);
  if (GetStateList() ||
      HasAction(DS_REQUEST, DS_FALSE) ||
      HasAction(DS_CALL, DS_FALSE))
  {
    InsertString(c_file, "backtracking:", 2, CG_WITH_EOL);
  }
  InsertString(c_file, "while(true)", 2, CG_WITH_EOL);
  InsertString(c_file, "{", 2, CG_WITH_EOL);
  
  /*************** Funktionalitaet der einzelnen Transitionen ***************/
  
  if (GetStateList() != NULL || GetStartTransition())
  {
    str.Format("switch(%scur_state)", PREFIX_VARIABLE);
    InsertString(c_file, str, 4, CG_WITH_EOL);
    InsertString(c_file, "{", 4, CG_WITH_EOL);

    if (GetStartTransition())
    {
      InsertHead(c_file, "Starttransition:", 6);
      str.Format("case %sstart:", PREFIX_STATE_TYPE_ID);
      InsertString(c_file, str, 6, CG_WITH_EOL); 
      str.Format("if (%stransition_id == kSCNoTransition)", PREFIX_VARIABLE);
      InsertString(c_file, str, 8, CG_WITH_EOL); 
      InsertString(c_file, "{", 8, CG_WITH_EOL); 
      str.Format("%stransition_id = State(%s%s, kSCNoAwakeDelay, &%ssignal_read);",
                 PREFIX_VARIABLE,
                 PREFIX_STATE_TYPE_VARIABLE, "start", PREFIX_VARIABLE);
      InsertString(c_file, str, 10, CG_WITH_EOL);
      InsertString(c_file, "}", 8, CG_WITH_EOL); 
      writer->CGSetIndentPos(8);
      result = GetStartTransition()->Write(writer);
      if (result != DS_OK) return result;
      InsertLine(c_file);
    }
    
    if (GetStateList() != NULL)
    {
      writer->CGSetIndentPos(6);
      label_list = new DSStringKeyList;
      assert(label_list);
      writer->CGSetLabelList(label_list);

      result = Run(writer, DS_STATE, CG_WRITE_CC);
      if (result != DS_OK) return result;

      writer->CGSetLabelList(NULL); // dadurch wird Liste auch geloescht!
    }

    num_of_requests = writer->CGGetLastRequestID(); // NumOfActions(DS_REQUEST);
    for (i = 0;
         i < num_of_requests;
         i++)
    {
      str.Format("case %srequest_%d:", PREFIX_STATE_TYPE_ID, i);
      InsertString(c_file, str, 6, CG_WITH_EOL); 
      str.Format("goto label_request_%d;", i);
      InsertString(c_file, str, 8, CG_WITH_EOL); 
      InsertString(c_file, "break;", 8, CG_WITH_EOL); 
      InsertLine(c_file);
    }

    num_of_calls = writer->CGGetLastCallID(); //NumOfActions(DS_CALL);
    for (i = 0;
         i < num_of_calls;
         i++)
    {
      str.Format("case %scall_%d:", PREFIX_STATE_TYPE_ID, i);
      InsertString(c_file, str, 6, CG_WITH_EOL); 
      str.Format("goto label_call_%d;", i);
      InsertString(c_file, str, 8, CG_WITH_EOL); 
      InsertString(c_file, "break;", 8, CG_WITH_EOL); 
      InsertLine(c_file);
    }

    InsertString(c_file, "default:", 6, CG_WITH_EOL); 
    str.Format("std::cerr << \"Process '%s': "
               "Invalid process state.\" << std::endl;",
               GetName()->GetString());
    InsertString(c_file, str, 8, CG_WITH_EOL); 
    InsertString(c_file, "break;", 8, CG_WITH_EOL);
    
    InsertString(c_file, "}", 4, CG_WITH_EOL); 
  }
  
  /********* switch-Scopes **************************************************/
  
  InsertString(c_file, "}", 2, CG_WITH_EOL);
  
  /********* Ende des while (true)-Scopes ***************************/

  InsertString(c_file, "}", 0, CG_WITH_EOL);
  
  /********* Ende des Prozess-Scopes ****************************************/

  writer->CGSetIndentPos(0);
  InsertLine(c_file);  
  InsertHead(c_file, "Save-Methode zur Speicherung des Prozesszustands", 0);
  result = WriteSave(writer);
  if (result != DS_OK) return result;

  InsertLine(c_file);  
  InsertHead(c_file, "Restore-Methode zur Restaurierung des Prozesszustands", 0);
  result = WriteRestore(writer);
  if (result != DS_OK) return result;
    
  InsertLine(c_file);  
  InsertHead(c_file, "Size-Methode liefert benoetigten Speicherplatz", 0);
  result = WriteSize(writer);
  if (result != DS_OK) return result;
    
  InsertLine(c_file);  
  InsertHead(c_file, "Display-Methode", 0);
  result = WriteStreamOperator(writer);
  if (result != DS_OK) return result;

  if (GetSensorList() || block->GetSensorList() || GetThisSystem()->GetSensorList())
  {
    InsertLine(c_file);  
    InsertHead(c_file, "SensorUpdate-Methode", 0);
    result = WriteSensorUpdate(writer);
    if (result != DS_OK) return result;
  }

  return DS_OK;  
}

/****************************************************************************
 * WriteEnum(): Schreiben der Prozesse in Aufzaehlungstyp
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteEnum(CGWriter *writer) const
{  
  DSResult result;
  DSBlockRef block;
  DSString last_id(writer->CGGetLastProcessID());
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  block = GetThisBlock();
  assert(block);
  
  if (block->GetProcessList()->AtStartOfList())
  {
    InsertString(*out, "enum", pos, CG_WITH_EOL);
    InsertString(*out, "{", pos, CG_WITH_EOL);
  }

  InsertString(*out, PREFIX_PROCESS_TYPE_ID, pos + 2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);

  if (block->GetProcessList()->AtStartOfList())
  {
    InsertString(*out, " = kSCProcessTypeIDBase + ", 0, CG_NO_EOL);
    InsertString(*out,
                 &last_id,
                 0, CG_NO_EOL);
  }
  writer->CGSetLastProcessID(writer->CGGetLastProcessID() + 1);

  if (!block->GetProcessList()->AtEndOfList())
  {
    InsertString(*out, ",", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "", 0, CG_WITH_EOL);
    InsertString(*out, "};", pos, CG_WITH_EOL);
  }

  return DS_OK;  
}

DSResult CGProcess::WriteParamDefaultConstructor(CGWriter *writer) const
{ 
  DSResult     result;
  DSFormalParam  *formal_param;
  DSSortRef    sort;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  for (formal_param = GetFirstFormalParam();
       formal_param;
       formal_param = GetNextFormalParam())
  {
    sort = formal_param->GetSortRef();
    assert(sort);

    // Gibt es einen in SDL spezifizierten Default-Wert?
    if (sort->GetDefaultInitialization())
    {
      InsertString(*out, PREFIX_VARIABLE, pos + 2, CG_NO_EOL);
      InsertString(*out, formal_param->GetName(), 0, CG_NO_EOL);
      InsertString(*out, " = ", 0, CG_NO_EOL);
      result = sort->GetDefaultInitialization()->Write(writer);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertString(*out, ";", 0, CG_WITH_EOL);
    }
    // Ist ein neutrales Element vorhanden?
    else if (sort->GetNeutralElement())
    {
      InsertString(*out, PREFIX_VARIABLE, pos + 2, CG_NO_EOL);
      InsertString(*out, formal_param->GetName(), 0, CG_NO_EOL);
      InsertString(*out, " = ", 0, CG_NO_EOL);
      InsertString(*out, sort->GetNeutralElement(), 0, CG_NO_EOL);
      InsertString(*out, ";", 0, CG_WITH_EOL);
    }
    // Ist es ein vordefinierter Datentyp?
    else if (!sort->GetIsComplex())
    {
      InsertString(*out, "memset(&", pos + 2, CG_NO_EOL);
      InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
      InsertString(*out, formal_param->GetName(), 0, CG_NO_EOL);
      InsertString(*out, ", 0, sizeof(", 0, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, "));", 0, CG_WITH_EOL);
    }
  }
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcess::WriteParamCopyConstructor(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(const ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "& orig)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "*this = orig;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcess::WriteParamClone(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCDataType *Clone(void) const", pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return new ", pos + 2, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(*this);", 0, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcess::WriteParamEqual(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean Equal(const SCDataType &second) const",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return (*this == *(", pos + 2, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " *)&second);", 0, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcess::WriteParamCompare(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSFormalParam  *formal_param;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean operator== (const ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " &second) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return (", pos + 2, CG_NO_EOL);

  for (formal_param = GetFirstFormalParam();
       formal_param;
       formal_param = GetNextFormalParam())
  {
    InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, formal_param->GetName(), 0, CG_NO_EOL);
    InsertString(*out, " == second.", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, formal_param->GetName(), 0, CG_NO_EOL);
    if (!GetFormalParamList()->AtEndOfList())
    {
      InsertString(*out, " && ", 0, CG_NO_EOL);
    }
  }
  InsertString(*out, ");", 0, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcess::WriteParamAssign(CGWriter *writer) const
{ 
  DSResult        result;
  CGFile         *out;
  CGPos           pos;
  DSFormalParam  *formal_param;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "& operator= (const ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " &orig)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  for (formal_param = GetFirstFormalParam();
       formal_param;
       formal_param = GetNextFormalParam())
  {
    InsertString(*out, PREFIX_VARIABLE, pos + 2, CG_NO_EOL);
    InsertString(*out, formal_param->GetName(), 0, CG_NO_EOL);
    InsertString(*out, " = orig.", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, formal_param->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }
  InsertString(*out, "return *this;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcess::WriteParamDisplay(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCStream& Display(SCStream &out) const", pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  InsertString(*out, "return (out << *this);", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcess::WriteParamStreamOperator(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSFormalParam  *formal_param;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "friend SCStream& operator<<(SCStream &out, const ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " &me)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  if (GetFormalParamList())
  {
    if (GetFormalParamList()->NumberOfElements() > 1)
    {
      InsertString(*out, "out << \"[\";", pos + 2, CG_WITH_EOL);
    }
  }
  for (formal_param = GetFirstFormalParam();
       formal_param;
       formal_param = GetNextFormalParam())
  {
    InsertString(*out, "out << me.", pos + 2, CG_NO_EOL);
    InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, formal_param->GetName(), 0, CG_NO_EOL);
    if (!GetFormalParamList()->AtEndOfList())
    {
      InsertString(*out, " << \", \"", 0, CG_NO_EOL);
    }
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }
  if (GetFormalParamList())
  {
    if (GetFormalParamList()->NumberOfElements() > 1)
    {
      InsertString(*out, "out << \"]\";", pos + 2, CG_WITH_EOL);
    }
  }

  InsertString(*out, "return out;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcess::WriteParamSave(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSFormalParam  *formal_param;
  DSSortRef    sort;
  DSString     str;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean Save(SCMem &save_area) const",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  for (formal_param = GetFirstFormalParam();
       formal_param;
       formal_param = GetNextFormalParam())
  {
    sort = formal_param->GetSortRef();
    assert(sort);

    if (!sort->GetIsComplex())
    {
      str.Format("save_area.Store(&%s%s, sizeof(",
                 PREFIX_VARIABLE,
                 formal_param->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, "));", 0, CG_WITH_EOL);
    }
    else
    {
      str.Format("%s%s.Save(save_area);",
                 PREFIX_VARIABLE,
                 formal_param->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcess::WriteParamRestore(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSFormalParam  *formal_param;
  DSSortRef    sort;
  DSString     str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean Restore(SCMem &save_area)",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  for (formal_param = GetFirstFormalParam();
       formal_param;
       formal_param = GetNextFormalParam())
  {
    sort = formal_param->GetSortRef();
    assert(sort);

    if (!sort->GetIsComplex())
    {
      str.Format("save_area.Restore(&%s%s, sizeof(",
                 PREFIX_VARIABLE,
                 formal_param->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, "));", 0, CG_WITH_EOL);
    }
    else
    {
      str.Format("%s%s.Restore(save_area);",
                 PREFIX_VARIABLE,
                 formal_param->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

 /****************************************************************************
 * CGWriteParamStruct(): Schreiben der Prozessparameter als struct
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteParamStruct(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  if (GetFormalParamList()) 
  {
    InsertString(*out, "struct ", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, " : public SCDataType", 0, CG_WITH_EOL);
    InsertString(*out, "{", pos, CG_WITH_EOL);

    writer->CGSetIndentPos(pos + 2);
    // Default-Konstruktor:
    InsertString(*out, "// Default Konstruktor:", pos + 2, CG_WITH_EOL);
    result = WriteParamDefaultConstructor(writer);
    if (result != DS_OK) return result;

    // Copy-Konstruktor:
    InsertString(*out, "// Copy Konstruktor:", pos + 2, CG_WITH_EOL);
    result = WriteParamCopyConstructor(writer);
    if (result != DS_OK) return result;

    InsertString(*out, "// Formale Prozess-Parameter:", pos + 2, CG_WITH_EOL);
    writer->CGSetIndentPos(pos);
    result = Run(writer, DS_FORMALPARAM, 
                 CG_WRITE_FORMAL_PARAM_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    writer->CGSetIndentPos(pos + 2);

    // Clone-Operator:
    InsertString(*out, "// Virtueller Clone-Operator:", pos + 2, CG_WITH_EOL);
    result = WriteParamClone(writer);
    if (result != DS_OK) return result;

    // Assign-Operator:
    InsertString(*out, "// Zuweisungsoperator:", pos + 2, CG_WITH_EOL);
    result = WriteParamAssign(writer);
    if (result != DS_OK) return result;

    // Compare-Operator:
    InsertString(*out, "// Vergleichsoperator:", pos + 2, CG_WITH_EOL);
    result = WriteParamCompare(writer);
    if (result != DS_OK) return result;

    // Equal-Operator:
    InsertString(*out, "// Virtueller Vergleichssoperator:", pos + 2, CG_WITH_EOL);
    result = WriteParamEqual(writer);
    if (result != DS_OK) return result;

    // <<-Operator:
    InsertString(*out, "// Ausgabeoperator:", pos + 2, CG_WITH_EOL);
    result = WriteParamStreamOperator(writer);
    if (result != DS_OK) return result;

    // Display-Operator:
    InsertString(*out, "// Virtueller Ausgabeoperator:", pos + 2, CG_WITH_EOL);
    result = WriteParamDisplay(writer);
    if (result != DS_OK) return result;

    // Save-Methode:
    InsertString(*out, "// Virtuelle Save-Methode:", pos + 2, CG_WITH_EOL);
    result = WriteParamSave(writer);
    if (result != DS_OK) return result;

    // Restore-Methode:
    InsertString(*out, "// Virtuelle Restore-Methode:", pos + 2, CG_WITH_EOL);
    result = WriteParamRestore(writer);
    if (result != DS_OK) return result;

    writer->CGSetIndentPos(pos);

    InsertString(*out, "};", pos, CG_WITH_EOL);
  }

  return DS_OK;
}


/****************************************************************************
 * WriteParam(): Schreiben der Signalparameter
 *               -> writer
 *               Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *               Seiteneffekte: Anlegen und Fuellen der .h-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteParam(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (GetFormalParamList()) 
  {
    InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, pos, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, " *", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_PROCESS_PARAM_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }

  return DS_OK;
}

/****************************************************************************
 * WriteTypeDefinition(): Schreiben der Prozesse in Aufzaehlungstyp
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteTypeHeader(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "class ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ": public SCProcessType", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "public:", pos + 2, CG_WITH_EOL);
  InsertString(*out, PREFIX_PROCESS_TYPE, pos + 4, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out,
               "(SCNatural type_key, const char *type_name, SCInteger max_processes, SCNatural input_queue_length) :",
               0, CG_WITH_EOL);
  InsertString(*out, "SCProcessType(type_key, type_name, max_processes, input_queue_length)",
               pos + 6, CG_WITH_EOL);
  InsertString(*out, "{", pos + 4, CG_WITH_EOL);
  InsertString(*out, "}", pos + 4, CG_WITH_EOL);
  InsertString(*out, "SCProcess *NewInstance(const SCProcessID creatorID = kSCNoProcessID, SCDataType *actual_params = NULL) const;", pos + 4, CG_WITH_EOL);
  InsertString(*out, "SCProcess *NewInstance(SCMem &save_area) const;", pos + 4, CG_WITH_EOL);
  InsertString(*out, "};", pos, CG_WITH_EOL);

  InsertString(*out, "static ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);        
  InsertString(*out, " * ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteInclude(): Schreiben eines Prozessincludes
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteInclude(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  InsertString(*out, "#include \"", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, SUFFIX_H, 0, CG_NO_EOL);
  InsertString(*out, "\"", 0, CG_WITH_EOL);
  return DS_OK;
}

/****************************************************************************
 * WriteTypeImplementation(): Anlegen der Prozesstyp-Objekte
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteTypeImplementation(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;
  DSBlockRef block;

  out = writer->CGGetFilePointer();
  assert (out);

  block = GetThisBlock();
  assert(block);

  block->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " * ", 0, CG_NO_EOL);
  block->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " = NULL;", 0, CG_WITH_EOL);

  InsertLine(*out);

  InsertString(*out, "SCProcess *", 0, CG_NO_EOL);
  block->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, "NewInstance(const SCProcessID creatorID, SCDataType *actual_params) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  if (GetFormalParamList() == NULL)
  {
    InsertString(*out, "(void)actual_params;", 2, CG_WITH_EOL);
    InsertLine(*out);
  }
  InsertString(*out, "return new ", 2, CG_NO_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "(creatorID, this", 0, CG_NO_EOL);
  if (GetFormalParamList())
  {
    InsertString(*out, ", (", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(),
                 0, CG_NO_EOL);
    InsertString(*out, " *)", 0, CG_NO_EOL);
    InsertString(*out, "actual_params", 0, CG_NO_EOL);
  }
  InsertString(*out, ");", 0, CG_WITH_EOL);
  InsertString(*out, "}", 0, CG_WITH_EOL);

  InsertLine(*out);

  InsertString(*out, "SCProcess *", 0, CG_NO_EOL);
  block->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, "NewInstance(SCMem &save_area) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  InsertString(*out, "return new ", 2, CG_NO_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "(save_area);", 0, CG_WITH_EOL);
  InsertString(*out, "}", 0, CG_WITH_EOL);

  InsertLine(*out);

  return DS_OK;
}

/****************************************************************************
 * WriteTypeInit(): Anlegen der Prozesstyp-Objekte
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteTypeInit(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_PROCESS_TYPE_VARIABLE, 2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " = new ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_TYPE_ID, 0, CG_NO_EOL); 
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ", ", 0, CG_NO_EOL);
  InsertString(*out, "\"", 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "\", ", 0, CG_NO_EOL);
  if (GetMaxInstances())
  { 
    writer->CGSetIndentPos(0);
    if ((result = GetMaxInstances()->Write(writer)) != DS_OK) return result;
    writer->CGSetIndentPos(pos);
  }
  else
  {
    InsertString(*out, CG_NAME_SCL_UNLIMITED_PROCESS, 0, CG_NO_EOL);
  }
  InsertString(*out, ", ", 0, CG_NO_EOL);
  if (GetQueueLength())
  { 
    writer->CGSetIndentPos(0);
    if ((result = GetQueueLength()->Write(writer)) != DS_OK) return result;
    writer->CGSetIndentPos(pos);
  }
  else
  {
    InsertString(*out, "kSCUnlimitedQueueLength", 0, CG_NO_EOL);
  }
  InsertString(*out, ");", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteTypeFinish(): Anlegen der Prozesstyp-Objekte
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteTypeFinish(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "delete ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteInitialize(): Schreiben der Prozessinitialisierungen
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteInitialize(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;
  
  out = writer->CGGetFilePointer();
  assert (out);

  InsertString(*out, "result = result & ", 2, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "_data::Initialize();", 0, CG_WITH_EOL);

  InsertString(*out, "result = result & ", 2, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Initialize();", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteFinish(): Schreiben der Prozessinitialisierungen
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteFinish(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;
  
  out = writer->CGGetFilePointer();
  assert (out);

  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "_data::Finish();", 0, CG_WITH_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Finish();", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * GetIdentifier(): Liefert den vollen Namen des Prozesses (mit Prefix)
 *                      Ergebnis: Referenz auf DSString-Objekt
 *                      Seiteneffekte: keine
 ****************************************************************************/
 
DSString & CGProcess::GetIdentifier(void) const
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
      std::cerr << "Unexpected type of parent object for CGProcess !" << std::endl;
      assert(DS_FALSE);
    }
    father = father->GetParent();
  }
  
  *full_name = PREFIX_PROCESS + *full_name;

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
 
DSResult CGProcess::WriteIdentifier(CGWriter *writer) const
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
 * WriteSave(): Speicherung aller Relevanten Prozessdaten
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteSave(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean ", pos, CG_NO_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Save(SCMem &save_area) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertHead(*out, "Speicherung der Daten der Basisklasse", pos + 2);
  InsertString(*out, "SCProcess::Save(save_area);", pos + 2, CG_WITH_EOL);
  InsertLine(*out);
  writer->CGSetIndentPos(pos + 2);
  if (GetVariableList())
  {
    InsertHead(*out, "Sicherung der Variablen:", pos + 2);
    result = Run(writer, DS_VARIABLE, CG_WRITE_SAVE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
  if (GetFormalParamList())
  {
    InsertHead(*out, "Sicherung der formalen Parameter", pos + 2);
    result = Run(writer, DS_FORMALPARAM, CG_WRITE_SAVE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
  InsertHead(*out, "Speicherung der Zustandsvariablen", pos + 2);
  str.Format("save_area.Store(&%scur_state, sizeof(%scur_state));",
             PREFIX_VARIABLE, PREFIX_VARIABLE);
  InsertString(*out, str, pos + 2, CG_WITH_EOL);
  InsertLine(*out);
  if (GetSensorList())
  {
    InsertHead(*out, "Sicherung der Sensoren:", pos + 2);
    result = Run(writer, DS_SENSOR, CG_WRITE_SAVE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL); 
  InsertString(*out, "}", pos, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);

  return DS_OK;
}

/****************************************************************************
 * WriteRestore(): Restaurierung aller relevanten Prozessdaten
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteRestore(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean ", pos, CG_NO_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Restore(SCMem &save_area)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertHead(*out, "Restaurierung der Daten der Basisklasse", pos + 2);
  InsertString(*out, "SCProcess::Restore(save_area);", pos + 2, CG_WITH_EOL);
  InsertLine(*out);
  writer->CGSetIndentPos(pos + 2);
  if (GetVariableList())
  {
    InsertHead(*out, "Restaurierung der Variablen:", pos + 2);
    result = Run(writer, DS_VARIABLE, CG_WRITE_RESTORE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
  if (GetFormalParamList())
  {
    InsertHead(*out, "Restaurierung der formalen Parameter", pos + 2);
    result = Run(writer, DS_FORMALPARAM, CG_WRITE_RESTORE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
  InsertHead(*out, "Restaurierung der Zustandsvariablen", pos + 2);
  str.Format("save_area.Restore(&%scur_state, sizeof(%scur_state));",
             PREFIX_VARIABLE, PREFIX_VARIABLE);
  InsertString(*out, str, pos + 2, CG_WITH_EOL);
  InsertLine(*out);
  if (GetSensorList())
  {
    InsertHead(*out, "Restaurierung der Sensoren:", pos + 2);
    result = Run(writer, DS_SENSOR, CG_WRITE_RESTORE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL); 
  InsertString(*out, "}", pos, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);

  return DS_OK;
}

/****************************************************************************
 * WriteSize(): Berechnet benoetigen Speicherplatz fuer Save und Restore
 *              -> writer
 *              Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *              Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteSize(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "void ", pos, CG_NO_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Size(SCSize *cur_size) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
/*  InsertString(*out, "#if _SC_VALIDATION_OPTIMIZE", 0, CG_WITH_EOL);
  InsertString(*out, "SCNatural size = cur_size->size, historySize = cur_size->historySize;", pos + 2, CG_WITH_EOL);
  InsertLine(*out);
  InsertStringComment(*out, "if (IsSizeValid())", "only false at first call!", pos + 2); 
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  InsertString(*out, "cur_size->size += GetCurrentSize();", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "cur_size->historySize += GetCurrentHistorySize();", pos + 4, CG_WITH_EOL); 
  InsertLine(*out);
  InsertString(*out, "return;", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);
  InsertString(*out, "#endif", 0, CG_WITH_EOL);
  InsertLine(*out); */
  InsertHead(*out, "Berechnung der Groesse der Basisklasse", pos + 2);
  InsertString(*out, "SCProcess::Size(cur_size);", pos + 2, CG_WITH_EOL);
  InsertLine(*out);
  writer->CGSetIndentPos(pos + 2);
  if (GetVariableList())
  {
    InsertHead(*out, "Berechnung der Variablengroessen:", pos + 2);
    result = Run(writer, DS_VARIABLE, CG_WRITE_SIZE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
  if (GetFormalParamList())
  {
    InsertHead(*out, "Berechnung der Groessen der formalen Parameter", pos + 2);
    result = Run(writer, DS_FORMALPARAM, CG_WRITE_SIZE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
  InsertHead(*out, "Groesse der Zustandsvariablen", pos + 2);
  str.Format("cur_size->size += sizeof(%scur_state);",
             PREFIX_VARIABLE);
  InsertString(*out, str, pos + 2, CG_WITH_EOL);
  InsertLine(*out);
  if (GetSensorList())
  {
    InsertHead(*out, "Berechnung der Sensorgroessen:", pos + 2);
    result = Run(writer, DS_SENSOR, CG_WRITE_SIZE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
/*  InsertString(*out, "#if _SC_VALIDATION_OPTIMIZE", 0, CG_WITH_EOL);
  InsertString(*out, "SetCurrentSize(cur_size->size - size);", pos + 2, CG_WITH_EOL); 
  InsertString(*out, "SetCurrentHistorySize(cur_size->historySize - historySize);", pos + 2, CG_WITH_EOL); 
  InsertString(*out, "SetSizeValid();", pos + 2, CG_WITH_EOL); 
  InsertString(*out, "#endif", 0, CG_WITH_EOL); */
  InsertString(*out, "}", pos, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);

  return DS_OK;
}

DSResult CGProcess::WriteDisplay(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCStream& ", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  writer->CGSetIndentPos(pos);
  InsertString(*out, "::Display(SCStream &out) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  InsertString(*out, "return (out << *this);", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcess::WriteStreamOperator(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCStream& ", pos, CG_NO_EOL);
  InsertString(*out, "operator<<(SCStream &out, const ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  writer->CGSetIndentPos(pos);
  InsertString(*out, " &me)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return me.Display(out);", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcess::WriteSensorUpdate(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSCardinal   i;
  DSSystemRef  system;
  DSBlockRef   block;
  DSString     str;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  system = GetThisSystem();
  assert(system);

  block = GetThisBlock();
  assert(block);

  InsertString(*out, "void ", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  writer->CGSetIndentPos(pos);
  InsertString(*out, "::SensorUpdate(const SCInteger action, ", 0, CG_NO_EOL);
  InsertString(*out, "const SCTraceSensorRole role, ", 0, CG_NO_EOL);
  InsertString(*out, "const struct SCTraceSensorUpdate &info)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "switch (action)", pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);

  // fuer jeden Event werden alle Sensor-Listen durchlaufen:
  for (i = cgTraceSchedulerInit;
       i < cgTraceMax;
       i++)
  {
    InsertString(*out, "case ", pos + 4, CG_NO_EOL);
    InsertString(*out, event_names[i], 0, CG_NO_EOL);
    InsertString(*out, ":", 0, CG_WITH_EOL); 
    if (event_roles[i] != cgTSRoleNone) // ist die Rolle wichtig ?
    {
      // Zusaetzliche if-Abfrage generieren um die richtige
      // Rolle des Prozesses zu gewaehrleisten:
      str.Format("if (role == %s)", event_role_names[event_roles[i]]);
      InsertString(*out, str, pos + 6, CG_WITH_EOL);
      InsertString(*out, "{", pos + 6, CG_WITH_EOL);
      pos += 2;
    }
    writer->CGSetIndentPos(pos + 6);
    writer->CGSetEvent(i);
    system->Run(writer, DS_SENSOR, CG_WRITE_UPDATE);  // System-Sensoren updaten
    block->Run(writer, DS_SENSOR, CG_WRITE_UPDATE);   // Block-Sensoren updaten
    Run(writer, DS_SENSOR, CG_WRITE_UPDATE);          // Prozess-Sensoren updaten
    if (event_roles[i] != cgTSRoleNone) // ist die Rolle wichtig ?
    {
      pos -= 2;
      InsertString(*out, "}", pos + 6, CG_WITH_EOL);
    }
    writer->CGSetIndentPos(pos);
    InsertString(*out, "break;", pos + 6, CG_WITH_EOL);
  } 
  InsertString(*out, "default:", pos + 4, CG_WITH_EOL);
  InsertString(*out, "break;", pos + 6, CG_WITH_EOL);
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}


/****************************************************************************
 * WriteInitializeImplementation(): Generiert die Initialisierungsfunktion
 *                                  eines Blocks.
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/

DSResult CGProcess::WriteInitializeImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  DSString str;
  DSCardinal transition_id;

  out = writer->CGGetFilePointer();
  assert (out);

  /*
   * 1. Daten-Klasse:
   */

  InsertString(*out, "SCBoolean ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "_data::Initialize(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  InsertString(*out, "SCBoolean result = true;", 2, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "if (SCScheduler::IsVerbose())", 2, CG_WITH_EOL);
  InsertString(*out, "{", 2, CG_WITH_EOL);
  InsertString(*out, "SCScheduler::outputStream << \"Initializing process ", 4, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "...\" << std::endl;", 0, CG_WITH_EOL);
  InsertString(*out, "}", 2, CG_WITH_EOL);
  InsertLine(*out);

  if (GetSortList())
  {
    InsertHead(*out, "Datentypen auf Prozessebene initialisieren:" , 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SORT, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetProcedureList() && HasComplexProcedures(DS_FALSE))
  {
    InsertHead(*out,
               "Prozedurtypen initialisieren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_PROCEDURE, CG_WRITE_TYPE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (DSSignalComponent::GetSignalList())
  {
    InsertHead(*out,
               "Signaltypen der SDL-Signale auf Prozessebene initialisieren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SIGNAL, CG_WRITE_TYPE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetTimerList())
  {
    InsertHead(*out,
               "Timertypen der SDL-Timer initialisieren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_TIMER, CG_WRITE_TYPE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  /********** Prozeduren initialisieren ****************************************/

  if (GetProcedureList() && HasComplexProcedures(DS_FALSE))
  {
    InsertHead(*out,
               "Statische Initialisierungsfunktionen der Prozeduren aufrufen:",
               2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_PROCEDURE, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  InsertLine(*out);
  InsertString(*out, "return result;", 2, CG_WITH_EOL);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  InsertLine(*out);

  /*
   * 2. Instanz-Klasse:
   */
   
  InsertString(*out, "SCBoolean ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Initialize(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  if (!GetInitInstances() ||
      !GetInitInstances()->IsZero())
  {
    InsertString(*out, "SCNatural i;", 2, CG_WITH_EOL);
    InsertString(*out, CG_NAME_SCL_PROCESS_ID, 2, CG_NO_EOL);
    InsertString(*out, " id;", 0, CG_WITH_EOL);
//    InsertString(*out, CG_NAME_SCL_PROCESS, 2, CG_NO_EOL);
//    InsertString(*out, " *pcs;", 0, CG_WITH_EOL);
  }
  str.Format("SCTransition *%stransition;", PREFIX_VARIABLE);
  InsertString(*out, str, 2, CG_WITH_EOL);
  InsertString(*out, "SCBoolean result = true;", 2, CG_WITH_EOL);
  InsertLine(*out);

  InsertHead(*out, "Initialisierung aller Zustaende des Prozesstyps:", 2);

  // Startzustand:
  InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, 2, CG_NO_EOL);
  InsertString(*out, "start", 0, CG_NO_EOL);
  InsertString(*out, " = new SCStateType(", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_ID, 0, CG_NO_EOL);
  InsertString(*out, "start", 0, CG_NO_EOL);
  InsertString(*out, ", \"", 0, CG_NO_EOL);
  InsertString(*out, "start", 0, CG_NO_EOL);
  InsertString(*out, "\");", 0, CG_WITH_EOL);

  if (GetStateList())
  {
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_STATE, CG_WRITE_TYPE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  InsertHead(*out, "Initialisierung aller Transitionen des Prozesstyps:", 2);

  // Starttransition:
  writer->CGSetLastTransitionID(0);
  InsertString(*out, PREFIX_VARIABLE, 2, CG_NO_EOL);
  InsertString(*out, "transition = new SCTransition(kSCTransitionIDBase + ",
               0, CG_NO_EOL);
  transition_id = writer->CGGetLastTransitionID();
  InsertCardinal(*out, transition_id, 0, CG_NO_EOL);
  InsertString(*out, ", ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, "start", 0, CG_NO_EOL);
  InsertString(*out, ", ", 0, CG_NO_EOL);
  InsertString(*out, "-1, NULL, kSCPrioInputNone", 0, CG_NO_EOL);
  InsertString(*out, ");", 0, CG_WITH_EOL);
  str.Format("assert(%stransition);", PREFIX_VARIABLE);
  InsertString(*out, str, 2, CG_WITH_EOL);

  writer->CGSetIndentPos(2);
  result = Run(writer, DS_STATE, CG_WRITE_TRANSITION_INIT);
  writer->CGSetIndentPos(0);
  InsertLine(*out);

  if (!GetInitInstances() ||
      !GetInitInstances()->IsZero())
  {
    InsertHead(*out, "Initiale Anzahl von Prozessinstanzen erzeugen:", 2);

    InsertString(*out, "for (i = 0; i < ", 2, CG_NO_EOL);
    if (GetInitInstances())
    {
      if ((result = GetInitInstances()->Write(writer)) != DS_OK) return result;
    }
    else
    {
      InsertString(*out, "1", 0, CG_NO_EOL);
    }
    InsertString(*out, "; i++)", 0, CG_WITH_EOL);
    InsertString(*out, "{", 2, CG_WITH_EOL);
/*    InsertString(*out, "pcs = new ", 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = Write(writer, CG_WRITE_IDENTIFIER);
    if (result != DS_OK) return result;
    InsertString(*out, "(", 0, CG_NO_EOL);
    InsertString(*out, CG_NAME_SCL_NO_PROCESS_ID, 0, CG_NO_EOL);
    InsertString(*out, ", ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_PROCESS_TYPE_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ");", 0, CG_WITH_EOL);
    InsertString(*out, "assert(pcs);", 4, CG_WITH_EOL); */
    InsertString(*out, "id = ", 4, CG_NO_EOL);
    InsertString(*out, CG_NAME_SCL_SCHEDULER, 0, CG_NO_EOL);
    InsertString(*out, "::Create(", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_PROCESS_TYPE_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
//    InsertString(*out, ", pcs);", 0, CG_WITH_EOL);
    InsertString(*out, ");", 0, CG_WITH_EOL);
    InsertString(*out, "assert(id != kSCNoProcessID);", 4, CG_WITH_EOL);
    InsertString(*out, "}", 2, CG_WITH_EOL);
    InsertLine(*out);
  }
  
  InsertString(*out, "return result;", 2, CG_WITH_EOL);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  InsertLine(*out);

  return DS_OK;
}

/****************************************************************************
 * WriteFinishImplementation(): Generiert die Beendigungsfunktion
 *                              eines Blocks.
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteFinishImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);

  /*
   * 1. Data:
   */
   
  InsertString(*out, "void ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "_data::", 0, CG_NO_EOL);
  InsertString(*out, "Finish(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  
  if (DSSignalComponent::GetSignalList())
  {
    InsertHead(*out,
               "Signaltypen der SDL-Signale auf Prozessebene zerstoeren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SIGNAL, CG_WRITE_TYPE_FINISH);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetTimerList())
  {
    InsertHead(*out,
               "Timertypen der SDL-Timer zerstoeren:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_TIMER, CG_WRITE_TYPE_FINISH);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetProcedureList() && HasComplexProcedures(DS_FALSE))
  {
    InsertHead(*out,
               "Prozedurtypen zerstoeren:",
               2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_PROCEDURE, CG_WRITE_TYPE_FINISH);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }

  InsertString(*out, "}", 0, CG_WITH_EOL);
  InsertLine(*out);

  /*
   * 2. Instanz:
   */
   
  InsertString(*out, "void ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, "Finish(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  
  InsertHead(*out,
             "Zustandstypen der Prozess-Zustaende zerstoeren:", 2);

  InsertString(*out, "delete ", 2, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, "start", 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  if (GetStateList())
  {
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_STATE, CG_WRITE_TYPE_FINISH);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }
  InsertLine(*out);

  InsertString(*out, "}", 0, CG_WITH_EOL);
  
  return DS_OK;
}


/****************************************************************************
 * WriteConstructor(): Generiert die Konstruktorfunktionen
 *                     eines Prozesses.
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteConstructor(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);

  InsertHead(*out, "Konstruktoren", 0);

  /*
   * 1. Data:
   */

  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  InsertString(*out, "_data::", 0, CG_NO_EOL);
  WriteIdentifier(writer);
  InsertString(*out, "_data(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  InsertLine(*out);

  /*
   * 2. Instanz
   */

  WriteIdentifier(writer);
  InsertString(*out, "::", 0, CG_NO_EOL); 
  WriteIdentifier(writer);
  InsertString(*out, "(const SCProcessID", 0, CG_NO_EOL); 
  InsertString(*out, " creator, const SCProcessType *proc_type", 0, CG_NO_EOL); 
  if (GetFormalParamList())
  {
    InsertString(*out, ", ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(),
                 0, CG_NO_EOL);
    InsertString(*out, " *actual_params", 0,
                 CG_NO_EOL);
  }
  InsertString(*out, ") : ", 0, CG_WITH_EOL);
  InsertString(*out, "SCProcess(creator, ", 2, CG_NO_EOL); 
  str.Format("proc_type, %sstart",
             PREFIX_STATE_TYPE_VARIABLE);
  InsertString(*out, str, 0, CG_NO_EOL);
  if (GetFormalParamList())
  {
    InsertString(*out, ", actual_params", 0, CG_NO_EOL); 
  }
  else
  {
    InsertString(*out, ", NULL", 0, CG_NO_EOL); 
  }
  if (HasNonConstantVariables())
  {
    InsertString(*out, ", true", 0, CG_NO_EOL); 
  }
  else
  {
    InsertString(*out, ", false", 0, CG_NO_EOL); 
  }
  InsertString(*out, ")", 0, CG_NO_EOL);

  // Sensoren initialisieren:

  if (GetSensorList())
  {
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SENSOR, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }
  InsertLine(*out);

  InsertString(*out, "{", 0, CG_WITH_EOL);
  InsertString(*out, "#if _SC_VALIDATION_OPTIMIZE", 0, CG_WITH_EOL);
  InsertHead(*out, "Ausgangsgroesse des Prozesses initialisieren:", 2);
  InsertString(*out, "SCSize my_size;", 2, CG_WITH_EOL);
  InsertLine(*out);
  InsertString(*out, "Size(&my_size);", 2, CG_WITH_EOL);
  InsertString(*out, "SetCurrentSize(my_size.size);", 2, CG_WITH_EOL); 
  InsertString(*out, "SetCurrentHistorySize(my_size.historySize);", 2, CG_WITH_EOL); 
  InsertString(*out, "#endif", 0, CG_WITH_EOL);
  InsertLine(*out);

  /******** Meldung ausgeben ************************/

  InsertHead(*out, "Meldung ausgeben:", 2);
  InsertString(*out, "if (IsVerbose())", 2, CG_WITH_EOL);
  InsertString(*out, "{", 2, CG_WITH_EOL);
  InsertString(*out, "SCScheduler::outputStream << \"Creating \" "
             "<< *this << std::endl;", 4, CG_WITH_EOL);
  InsertString(*out, "}", 2, CG_WITH_EOL);
  InsertLine(*out);

  /******** Prozessparameter kopieren falls vorhanden ************************/

  if (GetFormalParamList())
  {
    InsertHead(*out,
               "Initialisierung der formalen Parameter mit den aktuellen Parametern:",
               2);
    writer->CGSetIndentPos(2);

    result = Run(writer, DS_FORMALPARAM, CG_WRITE_FORMAL_PARAM_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  if (GetVariableList())
  {
    InsertHead(*out, "Variableninitialisierung:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_VARIABLE, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  InsertHead(*out, "Aktuellen Zustand auf Startzustand setzen:", 2);
  str.Format("%scur_state = %sstart;", PREFIX_VARIABLE, 
          PREFIX_STATE_TYPE_ID);
  InsertString(*out, str, 2, CG_WITH_EOL);

  /* Instanz sichtbar machen indem sie in die Prozesstabelle eingetragen wird */

  InsertLine(*out);
  InsertHead(*out, "Auf die Instanz darf nun zugegriffen werden:", 2);
  InsertString(*out, "GetAutomatonTable()->Insert(Self(), this);",
               2, CG_WITH_EOL);

  InsertString(*out, "}", 0, CG_WITH_EOL);
  InsertLine(*out);

  /******** Konstruktor zum Restaurieren aus Speicherbereich ********************/
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::", 0, CG_NO_EOL); 
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "(SCMem &save_area) : ", 0, CG_WITH_EOL);
  if (HasNonConstantVariables())
  {
    InsertString(*out, "SCProcess(true, (SCObject *)NULL)", 2, CG_NO_EOL);
  }
  else
  {
    InsertString(*out, "SCProcess(false, (SCObject *)NULL)", 2, CG_NO_EOL);
  }
  // Sensoren initialisieren:

  if (GetSensorList())
  {
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SENSOR, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }
  InsertLine(*out);

  InsertString(*out, "{", 0, CG_WITH_EOL);

  InsertString(*out, "Restore(save_area);", 2, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "#if _SC_VALIDATION_OPTIMIZE", 0, CG_WITH_EOL);
  InsertHead(*out, "Ausgangsgroesse des Prozesses initialisieren:", 2);
  InsertString(*out, "SCSize my_size;", 2, CG_WITH_EOL);
  InsertLine(*out);
  InsertString(*out, "Size(&my_size);", 2, CG_WITH_EOL);
  InsertString(*out, "SetCurrentSize(my_size.size);", 2, CG_WITH_EOL); 
  InsertString(*out, "SetCurrentHistorySize(my_size.historySize);", 2, CG_WITH_EOL); 
  InsertString(*out, "#endif", 0, CG_WITH_EOL);
  InsertLine(*out);

  /******** Meldung ausgeben ************************/

  InsertHead(*out, "Meldung ausgeben:", 2);
  InsertString(*out, "if (IsVerbose())", 2, CG_WITH_EOL);
  InsertString(*out, "{", 2, CG_WITH_EOL);
  InsertString(*out, "SCScheduler::outputStream << \"Recreated \" "
             "<< *this << std::endl;", 4, CG_WITH_EOL);
  InsertString(*out, "}", 2, CG_WITH_EOL);

  /* Instanz sichtbar machen indem sie in die Prozesstabelle eingetragen wird */

  InsertLine(*out);
  InsertHead(*out, "Auf die Instanz darf nun zugegriffen werden:", 2);
  InsertString(*out, "GetAutomatonTable()->Insert(Self(), this);",
               2, CG_WITH_EOL);
//  InsertString(*out, "GetType()->Allocate(this, false);",
//               2, CG_WITH_EOL);

  InsertString(*out, "}", 0, CG_WITH_EOL);
  InsertLine(*out);

  return DS_OK;
}

/****************************************************************************
 * WriteDestructor(): Generiert die Destruktorfunktion
 *                     eines Prozesses.
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcess::WriteDestructor(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertHead(*out, "Destruktor", 0);

  /*
   * 1. Data:
   */
   
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  InsertString(*out, "_data::~", 0, CG_NO_EOL);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "_data(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  InsertString(*out, "}", 0, CG_WITH_EOL);
  InsertLine(*out);

  /*
   * 2. Instanz
   */

  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::~", 0, CG_NO_EOL); 
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "(void)", 0, CG_WITH_EOL); 
  InsertString(*out, "{", 0, CG_WITH_EOL); 
  InsertHead(*out, "Meldung ausgeben:", 2);
  InsertString(*out, "if (IsVerbose())", 2, CG_WITH_EOL);
  InsertString(*out, "{", 2, CG_WITH_EOL);
  InsertString(*out, "SCScheduler::outputStream << \"Deleting \" "
             "<< *this << std::endl;", 4, CG_WITH_EOL);
  InsertString(*out, "}", 2, CG_WITH_EOL);
  InsertString(*out, "}", 0, CG_WITH_EOL); 
  InsertLine(*out);

  return DS_OK;
}
