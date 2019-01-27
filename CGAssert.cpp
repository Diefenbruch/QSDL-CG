/****************************************************************************
 *                 Modul: $RCSfile: CGAssert.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.2 $
 *
 * Aufgabe: Dieses Modul enthaelt die Generierung des C++-Codes fuer 
 *          entsprechende QSDL-Aktionen
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
#include "CGAssert.h"
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

CGAssert::CGAssert(DSObject* father,
                   DSString* name,
                   DSExpression * exp) :
  DSAssert(father, name, exp)
{
}
CGAssert::CGAssert(DSObject* father,
                   DSString& name,
                   DSExpression * exp) :
  DSAssert(father, name, exp)
{
}

CGAssert::CGAssert(DSObject* father,
                   const char* name,
                   DSExpression * exp) :
  DSAssert(father, name, exp)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGAssert::~CGAssert(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGAssert::New(DSObject *father) const
{
  return (DSAssert *)new CGAssert(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Assert
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGAssert::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen DSAssert-Typ
 *            -> writer
 *            -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR 
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGAssert::WriteImplementation(CGWriter *writer) const
{
  DSResult       result;
  DSExpression * the_exp;
  CGFile        *out;
  CGPos          pos;
 
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  result = WriteLabel(writer);
  if (result != DS_OK) return result;
  
  assert(GetExpression());
  assert(out);

  the_exp = GetExpression();

  InsertString(*out, "Assert(", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  the_exp->Write(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, ");", 0, CG_WITH_EOL);

  return DS_OK;
}     

/****************************************************************************
 * WriteLabel(): Schreibt das optionale Label der SDL-Aktion
 *               -> writer
 *               Ergebnis: DS_OK, falls Aktion erfolgreich war, sonst 
 *                         enthaelt DSResult das Ergebnis
 *               Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGAssert::WriteLabel(CGWriter *writer) const
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
