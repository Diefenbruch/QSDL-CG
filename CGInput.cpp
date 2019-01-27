/****************************************************************************
 *                 Modul: $RCSfile: CGInput.cpp,v $
 *
 * $Author: md $
 * $Date: 1997/10/24 13:45:00 $
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

#include "CGFile.h"
#include "CGInput.h"
#include "CGState.h"
#include "CGSignal.h"
#include "CGTimer.h"
#include "CGProcess.h"

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

CGInput::CGInput(DSObject               *father,
                 DSBoolean               prio,
                 DSBoolean               asterisk,
                 DSBoolean               spontanous,
                 DSInputSignalList      *s_list,
                 DSInputTimerSignalList *t_list,
                 DSExpression           *enabling,
                 DSTransition           *trans) :
  DSInput(father, prio, asterisk, spontanous, s_list, t_list, enabling, trans)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGInput::~CGInput(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGInput::New(DSObject *father) const
{
  return new CGInput(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Input
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGInput::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
 
  switch(what)
  {
    case CG_WRITE_INPUT_SET_H:
      result = WriteInputSetHeader((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;

    case CG_WRITE_INPUT_SET_CC:
      result = WriteInputSetImplementation((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    default:
      result = WriteImplementation((CGWriter *)writer);
      if (result != DS_OK) return result;
      break;
  }
 
  return DS_OK;
}
 
/****************************************************************************
 * WriteInputSetHeader(): schreibt Input-Signale einer SDL-Transition in
 *                  einen Array
 *                  -> writer
 *                  Ergebnis: DS_OK,falls Aktion erfolgreich war, 
 *                               sonst DS_ERROR
 *                  Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGInput::WriteInputSetHeader(CGWriter *writer) const
{
  DSResult   result;
  DSString  *state_name;
  DSString   str;
  CGFile    *out;
  CGPos      pos;
  DSCardinal transition_id;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  transition_id = writer->CGGetLastTransitionID();

  state_name = ((DSState *)GetParent())->GetFirstName();
  if (!state_name) // kannn bei 'State *' passieren!
  {
    assert(writer->CGGetState());
    state_name = writer->CGGetState()->GetFirstName();
    assert(state_name);
  }

  if (GetInputSignalList() ||
      GetInputTimerSignalList() ||
      GetIsAsterisk())
  {
    str.Format("static %s %s", CG_NAME_SCL_SIGNAL_ID, PREFIX_INPUT_SET);
    InsertString(*out, str, pos, CG_NO_EOL);
    InsertString(*out, state_name, 0, 
                 CG_NO_EOL);
    str.Format("_transition%d", transition_id);
    InsertString(*out, str, 0, CG_NO_EOL);
    InsertString(*out, "[];", 0, CG_WITH_EOL);
  }

  writer->CGSetLastTransitionID(transition_id + 1);

  return DS_OK;
}

/****************************************************************************
 * WriteInputSetImplementation(): schreibt Input-Signale einer SDL-Transition in
 *                  einen Array
 *                  -> writer
 *                  Ergebnis: DS_OK,falls Aktion erfolgreich war, 
 *                               sonst DS_ERROR
 *                  Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGInput::WriteInputSetImplementation(CGWriter *writer) const
{
  DSResult         result;
  DSCardinal       i = 0;
  DSResult         status;
  DSString        *state_name;
  DSSignalRefList *signal_ref_list;
  DSTimerRefList  *timer_ref_list;
  DSSignalRef      signal_ref;
  DSTimerRef       timer_ref;
  DSString         str;
  CGFile          *out;
  CGPos            pos;
  DSCardinal       transition_id;
  DSProcessRef     process;
  DSState         *state;
  DSObject        *father;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  transition_id = writer->CGGetLastTransitionID();

  state = (DSState *)GetParent();
  assert(state);
  state_name = state->GetFirstName();
  if (!state_name) // kannn bei 'State *' passieren!
  {
    assert(writer->CGGetState());
    state_name = writer->CGGetState()->GetFirstName();
    assert(state_name);
  }

  process = GetThisProcess();
  assert(process);

  // Listen aufbauen:
  signal_ref_list = new DSSignalRefList;
  assert(signal_ref_list);
  timer_ref_list = new DSTimerRefList;
  assert(timer_ref_list);

  if (!GetIsAsterisk())
  {
    result = GetSignalSet(signal_ref_list);
    if (result != DS_OK) return DS_ERROR;
    result = GetTimerSet(timer_ref_list);
    if (result != DS_OK) return DS_ERROR;
  }
  else // input * !
  {
    result = GetAsteriskSet(signal_ref_list, timer_ref_list);
    if (result != DS_OK) return DS_ERROR;
  }

  // Nachdem nun die 2 Namenslisten fuer Signale und
  // Timer erzeugt wurden, werden nun die Input-Sets
  // generiert:

  if (!signal_ref_list->IsEmpty() ||
      !timer_ref_list->IsEmpty())
  {
    i = 0;
    InsertString(*out, CG_NAME_SCL_SIGNAL_ID, pos, CG_NO_EOL);
    InsertString(*out, " ", 0, CG_NO_EOL);

    // Scope feststellen:
    father = state->GetParent();
    assert(father);
    writer->CGSetIndentPos(0);
    result = father->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
    InsertString(*out, "::", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_INPUT_SET, 0, CG_NO_EOL);
    InsertString(*out, state_name, 0, 
                 CG_NO_EOL);
    str.Format("_transition%d", transition_id);
    InsertString(*out, str, 0, CG_NO_EOL);
    InsertString(*out, "[] =", 0, CG_WITH_EOL);
    InsertString(*out, "{", pos, CG_WITH_EOL);
    for (status = signal_ref_list->MoveFirst();
         status == DS_OK;
         status = signal_ref_list->MoveNext())
    {
      if (i > 0)
      {
        InsertString(*out, ", ", 0, CG_WITH_EOL);
      }
      signal_ref = signal_ref_list->GetCurrentData();
      assert(signal_ref);
      InsertString(*out, PREFIX_SIGNAL_TYPE_ID, pos + 2, CG_NO_EOL);
      InsertString(*out, signal_ref->GetName(), 0, CG_NO_EOL);
      i++;
    }
    for (status = timer_ref_list->MoveFirst();
         status == DS_OK;
         status = timer_ref_list->MoveNext())
    {
      if (i > 0)
      {
        InsertString(*out, ", ", 0, CG_WITH_EOL);
      }
      timer_ref = timer_ref_list->GetCurrentData();
      assert(timer_ref);
      InsertString(*out, PREFIX_TIMER_TYPE_ID, pos + 2, CG_NO_EOL);
      InsertString(*out, timer_ref->GetName(), 0, CG_NO_EOL);
      i++;
    }
    InsertLine(*out);
    InsertString(*out, "};", pos, CG_WITH_EOL);
  }
  delete signal_ref_list;
  delete timer_ref_list;

  writer->CGSetLastTransitionID(transition_id + 1);

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen Input
 *                        -> writer
 *                        Ergebnis: DS_OK, falls alles geklappt hat, sonst
 *                        DS_ERROR
 *                        Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/

DSResult CGInput::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  DSState *state;
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  state = (DSState *)GetParent();
  assert(state);

  // Nur die NICHT-spontanen Transitionen
  // generieren;
  if (!GetIsSpontanous())
  {
    // jetzt cases fuer die einzelen input-Signale generieren:
    // 1. Signale
    // 2. Timer

    if (GetInputSignalList())
    {
      result = Run(writer, DS_INPUTSIGNAL);
      if (result != DS_OK) return result;
    }
    if (GetInputTimerSignalList())
    {
      result = Run(writer, DS_INPUTTIMERSIGNAL);
      if (result != DS_OK) return result;
    }
  } // if !spontan

  return DS_OK;
}
