/****************************************************************************
 *                 Modul: $RCSfile: CGTransition.cpp,v $
 *
 * $Author: hirche $
 * $Date: 1997/08/29 13:35:30 $
 * $Revision: 1.1 $
 *
 * Aufgabe: Dieses Modul enthaelt die Umsetzung aller SDL-Befehle innerhalb
 *          einer Transition.
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
#include <DS/DSSave.h>
#include <DS/DSRange.h>
#include <DS/DSSignal.h>
#include <DS/DSTimer.h>
#include <DS/DSInputSignal.h>
#include <DS/DSInputTimerSignal.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGExpression.h"
#include "CGOperandExpression.h"
#include "CGOperandLiteral.h"
#include "CGTransition.h"
#include "CGInput.h"
#include "CGNextState.h"
#include "CGDecision.h"
#include "CGState.h"

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

CGTransition::CGTransition(DSObject     *father,
                           DSActionList *a_list) :
  DSTransition(father, a_list)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGTransition::~CGTransition(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGTransition::New(DSObject *father) const
{
  return new CGTransition(father);
}

/****************************************************************************
 * Write(): schreibt Transitionsliste
 *                   -> writer
 *                   -> what
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGTransition::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
  switch (what)
  {
    case CG_WRITE_TRANSITION_INIT:
      result = WriteInit((CGWriter *)writer);
      break;

    case CG_WRITE_TRANSITION_CC:
    default:
      result = WriteImplementation((CGWriter *)writer);
      break;
  }
  return result;
}


DSResult CGTransition::WriteInit(CGWriter *writer) const
{
  DSResult   result;
  DSState   *state;
  CGFile    *out;
  CGPos      pos;
  DSCardinal transition_id;
  DSInput   *input;
  DSString  *state_name;
  DSString   str;

  input = (DSInput *)GetParent();
  assert(input);

  state = (DSState *)input->GetParent();
  assert(state);
  state_name = state->GetFirstName();
  if (!state_name) // kannn bei 'State *' passieren!
  {
    assert(writer->CGGetState());
    state_name = writer->CGGetState()->GetFirstName();
    assert(state_name);
  }

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_VARIABLE, pos, CG_NO_EOL);
  InsertString(*out, "transition = new SCTransition(kSCTransitionIDBase + ",
               0, CG_NO_EOL);
  transition_id = writer->CGGetLastTransitionID();
  InsertCardinal(*out, transition_id, 0, CG_NO_EOL);
  InsertString(*out, ", ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, state_name, 0, CG_NO_EOL);
  InsertString(*out, ", ", 0, CG_NO_EOL);
  if (input->GetIsSpontanous())  // input none ?
  {
    InsertString(*out, "-1, NULL, kSCPrioInputNone", 0, CG_NO_EOL);
  }
  else if (input->GetEnablingCondition() != NULL && // continuous signal?
           input->GetInputSignalList() == NULL &&
           input->GetInputTimerSignalList() == NULL)
  {
    InsertString(*out, "-1, NULL, ", 0, CG_NO_EOL);
    if (input->GetPriority())
    {
      InsertString(*out, input->GetPriority(), 0, CG_NO_EOL);
    }
    else
    {
      InsertString(*out, "kSCPrioNoPriority", 0, CG_NO_EOL);
    }
  }
  else // normaler (signalkonsumierender) Input!
  {
    InsertString(*out, "sizeof(", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_INPUT_SET, 0, CG_NO_EOL);
    InsertString(*out, state_name, 0, CG_NO_EOL);
    InsertString(*out, "_transition", 0, CG_NO_EOL);
    InsertCardinal(*out, transition_id, 0, CG_NO_EOL);
    InsertString(*out, ") / sizeof(SCSignalID), ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_INPUT_SET, 0, CG_NO_EOL);
    InsertString(*out, state_name, 0, CG_NO_EOL);
    InsertString(*out, "_transition", 0, CG_NO_EOL);
    InsertCardinal(*out, transition_id, 0, CG_NO_EOL);
    if (input->GetHasPriority())
    {
      InsertString(*out, ", kSCPrioPriorityInput", 0, CG_NO_EOL);
    }
    else
    {
      InsertString(*out, ", kSCPrioNormalInput", 0, CG_NO_EOL);
    }
  }
  InsertString(*out, ");", 0, CG_WITH_EOL);
  str.Format("assert(%stransition);", PREFIX_VARIABLE);
  InsertString(*out, str, pos, CG_WITH_EOL);

  writer->CGSetLastTransitionID(transition_id + 1);

  return DS_OK;
}

DSResult CGTransition::WriteImplementation(CGWriter *writer) const
{
  DSResult            result;
  CGFile             *out;
  CGPos               pos;
  DSCardinal          transition_id;
  DSInput            *input;
  DSInputSignal      *input_signal;
  DSInputTimerSignal *input_timer;
  DSString            comment;
  DSAction           *action;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  transition_id = writer->CGGetLastTransitionID();

  if (GetParent()->GetType() == DS_INPUT)
  {
    input = (DSInput *)GetParent();
    assert(input);

    comment = "";

    if (input->GetHasPriority())
    {
      comment += "Priority ";
    }
    if (input->GetIsSpontanous())
    {
      comment += "Input None";
    }
    if (input->GetIsAsterisk())
    {
      comment += "Input *";
    }
    if (input->GetInputSignalList() ||
        input->GetInputTimerSignalList())
    {
      comment += "Input signal(s):";
      for (input_signal = input->GetFirstInputSignal();
           input_signal;
           input_signal = input->GetNextInputSignal())
      {
        comment += " ";
        comment += *input_signal->GetSignalRef()->GetName();
      }
      for (input_timer = input->GetFirstInputTimerSignal();
           input_timer;
           input_timer = input->GetNextInputTimerSignal())
      {
        comment += " ";
        comment += *input_timer->GetTimerRef()->GetName();
      }
    }
    else if (!input->GetIsSpontanous() &&
             !input->GetIsAsterisk())
    {
      comment += "Continuous Signal";
    }
    InsertString(*out, "case kSCTransitionIDBase + ", pos, CG_NO_EOL);
    InsertCardinal(*out, transition_id, 0, CG_NO_EOL);
    InsertStringComment(*out, ":", comment.GetString(), 0);
    writer->CGSetIndentPos(pos + 2);
  }

  assert(GetParent());
  if (GetParent()->GetType() != DS_ACTION) // keine Subtransition eines Decisions ?
  {
    if (action_list->MoveLast() == DS_OK)
    {
      action = action_list->GetCurrentData();
      assert(action);
      assert(action->GetTag() == DS_NEXTSTATE ||
             action->GetTag() == DS_JOIN ||
             action->GetTag() == DS_RETURN ||
             action->GetTag() == DS_STOP ||
             action->GetTag() == DS_DECISION);
    }
    else
    {
      writer->cgCout << "Warning: Empty Transition!" << std::endl;
    }
  }

  result = Run(writer, DS_ACTION);
  if (GetParent()->GetType() == DS_INPUT) // keine Starttransition?
  {
    writer->CGSetIndentPos(pos);
    InsertString(*out, "break;", pos + 2, CG_WITH_EOL);
    InsertLine(*out);
  }

  writer->CGSetLastTransitionID(transition_id + 1);

  return result;
}

DSResult CGTransition::BuildSpontaneousInput(DSState *state1,
                                             DSState *state2,
                                             DSTransition *transition)
{
  DSInput *input;
  DSNextState *nextstate;
  DSActionList *a_list;
  DSAction *action = NULL;

  input = new CGInput(state1,   // parent
                      DS_FALSE, // no priority
                      DS_FALSE, // no asterisk input
                      DS_TRUE); // spontaneous input!
  assert(input);
  input->SetTransition(transition);
  transition->SetParent(input);

  a_list = transition->GetActionList();
  if (a_list)
  {
    a_list->MoveLast();
    action = a_list->GetCurrentData();
    assert(action);
  }

  if (state2 != NULL) // folgen dem Decision weitere Aktionen ?
  {
    // Endet die Transition nicht mit einer Terminator-Aktion?
    if (!a_list ||
        (action->GetTag() != DS_NEXTSTATE &&
         action->GetTag() != DS_RETURN &&
         action->GetTag() != DS_JOIN &&
         action->GetTag() != DS_STOP))
    { // nein ? -> dann Nextstate einfuegen!
      nextstate = new CGNextState(transition, (DSString *)NULL, state2);
      assert(nextstate);
    }
  }

  return DS_OK;
}

DSResult CGTransition::BuildContinuousSignal(DSState *state1,
                                             DSState *state2,
                                             DSTransition *transition,
                                             DSExpression *question,
                                             DSRange *answer)
{
  CGInput *input;
  CGExpression *expression1, *expression2, *expression3;
  CGOperandExpression *operand1, *operand2, *operand3;
  CGOperandLiteral *op_literal;
  CGNextState *nextstate;
  DSActionList *a_list;
  DSAction *action = NULL;

  assert(transition);

  input = new CGInput(state1,   // parent
                      DS_FALSE, // no priority
                      DS_FALSE, // no asterisk input
                      DS_FALSE); // no spontaneous input
  assert(input);
  input->SetTransition(transition);
  transition->SetParent(input);

  a_list = transition->GetActionList();
  if (a_list)
  {
    a_list->MoveLast();
    action = a_list->GetCurrentData();
    assert(action);
  }

  if (!answer) // ELSE-Transition?
  {
    input->SetPriority("0"); // ELSE Transition hat Prioritaet 0

    op_literal = new CGOperandLiteral(NULL, DS_SORT_LITERAL_NAME_TRUE,
                                      GetSortRefByLiteralName(DS_SORT_LITERAL_NAME_TRUE));
    assert(op_literal);

    // Creation + Setting of Enabling Condition:
    expression1 = new CGExpression(input, op_literal);
    assert(expression1);

    if (state2 != NULL) // Decision ist NICHT letzte Aktion ?
    {
      // Endet die Transition nicht mit einer Terminator-Aktion?
      if (!a_list ||
          (action->GetTag() != DS_NEXTSTATE &&
           action->GetTag() != DS_RETURN &&
           action->GetTag() != DS_JOIN &&
           action->GetTag() != DS_STOP))
      { // nein ? -> dann Nextstate einfuegen!
        nextstate = new CGNextState(transition, (DSString *)NULL, state2);
        assert(nextstate);
      }
    }

    return DS_OK;
  }

  input->SetPriority("1"); // Normale Answer-Transition hat Prioritaet 1

  // Zwei Operanden existieren auf jeden Fall (ausser bei ELSE):
   
  operand1 = new CGOperandExpression(NULL, question);
  assert(operand1);

  assert(answer->GetUpperBound());
  operand2 = new CGOperandExpression(NULL, answer->GetUpperBound());
  assert(operand2);
  
  switch (answer->GetRangeOperator())
  {
    case DS_RANGE_OPT_NONE:
      if (answer->GetLowerBound()) // Intervall ? (dann 3 Operanden!)
      {
        operand3 = new CGOperandExpression(NULL, answer->GetLowerBound());
        assert(operand3);
        
        // operand1: Question
        // operand2: Answer (Kein Intervall oder obere Intervallgrenze)
        // operand3: Answer (untere Intervallgrenze)
        // Falls Intervall: operand3 <= operand1 <= operand2
        
        // Teilbedingungen bilden:
        expression1 = new CGExpression(NULL, operand1, DS_OPT_GREATER_EQUAL, operand3);
        assert(expression1);
        expression2 = new CGExpression(NULL, operand1, DS_OPT_LESS_EQUAL, operand2);
        assert(expression2);
        
        // Teilbedingungen zu Operanden machen:
        operand1 = new CGOperandExpression(NULL, expression1);
        assert(operand1);
        operand2 = new CGOperandExpression(NULL, expression2);
        assert(operand2);
       
        // Teilbedingungen mit AND verknuepfen:
        expression3 = new CGExpression(input, operand1, DS_OPT_AND, operand2);
        assert(expression3);
      }
      else
      {
        // Creation + Setting of Enabling Condition:
        expression1 = new CGExpression(input, operand1, DS_OPT_EQUAL, operand2);
      }
      break;
    case DS_RANGE_OPT_EQUAL:
      expression1 = new CGExpression(input, operand1, DS_OPT_EQUAL, operand2);
      break;
    case DS_RANGE_OPT_NOT_EQUAL:
      expression1 = new CGExpression(input, operand1, DS_OPT_NOT_EQUAL, operand2);
      break;
    case DS_RANGE_OPT_GREATER:
      expression1 = new CGExpression(input, operand1, DS_OPT_GREATER, operand2);
      break;
    case DS_RANGE_OPT_GREATER_EQUAL:
      expression1 = new CGExpression(input, operand1, DS_OPT_GREATER_EQUAL, operand2);
      break;
    case DS_RANGE_OPT_LESS:
      expression1 = new CGExpression(input, operand1, DS_OPT_LESS, operand2);
      break;
    case DS_RANGE_OPT_LESS_EQUAL:
      expression1 = new CGExpression(input, operand1, DS_OPT_LESS_EQUAL, operand2);
      break;
   default:
      assert(DS_FALSE);
      return DS_ERROR;
  } // switch Range-Operator

  if (state2 != NULL) // Decision war nicht letzte Aktion der Ursprungstransition?
  {
    // Endet die Transition nicht mit einer Terminator-Aktion?
    if (!a_list ||
        (action->GetTag() != DS_NEXTSTATE &&
         action->GetTag() != DS_RETURN &&
         action->GetTag() != DS_JOIN &&
         action->GetTag() != DS_STOP))
    { // nein ? -> dann Nextstate einfuegen!
      nextstate = new CGNextState(transition, (DSString *)NULL, state2);
      assert(nextstate);
    }
  }

  return DS_OK;
}

DSResult CGTransition::DissolveDecision(DSDecision *decision,
                                        DSObject   *father,
                                        DSString   *state_name,
                                        DSCardinal &counter)
{
  DSResult status;
  DSNextState *nextstate;
  DSState *state1, *state2;
  DSString *new_state_name;
  DSActionList *new_action_list;
  DSSave *save;
  DSInput *input;
  DSTransition *transition;
  DSRange *answer;
  CGExpression *question;

  // Die Aktions-Liste wird zunaechst in zwei Teile zerlegt,
  // sodass die Decison das erste Element der zweiten Liste ist:
  status = action_list->GotoElement(decision);
  new_action_list = action_list->Split();
  new_action_list->MoveFirst();
  decision = (DSDecision *)new_action_list->RemoveElement();
  
  // Es werden ein oder zwei Hilfszustaende benoetigt
  // abhaengig davon ob die Decision das letzte Element
  // der action_list ist.
    
  new_state_name = new DSString;
  assert(new_state_name);
  if (state_name)  // keine Starttransition?
  {
    new_state_name->Format("%s_dec_%d_answ", state_name->GetString(), counter);
  }
  else             // Starttransition!
  {
    new_state_name->Format("start_dec_%d_answ", counter);
  }
  
  state1 = new CGState(father, new_state_name, DS_TRUE);
  assert(state1);

  if (!new_action_list->IsEmpty()) // folgen dem Decision weitere Aktionen ?
  {
    new_state_name = new DSString;
    assert(new_state_name);
    if (state_name)  // keine Starttransition?
    {
      new_state_name->Format("%s_dec_%d_tail", state_name->GetString(), counter);
    }
    else             // Starttransition!
    {
      new_state_name->Format("start_dec_%d_tail", counter);
    }
  
    state2 = new CGState(father, new_state_name, DS_TRUE);
    assert(state2);
  }
  else // urspruengliche Transition endet in dem Decision!
  {
    delete new_action_list;
    new_action_list = NULL;
    state2 = NULL;
  }

  counter++;

  // Ausgangszustand um NextState zu 1. Hilfszustand ergaenzen:

  if (decision->GetLabel())
  {
    nextstate = new CGNextState(this, (DSString *)decision->GetLabel()->Clone(), state1);
  }
  else
  {
    nextstate = new CGNextState(this, (DSString *)NULL, state1);
  }

  // 1. Hilfszustand fuellen:

  save = new DSSave(state1,    // parent
                    DS_TRUE,   // Save *
                    NULL);     // no exceptions
  assert(save);

  question = (CGExpression *)decision->GetQuestion();
  
  for (answer = decision->GetFirstAnswer(),
       transition = decision->GetFirstTransition();
       transition != NULL;                              // answer kann NULL sein!
       answer = decision->GetFirstAnswer(),             // hier NICHT Next verwenden!
       transition = decision->GetFirstTransition())
  {
    if (!question) // decision any?
    {
      BuildSpontaneousInput(state1, state2, transition);
    }
    else           // normales decision!
    {
      // question clonen, damit sie vom Destruktor von DSDecision
      // nicht zerstoert wird:

      BuildContinuousSignal(state1, state2,
                            transition,
                            (DSExpression *)question->Clone(), answer);
      // Answer entfernen, damit sie vom Destruktor von DSDecision
      // nicht zerstoert wird:
      decision->GetAnswerList()->RemoveElement();
    }
    // Transition entfernen, damit sie vom Destruktor von DSDecision
    // nicht zerstoert wird:
    decision->GetTransitionList()->RemoveElement();
  }
  if (decision->GetElseTransition())
  {
    BuildContinuousSignal(state1, state2,
                          (DSTransition *)decision->GetElseTransition()->Clone(),
                          NULL, NULL);
  }

  if (state2 != NULL) // urspruengliche Transition endete NICHT im Decision?
  {
    // 2. Hilfszustand fuellen:

    save = new DSSave(state2,    // parent
                      DS_TRUE,   // Save *
                      NULL);     // no exceptions
    assert(save);

    input = new CGInput(state2,   // parent
                        DS_FALSE, // no priority
                        DS_FALSE, // no asterisk input
                        DS_TRUE); // spontaneous input!
    assert(input);
  
    transition = new CGTransition(input, new_action_list);
    assert(transition);
  }

  return DS_OK;
}
