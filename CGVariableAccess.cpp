/****************************************************************************
 *                 Modul: $RCSfile: CGVariableAccess.cpp,v $
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

#include <DS/DSVariable.h>
#include <DS/DSVariableAccessField.h>
#include <DS/DSVariableAccessIndex.h>
#include <DS/DSVariableAccessElem.h>
#include <DS/DSNameSort.h>
#include <DS/DSProcedure.h>

#include "CGFile.h"
#include "CGVariableAccess.h"

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

CGVariableAccess::CGVariableAccess(DSObject                 *father,
                                   DSVariableRef             var_ref,
                                   DSVariableAccessElemList *elems) :
  DSVariableAccess(father, var_ref, elems)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGVariableAccess::~CGVariableAccess(void)
{
}
 
DSObject *CGVariableAccess::New(DSObject *father) const
{
  return new CGVariableAccess(father);
}

/****************************************************************************
 * Write(): schreibt den Zugriff auf eine Variable
 *                   -> writer
 *                   -> what
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGVariableAccess::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteVariableAccess((CGWriter *)writer);

  return result;
}

/****************************************************************************
 * Write(): schreibt Zugriff auf eine Variable
 *                   -> writer
 *                   Ergebnis: CG_OK,falls Aktion erfolgreich war, sonst 
 *                             enthaelt DSResult das Ergebnis
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGVariableAccess::WriteVariableAccess(CGWriter *writer) const
{
  DSResult result;
  DSVariableRef var_ref;
  DSProcedureRef procedure_ref;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  var_ref = GetVariableRef();
  assert(var_ref);

  procedure_ref = GetThisProcedure();

  if (procedure_ref &&               // befinden wir uns innerhalb
      procedure_ref->GetIsComplex()) // einer komplexen Proezedur?
  {
    if ((DSObject *)procedure_ref != // aber diese Variable gehoert nicht
        var_ref->GetParent())        // dieser Prozedur?
    {
      InsertString(*out, "((", pos, CG_NO_EOL);  // Cast schreiben ... 
      writer->CGSetIndentPos(0);
      var_ref->GetParent()->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, " *)", 0, CG_NO_EOL);
      InsertString(*out, "GetOwner())->", 0, CG_NO_EOL);
      pos = 0;
    }
  }
  InsertString(*out, PREFIX_VARIABLE, pos, CG_NO_EOL);
  InsertString(*out,
               var_ref->GetName(),
               0, CG_NO_EOL);

  result = Run(writer, DS_VARIABLEACCESSELEM);

  return result;
}
