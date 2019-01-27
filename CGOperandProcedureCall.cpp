/****************************************************************************
 *                 Modul: $RCSfile: CGOperandProcedureCall.cpp,v $
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

#include <DS/DSCall.h>

#include "CGFile.h"
#include "CGOperandProcedureCall.h"

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

CGOperandProcedureCall::CGOperandProcedureCall(DSObject *father,
                                               DSCall *procedure_call) :
  DSOperandProcedureCall(father, procedure_call)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperandProcedureCall::~CGOperandProcedureCall(void)
{
}

DSObject *CGOperandProcedureCall::New(DSObject *father) const
{
  return new CGOperandProcedureCall(father);
}

/****************************************************************************
 * Write(): schreibt einen als Prozeduraufruf gespeicherten Operanden
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOperandProcedureCall::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
  DSCallRef call_ref;
  
  (void)what;

  call_ref = GetProcedureCall();
  result = call_ref->Write(writer, what);

  return result;
}
