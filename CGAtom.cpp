/****************************************************************************
 *                 Modul: $RCSfile: CGAtom.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/13 12:37:52 $
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
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGAtom.h"
#include "CGProcedure.h"
#include "CGSort.h"

#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Externe Atomn
 ****************************************************************************/

/****************************************************************************
 * Globale Atomn
 ****************************************************************************/

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGAtom::CGAtom(DSObject     *father,
               DSString     *n,
               DSExpression *prop) :
  DSAtom(father, n, prop)
{
}

CGAtom::CGAtom(DSObject     *father,
               DSString&     n,
               DSExpression *prop) :
  DSAtom(father, n, prop)
{
}

CGAtom::CGAtom(DSObject     *father,
               const char   *n,
               DSExpression *prop) :
  DSAtom(father, n, prop)
{
}


/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGAtom::~CGAtom(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGAtom::New(DSObject *father) const
{
  return new CGAtom(father);
}

/****************************************************************************
 * Write(): schreibt eine SDL-Atom
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                    DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGAtom::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
  DSObject *father;
  
  father = GetParent();
  assert(father);
  
  switch (what)
  {
    case CG_WRITE_H:
      result = WriteHeader((CGWriter *)writer);
      break;
    case CG_WRITE_CC:
      result = WriteImplementation((CGWriter *)writer);
      break;
    case CG_WRITE_INIT:
      result = WriteInitialize((CGWriter *)writer);
      break;

    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }
  return result;
}

/****************************************************************************
 * WriteHeader(): schreibt Prototyp Atom-Funktion 
 *              -> writer
 *              Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                    Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGAtom::WriteHeader(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;
  DSString  str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  str.Format("SCBoolean %s%s(void);",
             PREFIX_ATOM, GetName()->GetString());
  InsertString(*out, str, pos, CG_WITH_EOL);

  return DS_OK;
}
     
/****************************************************************************
 * WriteImplementation(): schreibt Atom-Funtion
 *                        Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *                        Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGAtom::WriteImplementation(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;
  DSString  str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  assert(GetProposition());
  
  str.Format("SCBoolean %s%s(void)",
             PREFIX_ATOM, GetName()->GetString());
  InsertString(*out, str, pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return ", pos + 2, CG_NO_EOL);
  GetProposition()->Write(writer);
  InsertString(*out, ";", 0 ,CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);
  InsertLine(*out);
  
  return DS_OK;
}

/****************************************************************************
 * WriteInitialize(): schreibt Initialisierung des Atom-Arrays
 *              -> writer
 *              Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                    Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGAtom::WriteInitialize(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;
  DSString  str;
  DSCardinal aid;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  aid = writer->CGGetLastAtomID();
/*
  str.Format("%sname2FuncTable[",
             PREFIX_VARIABLE);
  InsertString(*out, str, pos, CG_NO_EOL);
  
  InsertCardinal(*out, id, 0, CG_NO_EOL);
  InsertString(*out, "].name = \"", 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "\";", 0, CG_WITH_EOL);

  str.Format("%sname2FuncTable[",
             PREFIX_VARIABLE);
  InsertString(*out, str, pos, CG_NO_EOL);
  
  InsertCardinal(*out, id, 0, CG_NO_EOL);
  InsertString(*out, "].f = ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_ATOM, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);
*/

  if (aid != 0)
  {
    InsertString(*out, ",", 0, CG_WITH_EOL);
  }
  InsertString(*out, "{\"", pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "\", ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_ATOM, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "}", 0, CG_NO_EOL);
    
  writer->CGSetLastAtomID(aid + 1);

  return DS_OK;
}
