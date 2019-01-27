/****************************************************************************
 *                 Modul: $RCSfile: CGVariable.cpp,v $
 *
 * $Author: hirche $
 * $Date: 1997/08/29 13:35:30 $
 * $Revision: 1.1 $
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
#include "CGVariable.h"
#include "CGProcedure.h"
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

CGVariable::CGVariable(DSObject     *father,
                       DSString     *n,
                       DSSortRef     s,
                       DSExpression *i,
                       DSBoolean     reveal,
                       DSBoolean     exprt,
                       DSString     *exprt_as) :
  DSVariable(father, n, s, i, reveal, exprt, exprt_as)
{
}

CGVariable::CGVariable(DSObject     *father,
                       DSString&     n,
                       DSSortRef     s,
                       DSExpression *i,
                       DSBoolean     reveal,
                       DSBoolean     exprt,
                       DSString&     exprt_as) :
  DSVariable(father, n, s, i, reveal, exprt, exprt_as)
{
}

CGVariable::CGVariable(DSObject     *father,
                       const char   *n,
                       DSSortRef     s,
                       DSExpression *i,
                       DSBoolean     reveal,
                       DSBoolean     exprt,
                       const char   *exprt_as) :
  DSVariable(father, n, s, i, reveal, exprt, exprt_as)
{
}

CGVariable::CGVariable(DSObject     *father,
                       DSString     *n,
                       DSSortRef     s,
                       DSBoolean     io) :
  DSVariable(father, n, s, io)
{
}

CGVariable::CGVariable(DSObject     *father,
                       DSString&     n,
                       DSSortRef     s,
                       DSBoolean     io) :
  DSVariable(father, n, s, io)
{
}

CGVariable::CGVariable(DSObject     *father,
                       const char   *n,
                       DSSortRef     s,
                       DSBoolean     io) :
  DSVariable(father, n, s, io)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGVariable::~CGVariable(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGVariable::New(DSObject *father) const
{
  return new CGVariable(father);
}

/****************************************************************************
 * Write(): schreibt eine SDL-Variable
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                    DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGVariable::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
  DSObject *father;
  
  father = GetParent();
  assert(father);
  
  switch (what)
  {
    case CG_WRITE_H:
      result = WriteVariable((CGWriter *)writer);
      break;
    case CG_WRITE_INIT:
      result = WriteInit((CGWriter *)writer);
      break;
    case CG_WRITE_FORMAL_PARAM_CC:
      switch(father->GetType())
      {
        case DS_PROCESS:
          result = WriteProcessFormalParamImplementation((CGWriter *)writer);
          break;
        case DS_PROCEDURE:
          result = WriteProcedureFormalParamImplementation((CGWriter *)writer);
          break;
        default:
          assert(DS_FALSE);
          return DS_ERROR;
      }
      break;
    case CG_WRITE_FORMAL_PARAM_H:
      switch(father->GetType())
      {
        case DS_PROCESS:
          result = WriteProcessFormalParamHeader((CGWriter *)writer);
          break;
        case DS_PROCEDURE:
          result = WriteProcedureFormalParamHeader((CGWriter *)writer);
          break;
        default:
          assert(DS_FALSE);
          return DS_ERROR;
      }
      break;
    
    case CG_WRITE_SAVE:
      result = WriteSave((CGWriter *)writer);
      break;
    
    case CG_WRITE_RESTORE:
      result = WriteRestore((CGWriter *)writer);
      break;

    case CG_WRITE_SIZE:
      result = WriteSize((CGWriter *)writer);
      break;

    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }
  return DS_OK;
}

/****************************************************************************
 * WriteVariable(): schreibt Variablen eines Prozesses oder einer Prozedur
 *                  -> writer
 *                  Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                        Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGVariable::WriteVariable(CGWriter *writer) const
{
  DSResult  result;
  DSSortRef sort;
  CGFile   *out;
  CGPos     pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  sort = GetSortRef();
  assert(sort);
  sort->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, " ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);

  // Nur Prozedurvariablen direkt initialisieren,
  // Prozessvariablen werden vom Konstruktor
  // der Prozessklasse initialisiert (wird von
  // ANSI-C++ verlangt):
  if (GetParent()->GetType() != DS_PROCEDURE ||
      ((CGProcedure *)GetParent())->GetIsComplex())
  {
    InsertString(*out, ";", 0, CG_WITH_EOL);
    return DS_OK;
  }

  // Initialisierung vorhanden?
  if (GetInit())
  {
    InsertString(*out, " = ", 0, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = GetInit()->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    {
      return result;
    }
  }
  // Default-Initialisierung fuer Datentyp vorhanden?
  else if (sort->GetDefaultInitialization())
  {
    InsertString(*out, " = ", 0, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = sort->GetDefaultInitialization()->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    {
      return result;
    }
  }
  // Ist ein neutrales Element vorhanden?
  else if (sort->GetNeutralElement())
  {
    InsertString(*out, " = ", 0, CG_NO_EOL);
    InsertString(*out, sort->GetNeutralElement(), 0, CG_NO_EOL);
  }
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}
     
/****************************************************************************
 * WriteInit(): schreibt Variableninitialisierung eines Prozesses
 *              Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *              Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGVariable::WriteInit(CGWriter *writer) const
{
  DSResult  result;
  DSSortRef sort;
  CGFile   *out;
  CGPos     pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (GetInit() == NULL) // keine Initialisierung vorgegeben?
  {
    sort = GetSortRef();
    // Default-Initialisierung fuer den Datentyp vorhanden?
    if (sort->GetDefaultInitialization())
    {
      InsertString(*out, PREFIX_VARIABLE, pos, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      InsertString(*out, " = ", 0, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      result = sort->GetDefaultInitialization()->Write(writer);
      writer->CGSetIndentPos(pos);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertString(*out, ";", 0, CG_WITH_EOL);
    }
    // Ist ein neutrales Element vorhanden?
    else if (sort->GetNeutralElement())
    {
      InsertString(*out, PREFIX_VARIABLE, pos, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      InsertString(*out, " = ", 0, CG_NO_EOL);
      InsertString(*out, sort->GetNeutralElement(), 0, CG_NO_EOL);
      InsertString(*out, ";", 0, CG_WITH_EOL);
    }
    // Ist es ein vordefinierter SDL-Datentyp?
    else if (!sort->GetIsComplex())
    {
      InsertString(*out, "memset(&", pos, CG_NO_EOL);
      InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      InsertString(*out, ", 0, sizeof(", 0, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, "));", 0, CG_WITH_EOL);
    }
  }
  else // Initialisierung angegeben!
  {
    InsertString(*out, PREFIX_VARIABLE, pos, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, " = ", 0, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = GetInit()->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK) return DS_ERROR;
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }
  return DS_OK;
}

/*****************************************************************************
 * WriteFormalParamImplementation(): Initialisierung von formalen Parametern 
 *                   -> writer
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGVariable::WriteProcessFormalParamImplementation(CGWriter * writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  InsertString(*out, PREFIX_VARIABLE, 2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " = ", 0, CG_NO_EOL);
  InsertString(*out, "actual_params->", 0, CG_NO_EOL);     
  InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteProcessFormalParamHeader(): Nur fuer Prozesse! (kommt ins Block-H-File)
 *                   -> writer
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGVariable::WriteProcessFormalParamHeader(CGWriter * writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSSortRef sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  assert(GetSortRef());

  sort = GetSortRef();
  writer->CGSetIndentPos(pos + 2);
  sort->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, " ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);

  return DS_OK;
}

/****************************************************************************
 * WriteProcedureFormalParamHeader(): schreibt die formalen Parameter einer
 *                   Prozedur in eine Structure (h_file!)
 *                   -> writer
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGVariable::WriteProcedureFormalParamHeader(CGWriter * writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSProcedure *procedure;
  DSSortRef sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  assert(GetParent()->GetType() == DS_PROCEDURE);

  procedure = (DSProcedure *)GetParent();
  assert(procedure);

  if (procedure->GetIsComplex())
  {
    assert(GetSortRef());

    sort = GetSortRef();
    writer->CGSetIndentPos(pos + 2);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(*out, " ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
    writer->CGSetIndentPos(pos);
  }
  else
  {
    // Der Rueckgabeparameter gehoert nicht direkt zu den formalen Parametern:
    if (this == procedure->GetReturnParamRef())
    {
      return DS_OK;
    }

    if (!procedure->GetFormalParamList()->AtStartOfList())
    {
      InsertString(*out, ", ", 0, CG_NO_EOL);
    }

    sort = GetSortRef();
    writer->CGSetIndentPos(0);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);

    if (GetIsInOut())
    {
      InsertString(*out, " &", 0, CG_NO_EOL);
    }
  }
  
  return DS_OK;
}

/****************************************************************************
 * WriteProcedureFormalParamImplementation(): Initialisiert formale 
 *                   Prozedurparameter (c_file!)
 *                   -> writer
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGVariable::WriteProcedureFormalParamImplementation(CGWriter * writer) const
{
  DSResult result;
  DSProcedure *procedure;
  CGFile *out;
  CGPos   pos;
  DSSortRef sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  assert(GetParent()->GetType() == DS_PROCEDURE);

  procedure = (DSProcedure *)GetParent();
  assert(procedure);

  if (procedure->GetIsComplex())
  {
     InsertString(*out, PREFIX_VARIABLE, 2, CG_NO_EOL);
     InsertString(*out, GetName(), 0, CG_NO_EOL);
     InsertString(*out, " = ", 0, CG_NO_EOL);
     InsertString(*out, "actual_params->", 0, CG_NO_EOL);
     InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
     InsertString(*out, GetName(), 0, CG_NO_EOL);
     InsertString(*out, ";", 0, CG_WITH_EOL);
  }
  else
  {
    // Der Rueckgabeparameter gehoert nicht direkt zu den formalen Parametern:
    if (this == procedure->GetReturnParamRef())
    {
      return DS_OK;
    }

    if (!procedure->GetFormalParamList()->AtStartOfList())
    {
      InsertString(*out, ", ", 0, CG_NO_EOL);
    }

    sort = GetSortRef();
    writer->CGSetIndentPos(0);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, " ", 0, CG_NO_EOL);

    if (GetIsInOut())
    {
      InsertString(*out, "&", 0, CG_NO_EOL);
    }
    InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
  }
  
  return DS_OK;
}

/****************************************************************************
 * WriteSave(): Speicher Variableninhalt in dem uebergebenen Speicherbereich 
 ****************************************************************************/

DSResult CGVariable::WriteSave(CGWriter * writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSString str;
  DSSortRef sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  sort = GetSortRef();
  assert(sort);

  if (!sort->GetIsComplex()) // Datentyp ohne eigene Save()-Methode ?
  {
    if (GetIsHistory() == DS_FALSE)
    {
      str.Format("save_area.Store(&%s%s, sizeof(",
                 PREFIX_VARIABLE, GetName()->GetString());
    }
    else
    {
      str.Format("save_area.HistoryStore(&%s%s, sizeof(",
                 PREFIX_VARIABLE, GetName()->GetString());
    }
    InsertString(*out, str, pos, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, "));", 0, CG_WITH_EOL);     
  }
  else
  {
    if (GetIsHistory() == DS_FALSE)
    {
      str.Format("%s%s.Save(save_area);", PREFIX_VARIABLE, GetName()->GetString());
    }
    else
    {
      str.Format("%s%s.HistorySave(save_area);", PREFIX_VARIABLE, GetName()->GetString());
    }
    InsertString(*out, str, pos, CG_WITH_EOL);
  }
  
  return DS_OK;
}

/****************************************************************************
 * WriteRestore(): Restauriert Variableninhalt aus dem uebergebenen Speicherbereich 
 ****************************************************************************/

DSResult CGVariable::WriteRestore(CGWriter * writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSString str;
  DSSortRef sort;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  sort = GetSortRef();
  assert(sort);

  if (!sort->GetIsComplex()) // Datentyp ohne eigene Restore()-Methode ?
  {
    if (GetIsHistory() == DS_FALSE)
    {
      str.Format("save_area.Restore(&%s%s, sizeof(",
                 PREFIX_VARIABLE, GetName()->GetString());
    }
    else
    {
      str.Format("save_area.HistoryRestore(&%s%s, sizeof(",
                 PREFIX_VARIABLE, GetName()->GetString());
    }
    InsertString(*out, str, pos, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, "));", 0, CG_WITH_EOL);     
  }
  else // komplexer Datentyp !
  {
    if (GetIsHistory() == DS_FALSE)
    {
      str.Format("%s%s.Restore(save_area);", PREFIX_VARIABLE, GetName()->GetString());
    }
    else   
    {
      str.Format("%s%s.HistoryRestore(save_area);", PREFIX_VARIABLE, GetName()->GetString());
    }
    InsertString(*out, str, pos, CG_WITH_EOL);
  }
  
  return DS_OK;
}

/****************************************************************************
 * WriteSize(): Berechnet Speicherplatz fuer die Variable
 ****************************************************************************/

DSResult CGVariable::WriteSize(CGWriter * writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSString str;
  DSSortRef sort;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  sort = GetSortRef();
  assert(sort);

  if (!sort->GetIsComplex()) // kein komplexer Datentyp ?
  {
    if (GetIsHistory())      // History-Variable ?
    {
      InsertString(*out, "cur_size->historySize += sizeof(", pos, CG_NO_EOL);
    }
    else
    {
      InsertString(*out, "cur_size->size += sizeof(", pos, CG_NO_EOL);
    }
    writer->CGSetIndentPos(0);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, ");", 0, CG_WITH_EOL);     
  }
  else
  {
    if (GetIsHistory())      // History-Variable ?
    {
      str.Format("%s%s.HistorySize(cur_size);", PREFIX_VARIABLE, GetName()->GetString());
    }
    else
    {
      str.Format("%s%s.Size(cur_size);", PREFIX_VARIABLE, GetName()->GetString());
    }
    InsertString(*out, str, pos, CG_WITH_EOL);
  }
  return DS_OK;
}
