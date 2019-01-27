/****************************************************************************
 *                 Modul: $RCSfile: CGVariableAccessIndex.cpp,v $
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

#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGVariableAccessIndex.h"

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

CGVariableAccessIndex::CGVariableAccessIndex(DSObject     *father,
                                             DSExpression *ind) :
  DSVariableAccessIndex(father, ind)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGVariableAccessIndex::~CGVariableAccessIndex(void)
{
}

DSObject *CGVariableAccessIndex::New(DSObject *father) const
{
  return new CGVariableAccessIndex(father);
}

/****************************************************************************
 * Write(): schreibt den Zugriff auf eine SCArray-Variable
 *                   -> writer
 *                   -> what
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGVariableAccessIndex::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteVariableAccessIndex((CGWriter *)writer);

  return result;
}

/****************************************************************************
 * WriteVariableAccessIndex(): schreibt den Zugriff auf eine SCArray-Variable
 *                   -> writer
 *                   -> what
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGVariableAccessIndex::WriteVariableAccessIndex(CGWriter *writer) const
{
  DSResult result;
  DSExpression *index;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  index = GetIndex();
  assert (index);
  InsertString(*out, "[", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = index->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, "]", 0, CG_NO_EOL);

  return DS_OK;
}
