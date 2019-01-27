/****************************************************************************
 *                 Modul: $RCSfile: CGCall.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/29 15:34:28 $
 * $Revision: 1.3 $
 *
 * Aufgabe: Dieses Modul enthaelt die Generierung des C++-Codes fuer 
 *          entsprechende SDL-Aktionen
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
#include "CGCall.h"
#include "CGProcedure.h"
#include "CGVariable.h"
#include "CGExpression.h"

#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Externe Variablen
 ****************************************************************************/

/****************************************************************************
 * Globale Variablen
 ****************************************************************************/

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGCall::CGCall(DSObject *father,
               DSString *label_name,
               DSProcedureRef proc,
               DSActualParamList *act_par_list) :
  DSCall(father, label_name, proc, act_par_list)
{
}

CGCall::CGCall(DSObject *father,
               DSString& label_name,
               DSProcedureRef proc,
               DSActualParamList *act_par_list) :
  DSCall(father, label_name, proc, act_par_list)
{
}

CGCall::CGCall(DSObject *father,
               const char *label_name,
               DSProcedureRef proc,
               DSActualParamList *act_par_list) :
  DSCall(father, label_name, proc, act_par_list)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGCall::~CGCall(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGCall::New(DSObject *father) const
{
  return (DSCall *)new CGCall(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Call
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGCall::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen Prozeduraufruf
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          -> writer
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGCall::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  DSActualParam *actual_param;
  DSFormalParam *formal_param;
  DSProcedureRef procedure;
  DSString str;
  DSBoolean first = DS_TRUE;
  CGFile *out;
  CGPos pos;
  DSObject *father;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  result = WriteLabel(writer);
  if (result != DS_OK) return result;
   
  procedure = GetProcedureRef();
  assert(procedure);

  if (procedure->GetIsComplex())
  {
    if (GetActualParamList())
    {
      InsertString(*out, PREFIX_PROCEDURE_PARAM_VARIABLE, pos, CG_NO_EOL);
      InsertString(*out, procedure->GetName(), 0, CG_NO_EOL);
      InsertString(*out, " = new ", 0, CG_NO_EOL);
      InsertString(*out, PREFIX_PROCEDURE_PARAM_TYPE, 0, CG_NO_EOL);
      InsertString(*out, procedure->GetName(), 0, CG_NO_EOL);
      InsertString(*out, ";", 0, CG_WITH_EOL);
      InsertString(*out, "assert(", pos, CG_NO_EOL);
      InsertString(*out, PREFIX_PROCEDURE_PARAM_VARIABLE, 0, CG_NO_EOL);
      InsertString(*out, procedure->GetName(), 0, CG_NO_EOL);
      InsertString(*out, ");", 0, CG_WITH_EOL);
    }

    for (actual_param = GetFirstActualParam(),
         formal_param = procedure->GetFirstFormalParam(); 
         actual_param && formal_param;
         actual_param = GetNextActualParam(),
         formal_param = procedure->GetNextFormalParam())
    {
      InsertString(*out, PREFIX_PROCEDURE_PARAM_VARIABLE, pos, CG_NO_EOL);     
      InsertString(*out, procedure->GetName(),
                   0, CG_NO_EOL); 
      InsertString(*out, "->", 0, CG_NO_EOL);
      InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
      InsertString(*out, formal_param->GetName(), 0, CG_NO_EOL);
      InsertString(*out, " = ", 0, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      result = actual_param->Write(writer);
      writer->CGSetIndentPos(pos);
      if (result != DS_OK) return result;
      InsertString(*out, ";", 0, CG_WITH_EOL);
    }

    str.Format("%scur_state = %scall_%d;", PREFIX_VARIABLE,
               PREFIX_STATE_TYPE_ID,
               writer->CGGetLastCallID());
    InsertString(*out, str, pos, CG_WITH_EOL);
    InsertString(*out, CG_NAME_SCL_CALL, pos, CG_NO_EOL);
    InsertString(*out, "(", 0, CG_NO_EOL);
    InsertString(*out, "new ", 0, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = procedure->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
    InsertString(*out, "(this, GetOwner(), ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_PROCEDURE_TYPE_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, procedure->GetName(),
                 0, CG_NO_EOL); 
    if (procedure->GetFormalParamList())
    {
      InsertString(*out, ", ", 0, CG_NO_EOL);
      InsertString(*out, PREFIX_PROCEDURE_PARAM_VARIABLE, 0, CG_NO_EOL);
      InsertString(*out, procedure->GetName(),
                   0, CG_NO_EOL);
    }
    InsertString(*out, "));", 0, CG_WITH_EOL);

    //
    // Backtracking-Behandlung:
    //
    
    str.Format("if (%scur_state != %scall_%d)",
               PREFIX_VARIABLE, PREFIX_STATE_TYPE_ID,
               writer->CGGetLastCallID());
    InsertStringComment(*out, str.GetString(), "Backtracked ?", pos);
    InsertString(*out, "{", pos, CG_WITH_EOL);
    str.Format("if (IsInState())",
               PREFIX_VARIABLE, PREFIX_STATE_TYPE_ID);
    InsertStringComment(*out, str.GetString(), "Backtracked to normal state ?", pos + 2);
    InsertString(*out, "{", pos + 2, CG_WITH_EOL);
    str.Format("%stransition_id = Transition(&%ssignal_read);",
               PREFIX_VARIABLE, PREFIX_VARIABLE);
    InsertStringComment(*out, str.GetString(), "Choose next transition !", pos + 4);
    InsertString(*out, "}", pos + 2, CG_WITH_EOL);
    InsertStringComment(*out, "goto backtracking;", "Jump to current state !", pos + 2);
    InsertString(*out, "}", pos, CG_WITH_EOL);
    str.Format("label_call_%d:", writer->CGGetLastCallID());
    InsertString(*out, str, pos, CG_WITH_EOL);
    writer->CGSetLastCallID(writer->CGGetLastCallID() + 1);
  }
  else // triviale Prozedur!
  {
    DSProcedureRef calling_procedure;
    
    calling_procedure = GetThisProcedure();

    if (calling_procedure &&               // befinden wir uns innerhalb
        calling_procedure->GetIsComplex()) // einer komplexen Prozedur?
    {
      if ((DSObject *)calling_procedure != // aber dieaufgerufene Prozedur
          procedure->GetParent())          // ist keine Sub-Prozedur?
      {
        InsertString(*out, "((", pos, CG_NO_EOL);  // Cast schreiben ...
        writer->CGSetIndentPos(0);
        procedure->GetParent()->Write(writer, CG_WRITE_IDENTIFIER);
        writer->CGSetIndentPos(pos);
        InsertString(*out, " *)", 0, CG_NO_EOL);
        InsertString(*out, "GetOwner())->", 0, CG_NO_EOL);
        pos = 0;
      }
    }
    
    InsertString(*out, procedure->GetName(), pos, CG_NO_EOL)
    InsertString(*out, "(", 0, CG_NO_EOL);

    // Eingabeparameter vorbereiten:
    if (GetActualParamList() && procedure->GetFormalParamList())
    {
      for (formal_param = procedure->GetFirstFormalParam(),
           actual_param = GetFirstActualParam();
           formal_param && actual_param;
           formal_param = procedure->GetNextFormalParam(),
           actual_param = GetNextActualParam())
      {
        // Der Rueckgabeparameter gehoert nicht
        // direkt zu den formalen Parametern:
        if (formal_param == procedure->GetReturnParamRef())
        {
          formal_param = procedure->GetNextFormalParam();
        }

        if (!first)
        {
          InsertString(*out, ", ", 0, CG_NO_EOL);
        }
        else
        {
          first = DS_FALSE;
        }
        writer->CGSetIndentPos(0);
        result = actual_param->Write(writer, DS_ALL);
        writer->CGSetIndentPos(pos);
        if (result != DS_OK)
        {
           return result;
        }
      }
    }
    InsertString(*out, ")", 0, CG_NO_EOL);
  
    father = DSCall::GetParent();
    assert(father);
    if (father->GetType() == DS_TRANSITION)  // kein Value Returning Call?
    {
      InsertString(*out, ";", 0, CG_WITH_EOL);
    }
  }

  return DS_OK;
}     

/****************************************************************************
 * WriteLabel(): Schreibt das optionale Label der SDL-Aktion
 *               -> writer
 *               Ergebnis: DS_OK, falls Aktion erfolgreich war, sonst 
 *                         enthaelt DSResult das Ergebnis
 *               Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGCall::WriteLabel(CGWriter *writer) const
{
  DSResult result;
  DSStringKeyList *label_list;
  DSString *new_label;
  CGFile *out;
  CGPos pos;

  if (GetLabel() == NULL) return DS_OK;

  out = writer->CGGetFilePointer();
  assert(out);

  pos = writer->CGGetIndentPos();

  label_list = writer->CGGetLabelList();

  if (label_list == NULL ||  // kann innerhalb einer Starttransition passieren
      !label_list->IsKeyInList(GetLabel()))
  {
    InsertString(*out, PREFIX_LABEL, pos, CG_NO_EOL);
    InsertString(*out, GetLabel(), 0, CG_NO_EOL);
    InsertString(*out, ":", 0, CG_WITH_EOL);
    if (label_list != NULL)
    {
      new_label = new DSString(*GetLabel());
      assert(new_label);
      assert(label_list->Append(new DSString(*new_label), new_label) == DS_OK);
    }
  }
  
  return DS_OK;
}     
