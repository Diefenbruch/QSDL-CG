/****************************************************************************
 *                 Modul: $RCSfile: CGDecision.cpp,v $
 *
 * $Author: hirche $
 * $Date: 1997/08/29 13:35:30 $
 * $Revision: 1.1 $
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
#include <DS/DSRange.h>
#include <DS/DSOperand.h>

#include "CGFile.h"
#include "CGDecision.h"
#include "CGExpression.h"
#include "CGTransition.h"

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

CGDecision::CGDecision(DSObject *father,
                       DSString *label_name,
                       DSExpression *quest,
                       DSRangeList *answers,
                       DSTransitionList *transitions,
                       DSTransition *else_t) :
  DSDecision(father, label_name, quest, answers, transitions, else_t)
{
}

CGDecision::CGDecision(DSObject *father,
                       DSString &label_name,
                       DSExpression *quest,
                       DSRangeList *answers,
                       DSTransitionList *transitions,
                       DSTransition *else_t) :
  DSDecision(father, label_name, quest, answers, transitions, else_t)
{
}

CGDecision::CGDecision(DSObject *father,
                       const char *label_name,
                       DSExpression *quest,
                       DSRangeList *answers,
                       DSTransitionList *transitions,
                       DSTransition *else_t) :
  DSDecision(father, label_name, quest, answers, transitions, else_t)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGDecision::~CGDecision(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGDecision::New(DSObject *father) const
{
  return (DSDecision *)new CGDecision(father);
}

/****************************************************************************
 * WriteCondition(): schreibt eine Bedingung eines Decision
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                   -> writer
 *                   -> question: Bedingung
 *                   -> answer: SDL-Range (offen oder geschlossen)
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGDecision::WriteCondition(CGWriter     *writer,
                                    DSExpression *quest,
                                    DSRange      *answer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;

  out = writer->CGGetFilePointer();
  pos = writer->CGGetIndentPos();

  result = quest->Write(writer);
  if (result != DS_OK) return result;

  switch (answer->GetRangeOperator())
  {
    case DS_RANGE_OPT_NONE: 
      if (answer->GetLowerBound()) // Intervall ?
      {
        InsertString(*out, " >= ", 0, CG_NO_EOL);
        writer->CGSetIndentPos(0);
        result = answer->GetLowerBound()->Write(writer);
        writer->CGSetIndentPos(pos);
        if (result != DS_OK) return result;
        InsertString(*out, " && ", 0, CG_NO_EOL);
        writer->CGSetIndentPos(0);
        result = quest->Write(writer);
        writer->CGSetIndentPos(pos);
        if (result != DS_OK) return result;
        InsertString(*out, " <= ", 0, CG_NO_EOL);
      }
      else
      {
        InsertString(*out, " == ", 0, CG_NO_EOL);
      }
      break;
    case DS_RANGE_OPT_EQUAL:
      InsertString(*out, " == ", 0, CG_NO_EOL);
      break;
    case DS_RANGE_OPT_NOT_EQUAL:
      InsertString(*out, " != ", 0, CG_NO_EOL);
      break;
    case DS_RANGE_OPT_GREATER:
      InsertString(*out, " > ", 0, CG_NO_EOL);
      break;
    case DS_RANGE_OPT_GREATER_EQUAL:
      InsertString(*out, " >= ", 0, CG_NO_EOL);
      break;
    case DS_RANGE_OPT_LESS:
      InsertString(*out, " < ", 0, CG_NO_EOL);
      break;
    case DS_RANGE_OPT_LESS_EQUAL:
      InsertString(*out, " <= ", 0, CG_NO_EOL);
      break;
   default:
      assert(DS_FALSE);
  } // switch Range-Operator

  writer->CGSetIndentPos(0);
  assert(answer->GetUpperBound());
  result = answer->GetUpperBound()->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteIf(): schreibt eine SDL-Decison als C-if
 *            -> writer
 *            Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *            Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGDecision::WriteIf(CGWriter *writer) const
{
  DSResult      result;
  DSExpression *quest;
  DSRange      *answer;
  DSTransition *transition;
  CGFile  *out;
  CGPos    pos;

  out = writer->CGGetFilePointer();
  pos = writer->CGGetIndentPos();

  quest = GetQuestion();
  assert(quest);

  answer = GetFirstAnswer();
  assert(answer);

  transition = GetFirstTransition();
  assert(transition);

  InsertString(*out, "if (", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = WriteCondition(writer, quest, answer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, ")", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  writer->CGSetIndentPos(pos + 2);
  result = transition->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, "}", pos, CG_WITH_EOL);

  transition = GetElseTransition();
  if (transition)
  {
    InsertString(*out, "else", pos, CG_WITH_EOL);
    InsertString(*out, "{", pos, CG_WITH_EOL);
    writer->CGSetIndentPos(pos + 2);
    result = transition->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
    InsertString(*out, "}", pos, CG_WITH_EOL);
  }

  return DS_OK;
}

/****************************************************************************
 * WriteSwitch(): schreibt eine SDL-Decison als C-switch
 *                -> writer
 *                Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                          DS_ERROR
 *                Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGDecision::WriteSwitch(CGWriter *writer) const
{
  DSResult      result;
  DSExpression *quest;
  DSRange      *answer;
  DSTransition *transition;
  CGFile  *out;
  CGPos    pos;

  out = writer->CGGetFilePointer();
  pos = writer->CGGetIndentPos();

  quest = GetQuestion();
  assert(quest);

  InsertString(*out, "switch (", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = quest->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, ")", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  for (answer = GetFirstAnswer(),
       transition = GetFirstTransition();
       answer && transition;
       answer = GetNextAnswer(),
       transition = GetNextTransition())
  {
    InsertString(*out, "case ", pos + 2, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = answer->GetUpperBound()->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
    InsertString(*out, ":", 0, CG_WITH_EOL);
    writer->CGSetIndentPos(pos + 4);
    result = transition->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
    InsertString(*out, "break;", pos + 4, CG_WITH_EOL);
  }
  transition = GetElseTransition();
  if (transition)
  {
    InsertString(*out, "default:", pos + 2, CG_WITH_EOL);
    writer->CGSetIndentPos(pos + 4);
    result = transition->Write(writer);
    if (result != DS_OK) return result;
    writer->CGSetIndentPos(pos);
    InsertString(*out, "break;", pos + 4, CG_WITH_EOL);
  }
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * Write(): Hauptfunktion der Codegenerierung fuer ein Decision-Konstrukt
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGDecision::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult      result;
  DSExpression *quest;
  DSRange      *answer;
  DSOperand    *operand;
  DSExpression *expression;
  DSTransition *transition;
  DSBoolean     use_switch, is_any;
  DSCardinal    i, num_of_answers, num_of_transitions;
  DSString      str;
  CGFile       *out;
  CGPos         pos;

  (void)what;
  
  out = ((CGWriter *)writer)->CGGetFilePointer();
  pos = ((CGWriter *)writer)->CGGetIndentPos();

  result = WriteLabel((CGWriter *)writer);
  if (result != DS_OK) return result;
  
  quest = GetQuestion();
  if (quest == NULL) // Decison ANY ?
  {
    is_any = DS_TRUE;
  }
  else
  {
    is_any = DS_FALSE;
  }

  if (!is_any)
  {
    num_of_answers = GetAnswerList()->NumberOfElements();
  }
  else // Decision ANY !
  {
    num_of_answers = 0;
  }

  num_of_transitions = GetTransitionList()->NumberOfElements();

  if (!is_any && num_of_answers == 1)
  {
    return WriteIf((CGWriter *)writer);
  }

  use_switch = DS_TRUE;

  for (answer = GetFirstAnswer();
       answer;
       answer = GetNextAnswer())
  {
    // Nicht nur UpperBound sondern auch LowerBound
    // gesetzt oder Offener Range-Bereich?
    if (answer->GetLowerBound() != NULL ||
        answer->GetRangeOperator() != DS_RANGE_OPT_NONE)
    {
      use_switch = DS_FALSE;
      break;
    }
    // Ist der UpperBound 'nicht-trivial', d.h.
    // ein zusammengesetzter Ausdruck?
    expression = answer->GetUpperBound();
    if (expression->GetOperand2() != NULL)
    {
      use_switch = DS_FALSE;
      break;
    }
    // Ausdruck ist nicht zusammengesetzt, aber
    // vielleicht 'nicht konstant' ?
    operand = expression->GetOperand1();
    if (operand->GetTag() != DS_OPN_CONSTANT &&
        operand->GetTag() != DS_OPN_LITERAL)
    {
      use_switch = DS_FALSE;
      break;
    }
  }

  if (!is_any && use_switch)
  {
    return WriteSwitch((CGWriter *)writer);
  }

  // Die SDL-Decision muss in eine SCL-Decision umgewandelt werden:
  str.Format("switch (Decision(%d, ", num_of_transitions);
  InsertString(*out, str, pos, CG_NO_EOL);
  if (!is_any)
  {
    i = 1;
    for (answer = GetFirstAnswer();
         answer;
         answer = GetNextAnswer())
    {
      ((CGWriter *)writer)->CGSetIndentPos(0);
      result = WriteCondition((CGWriter *)writer, quest, answer);
      ((CGWriter *)writer)->CGSetIndentPos(pos);
      if (result != DS_OK) return result;
      if (i < num_of_answers)
      {
        InsertString(*out, ", ", 0, CG_NO_EOL);
      }
      i++;
    }
  }
  else  // Decison ANY !
  {
    for (i = 1;
         i <= num_of_transitions;
         i++)
    {
      InsertString(*out, "true", 0, CG_NO_EOL);
      if (i < num_of_transitions)
      {
        InsertString(*out, ", ", 0, CG_NO_EOL);
      }
    }
  }
  InsertString(*out, "))", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  i = 0;
  for (transition = GetFirstTransition();
       transition;
       transition = GetNextTransition())
  {
    str.Format("case kSCDecisionBase + %d:", i);
    InsertString(*out, str, pos + 2, CG_WITH_EOL);
    ((CGWriter *)writer)->CGSetIndentPos(pos + 4);
    result = transition->Write(writer);
    if (result != DS_OK) return result;
    ((CGWriter *)writer)->CGSetIndentPos(pos);
    InsertString(*out, "break;", pos + 4, CG_WITH_EOL);
    i++;
  }
  InsertString(*out, "default:", pos + 2, CG_WITH_EOL);
  transition = GetElseTransition();
  if (transition)
  {
    ((CGWriter *)writer)->CGSetIndentPos(pos + 4);
    result = transition->Write(writer);
    if (result != DS_OK) return result;
    ((CGWriter *)writer)->CGSetIndentPos(pos);
  }
  else 
  {
    InsertString(*out, "std::cerr << \"Not specified decision value!\" << std::endl;",
                 pos + 4, CG_WITH_EOL);
  }
  InsertString(*out, "break;", pos + 4, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}     

/****************************************************************************
 * WriteLabel(): Schreibt das optionale Label der SDL-Aktion
 *               -> writer
 *               Ergebnis: DS_OK, falls Aktion erfolgreich war, sonst 
 *                         enthaelt DSResult das Ergebnis
 *               Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGDecision::WriteLabel(CGWriter *writer) const
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
