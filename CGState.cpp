/****************************************************************************
 *                 Modul: $RCSfile: CGState.cpp,v $
 *
 * $Author: md $
 * $Date: 1997/10/30 09:56:14 $
 * $Revision: 1.2 $
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
#include <DS/DSProcess.h>
#include <DS/DSSignal.h>
#include <DS/DSInput.h>
#include <DS/DSSave.h>
#include <DS/DSSignalItemList.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGState.h"
#include "CGProcedure.h"

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

CGState::CGState(DSObject        *father,
                 DSStringKeyList *n,
                 DSBoolean inter_state) :
  DSState(father, n, inter_state)
{
}

CGState::CGState(DSObject *father,
                 DSString *n,
                 DSBoolean inter_state) :
  DSState(father, n, inter_state)
{
}

CGState::CGState(DSObject *father,
                 DSString &n,
                 DSBoolean inter_state) :
  DSState(father, n, inter_state)
{
}

CGState::CGState(DSObject *father,
                 const char *n,
                 DSBoolean inter_state) :
  DSState(father, n, inter_state)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGState::~CGState(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGState::New(DSObject *father) const
{
  return new CGState(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung, schreibt alle zu einem
 *                    State gehoerenden Elemente raus
 *             -> writer
 *             -> what
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst enthaelt
 *             CG_RESULT den aufgetretenen Fehler
 *             Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/

DSResult CGState::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  if (GetIsAsterisk() && what != CG_WRITE_ENUM)
  {
    return DS_OK; // fuer 'State *' wird grundsaetzlich kein Code
                  // erzeugt, da deren Transitionen in speziellen
                  // Listen der anderen Zustaende eingefuegt sind.
                  // (siehe DSStateComponent.h und DSState.h)
  }

  switch(what)
  {
    case CG_WRITE_ENUM:
      result = WriteTypeEnum((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_INPUT_SET_H:
    case CG_WRITE_INPUT_SET_CC:
      result = WriteInputList((CGWriter *)writer, what);
      if (result != DS_OK) return DS_ERROR;
      break;

    case CG_WRITE_SAVE_SET_H:
      result = WriteSaveSetHeader((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_SAVE_SET_CC:
      result = WriteSaveSetImplementation((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_SET_ENABLING_CONDITION:
      result = WriteSetEnablingConditions((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_CC:
      result = WriteImplementation((CGWriter *)writer);
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
      
    case CG_WRITE_TRANSITION_CC:
      result = WriteTransitionImplementation((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
 
    case CG_WRITE_TRANSITION_INIT:
      result = WriteTransitionList((CGWriter *)writer, what);
      if (result != DS_OK) return DS_ERROR;
      break;
         
    default:
      assert (DS_FALSE);
      break;
      
  }
  return DS_OK;
}

/****************************************************************************
 * WriteSaveSetHeader(): schreibt Input-Signale einer SDL-Transition in
 *                  einen Array
 *                  -> writer
 *                  Ergebnis: DS_OK,falls Aktion erfolgreich war, 
 *                               sonst DS_ERROR
 *                  Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGState::WriteSaveSetHeader(CGWriter *writer) const
{
  DSResult         result;
  DSString        *state_name;
  DSString         str;
  CGFile          *out;
  CGPos            pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (GetSaveType() != DS_SAVE_SIGNALS || GetIsAsterisk())
  {
    return DS_OK;
  }

  state_name = GetFirstName();
  assert(state_name);

  if (GetSaveList())
  {
    str.Format("static const %s %s", CG_NAME_SCL_SIGNAL_ID, PREFIX_SAVE_SET);
    InsertString(*out, str, pos, CG_NO_EOL);
    InsertString(*out, state_name, 0, 
                 CG_NO_EOL);
    InsertString(*out, "[];", 0, CG_WITH_EOL);
  }
  return DS_OK;
}

/****************************************************************************
 * WriteSaveSetImplementation(): generiert fuer die Save-Signale eines SDL-
 *                 Zustandes einen const-Array
 *                 -> writer
 *                 Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                           DS_ERROR
 *                 Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGState::WriteSaveSetImplementation(CGWriter *writer) const
{
  DSResult          result;
  DSSave           *save;
  DSSignalRef       save_signal;
  DSCardinal        i = 0;
  DSString         *state_name;
  DSSignalItemList *siglist;
  DSString          str;
  DSObject         *father;
  CGFile           *out;
  CGPos             pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (GetSaveType() != DS_SAVE_SIGNALS || GetIsAsterisk())
  {
    return DS_OK;
  }
  state_name = GetFirstName();
  i = 0;
  assert(GetSaveList());


  // Scope feststellen:
  father = GetParent();
  assert(father);

  for (save = GetFirstSave();
       save;
       save = GetNextSave())
  {
    siglist = save->GetSignalList();
    assert(siglist);
    for (save_signal = siglist->GetFirstSignalRef(); 
         save_signal;
         save_signal = siglist->GetNextSignalRef())
    { 
      if (i > 0)
      {
        InsertString(*out, ", ", 0, CG_WITH_EOL);
      }
      else // i == 0
      {
        str.Format("const %s ", CG_NAME_SCL_SIGNAL_ID);
        InsertString(*out, str, pos, CG_NO_EOL);
        switch(father->GetType())
        {
          case DS_PROCESS:
            writer->CGSetIndentPos(0);
            result = ((DSProcess *)father)->Write(writer, CG_WRITE_IDENTIFIER);
            writer->CGSetIndentPos(pos);
            if (result != DS_OK) return result;
            InsertString(*out, "::", 0, CG_NO_EOL);
            break;
          case DS_PROCEDURE:  // bei Prozeduren ist kein Prefix notwendig!
            break;
          default:
            assert(DS_FALSE);
            break;
        }
        InsertString(*out, PREFIX_SAVE_SET, 0, CG_NO_EOL);
        InsertString(*out, GetFirstName(), 0, CG_NO_EOL);
        InsertString(*out, "[] =", 0, CG_WITH_EOL)
        InsertString(*out, "{", pos, CG_WITH_EOL)
      }
      InsertString(*out, PREFIX_SIGNAL_TYPE_ID, pos + 2, CG_NO_EOL);
      InsertString(*out, save_signal->GetName(), 0, CG_NO_EOL);

      i++;
    }
  }
  if (i > 0)
  {
    InsertLine(*out);
    InsertString(*out, "};", pos, CG_WITH_EOL);
  }

  return DS_OK;
}

/****************************************************************************
 * WriteSetEnablingConditions(): generiert fuer Enabling-Conditions den
 *                               SCL-Set-Befehl
 *                               -> writer
 *                               Ergebnis: DS_OK,falls Aktion erfolgreich war,
 *                                         sonst DS_ERROR
 *                               Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGState::WriteSetEnablingConditions(CGWriter *writer) const
{
  DSCardinal num;
  DSResult result;
  DSInput *input;
  CGPos pos;
  CGFile *out;
  DSString str;
  DSCardinal counter = 0;
  
  pos = writer->CGGetIndentPos();
  out = writer->CGGetFilePointer();
  
  num = HasEnablingConditions() + HasContinuousSignals();
    
  if (num > 0) // Enabling Conditions vorhanden
  {
    str.Format("SetEnablingConditions(%s%s, %i, ",
               PREFIX_STATE_TYPE_VARIABLE,
               GetFirstName()->GetString(), num);
    InsertString(*out, str, pos, CG_NO_EOL);
    num = 0;
    
    for (input = GetFirstInput();
         input;
         input = GetNextInput() )
    {
      if (input->GetEnablingCondition())
      {
        if (counter) InsertString(*out, ", ", 0, CG_NO_EOL);
        str.Format("kSCTransitionIDBase + %i, ", num);
        InsertString(*out, str, 0, CG_NO_EOL);
        writer->CGSetIndentPos(0);
        input->GetEnablingCondition()->Write(writer);
        counter++;
      }
      num++;
    }
    InsertString(*out, ");", 0, CG_WITH_EOL);
  }

  return DS_OK;
}

/****************************************************************************
 * WriteClearEnablingConditions(): generiert fuer Enabling-Conditions den
 *                                SCL-Clear-Befehl
 *                                -> writer
 *                                Ergebnis: DS_OK,falls Aktion erfolgreich war,
 *                                          sonst DS_ERROR
 *                                Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGState::WriteClearEnablingConditions(CGWriter *writer) const
{
  DSResult result;
  CGPos pos;
  CGFile *out;
  
  pos = writer->CGGetIndentPos();
  out = writer->CGGetFilePointer();
  
  if (HasEnablingConditions() ||
      HasContinuousSignals()) // Enabling Conditions vorhanden?
  {
    InsertString(*out, "ClearEnablingConditions();", pos, CG_WITH_EOL);
  }

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen Zustand eines Prozesses oder einer
 *                        Prozedur
 *                        -> writer
 *                        Ergebnis: DS_OK, falls alles geklappt hat, sonst
 *                                  DS_ERROR
 *                        Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGState::WriteImplementation(CGWriter *writer) const
{
  DSResult  result;
  DSString  str;
  DSObject *father = NULL;
  DSString *state_name;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  state_name = GetFirstName();
  assert(state_name);

  if (writer->CGVerbose())
  {
    writer->cgCout << "    State " << state_name << "...";
  }
  father = GetParent();
  assert(father);

  str.Format("Zustand '%s':", state_name->GetString());
  InsertHead(*out, str, pos);
  InsertString(*out, "case ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_ID, 0, CG_NO_EOL);
  InsertString(*out, GetFirstName(), 0, CG_NO_EOL);
  InsertString(*out, ":", 0, CG_WITH_EOL);
  str.Format("if (%stransition_id == kSCNoTransition)", PREFIX_VARIABLE);
  InsertString(*out, str, pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  writer->CGSetIndentPos(pos + 4);
  result = WriteSetEnablingConditions(writer);
  if (result != DS_OK) return result;
  writer->CGSetIndentPos(pos);
  str.Format("%stransition_id = State(%s%s",
             PREFIX_VARIABLE,
             PREFIX_STATE_TYPE_VARIABLE, state_name->GetString());
  InsertString(*out, str, pos + 4, CG_NO_EOL);
  if (GetAwakeTime())
  {
    InsertString(*out, ", ", 0, CG_NO_EOL);
     
    writer->CGSetIndentPos(0);
    result = GetAwakeTime()->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
  }
  else
  {
    InsertString(*out, ", kSCNoAwakeDelay", 0, CG_NO_EOL); 
  }
  str.Format(", &%ssignal_read);", PREFIX_VARIABLE);
  InsertString(*out, str, 0, CG_WITH_EOL);

  // Test ob Backtracking stattgefunden hat: (Validation)
  str.Format("if (%scur_state != ", PREFIX_VARIABLE);
  InsertString(*out, str, pos + 4, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_ID, 0, CG_NO_EOL);
  InsertString(*out, GetFirstName(), 0, CG_NO_EOL);
  InsertString(*out, ")", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos + 4, CG_WITH_EOL);
  InsertString(*out, "goto backtracking;", pos + 6, CG_WITH_EOL);
  InsertString(*out, "}", pos + 4, CG_WITH_EOL);

  InsertString(*out, "}", pos + 2, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);

  writer->CGSetIndentPos(pos + 2);
  result = WriteClearEnablingConditions(writer);
  if (result != DS_OK) return result;

  InsertLine(*out);
  
  // 1. Switch: RetrieveInputData()
  if (HasInputData())
  {
    InsertHead(*out, "Signalparamter des konsumierten Signals lesen:", pos + 2);
    writer->CGSetIndentPos(pos + 2);
    result = WriteInputImplementation(writer);
    if (result != DS_OK) return result;
    writer->CGSetIndentPos(pos);
    InsertLine(*out);
  }

  // 2. Switch: Transitionen
  str.Format("Transitionen des Zustands '%s'", state_name->GetString());
  InsertHead(*out, str, pos + 2);
  writer->CGSetIndentPos(pos + 2);
  result = WriteTransitionImplementation(writer);
  if (result != DS_OK) return result;
  writer->CGSetIndentPos(pos);

  InsertString(*out, "break;", pos + 2, CG_WITH_EOL);
  InsertLine(*out);

  if (writer->CGVerbose())
  {
    writer->cgCout << "done." << std::endl;
  }
 
  return DS_OK;
}   

/****************************************************************************
 * WriteTypeEnum(): Schreiben der State-IDs als Aufzaehlung
 *                  -> writer
 *                  Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                  Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGState::WriteTypeEnum(CGWriter *writer) const
{
  DSResult result;
  DSString *state_name;
  DSBoolean first = DS_FALSE, last = DS_FALSE;
  CGFile *out;
  CGPos   pos;
  DSObject *father;
  DSCardinal last_state_id;
  DSCardinal num_of_requests = 0;
  DSCardinal num_of_calls = 0;
  DSCardinal i;

  state_name = GetFirstName();

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  last_state_id = writer->CGGetLastStateID();

  father = GetParent();

  switch(father->GetType())
  {
    case DS_PROCESS:
      if (((DSProcess *)father)->GetStateList()->AtStartOfList())
      {
        first = DS_TRUE;
      }
      if (((DSProcess *)father)->GetStateList()->AtEndOfList())
      {
        last = DS_TRUE;
        num_of_requests = ((DSProcess *)father)->NumOfActions(DS_REQUEST, DS_FALSE);
        num_of_calls = ((DSProcess *)father)->NumOfActions(DS_CALL, DS_FALSE);
      }
      break;
    case DS_PROCEDURE:
      if (((DSProcedure *)father)->GetStateList()->AtStartOfList())
      {
        first = DS_TRUE;
      }
      if (((DSProcedure *)father)->GetStateList()->AtEndOfList())
      {
        last = DS_TRUE;
        num_of_requests = ((DSProcedure *)father)->NumOfActions(DS_REQUEST, DS_FALSE);
        num_of_calls = ((DSProcedure *)father)->NumOfActions(DS_CALL, DS_FALSE);
      }
      break;
    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }

  if (first)
  {
    InsertString(*out, "enum", pos, CG_WITH_EOL);
    InsertString(*out, "{", pos, CG_WITH_EOL);
    InsertString(*out, PREFIX_STATE_TYPE_ID, pos + 2, CG_NO_EOL);
    InsertString(*out, "start = kSCStateTypeIDBase + ", 0, CG_NO_EOL);
    InsertCardinal(*out, last_state_id, 0, CG_NO_EOL);
    last_state_id++;
  }

  if (state_name != NULL)
  {
    InsertString(*out, ",", 0, CG_WITH_EOL);
    InsertString(*out, PREFIX_STATE_TYPE_ID, pos + 2, CG_NO_EOL);
    InsertString(*out, state_name, 0, CG_NO_EOL);
    last_state_id++;
  }

  if (last)
  {
    for (i = 0;
         i < num_of_requests;
         i++)
    {
      InsertString(*out, ",", 0, CG_WITH_EOL);
      InsertString(*out, PREFIX_STATE_TYPE_ID, pos + 2, CG_NO_EOL);
      InsertString(*out, "request_", 0, CG_NO_EOL);
      InsertCardinal(*out, i, 0, CG_NO_EOL);
      last_state_id++;
    }
    for (i = 0;
         i < num_of_calls;
         i++)
    {
      InsertString(*out, ",", 0, CG_WITH_EOL);
      InsertString(*out, PREFIX_STATE_TYPE_ID, pos + 2, CG_NO_EOL);
      InsertString(*out, "call_", 0, CG_NO_EOL);
      InsertCardinal(*out, i, 0, CG_NO_EOL);
      last_state_id++;
    }
    InsertLine(*out);
    InsertString(*out, "};", pos, CG_WITH_EOL);
  }

  writer->CGSetLastStateID(last_state_id);

  return DS_OK;
}

/****************************************************************************
 * WriteTypeHeader(): Schreiben der Statetype-Deklarationen
 *               -> writer
 *               Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *               Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/
 
DSResult CGState::WriteTypeHeader(CGWriter *writer) const
{
  DSString *state_name;
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  state_name = GetFirstName();

  InsertString(*out, "static SCStateType * ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL); 
  InsertString(*out, state_name, 0, CG_NO_EOL); 
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteTypeImplementation(): schreibt Signalinfos in eine Header-Datei
 *                            -> writer
 *                            Ergebnis: DS_OK, falls alles geklappt hat,sonst
 *                                      DS_ERROR
 *                            Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGState::WriteTypeImplementation(CGWriter *writer) const
{
  DSResult   result;
  DSString  *state_name;
  DSObject  *father;
  CGFile    *out;
  CGPos      pos;
  DSString   str;
//  DSSaveType save_result;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  state_name = GetFirstName();
  assert(state_name);

  // Scope feststellen:
  father = GetParent();
  assert(father);

  InsertString(*out, "SCStateType * ", pos, CG_NO_EOL);

  writer->CGSetIndentPos(0);

  switch (father->GetType())
  {                          
                             
    case DS_PROCESS:
      result = ((DSProcess *)father)->Write(writer, CG_WRITE_IDENTIFIER);
      break;
      
    case DS_PROCEDURE:
      result = ((DSProcedure *)father)->Write(writer, CG_WRITE_IDENTIFIER);
      break;
    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }

  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;

  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, state_name, 0, CG_NO_EOL);
  InsertString(*out, " = NULL;", 0, CG_WITH_EOL);

  return DS_OK; 

/*
  // Ab hier kommt nur noch Code fuer Prozeduren:
  InsertString(*out, "static SCStateType * ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, state_name, 0, CG_NO_EOL);
  InsertString(*out, " = new SCStateType(", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_ID, 0, CG_NO_EOL);
  InsertString(*out, state_name, 0, CG_NO_EOL);
  InsertString(*out, ", \"", 0, CG_NO_EOL);
  InsertString(*out, state_name, 0, CG_NO_EOL);
  InsertString(*out, "\"", 0, CG_NO_EOL);
  if (GetSaveList())
  {
    // Testen ob Save's im aktuellen Zustand vorkommen:
    save_result = GetSaveType();

    switch (save_result)
    {
      case DS_SAVE_NONE:
        InsertString(*out, ", 0, NULL, false", 0, CG_NO_EOL);
        break;

      case DS_SAVE_SIGNALS:
        str.Format(", sizeof(%s", PREFIX_SAVE_SET);
        InsertString(*out, str, 0, CG_NO_EOL);
        InsertString(*out, state_name, 0, CG_NO_EOL);
        InsertString(*out, ") / sizeof(SCSignalID), ", 0, CG_NO_EOL);
        InsertString(*out, PREFIX_SAVE_SET, 0, CG_NO_EOL);
        InsertString(*out, state_name, 0, CG_NO_EOL);
        InsertString(*out, ", false", 0, CG_NO_EOL);
        break;

      case DS_SAVE_ALL:
      case 3:
        InsertString(*out, ", 0, NULL, true", 0, CG_NO_EOL);
        break;

      default:
        assert(DS_FALSE);
        break;
    }
  }

  // Testen ob dieser Zustand durch die Linearisierung
  // einer Decision-Kontrollstruktur entstanden ist:
  if (GetIsIntermediate())
  {
    InsertString(*out, ", true", 0, CG_NO_EOL);
  }
  else
  {
    InsertString(*out, ", false", 0, CG_NO_EOL);
  }

  InsertString(*out, ");", 0, CG_WITH_EOL);
*/
}

/****************************************************************************
 * WriteTypeInit(): schreibt Signalinfos in eine Header-Datei
 *                            -> writer
 *                            Ergebnis: DS_OK, falls alles geklappt hat,sonst
 *                                      DS_ERROR
 *                            Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGState::WriteTypeInit(CGWriter *writer) const
{
  DSResult   result;
  DSString  *state_name;
  CGFile    *out;
  CGPos      pos;
  DSString   str;
  DSSaveType save_result;
  DSObject  *father;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  state_name = GetFirstName();
  assert(state_name);

  father = GetParent();
  assert(father);

  InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, pos, CG_NO_EOL);
  InsertString(*out, state_name, 0, CG_NO_EOL);
  InsertString(*out, " = new SCStateType(", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_ID, 0, CG_NO_EOL);
  InsertString(*out, state_name, 0, CG_NO_EOL);
  InsertString(*out, ", \"", 0, CG_NO_EOL);
  InsertString(*out, state_name, 0, CG_NO_EOL);
  InsertString(*out, "\"", 0, CG_NO_EOL);
  if (GetSaveList())
  {
    // Testen ob Save's im aktuellen Zustand vorkommen:
    save_result = GetSaveType();

    switch (save_result)
    {
      case DS_SAVE_NONE:
        InsertString(*out, ", 0, NULL, false", 0, CG_NO_EOL);
        break;

      case DS_SAVE_SIGNALS:
        str.Format(", sizeof(%s", PREFIX_SAVE_SET);
        InsertString(*out, str, 0, CG_NO_EOL);
        InsertString(*out, state_name, 0, CG_NO_EOL);
        InsertString(*out, ") / sizeof(SCSignalID), ", 0, CG_NO_EOL);
        InsertString(*out, PREFIX_SAVE_SET, 0, CG_NO_EOL);
        InsertString(*out, state_name, 0, CG_NO_EOL);
        InsertString(*out, ", false", 0, CG_NO_EOL);
        break;

      case DS_SAVE_ALL:
      case 3:
        InsertString(*out, ", 0, NULL, true", 0, CG_NO_EOL);
        break;

      default:
        assert(DS_FALSE);
        break;
    }
  }
  else
  {
    InsertString(*out, ", 0, NULL, false", 0, CG_NO_EOL);
  }

  // Testen ob dieser Zustand durch die Linearisierung
  // einer Decision-Kontrollstruktur entstanden ist:
  if (GetIsIntermediate())
  {
    InsertString(*out, ", true", 0, CG_NO_EOL);
  }
  else
  {
    InsertString(*out, ", false", 0, CG_NO_EOL);
  }


  InsertString(*out, ", ", 0, CG_NO_EOL);
  switch(father->GetType())
  {
    case DS_PROCESS:
      InsertString(*out, PREFIX_PROCESS_TYPE_VARIABLE, 0, CG_NO_EOL);
      InsertString(*out, ((DSProcess *)father)->GetName(), 0, CG_NO_EOL); 
      break;
    case DS_PROCEDURE:
      InsertString(*out, PREFIX_PROCEDURE_TYPE_VARIABLE, 0, CG_NO_EOL);
      InsertString(*out, ((DSProcedure *)father)->GetName(), 0, CG_NO_EOL); 
      break;
    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }


  InsertString(*out, ");", 0, CG_WITH_EOL);

  return DS_OK; 
}

/****************************************************************************
 * WriteTypeFinish(): schreibt Signalinfos in eine Header-Datei
 *                            -> writer
 *                            Ergebnis: DS_OK, falls alles geklappt hat,sonst
 *                                      DS_ERROR
 *                            Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGState::WriteTypeFinish(CGWriter *writer) const
{
  DSResult   result;
  DSString  *state_name;
  CGFile    *out;
  CGPos      pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  state_name = GetFirstName();
  assert(state_name);

  InsertString(*out, "delete ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, state_name, 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteInputImplementation(): schreibt Transitionen eines Zustands
 *                   -> writer
 *                   Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                   Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGState::WriteInputImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile   *out;
  CGPos     pos;
  DSString  str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  str.Format("if (%ssignal_read)", PREFIX_VARIABLE);
  InsertString(*out, str, pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL); 
  str.Format("switch(%ssignal_read->GetID())", PREFIX_VARIABLE);
  InsertString(*out, str, pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL); 

  // Jetzt werden die einzelnen Faelle fuer alle
  // Input-Signal rausgeschrieben:
  writer->CGSetIndentPos(pos + 4);
  result = Run(writer, DS_INPUT);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteTransitionImplementation(): schreibt Transitionen eines Zustands
 *                   -> writer
 *                   Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                   Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGState::WriteTransitionImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile   *out;
  CGPos     pos;
  DSString   str;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "switch(", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, "transition_id)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  writer->CGSetLastTransitionID(0);
  writer->CGSetIndentPos(pos + 2);
  result = Run(writer, DS_TRANSITION, CG_WRITE_CC);
  writer->CGSetIndentPos(pos);

  InsertString(*out, "case kSCNoTransition:", pos + 2, CG_WITH_EOL); 
  InsertString(*out, "break;", pos + 4, CG_WITH_EOL);
  
  InsertString(*out, "default:", pos + 2, CG_WITH_EOL); 
  str.Format("std::cerr << \"State '%s': "
             "Invalid transition.\" << std::endl;",
             GetFirstName()->GetString());
  InsertString(*out, str, pos + 4, CG_WITH_EOL); 
  InsertString(*out, "break;", pos + 4, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return result;
}

DSResult CGState::WriteTransitionList(CGWriter *writer, DSCardinal what) const
{
  DSResult result;

  writer->CGSetLastTransitionID(0);
  writer->CGSetState(this);
  result = Run(writer, DS_TRANSITION, what);

  return result;
}

DSResult CGState::WriteInputList(CGWriter *writer, DSCardinal what) const
{
  DSResult result;

  writer->CGSetLastTransitionID(0);
  writer->CGSetState(this);
  result = Run(writer, DS_INPUT, what);

  return result;
}

