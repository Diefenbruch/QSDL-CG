/****************************************************************************
 *                 Modul: $RCSfile: CGOperandSynonym.cpp,v $
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

#include <DS/DSSynonym.h>

#include "CGFile.h"
#include "CGOperandSynonym.h"

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

CGOperandSynonym::CGOperandSynonym(DSObject *father,
                                   DSSynonymRef syn_ref) :
  DSOperandSynonym(father, syn_ref)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperandSynonym::~CGOperandSynonym(void)
{
}

DSObject *CGOperandSynonym::New(DSObject *father) const
{
  return new CGOperandSynonym(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer OperandSynonym
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOperandSynonym::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen als Synonym gespeicherten Operanden
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOperandSynonym::WriteImplementation(CGWriter *writer) const
{    
  DSSynonymRef synonym;
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  synonym = GetSynonymRef();
  
  if (GetThisAtom())  // used in atom ?
  {
    synonym->GetParent()->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(*out, "::", 0, CG_NO_EOL);
  }
  InsertString(*out, PREFIX_SYNONYM, pos, CG_NO_EOL);
  InsertString(*out, synonym->GetName(), 0, CG_NO_EOL);

  return DS_OK;
}
