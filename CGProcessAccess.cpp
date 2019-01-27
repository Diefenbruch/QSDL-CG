/****************************************************************************
 *                 Modul: $RCSfile: CGProcessAccess.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.3 $
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

#include <DS/DSProcess.h>
#include <DS/DSExpression.h>
#include <DS/DSString.h>
#include <DS/DSVariableAccess.h>
#include <DS/DSSensor.h>
#include <DS/DSBlock.h>

#include "CGFile.h"
#include "CGProcessAccess.h"
#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Externe Processn
 ****************************************************************************/

/****************************************************************************
 * Globale Processn
 ****************************************************************************/

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGProcessAccess::CGProcessAccess(DSObject* father,    // Vaterobjekt
                                 DSProcessRef p_ref,  // Referenz auf Prozess
                                 DSExpression* p_id,  // Identifizierungsnummer
                                 DSVariableAccess* v_access): // Referenz auf Variable
  DSProcessAccess(father, p_ref, p_id, v_access)
{
}

CGProcessAccess::CGProcessAccess(DSObject* father,   // Vaterobjekt
                                 DSProcessRef p_ref, // Referenz auf Prozess
                                 DSExpression* p_id, // Identifizierungsnummer
                                 DSSensorRef s_ref): // Referenz auf Sensor
  DSProcessAccess(father, p_ref, p_id, s_ref)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGProcessAccess::~CGProcessAccess(void)
{
}
 
DSObject *CGProcessAccess::New(DSObject *father) const
{
  return new CGProcessAccess(father);
}

/****************************************************************************
 * Write(): schreibt den Zugriff auf eine Process
 *                   -> writer
 *                   -> what
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGProcessAccess::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteProcessAccess((CGWriter *)writer);

  return result;
}

/****************************************************************************
 * Write(): schreibt Zugriff auf eine Process
 *                   -> writer
 *                   Ergebnis: CG_OK,falls Aktion erfolgreich war, sonst 
 *                             enthaelt DSResult das Ergebnis
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGProcessAccess::WriteProcessAccess(CGWriter *writer) const
{
  DSResult           result;
  DSExpression *     process_id;
  CGFile *           out;
  CGPos              pos;
  DSProcessRef       process_ref;
  DSString           str;
  DSVariableAccess * var_access;
  DSSensorRef        p_sensor_ref;
  DSBlockRef         block_ref;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  process_ref = GetProcessRef();
  assert(process_ref);

  block_ref = process_ref->GetThisBlock();
  assert(block_ref);
    
  var_access = GetVariableAccess();
  p_sensor_ref = GetSensorRef();

  if (!var_access && !p_sensor_ref) // soll nur auf den Process-Typ
  {                                 // zugegriffen werden (z.B. bei
                                    // Sensorabfrage)
    block_ref->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(*out, "::", 0, CG_NO_EOL);
    str.Format("%s%s",
               PREFIX_PROCESS_TYPE_VARIABLE,
               process_ref->GetName()->GetString());
    InsertString(*out, str, 0, CG_NO_EOL);
    
    return DS_OK;
  }
    
  InsertString(*out, "((", pos, CG_NO_EOL);
  process_ref->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, " *)", 0, CG_NO_EOL);
  process_id = GetProcessID();
  if (GetProcessID())
  {
    InsertString(*out, "SCScheduler::GetRunnableFromID(", 0, CG_NO_EOL);
    process_id->Write(writer);
    InsertString(*out, ")", 0, CG_NO_EOL);
  }
  else
  {
    block_ref->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(*out, "::", 0, CG_NO_EOL);
    str.Format("%s%s->GetAProcess()",
               PREFIX_PROCESS_TYPE_VARIABLE,
               process_ref->GetName()->GetString());
    InsertString(*out, str, 0, CG_NO_EOL);
  }
  InsertString(*out, ")->", 0, CG_NO_EOL);

  if (var_access)
  {
    var_access->Write(writer);
  }
  else if (GetSensorRef())
  {
    p_sensor_ref->Write(writer, CG_WRITE_IDENTIFIER);
  }
  
  return result;
}
