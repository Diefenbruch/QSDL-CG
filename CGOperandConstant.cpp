/****************************************************************************
 *                 Modul: $RCSfile: CGOperandConstant.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
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
#include <DS/DSSynonym.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGOperandConstant.h"

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

CGOperandConstant::CGOperandConstant(DSObject *father,
                                     DSString *cons, DSBoolean num) :
  DSOperandConstant(father, cons, num)
{
}

CGOperandConstant::CGOperandConstant(DSObject *father,
                                     DSString &cons, DSBoolean num) :
  DSOperandConstant(father, cons, num)
{
}

CGOperandConstant::CGOperandConstant(DSObject *father,
                                     const char *cons, DSBoolean num) :
  DSOperandConstant(father, cons, num)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperandConstant::~CGOperandConstant(void)
{
}

DSObject *CGOperandConstant::New(DSObject *father) const
{
  return new CGOperandConstant(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer OperandConstant
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOperandConstant::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen als Konstante gespeicherten Operanden 
 *          -> writer
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                    DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOperandConstant::WriteImplementation(CGWriter *writer) const
{
  DSExpression *expression;
  DSSort *sort;
  DSSynonym *synonym;
  DSString *s_komma;
  DSString str;
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (*GetConstant() == DS_NAME_EXTERNAL)
  {
    expression = (DSExpression*)GetParent();
    assert(expression);
    assert(expression->GetParent()->GetType() == DS_SORT);
    sort = (DSSort*)expression->GetParent();
    assert(sort->GetTag() == DS_SORT_SYNONYM);
    synonym = (DSSynonym*)sort;
    DSString synonym_name(synonym->GetName()->GetString());
    synonym_name.ToUpper();
    sort = (DSSort*)synonym->GetSortRef();
    assert(sort);
    if (*sort->GetName() == DS_SORT_NAME_INTEGER ||
        *sort->GetName() == DS_SORT_NAME_NATURAL)
    {
      str.Format("getenv(\"%s%s\") != NULL ?",
                 PREFIX_QUEST_ENV, synonym_name.GetString());
      InsertString(*out, str, pos, CG_WITH_EOL);
      str.Format("(std::cout << \"External Synonym '%s' evaluates to \" "
                         "<< atoi(getenv(\"%s%s\")) << std::endl, atoi(getenv(\"%s%s\"))) : ",
                 synonym->GetName()->GetString(),
                 PREFIX_QUEST_ENV, synonym_name.GetString(),
                 PREFIX_QUEST_ENV, synonym_name.GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
      str.Format("(std::cerr << \"Environment Variable %s%s for external Synonym"
                 " '%s' not set!\\n\", 0);",
                 PREFIX_QUEST_ENV, synonym_name.GetString(),
                 synonym->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
      str.Format("if (getenv(\"%s%s\") == NULL)",
              PREFIX_QUEST_ENV, synonym_name.GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
      InsertString(*out, "result = false", pos + 4, CG_NO_EOL);
    }
    else if (*sort->GetName() == DS_SORT_NAME_REAL ||
             *sort->GetName() == DS_SORT_NAME_TIME ||
             *sort->GetName() == DS_SORT_NAME_DURATION)
    {
      str.Format("getenv(\"%s%s\") != NULL ?",
              PREFIX_QUEST_ENV, synonym_name.GetString());
      InsertString(*out, str, pos, CG_WITH_EOL);
      str.Format("(std::cout << \"External Synonym '%s' evaluates to \" "
                       "<< atof(getenv(\"%s%s\")) << std::endl, atof(getenv(\"%s%s\"))) : ",
                 synonym->GetName()->GetString(),
                 PREFIX_QUEST_ENV, synonym_name.GetString(),
                 PREFIX_QUEST_ENV, synonym_name.GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
      str.Format("(std::cerr << \"Environment Variable %s%s for external Synonym"
                 " '%s' not set!\\n\", 0.0);",
                 PREFIX_QUEST_ENV, synonym_name.GetString(),
                 synonym->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
      str.Format("if (getenv(\"%s%s\") == NULL)",
              PREFIX_QUEST_ENV, synonym_name.GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
      InsertString(*out, "result = false", pos + 4, CG_NO_EOL);
    }
    else
    {
      std::cerr << "Illegal type for external synonym " << &synonym_name << "!\n";
      return DS_ERROR;
    }
    return DS_OK;
  }
      
  if (*GetConstant() == DS_SORT_OPERATOR_NAME_NOW)
  {
    InsertString(*out, "Now()", pos, CG_NO_EOL);
    return DS_OK;
  }
  if (*GetConstant() == DS_SORT_OPERATOR_NAME_SELF)
  {
    InsertString(*out, "Self()", pos, CG_NO_EOL);
    return DS_OK;
  }
  if (*GetConstant() == DS_SORT_OPERATOR_NAME_OFFSPRING)
  {
    InsertString(*out, "Offspring()", pos, CG_NO_EOL);
    return DS_OK;
  }
  if (*GetConstant() == DS_SORT_OPERATOR_NAME_PARENT)
  {
    InsertString(*out, "Parent()", pos, CG_NO_EOL);
    return DS_OK;
  }
  if (*GetConstant() == DS_SORT_OPERATOR_NAME_SENDER)
  {
    InsertString(*out, "Sender()", pos, CG_NO_EOL);
    return DS_OK;
  }
  s_komma = GetConstant();
  if ((*s_komma)[0] == '\'' &&
      (*s_komma)[s_komma->Length() - 1] == '\'')
  {
    (*s_komma)[0] = '\"';
    (*s_komma)[s_komma->Length() - 1] = '\"';
    InsertString(*out, s_komma, pos, CG_NO_EOL);
    return DS_OK;
  }
    
  InsertString(*out, GetConstant(), 
               pos, CG_NO_EOL);
  return DS_OK;
} 
