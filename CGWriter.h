#ifndef _WRITER_H_
#define _WRITER_H_

#include <DS/DSStringList.h>
#include <DS/DSBlockList.h>
#include <DS/DSProcessList.h>
#include <DS/DSProcedureList.h>
#include <DS/DSMachineList.h>
#include <DS/DSSortList.h>
#include <DS/DSWriter.h>
#include <DS/DSRef.h>

#include "CGBasicTypes.h"

/******************************************************************************
 * Klasse CGWriter
 * Zweck: Wirft die Codegenerierung an, ruft die entsprechenden Generierungs-
 *        funktionen der anderen CG-Klassen auf und generiert die Datei, in
 *        der alle benoetigten Template-Klassen instanziiert werden
 ******************************************************************************/

class CGWriter : public DSWriter
{
  public:
    CGWriter(class DSSystem*,                        // System
             class DSStream &myCout,                 // Ausgabestream fuer Meldungen 
             DSBoolean = DS_TRUE,                    // Makefile erzeugen ?
             DSBoolean = DS_TRUE,                    // Templatefile erzeugen ?
             DSBoolean = DS_TRUE,                    // Experimentfile erzeugen ?
             DSBoolean = DS_FALSE,                   // Meldungen ?
             DSBoolean = DS_FALSE,                   // Pretty-Printing?
             DSBoolean = DS_FALSE,                   // Incrementelle Codegen.?
             DSBoolean = DS_FALSE,                   // Informal Text?
             DSBoolean = DS_FALSE,                   // support PEV ?
             DSBoolean = DS_FALSE);                  // Debug?
    ~CGWriter(void);

    DSResult         CGWrite(void);
    DSString        *CGGetOutputDir(void) const;    
    class DSSystem  *CGGetSystem(void) const;
    DSBoolean        CGCreateMakefile(void) const;    
    DSBoolean        CGCreateTemplatefile(void) const;    
    DSBoolean        CGVerbose(void) const;    
    DSBoolean        CGPrettyPrinting(void) const;    
    DSBoolean        CGIncrementalCodeGeneration(void) const;    
    DSBoolean        CGGetWriteInformalText(void) const;
    DSBoolean        CGSupportPEV(void) const;    
    DSBoolean        CGDebug(void) const;    
    class CGFile    *CGGetFilePointer(void) const;
    CGPos            CGGetIndentPos(void) const;

    DSCardinal       CGGetLastSignalID(void) const;
    DSCardinal       CGGetLastProcessID(void) const;
    DSCardinal       CGGetLastProcedureID(void) const;
    DSStringKeyList *CGGetLabelList(void) const;
    DSCardinal       CGGetLastMachineID(void) const;
    DSCardinal       CGGetLastServiceID(void) const;
    DSCardinal       CGGetLastStateID(void) const;
    DSCardinal       CGGetLastTransitionID(void) const;
    DSCardinal       CGGetLastRequestID(void) const;
    DSCardinal       CGGetLastCallID(void) const;
    DSCardinal       CGGetLastAtomID(void) const;
    DSMachineServiceRefList *CGGetServiceList(void) const;
    DSStateRef       CGGetState(void) const;
    DSCardinal       CGGetEvent(void) const;    // for sensor tracing

    DSResult         CGSetFilePointer(class CGFile *);
    DSResult         CGSetIndentPos(CGPos);
    DSResult         CGSetLabelList(DSStringKeyList *);
    DSResult         CGSetLastSignalID(DSCardinal);
    DSResult         CGSetLastProcessID(DSCardinal);
    DSResult         CGSetLastProcedureID(DSCardinal);
    DSResult         CGSetLastMachineID(DSCardinal);
    DSResult         CGSetLastServiceID(DSCardinal);
    DSResult         CGSetLastStateID(DSCardinal);
    DSResult         CGSetLastTransitionID(DSCardinal);
    DSResult         CGSetLastRequestID(DSCardinal);
    DSResult         CGSetLastCallID(DSCardinal);
    DSResult         CGSetLastAtomID(DSCardinal);
    DSResult         CGSetState(DSStateRef);
    DSResult         CGSetEvent(DSCardinal);     // for sensor tracing

    DSResult CGEvaluateEnvironment(void);
    DSResult CGMakeDir(DSString *);

  private:
    DSResult CGWriteSystemFiles(void);
    DSResult CGWriteBlockFiles(DSBlockKeyList*);
    DSResult CGWriteProcessFiles(DSProcessKeyList*);
    DSResult CGWriteProcedureFiles(DSProcedureKeyList*);
    DSResult CGWriteMachineFiles(DSMachineKeyList*);
    DSResult CGWriteMainFile(void);
    DSResult CGWriteMakeFile(void);
    DSResult CGWriteTemplateFile(void);
    DSResult CGWriteExperimentFile(void);
    DSResult CGWriteProcessSensors(DSProcess *);
    DSResult CGWriteProcessDisplays(DSProcess *, DSCardinal &);
    DSResult CGWriteMachineSensors(DSMachine *);
    DSResult CGWriteMachineDisplays(DSMachine *, DSCardinal &);
    DSResult CGWriteTemplates(DSSortList *);

    class DSSystem    *system;

  public:
    class DSStream &cgCout;

  private:
    /**********************************************/
    /* Variablen zur Auswertung des Environments: */
    /**********************************************/
    class DSString    *output_dir;

    DSBoolean    generate_makefile;
    DSBoolean    generate_templatefile;
    DSBoolean    generate_experimentfile;
    DSBoolean    verbose;
    DSBoolean    pretty_printing;
    DSBoolean    generate_informal_text;
    DSBoolean    incremental_code_generation;
    DSBoolean    support_pev;
    DSBoolean    generate_debug;

    DSCardinal   last_signal_id;
    DSCardinal   last_process_id;
    DSCardinal   last_procedure_id;
    DSCardinal   last_machine_id;
    DSCardinal   last_service_id;
    DSCardinal   last_state_id;
    DSCardinal   last_transition_id;
    DSCardinal   last_request_id;
    DSCardinal   last_call_id;
    DSCardinal   last_atom_id;
    DSMachineServiceRefList    *machine_service_list; // Liste aller Dienste im System
    DSStringKeyList *label_list; // Namen der Labels des aktuellen Prozesses
    DSStateRef   real_state; // wird fuer Inputs der 'State *' benoetigt
                             // um den Zustand zu bestimmen dem sie
                             // aktuell zugeordnet sind
    DSCardinal   trace_event;

    CGFile *current_file;
    CGPos   current_pos;
};

#endif
