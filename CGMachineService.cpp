/****************************************************************************
 *                 Modul: $RCSfile: CGMachineService.cpp,v $
 *
 * $Author: md $
 * $Date: 1997/10/22 16:09:19 $
 * $Revision: 1.2 $
 *
 * Aufgabe: Dieses Modul enthaelt die Funktionen, mit denen der C++-Code aus
 *          der Datenstruktur generiert wird
 * Funktionen: Write(): Oberfunktion der Codegenerierung
 *             WriteEnum(): Schreiben der Geschwindigkeiten in Enum-Typ
 *             WriteImplementation(): Initialisierung der Geschwindigkeiten
 *             WriteHeader(): Definition der Geschwindigkeiten  
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
#include <DS/DSSystem.h>
#include <DS/DSBlock.h>

#include "CGFile.h"
#include "CGMachineService.h"

#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGMachineService::CGMachineService(DSObject      *father,
                                   DSString      *n,
                                   DSSortRefList *sort_list) :
  DSMachineService(father, n, sort_list)
{
}

CGMachineService::CGMachineService(DSObject      *father,
                                   DSString      &n,
                                   DSSortRefList *sort_list) :
  DSMachineService(father, n, sort_list)
{
}

CGMachineService::CGMachineService(DSObject      *father,
                                   const char    *n,
                                   DSSortRefList *sort_list) :
  DSMachineService(father, n, sort_list)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGMachineService::~CGMachineService(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGMachineService::New(DSObject *father) const
{
  return new CGMachineService(father);
}

/****************************************************************************
 * Write: Oberfunktion der Codegenerierung 
 *        -> writer
 *        -> what
 *        Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *        Seiteneffekte: Aufruf der privaten Codegenerierungsfkt.
 ****************************************************************************/

DSResult CGMachineService::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
  
  switch (what)
  {
    case CG_WRITE_ENUM:
      result = WriteEnum((CGWriter *)writer);
      return (result == DS_OK) ? DS_OK : DS_ERROR;
      break;
      
    case CG_WRITE_CC:
      result = WriteImplementation((CGWriter *)writer);
      return (result == DS_OK) ? DS_OK : DS_ERROR;
      break;
      
    case CG_WRITE_H:
      result = WriteHeader((CGWriter *)writer);
      return (result == DS_OK) ? DS_OK : DS_ERROR;
      break;
      
    case CG_WRITE_INIT:
      result = WriteInit((CGWriter *)writer);
      return (result == DS_OK) ? DS_OK : DS_ERROR;
      break;
      
    case CG_WRITE_FINISH:
      result = WriteFinish((CGWriter *)writer);
      return (result == DS_OK) ? DS_OK : DS_ERROR;
      break;
      
    default:
      assert(DS_FALSE);
      return DS_ERROR;
      break;
  }
  return DS_OK;
}        

/****************************************************************************
 * WriteHeader(): Schreiben der SCRequest-Definitionen
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/

DSResult CGMachineService::WriteHeader(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "static SCRequestType * ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK; 
}

/****************************************************************************
 * WriteImplementation(): Erzeugen der SCRequest-Objekte
 *                        -> writer
 *                        Ergebnis: DS_OK, falls alles geklappt hat, sonst 
 *                                  DS_ERROR
 *                        Seiteneffekte: Anlegen und Fuellen der .cc-Files
 ****************************************************************************/

DSResult CGMachineService::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSObject *father;
  
  out = writer->CGGetFilePointer();
  pos = writer->CGGetIndentPos();

  father = GetParent();
  assert(father);
  
  InsertString(*out, "SCRequestType * ", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  father->Write(writer, CG_WRITE_IDENTIFIER);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " = NULL;", 0, CG_WITH_EOL);

  return DS_OK; 
}

/****************************************************************************
 * WriteInit(): Initialisieren der SCRequest-Objekte
 *              -> writer
 *              Ergebnis: DS_OK, falls alles geklappt hat, sonst 
 *                        DS_ERROR
 *              Seiteneffekte: Anlegen und Fuellen der .cc-Files
 ****************************************************************************/

DSResult CGMachineService::WriteInit(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_VARIABLE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " = new SCRequestType(", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_ID, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ", \"", 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "\");", 0, CG_WITH_EOL);

  return DS_OK; 
}

/****************************************************************************
 * WriteFinish(): Zerstoeren der SCRequest-Objekte
 *              -> writer
 *              Ergebnis: DS_OK, falls alles geklappt hat, sonst 
 *                        DS_ERROR
 *              Seiteneffekte: Anlegen und Fuellen der .cc-Files
 ****************************************************************************/

DSResult CGMachineService::WriteFinish(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  pos = writer->CGGetIndentPos();

  InsertString(*out, "delete ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK; 
}

/****************************************************************************
 * WriteEnum(): Schreiben der MaschinenServiceIDs in Aufzaehlungstyp
 *              -> writer
 *              Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *              Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/

DSResult CGMachineService::WriteEnum(CGWriter *writer) const
{
  DSResult      result;
  DSBoolean     first = DS_FALSE, last = DS_FALSE;
  DSObject     *father;
  DSMachineServiceRefList *machine_service_list;
  DSString     *tmp_str;
  CGFile       *out;
  CGPos         pos;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  father = GetParent();

  switch(father->GetType())
  {
    case DS_BLOCK:
      if (((DSBlock *)father)->GetMachineServiceList()->AtStartOfList())
      {
        first = DS_TRUE;
      }
      if (((DSBlock *)father)->GetMachineServiceList()->AtEndOfList())
      {
        last = DS_TRUE;
      }
      break;
    case DS_BLOCKSUBSTRUCTURE:
      if (((DSBlockSubstructure *)father)->GetMachineServiceList()->AtStartOfList())
      {
        first = DS_TRUE;
      }
      if (((DSBlockSubstructure *)father)->GetMachineServiceList()->AtEndOfList())
      {
        last = DS_TRUE;
      }
      break;
    case DS_SYSTEM:
      if (((DSSystem *)father)->GetMachineServiceList()->AtStartOfList())
      {
        first = DS_TRUE;
      }
      if (((DSSystem *)father)->GetMachineServiceList()->AtEndOfList())
      {
        last = DS_TRUE;
      }
      break;
    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }

  if (first)
  {
    InsertString(*out, "enum", pos, CG_WITH_EOL);
    InsertString(*out, "{", pos, CG_WITH_EOL);
  }
  InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_ID, pos + 2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  if (first)
  {
    InsertString(*out, " = kSCRequestTypeIDBase + ", 0, CG_NO_EOL);
    tmp_str = new DSString(writer->CGGetLastServiceID());
    assert(tmp_str);
    InsertString(*out, tmp_str, 0, CG_NO_EOL);
    delete tmp_str;
  }
  writer->CGSetLastServiceID(writer->CGGetLastServiceID() + 1);

  if (!last)
  {
    InsertString(*out, ",", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "", 0, CG_WITH_EOL);
    InsertString(*out, "};", pos, CG_WITH_EOL);
  }

  // Alle Dienste in Liste merken:
  machine_service_list = writer->CGGetServiceList();
  assert(machine_service_list);
  assert(GetName());
  
  assert(machine_service_list->Append(this) == DS_OK);

  return DS_OK;
}
