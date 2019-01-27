/****************************************************************************
 *                 Modul: $RCSfile: CGExpression.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 11:16:52 $
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

#include <DS/DSOperand.h>

#include "CGFile.h"
#include "CGExpression.h"
#include "CGSort.h"

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

CGExpression::CGExpression(DSObject  *father,
                           DSOperand *new_operand1,
                           DSExpressionOperator new_operator,
                           DSOperand *new_operand2,
                           DSSortRef  s_ref) :
  DSExpression(father, new_operand1, new_operator, new_operand2, s_ref)
{
}

CGExpression::CGExpression(CGExpression &orig) :
  DSExpression(orig)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGExpression::~CGExpression(void)
{
}

DSObject *CGExpression::New(DSObject *father) const
{
  return new CGExpression(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Expression
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGExpression::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
 
  (void)what;

  result = WriteImplementation((CGWriter *)writer, what);
  if (result != DS_OK) return result;
 
  return DS_OK;
}
 
/****************************************************************************
 * WriteImplementation(): schreibt einen Ausdruck aus der Datenstruktur
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                    enthaelt DSResult das Ergebnis
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGExpression::WriteImplementation(CGWriter *writer,
                                           DSCardinal what) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  assert(GetParent());

  InsertString(*out, "", pos, CG_NO_EOL);

  if (GetOperand1() &&
      GetOperand2())
  {
    if (GetParent()->GetType() == DS_OPERAND)
    {
      InsertString(*out, "(", 0, CG_NO_EOL);
    }
    writer->CGSetIndentPos(0);
    result = GetOperand1()->Write(writer, what);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    { 
      return result;
    }
    writer->CGSetIndentPos(0);
    result = WriteOperator((CGWriter *)writer, GetOperator());
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    { 
      return result;
    }
    writer->CGSetIndentPos(0);
    result = GetOperand2()->Write(writer, what);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    { 
      return result;
    }
    if (GetParent()->GetType() == DS_OPERAND)
    {
      InsertString(*out, ")", 0, CG_NO_EOL);
    }
  }
  else if (GetOperand1()) // nur ein Operand! => unaerer Operand!
  {
    writer->CGSetIndentPos(0);
    result = WriteOperator((CGWriter *)writer, GetOperator());
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    { 
      return result;
    }
    writer->CGSetIndentPos(0);
    result = GetOperand1()->Write(writer, what);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    { 
      return result;
    }
  }
  else // leerer Ausdruck (z.B. beim Auslassen von Parametern bei
  {    // Output, Create, Call, Input)
    if (GetSortRef() &&                    // existiert ein Neutrales Element ?
        GetSortRef()->GetNeutralElement())
    {
      InsertString(*out, GetSortRef()->GetNeutralElement(), 0, CG_NO_EOL);
    }
    else
    {
      InsertString(*out, "NULL", 0, CG_NO_EOL); // what else can be done?
    }
  }  

  return DS_OK;
}

/****************************************************************************
 * WriteOperator(): schreibt einen Operator aus der Datenstruktur
 *                  -> writer
 *                  -> op: DSExpressionOperator in Datenstruktur
 *                  Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                            enthaelt DSResult das Ergebnis
 *                  Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGExpression::WriteOperator(CGWriter *writer,
                                     DSExpressionOperator optr) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  switch (optr)
  {
    case DS_OPT_OR:
      InsertString(*out, " || ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_XOR:
      InsertString(*out, " | ", pos, CG_NO_EOL);
      break;
      
    case DS_OPT_AND:
      InsertString(*out, " && ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_EQUAL:
      InsertString(*out, " == ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_NOT_EQUAL:
      InsertString(*out, " != ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_GREATER:
      InsertString(*out, " > ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_GREATER_EQUAL:
      InsertString(*out, " >= ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_LESS:
      InsertString(*out, " < ", pos, CG_NO_EOL);
      break;
      
    case DS_OPT_LESS_EQUAL:
      InsertString(*out, " <= ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_IN:
      InsertString(*out, " in ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_ADDITION:
      InsertString(*out, " + ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_SUBTRACTION:
      InsertString(*out, " - ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_CONCATENATION:
      InsertString(*out, " || ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_MULTIPLICATION:
      InsertString(*out, " * ", pos, CG_NO_EOL);
      break;
      
    case DS_OPT_DIVISION:
      InsertString(*out, " / ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_MOD:
      InsertString(*out, " % ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_REM:
      InsertString(*out, " rem ", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_MINUS:
      InsertString(*out, " -", pos, CG_NO_EOL);
      break;
    
    case DS_OPT_NOT:
      InsertString(*out, " !", pos, CG_NO_EOL);
      break;

    case DS_OPT_NONE:
      return DS_OK;
      break;
  
    default:
      assert(DS_FALSE);
      break;
  }

  return DS_OK;
}
