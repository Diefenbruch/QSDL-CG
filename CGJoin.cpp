/****************************************************************************
 *                 Modul: $RCSfile: CGJoin.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.2 $
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
#include "CGJoin.h"

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

CGJoin::CGJoin(DSObject *father,
               DSString* connector,
               DSString *label_name) :
  DSJoin(father, connector, label_name)
{
}

CGJoin::CGJoin(DSObject *father,
               DSString& connector,
               DSString& label_name) :
  DSJoin(father, connector, label_name)
{
}

CGJoin::CGJoin(DSObject *father,
               const char* connector,
               const char *label_name) :
  DSJoin(father, connector, label_name)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGJoin::~CGJoin(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGJoin::New(DSObject *father) const
{
  return (DSJoin *)new CGJoin(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Join
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGJoin::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * Write(): schreibt einen DSJoin-Typ
 *          -> writer
 *          Ergebnis: CG_OK,falls Aktion erfolgreich war, DS_ERROR sonst
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGJoin::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;

  assert(writer);
  out = writer->CGGetFilePointer();
  assert(out);

  pos = writer->CGGetIndentPos();

  result = WriteLabel(writer);
  if (result != DS_OK) return result;
  
  assert(GetConnectorRef());

  InsertString(*out, "goto ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_LABEL, 0, CG_NO_EOL);
  InsertString(*out, GetConnectorRef(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}     

/****************************************************************************
 * WriteLabel(): Schreibt das optionale Label der SDL-Aktion
 *               -> writer
 *               Ergebnis: DS_OK, falls Aktion erfolgreich war, sonst 
 *                         enthaelt DSResult das Ergebnis
 *               Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGJoin::WriteLabel(CGWriter *writer) const
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
