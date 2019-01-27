/****************************************************************************
 *                 Modul: $RCSfile: CGSynonym.cpp,v $
 *
 * $Author: md $
 * $Date: 1997/11/25 17:43:17 $
 * $Revision: 1.2 $
 *
 * Aufgabe: Dieser Modul schreibt SDL-Datentypen in entsprechende SCL-Typen um
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
#include "CGSynonym.h"
#include "CGSort.h"
#include "CGProcedure.h"

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
 
CGSynonym::CGSynonym(DSObject     *father,
                     DSString     *sort_name,
                     DSExpression *default_ref,
                     DSSortRef     sort_ref) :
  DSSynonym(father, sort_name, default_ref, sort_ref)
{
}
 
CGSynonym::CGSynonym(DSObject     *father,
                     DSString&    sort_name,
                     DSExpression *default_ref,
                     DSSortRef     sort_ref) :
  DSSynonym(father, sort_name, default_ref, sort_ref)
{
}
 
CGSynonym::CGSynonym(DSObject     *father,
                     const char   *sort_name,
                     DSExpression *default_ref,
                     DSSortRef     sort_ref) :
  DSSynonym(father, sort_name, default_ref, sort_ref)
{
}
 
/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGSynonym::~CGSynonym(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGSynonym::New(DSObject *father) const
{
  return new CGSynonym(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer einen Synonym
 *                   -> writer
 *                   -> what
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGSynonym::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
  DSObject *father;
  
  switch(what)
  {
    case CG_WRITE_H:
      father = GetParent();
      assert(father);
      
      if (father->GetType() == DS_PROCEDURE &&
          !((DSProcedure *)father)->GetIsComplex())
      {
        result = WriteImplementation((CGWriter *)writer);
      }
      else
      {
        result = WriteHeader((CGWriter *)writer);
      }
      break;

    case CG_WRITE_CC:
      result = WriteImplementation((CGWriter *)writer);
      break;

    case CG_WRITE_INIT:
      result = WriteInit((CGWriter *)writer);
      break;

    case CG_WRITE_IDENTIFIER:
      result = WriteIdentifier((CGWriter *)writer);
      break;

    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }

  return DS_OK;
}

/****************************************************************************
 * WriteHeader(): schreibt eine SCL-Synonym
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/
 
DSResult CGSynonym::WriteHeader(CGWriter *writer) const
{ 
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSSortRef syn_sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  assert(GetValue());

  if (GetSortRef())
  {   
    InsertString(*out, "static ", pos, CG_NO_EOL);
    syn_sort = GetSortRef();
    writer->CGSetIndentPos(0);
    syn_sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, " ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_SYNONYM, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL); 
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }
  else 
  {
    InsertString(*out, "#define ", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_SYNONYM, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL); 
    InsertString(*out, " ", 0, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = GetValue()->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    { 
      return result;
    }
    InsertLine(*out);
  }
  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt eine SCL-Synonym
 *                        -> writer
 *                        Ergebnis: DS_OK, falls alles geklappt hat, sonst 
 *                                  DS_ERROR
 *                        Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/
 
DSResult CGSynonym::WriteImplementation(CGWriter *writer) const
{ 
  DSResult  result;
  DSObject *father;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  assert(GetValue());

  father = GetParent();

  if (GetSortRef())
  {   
    InsertString(*out, "", pos, CG_NO_EOL);

    writer->CGSetIndentPos(0);
    if (GetSortRef()->GetIsPredefined())
    {
      GetSortRef()->Write(writer, CG_WRITE_IDENTIFIER);
    }
    else
    {
      if (father->GetType() != DS_PROCEDURE ||
          ((DSProcedure *)father)->GetIsComplex())
      {
        father->Write(writer, CG_WRITE_IDENTIFIER);
        InsertString(*out, "::", 0, CG_NO_EOL);
      }
      GetSortRef()->Write(writer, CG_WRITE_IDENTIFIER);
    }
    InsertString(*out, " ", 0, CG_NO_EOL);
    if (father->GetType() != DS_PROCEDURE ||
        ((DSProcedure *)father)->GetIsComplex())
    {
      father->Write(writer, CG_WRITE_IDENTIFIER);
      if (father->GetType() == DS_PROCESS)
      {
        InsertString(*out, "_data", 0, CG_NO_EOL);
      }
      InsertString(*out, "::", 0, CG_NO_EOL);
    }
    InsertString(*out, PREFIX_SYNONYM, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL); 
    writer->CGSetIndentPos(pos);
    if (father->GetType() != DS_PROCEDURE)
    {
      InsertString(*out, ";", 0, CG_WITH_EOL);
      return DS_OK;
    }
    InsertString(*out, " = ", 0, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = GetValue()->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    { 
      return result;
    }
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "#define ", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_SYNONYM, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, " ", 0, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = GetValue()->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    {
      return result;
    }
    InsertLine(*out);
  }
  return DS_OK;
}

/****************************************************************************
 * WriteInit(): schreibt eine SCL-Synonym
 *                        -> writer
 *                        Ergebnis: DS_OK, falls alles geklappt hat, sonst 
 *                                  DS_ERROR
 *                        Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/
 
DSResult CGSynonym::WriteInit(CGWriter *writer) const
{ 
  DSResult  result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  assert(GetValue());

  if (GetSortRef())
  {   
    InsertString(*out, PREFIX_SYNONYM, pos, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL); 
    InsertString(*out, " = ", 0, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = GetValue()->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    { 
      return result;
    }
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }

  return DS_OK;
}

DSResult CGSynonym::WriteIdentifier(CGWriter *writer) const
{ 
  DSResult   result;
  CGFile    *out;
  CGPos      pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  assert (GetName());

  if (GetIsPredefined())
  {
    DSString tmp(*GetName());

    InsertString(*out, "SC", pos, CG_NO_EOL);
    tmp.ToUpper(1);
    InsertString(*out, tmp, 0, CG_NO_EOL);
  }
  else
  {
    InsertString(*out, PREFIX_DATA_TYPE, pos, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
  }

  return DS_OK;
}
