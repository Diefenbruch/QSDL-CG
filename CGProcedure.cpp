/****************************************************************************
 *                 Modul: $RCSfile: CGProcedure.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/02/09 11:09:56 $
 * $Revision: 1.7 $
 *
 * Aufgabe: Dieses Modul enthaelt Hilfsfunktionen
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
#include <DS/DSProcess.h>
#include <DS/DSTransition.h>
#include <DS/DSVariable.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGSort.h"
#include "CGProcedure.h"

#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Externe Variablen
 ****************************************************************************/

extern char *event_names[];
extern CGEventRoles event_roles[];
extern char *event_role_names[];

/****************************************************************************
 * Globale Variablen
 ****************************************************************************/

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGProcedure::CGProcedure(DSObject* father, DSString* proc_name) :
             DSProcedure(father, proc_name)
{
}
CGProcedure::CGProcedure(DSObject* father, DSString& proc_name) :
             DSProcedure(father, proc_name)
{
}
CGProcedure::CGProcedure(DSObject* father, const char* proc_name) :
             DSProcedure(father, proc_name)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGProcedure::~CGProcedure(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGProcedure::New(DSObject *father) const
{
  return new CGProcedure(father);
}

/****************************************************************************
 * Write(): Schreiben der Prozeduren
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/

DSResult CGProcedure::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
  
  switch (what)
  {
    case CG_WRITE_H:
      if (GetIsComplex())
      {
        result = WriteHeaderComplex((CGWriter *)writer);
        return (result == DS_OK) ? DS_OK : DS_ERROR;
      }
      else
      {
        result = WriteHeaderSimple((CGWriter *)writer);
        return (result == DS_OK) ? DS_OK : DS_ERROR;
      }
      break;
      
    case CG_WRITE_CC:
      if (GetIsComplex())
      {
        result = WriteImplementationComplex((CGWriter *)writer);
        return (result == DS_OK) ? DS_OK : DS_ERROR;
      }
      else
      {
        result = WriteImplementationSimple((CGWriter *)writer);
        return (result == DS_OK) ? DS_OK : DS_ERROR;
      }
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
      
    case CG_WRITE_FRIEND:
      result = WriteFriend((CGWriter *)writer);
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
 * WriteHeaderSimple(): Schreiben der Prozeduren
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen der .h-Files
 ****************************************************************************/
 
DSResult CGProcedure::WriteHeaderSimple(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSSortRef sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

//  writer->CGSetLastRequestID(0);

  if (GetReturnParamRef() == NULL)
  {
    InsertString(*out, "void ", pos, CG_NO_EOL);
  }
  else
  {
    sort = GetReturnParamRef()->GetSortRef();
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(*out, " ", 0, CG_NO_EOL);
  }
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(", 0, CG_NO_EOL);
  if (GetFormalParamList())
  {
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_FORMALPARAM, 
                 CG_WRITE_FORMAL_PARAM_H);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }
  else
  {
    InsertString(*out, "void", 0, CG_NO_EOL);
  }
  InsertString(*out, ");", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteImplementationSimple(): Schreiben der Prozeduren  
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcedure::WriteImplementationSimple(CGWriter *writer) const
{
  DSResult result;
  DSObject *father;
  DSString str;
  CGPos pos;
  CGFile *out;
  DSSortRef sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  father = GetParent();
  assert(father);

  switch(father->GetType())
  {
    case DS_SYSTEM:
    case DS_BLOCK:
    case DS_BLOCKSUBSTRUCTURE:
      writer->CGSetIndentPos(0);
      if (GetReturnParamRef() == NULL)
      {
        InsertString(*out, "void ", pos, CG_NO_EOL);
      }
      else
      {
        sort = GetReturnParamRef()->GetSortRef();
        if (!sort->GetIsPredefined())
        {
          father->Write(writer, CG_WRITE_IDENTIFIER);
          InsertString(*out, "::", 0, CG_NO_EOL);
        }
        sort->Write(writer, CG_WRITE_IDENTIFIER);
        InsertString(*out, " ", 0, CG_NO_EOL);
      }
      father->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      break;
    case DS_PROCESS:
      if (GetReturnParamRef() == NULL)
      {
        InsertString(*out, "void ", pos, CG_NO_EOL);
      }
      else
      {
        sort = GetReturnParamRef()->GetSortRef();
        if (!sort->GetIsPredefined())
        {
          writer->CGSetIndentPos(0);
          result = ((DSProcess *)father)->Write(writer, CG_WRITE_IDENTIFIER);
          if (result != DS_OK) return result;
          InsertString(*out, "::", 0, CG_NO_EOL);
        }
        writer->CGSetIndentPos(0);
        sort->Write(writer, CG_WRITE_IDENTIFIER);
        writer->CGSetIndentPos(pos);
        InsertString(*out, " ", 0, CG_NO_EOL);
      }
      writer->CGSetIndentPos(0);
      result = ((DSProcess *)father)->Write(writer, CG_WRITE_IDENTIFIER);
      if (result != DS_OK) return result;
      break;
    default:
      assert(DS_FALSE);
      return DS_ERROR;
      break;
  }
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(", 0, CG_NO_EOL);
  if (GetFormalParamList())
  {
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_FORMALPARAM,
                 CG_WRITE_FORMAL_PARAM_CC);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }
  else
  {
    InsertString(*out, "void", 0, CG_NO_EOL);
  }
  InsertString(*out, ")", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);

  /********* Sorten ******************************************************/

  if (GetSortList())
  {
    InsertHead(*out, "Benutzerdefinierte SDL-Datentypen auf Prozedurebene",
               2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SORT, CG_WRITE_H);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  /******** Rueckgabewert *************************************************/

  if (GetReturnParamRef())
  {
    InsertHead(*out, "Rueckgabewert der Prozedur:", 2);
    sort = GetReturnParamRef()->GetSortRef();
    writer->CGSetIndentPos(2);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, " ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, GetReturnParamRef()->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
    InsertLine(*out);
  }

  /******** Variablen *************************************************/

  if (GetVariableList())
  {
    InsertHead(*out, "Lokale Variablen der Prozedur:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_VARIABLE, CG_WRITE_H);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
                                  
  /************ Starttransition **********************************************/
  
  if (GetStartTransition())
  {
    InsertHead(*out, "Starttransition:", 2);
    writer->CGSetIndentPos(2);
    result = GetStartTransition()->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    {
      return result;
    }
  }

  InsertString(*out, "}", 0, CG_WITH_EOL);
  InsertLine(*out);
  return DS_OK;
}


/****************************************************************************
 * WriteHeaderComplex(): Oberfunktion der Generierung des h-Files fuer Prozesse
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen der .h-Files
 ****************************************************************************/

DSResult CGProcedure::WriteHeaderComplex(CGWriter *writer) const
{
  DSResult result;
  DSString str;
  DSBlockRef block;
  DSProcessRef process;
  DSString filename;

  process = GetThisProcess();
  block = GetThisBlock();

  writer->CGSetLastRequestID(0);
  writer->CGSetLastCallID(0);

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
  if (process)
    process->Write(writer, CG_WRITE_INCLUDE);
  else if (block)
    block->Write(writer, CG_WRITE_INCLUDE);
  InsertLine(h_file); 

  InsertIncludeSCL(h_file, "SCProcedure.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(h_file, "SCStateType.h", 0, CG_WITH_EOL);
  if (GetSensorList())
  {
    InsertIncludeSCL(h_file, "SCTraceSensor.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCSensorProcess.h", 0, CG_WITH_EOL);
  }
  InsertLine(h_file); 

  str.Format("Prozedurklasse der SDL-Prozedur '%s'",
             GetName()->GetString());
  InsertHead(h_file, str, 0);
  InsertString(h_file, "class ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file, " : public SCProcedure, ", 0, CG_NO_EOL);
  InsertString(h_file, "public ", 0, CG_NO_EOL);
  if (process)
  {
    process->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(h_file, "_data", 0, CG_WITH_EOL);
  }
  else if (block)
    block->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(h_file, "{", 0, CG_WITH_EOL);
   
  InsertString(h_file, "public:", 2, CG_WITH_EOL); 
  InsertHead(h_file,
             "Initialisierungsfunktion der Prozedur (statisch)", 4);
  InsertString(h_file, "static SCBoolean ", 4, CG_NO_EOL);
  InsertString(h_file, "Initialize(void);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file,
             "Beendigungsfunktion der Prozedur (statisch)", 4);
  InsertString(h_file, "static void ", 4, CG_NO_EOL);
  InsertString(h_file, "Finish(void);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  /********* Konstruktoren der Prozedurklasse **********************************/

  InsertHead(h_file, "Konstruktoren", 4);
  writer->CGSetIndentPos(4);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file,
               "(class SCAutomaton *, class SCProcess *",
               0, CG_NO_EOL); 
  InsertString(h_file, ", class SCProcedureType *", 0, CG_NO_EOL);
  if (GetFormalParamList())
  {
    InsertString(h_file, ", ", 0, CG_NO_EOL);
    if (process)
    {
      writer->CGSetIndentPos(0);
      process->Write(writer, CG_WRITE_IDENTIFIER);
      InsertString(h_file, "::", 0, CG_NO_EOL);
    }
    InsertString(h_file, PREFIX_PROCEDURE_PARAM_TYPE, 0, CG_NO_EOL);
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
  
  if (GetSensorList())
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

  InsertString(h_file, "private:", 2, CG_WITH_EOL); 
  
  /********* Sorten ******************************************************/
  
  if (GetSortList())
  {
    InsertHead(h_file, "Benutzerdefinierte SDL-Datentypen auf Prozedurebene",
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
    DSProcedure *procedure;
    
    InsertHead(h_file, "Prototypen fuer die Sub-Prozeduren", 4);
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

  /******** Variablen *************************************************/

  if (GetVariableList())
  {
    InsertHead(h_file, "Lokale Variablen der Prozedur", 4);
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
    InsertHead(h_file, "Formale Parameter der Prozedur", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_FORMALPARAM, CG_WRITE_H);
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

  InsertHead(h_file, "IDs der Zustandstypen der Prozedur", 4);
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
  InsertHead(h_file, "Zustandstypen der Prozedur (statisch)", 4);

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
    InsertHead(h_file, "Input-Sets der Transitionen der Prozedur (statisch)", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_STATE, CG_WRITE_INPUT_SET_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);

    if (HasSaves(DS_FALSE))
    {
      InsertHead(h_file, "Save-Sets der Zustaende der Prozedur (statisch)", 4);
      writer->CGSetIndentPos(4);
      result = Run(writer, DS_STATE, CG_WRITE_SAVE_SET_H);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertLine(h_file);
    }
  }

  /******** Sensoren *************************************************/

  if (GetSensorList())
  {
    InsertHead(h_file, "Sensoren der Prozedur", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_SENSOR, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }
  
  /******** Standardvariablen zur Prozedursteuerung **************************/

  InsertHead(h_file, "Standardvariablen zur Prozedursteuerung", 4);
  str.Format("SCInteger %scur_state;", PREFIX_VARIABLE);
  InsertString(h_file, str, 4, CG_WITH_EOL);
  str.Format("SCInteger %stransition_id;", PREFIX_VARIABLE);
  InsertString(h_file, str, 4, CG_WITH_EOL);
  str.Format("SCTransition *%stransition;", PREFIX_VARIABLE);
  InsertString(h_file, str, 4, CG_WITH_EOL);

  str.Format("SCSignalType *%ssignal_read;", PREFIX_VARIABLE);
  InsertString(h_file, str, 4, CG_WITH_EOL);

  InsertString(h_file, "};", 0, CG_WITH_EOL);
  InsertLine(h_file);
  InsertString(h_file, "#endif", 0, CG_WITH_EOL);
  
  return DS_OK;
}


/****************************************************************************
 * WriteImplementationComplex(): Oberfunktion der der Generierung des cc-Files 
 *                        fuer Prozeduren mit Zustaenden oder Request-Aktionen
 *                       -> writer
 *                       Ergebnis: DS_OK, falls alles geklappt hat, sonst 
 *                       DS_ERROR
 *                       Seiteneffekte: Anlegen und Fuellen der .cc-Files
 ****************************************************************************/

DSResult CGProcedure::WriteImplementationComplex(CGWriter *writer) const
{
  DSResult result;
  DSBlockRef block;
  DSProcessRef process;
  DSString str;
  DSString filename;
  DSStringKeyList *label_list;
  DSCardinal num_of_requests;
  DSCardinal num_of_calls;
  DSCardinal i;

  block = GetThisBlock();  // don't assert block!

  process = GetThisProcess();  // don't assert process!

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

  InsertCopyright(c_file);
  InsertHead(c_file, "Includes", 0);
  
  // alle Block-H-Files includieren (fuer Output notwendig)
  writer->CGGetSystem()->Run(writer, DS_BLOCK, CG_WRITE_INCLUDE);

  if (block)
  {
    // alle Process-H-Files des Blocks includieren (fuer Create notwendig)
    block->Run(writer, DS_PROCESS, CG_WRITE_INCLUDE);

    // alle Maschinen-H-Files des Blocks includieren (fuer Sensorzugriff notwendig)
    block->Run(writer, DS_MACHINE, CG_WRITE_INCLUDE);
  }

  if (process)
  {
    // alle Procedur-H-Files des Prozesses includieren (fuer Calls notwendig)
    process->Run(writer, DS_PROCEDURE, CG_WRITE_INCLUDE);
  }
  else
  {
    // Eigenes Header-File includueren:
    WriteInclude(writer);
  }

  InsertLine(c_file);
  
  if (writer->CGDebug())
  {
    str.Format("#include <%s>", CG_DEBUG_INCLUDE);
    InsertString(c_file, str, 0, CG_WITH_EOL);
    InsertLine(c_file);
  }
  InsertIncludeSCL(c_file, "SCScheduler.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(c_file, "SCTransition.h", 0, CG_WITH_EOL);
  InsertLine(c_file);

  /********** Initialisierung der statischen Konstanten  **********************/
  
  if (GetSortList())
  {
    InsertHead(c_file, "Konstanten (Synonyme) auf Prozedurebene (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_SORT, CG_WRITE_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /***** Save-Sets der Zustaende ********************************************/

  if (GetStateList())
  {
    InsertHead(c_file, "Save-Sets der Prozedurzustaende (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_STATE, CG_WRITE_SAVE_SET_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /***** Schreiben der State-Typen  ***************/

  InsertHead(c_file,
             "Typen der SDL-Zustaende auf Prozedurebene (statisch)", 0);

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
    InsertHead(c_file, "Input-Sets der Prozedurtransitionen (statisch)", 0);
    writer->CGSetIndentPos(0);
    result = Run(writer, DS_STATE, CG_WRITE_INPUT_SET_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  /***** Initialisierungsfunktion ********************************************/

  InsertHead(c_file, "Initialisierungsfunktion der SDL-Prozedurklasse (statisch)", 0);
  writer->CGSetIndentPos(0);
  result = WriteInitializeImplementation(writer);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(c_file);

  /***** Beendigungsfunktion ********************************************/

  InsertHead(c_file, "Beendigungsfunktion der SDL-Prozedurklasse (statisch)", 0);
  writer->CGSetIndentPos(0);
  result = WriteFinishImplementation(writer);
  if (result != DS_OK)
  {
    return DS_ERROR;
  }
  InsertLine(c_file);

  /********* Konstruktoren der Prozedurklasse **********************************/
  
  InsertHead(c_file, "Konstruktoren", 0);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(c_file, "::", 0, CG_NO_EOL); 
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(c_file, "(SCAutomaton *", 0, CG_NO_EOL); 
  InsertString(c_file,
               "caller, SCProcess *owner, SCProcedureType *proc_type",
               0, CG_NO_EOL); 
  if (GetFormalParamList())
  {
    InsertString(c_file, ", ", 0, CG_NO_EOL);
    if (process)
    {
      writer->CGSetIndentPos(0);
      process->Write(writer, CG_WRITE_IDENTIFIER);
      InsertString(c_file, "::", 0, CG_NO_EOL);
    }
    InsertString(c_file, PREFIX_PROCEDURE_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(c_file, GetName(),
                 0, CG_NO_EOL);
    InsertString(c_file, " *actual_params", 0, 
                 CG_NO_EOL);
  }
  InsertString(c_file, ") : ", 0, CG_WITH_EOL);
  InsertString(c_file, "SCProcedure(caller, owner, ", 2, CG_NO_EOL); 
  str.Format("proc_type, %sstart",
             PREFIX_STATE_TYPE_VARIABLE);
  InsertString(c_file, str, 0, CG_NO_EOL); 
  if (GetFormalParamList())
  {
    InsertString(c_file, ", actual_params", 0, CG_NO_EOL); 
  }
  else
  {
    InsertString(c_file, ", NULL", 0, CG_NO_EOL); 
  }
  if (HasNonConstantVariables())
  {
    InsertString(c_file, ", true", 0, CG_NO_EOL); 
  }
  else
  {
    InsertString(c_file, ", false", 0, CG_NO_EOL); 
  }
  InsertString(c_file, ")", 0, CG_NO_EOL);

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
  InsertLine(c_file);

  InsertString(c_file, "{", 0, CG_WITH_EOL);
  InsertString(c_file, "#if _SC_VALIDATION_OPTIMIZE", 0, CG_WITH_EOL);
  InsertHead(c_file, "Ausgangsgroesse der Prozedur initialisieren:", 2);
  InsertString(c_file, "SCSize my_size;", 2, CG_WITH_EOL);
  InsertLine(c_file);
  InsertString(c_file, "Size(&my_size);", 2, CG_WITH_EOL);
  InsertString(c_file, "SetCurrentSize(my_size.size);", 2, CG_WITH_EOL); 
  InsertString(c_file, "SetCurrentHistorySize(my_size.historySize);", 2, CG_WITH_EOL); 
  InsertString(c_file, "#endif", 0, CG_WITH_EOL);
  InsertLine(c_file);

  /******** Meldung ausgeben ************************/

  InsertHead(c_file, "Meldung ausgeben:", 2);
  InsertString(c_file, "if (IsVerbose())", 2, CG_WITH_EOL);
  InsertString(c_file, "{", 2, CG_WITH_EOL);
  InsertString(c_file, "SCScheduler::outputStream << \"Creating \" "
             "<< *this << std::endl;", 4, CG_WITH_EOL);
  InsertString(c_file, "}", 2, CG_WITH_EOL);
  InsertLine(c_file);

  /******** Prozedurparameter kopieren falls vorhanden ************************/

  if (GetFormalParamList())
  {
    InsertHead(c_file,
               "Initialisierung der formalen Parameter mit den aktuellen Parametern:",
               2);
    writer->CGSetIndentPos(2);

    result = Run(writer, DS_FORMALPARAM, CG_WRITE_FORMAL_PARAM_CC);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  if (GetVariableList())
  {
    InsertHead(c_file, "Variableninitialisierung:", 2);
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_VARIABLE, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(c_file);
  }

  InsertHead(c_file, "Aktuellen Zustand auf Startzustand setzen:", 2);
  str.Format("%scur_state = %sstart;", PREFIX_VARIABLE, 
          PREFIX_STATE_TYPE_ID);
  InsertString(c_file, str, 2, CG_WITH_EOL);

  /* Instanz sichtbar machen indem sie in die Prozesstabelle eingetragen wird */

  InsertLine(c_file);
  InsertHead(c_file, "Auf die Instanz darf nun zugegriffen werden:", 2);
  InsertString(c_file, "GetAutomatonTable()->Insert(GetID(), this);",
               2, CG_WITH_EOL);

  InsertString(c_file, "}", 0, CG_WITH_EOL);
  InsertLine(c_file);

  /******** Konstruktor zum Restaurieren aus Speicherbereich ********************/
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(c_file, "::", 0, CG_NO_EOL); 
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(c_file, "(SCMem &save_area) : ", 0, CG_WITH_EOL);
  if (HasNonConstantVariables())
  {
    InsertString(c_file, "SCProcedure(true, (SCObject *)NULL)", 2, CG_NO_EOL);
  }
  else
  {
    InsertString(c_file, "SCProcedure(false, (SCObject *)NULL)", 2, CG_NO_EOL);
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
  InsertLine(c_file);

  InsertString(c_file, "{", 0, CG_WITH_EOL);

  InsertString(c_file, "Restore(save_area);", 2, CG_WITH_EOL);
  InsertLine(c_file);

  InsertString(c_file, "#if _SC_VALIDATION_OPTIMIZE", 0, CG_WITH_EOL);
  InsertHead(c_file, "Ausgangsgroesse der Prozedur initialisieren:", 2);
  InsertString(c_file, "SCSize my_size;", 2, CG_WITH_EOL);
  InsertLine(c_file);
  InsertString(c_file, "Size(&my_size);", 2, CG_WITH_EOL);
  InsertString(c_file, "SetCurrentSize(my_size.size);", 2, CG_WITH_EOL); 
  InsertString(c_file, "SetCurrentHistorySize(my_size.historySize);", 2, CG_WITH_EOL); 
  InsertString(c_file, "#endif", 0, CG_WITH_EOL);
  InsertLine(c_file);

  /******** Meldung ausgeben ************************/

  InsertHead(c_file, "Meldung ausgeben:", 2);
  InsertString(c_file, "if (IsVerbose())", 2, CG_WITH_EOL);
  InsertString(c_file, "{", 2, CG_WITH_EOL);
  InsertString(c_file, "SCScheduler::outputStream << \"Recreated \" "
             "<< *this << std::endl;", 4, CG_WITH_EOL);
  InsertString(c_file, "}", 2, CG_WITH_EOL);

  /* Instanz sichtbar machen indem sie in die Prozesstabelle eingetragen wird */

  InsertLine(c_file);
  InsertHead(c_file, "Auf die Instanz darf nun zugegriffen werden:", 2);
  InsertString(c_file, "GetAutomatonTable()->Insert(GetID(), this);",
               2, CG_WITH_EOL);
//  InsertString(c_file, "GetType()->Allocate(this, false);",
//               2, CG_WITH_EOL);

  InsertString(c_file, "}", 0, CG_WITH_EOL);
  InsertLine(c_file);

  /********* Destruktor der Prozessklasse **********************************/
  
  InsertHead(c_file, "Destruktor", 0);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(c_file, "::~", 0, CG_NO_EOL); 
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(c_file, "(void)", 0, CG_WITH_EOL); 
  InsertString(c_file, "{", 0, CG_WITH_EOL); 
  InsertHead(c_file, "Meldung ausgeben:", 2);
  InsertString(c_file, "if (IsVerbose())", 2, CG_WITH_EOL);
  InsertString(c_file, "{", 2, CG_WITH_EOL);
  InsertString(c_file, "SCScheduler::outputStream << \"Deleting \" "
             "<< *this << std::endl;", 4, CG_WITH_EOL);
  InsertString(c_file, "}", 2, CG_WITH_EOL);
  InsertString(c_file, "}", 0, CG_WITH_EOL); 
  InsertLine(c_file);

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
  writer->CGSetLastCallID(0);    // auch Prozeduren veraendern diesen Zaehler!

  /*************** Body-Funktion der Prozessklasse **************************/
    
  InsertHead(c_file, "Implementierungsfunktion der SDL-Prozedur", 0);
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

    num_of_requests = NumOfActions(DS_REQUEST, DS_FALSE);
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

    num_of_calls = NumOfActions(DS_CALL, DS_FALSE);
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
    str.Format("cerr << \"Procedure '%s': "
               "Invalid procedure state.\" << std::endl;",
               GetName()->GetString());
    InsertString(c_file, str, 8, CG_WITH_EOL); 
    InsertString(c_file, "break;", 8, CG_WITH_EOL);
    
    InsertString(c_file, "}", 4, CG_WITH_EOL); 
  }
  
  /********* switch-Scopes **************************************************/
  
  InsertString(c_file, "}", 2, CG_WITH_EOL);
  
  /********* Ende des while (true)-Scopes ***************************/

  InsertString(c_file, "}", 0, CG_WITH_EOL);
  
  /********* Ende des Prozedur-Scopes ****************************************/

  writer->CGSetIndentPos(0);
  InsertLine(c_file);  
  InsertHead(c_file, "Save-Methode zur Speicherung des Prozedurzustands", 0);
  result = WriteSave(writer);
  if (result != DS_OK) return result;

  InsertLine(c_file);  
  InsertHead(c_file, "Restore-Methode zur Restaurierung des Prozedurzustands", 0);
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

  if (GetSensorList())
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
 
DSResult CGProcedure::WriteEnum(CGWriter *writer) const
{  
  DSResult result;
  DSProcessRef process;
  DSString last_id(writer->CGGetLastProcedureID());
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  process = GetThisProcess();
  assert(process);

  
  if (process->GetProcedureList()->AtStartOfList())
  {
    InsertString(*out, "enum", pos, CG_WITH_EOL);
    InsertString(*out, "{", pos, CG_WITH_EOL);
  }


  InsertString(*out, PREFIX_PROCEDURE_TYPE_ID, pos + 2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);

  if (process->GetProcedureList()->AtStartOfList())
  {
    InsertString(*out, " = kSCProcedureTypeIDBase + ", 0, CG_NO_EOL);
    InsertString(*out,
                 &last_id,
                 0, CG_NO_EOL);
  }
  writer->CGSetLastProcedureID(writer->CGGetLastProcedureID() + 1);

  if (!process->GetProcedureList()->AtEndOfList())
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

DSResult CGProcedure::WriteParamDefaultConstructor(CGWriter *writer) const
{ 
  DSResult     result;
  DSFormalParam  *formal_param;
  DSSortRef    sort;
  CGFile      *out;
  CGPos        pos;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_PROCEDURE_PARAM_TYPE, pos, CG_NO_EOL);
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

DSResult CGProcedure::WriteParamCopyConstructor(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_PROCEDURE_PARAM_TYPE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(const ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "& orig)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "*this = orig;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcedure::WriteParamClone(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCDataType *Clone(void) const", pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return new ", pos + 2, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(*this);", 0, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcedure::WriteParamEqual(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean Equal(const SCDataType &second) const",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return (*this == *(", pos + 2, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " *)&second);", 0, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcedure::WriteParamCompare(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSFormalParam  *formal_param;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean operator== (const ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_PARAM_TYPE, 0, CG_NO_EOL);
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

DSResult CGProcedure::WriteParamAssign(CGWriter *writer) const
{ 
  DSResult        result;
  CGFile         *out;
  CGPos           pos;
  DSFormalParam  *formal_param;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_PROCEDURE_PARAM_TYPE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "& operator= (const ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_PARAM_TYPE, 0, CG_NO_EOL);
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

DSResult CGProcedure::WriteParamDisplay(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCStream& Display(SCStream &out) const", pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  InsertString(*out, "return (out << *this);", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGProcedure::WriteParamStreamOperator(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSFormalParam  *formal_param;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "friend SCStream& operator<<(SCStream &out, const ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_PARAM_TYPE, pos, CG_NO_EOL);
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

DSResult CGProcedure::WriteParamSave(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSFormalParam  *formal_param;
  DSSortRef    sort;
  DSString     str;
  
  if (!GetIsComplex())
    return DS_OK;

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

DSResult CGProcedure::WriteParamRestore(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSFormalParam  *formal_param;
  DSSortRef    sort;
  DSString     str;

  if (!GetIsComplex())
    return DS_OK;

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
 
DSResult CGProcedure::WriteParamStruct(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  if (GetFormalParamList()) 
  {
    InsertString(*out, "struct ", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_PROCEDURE_PARAM_TYPE, 0, CG_NO_EOL);
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

    InsertString(*out, "// Formale Prozedur-Parameter:", pos + 2, CG_WITH_EOL);
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
 
DSResult CGProcedure::WriteParam(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (GetFormalParamList()) 
  {
    InsertString(*out, PREFIX_PROCEDURE_PARAM_TYPE, pos, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, " *", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_PROCEDURE_PARAM_VARIABLE, 0, CG_NO_EOL);
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
 
DSResult CGProcedure::WriteTypeHeader(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;
  CGPos   pos;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "class ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ": public SCProcedureType", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "public:", pos + 2, CG_WITH_EOL);
  InsertString(*out, PREFIX_PROCEDURE_TYPE, pos + 4, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out,
               "(SCNatural type_key, const char *type_name, const SCObject *father) :",
               0, CG_WITH_EOL);
  InsertString(*out, "SCProcedureType(type_key, type_name, father)",
               pos + 6, CG_WITH_EOL);
  InsertString(*out, "{", pos + 4, CG_WITH_EOL);
  InsertString(*out, "}", pos + 4, CG_WITH_EOL);
  InsertString(*out, "SCProcedure *NewInstance(SCMem &save_area) const;", pos + 4, CG_WITH_EOL);
  InsertString(*out, "};", pos, CG_WITH_EOL);

  InsertString(*out, "static ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);        
  InsertString(*out, " * ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_TYPE_VARIABLE, 0, CG_NO_EOL);
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
 
DSResult CGProcedure::WriteInclude(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;

  if (!GetIsComplex())
    return DS_OK;

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
 
DSResult CGProcedure::WriteTypeImplementation(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;
  DSObject *father;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);

  father = GetParent();
  assert(father);
  
  father->Write(writer, CG_WRITE_IDENTIFIER);
  if (father->GetType() == DS_PROCESS)
  {
    InsertString(*out, "_data", 0, CG_NO_EOL);
  }
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " * ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  father->Write(writer, CG_WRITE_IDENTIFIER);
  if (father->GetType() == DS_PROCESS)
  {
    InsertString(*out, "_data", 0, CG_NO_EOL);
  }
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " = NULL;", 0, CG_WITH_EOL);

  InsertLine(*out);

  InsertString(*out, "SCProcedure *", 0, CG_NO_EOL);
  father->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_TYPE, 0, CG_NO_EOL);
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
 * WriteTypeInit(): Anlegen der Prozedurtyp-Objekte
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/

DSResult CGProcedure::WriteTypeInit(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  DSProcessRef process;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);

  InsertString(*out, PREFIX_PROCEDURE_TYPE_VARIABLE, 2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " = new ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_TYPE_ID, 0, CG_NO_EOL); 
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ", ", 0, CG_NO_EOL);
  InsertString(*out, "\"", 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "\", ", 0, CG_NO_EOL);
  
  // Prozesstyp als Vaterobjekt fuer den
  // Prozeduretyp setzen (wichtig fuer
  // SCStateType):
  
  process = GetThisProcess();
  if (process)
  {  
    InsertString(*out, PREFIX_PROCESS_TYPE_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, process->GetName(), 0, CG_NO_EOL);
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

DSResult CGProcedure::WriteTypeFinish(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "delete ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCEDURE_TYPE_VARIABLE, 0, CG_NO_EOL);
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

DSResult CGProcedure::WriteInitialize(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);

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

DSResult CGProcedure::WriteFinish(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);

  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Finish();", 0, CG_WITH_EOL);

  return DS_OK;
}


/****************************************************************************
 * GetIdentifier(): Liefert den vollen Namen des Systems (mit Prefix)
 *                      Ergebnis: Referenz auf DSString-Objekt
 *                      Seiteneffekte: keine
 ****************************************************************************/

DSString & CGProcedure::GetIdentifier(void) const
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
      case DS_PROCESS:
        *full_name = *((DSProcess *)father)->GetName() + "_" + *full_name;
        break;
      case DS_PROCEDURE:
        *full_name = *((DSProcedure *)father)->GetName() + "_" + *full_name;
        break;
    default:
      std::cerr << "Unexpected type of parent object for CGBlock !" << std::endl;
      assert(DS_FALSE);
    }
    father = father->GetParent();
  }
  
  *full_name = PREFIX_PROCEDURE + *full_name;

  assert(GetName());    
  *full_name += *GetName();
  
  return *full_name;    
}


/****************************************************************************
 * WriteFullIdentifier(): Schreiben des vollen System-Namens (mit Prefix)
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcedure::WriteIdentifier(CGWriter *writer) const
{  
  DSString full_name;
  DSResult result;
  CGFile *out;
  CGPos pos;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  full_name = GetIdentifier();
  InsertString(*out, full_name, pos, CG_NO_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteSave(): Speicherung aller Relevanten Prozedurdaten
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGProcedure::WriteSave(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean ", pos, CG_NO_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Save(SCMem &save_area) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertHead(*out, "Speicherung der Daten der Basisklasse", pos + 2);
  InsertString(*out, "SCProcedure::Save(save_area);", pos + 2, CG_WITH_EOL);
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
 
DSResult CGProcedure::WriteRestore(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean ", pos, CG_NO_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Restore(SCMem &save_area)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertHead(*out, "Restaurierung der Daten der Basisklasse", pos + 2);
  InsertString(*out, "SCProcedure::Restore(save_area);", pos + 2, CG_WITH_EOL);
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
 
DSResult CGProcedure::WriteSize(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "void ", pos, CG_NO_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Size(SCSize *cur_size) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertHead(*out, "Berechnung der Groesse der Basisklasse", pos + 2);
  InsertString(*out, "SCProcedure::Size(cur_size);", pos + 2, CG_WITH_EOL);
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
  InsertString(*out, "}", pos, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);

  return DS_OK;
}

DSResult CGProcedure::WriteDisplay(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  if (!GetIsComplex())
    return DS_OK;

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

DSResult CGProcedure::WriteStreamOperator(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSString str;

  if (!GetIsComplex())
    return DS_OK;

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

DSResult CGProcedure::WriteSensorUpdate(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSCardinal   i;
  DSSystemRef  system;
  DSBlockRef   block;
  DSString     str;
  
  if (!GetIsComplex())
    return DS_OK;

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
 
DSResult CGProcedure::WriteInitializeImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  DSString str;
  DSCardinal transition_id;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);

  InsertString(*out, "SCBoolean ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Initialize(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);
  str.Format("SCTransition *%stransition;", PREFIX_VARIABLE);
  InsertString(*out, str, 2, CG_WITH_EOL);
  InsertString(*out, "SCBoolean result = true;", 2, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "if (SCScheduler::IsVerbose())", 2, CG_WITH_EOL);
  InsertString(*out, "{", 2, CG_WITH_EOL);
  InsertString(*out, "SCScheduler::outputStream << \"Initializing procedure ", 4, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "...\" << std::endl;", 0, CG_WITH_EOL);
  InsertString(*out, "}", 2, CG_WITH_EOL);
  InsertLine(*out);

  if (GetSortList())
  {
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SORT, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }

  InsertHead(*out, "Initialisierung aller Zustaende des Prozedurtyps:", 2);

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
  }
  InsertLine(*out);

  InsertHead(*out, "Initialisierung aller Transitionen des Prozedurtyps:", 2);

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
 
DSResult CGProcedure::WriteFinishImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  DSString str;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);

  InsertString(*out, "void ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, "Finish(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", 0, CG_WITH_EOL);

  InsertHead(*out,
             "Zustandstypen der Prozedur-Zustaende zerstoeren:", 2);

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

  InsertString(*out, "}", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteFriend(): Schreiben der Friend-Deklarationen
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/

DSResult CGProcedure::WriteFriend(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;

  if (!GetIsComplex())
    return DS_OK;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "friend class ", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}

