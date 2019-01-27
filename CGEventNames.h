const char *event_names[cgTraceMax + 1] =   // keep up to date with SCTraceTypes.h !
{
  "",                                 //  0 Dummy fuer ersten Wert
  CG_NAME_EVENT_SCHEDULER_INIT,       //  1 Init-Befehl fuer Scheduler
  CG_NAME_EVENT_SCHEDULER_STOP,       //  2 Stop-Befehl fuer Scheduler
  CG_NAME_EVENT_TIME_CHANGE,          //  3 Simulationszeit hat sich geaendert
  CG_NAME_EVENT_PROCESS_CREATE,       //  4 Prozess wird erzeugt
  CG_NAME_EVENT_PROCESS_STOP,         //  5 Prozess wird zerstoert
  CG_NAME_EVENT_MACHINE_CREATE,       //  6 Maschine wird erzeugt
  CG_NAME_EVENT_MACHINE_STOP,         //  7 Maschine wird zerstoert
  CG_NAME_EVENT_STATE_CHANGE,         //  8 Zustandswechsel
  CG_NAME_EVENT_SIGNAL_SEND,          //  9 Output an einen anderen Prozess
  CG_NAME_EVENT_SIGNAL_NOT_SENT,      // 10 Output an einen anderen Prozess
  CG_NAME_EVENT_SIGNAL_RECEIVE,       // 11 Prozess hat ein Signal empfangen
  CG_NAME_EVENT_SIGNAL_LOSE,          // 12 Signal ist verlorengegangen (Queue-Ueberlauf)
  CG_NAME_EVENT_SIGNAL_CONSUME,       // 13 Signal wurde konsumiert
  CG_NAME_EVENT_SIGNAL_SAVE,          // 14 Signal wurde zwischengespeichert (save)
  CG_NAME_EVENT_SIGNAL_DROP,          // 15 Signal wurde verworfen (implizite Konsumierung)
  CG_NAME_EVENT_SPONT_TRANS,          // 16 Spontantransition (input none) hat gefeuert
  CG_NAME_EVENT_CONT_SIGNAL,          // 17 Continuous Signal hat gefeuert
  CG_NAME_EVENT_SERVICE_REQUEST,      // 18 Request von Maschine empfangen
  CG_NAME_EVENT_SERVICE_FINISH,       // 19 Request ist beendet, Prozess wird freigegeben
  CG_NAME_EVENT_SERVICE_START,        // 20 Start der Bearbeitung eines Requests
  CG_NAME_EVENT_SERVICE_INTERRUPT,    // 21 Unterbrechung der Bearbeitung eines Requests
  CG_NAME_EVENT_TIMER_SET,            // 22 Timer wurde gesetzt
  CG_NAME_EVENT_TIMER_RESET,          // 23 Timer wurde geloescht
  CG_NAME_EVENT_TIMER_FIRE,           // 24 Timer wurde ausgeloest
  CG_NAME_EVENT_TIMER_REMOVE,         // 25 Timer in InputQueue geloescht
  CG_NAME_EVENT_PROCEDURE_CALL,       // 26 Prozedur wird aufgerufen
  CG_NAME_EVENT_PROCEDURE_RETURN,     // 27 Prozedur wird aufgerufen
  CG_NAME_EVENT_DEADLOCK,             // 28 Totaler Deadlock des Systems
  CG_NAME_EVENT_CYCLE_START,          // 29 Zyklisches Verhalten des Systems
  CG_NAME_EVENT_CYCLE_END,            // 30 Zyklisches Verhalten des Systems
  ""                                  // 30 Dummy fuer letzten Wert
};

// Wenn fuer ein Ereignis i event_role[i] != cgTraceRoleNone
// ist, dann wird von CGProcess eine zusaetzliche if-Abfrage
// generiert, um zu verhindern, dass zwei Prozesse von
// einen Ereigniss informiert werden. Dies liegt daran,
// dass SCTraceSensor (siehe SCL) sehr gespraechig ist
// und immer auch den Partnerprozess von bestimmten Ereignissen
// informiert. So erhaelt beispielsweise der Sender eines
// Signals eine Information wenn sein Signal beim Empfaenger
// angekommen ist.

CGEventRoles event_roles[cgTraceMax + 1] =
{
  cgTSRoleNone,      //  0 Dummy
  cgTSRoleNone,      //  1 SchedulerInit
  cgTSRoleNone,      //  2 SchedulerStop
  cgTSRoleNone,      //  3 TimeChange
  cgTSRoleOffspring, //  4 ProcessCreate
  cgTSRoleNone,      //  5 ProcessStop
  cgTSRoleNone,      //  6 MachineCreate
  cgTSRoleNone,      //  7 MachineStop
  cgTSRoleNone,      //  8 StateChange
  cgTSRoleSender,    //  9 SignalSend
  cgTSRoleNone,      // 10 SignalNotSent
  cgTSRoleReceiver,  // 11 SignalReceive
  cgTSRoleReceiver,  // 12 SignalLose
  cgTSRoleReceiver,  // 13 SignalConsume
  cgTSRoleReceiver,  // 14 SignalSave
  cgTSRoleReceiver,  // 15 SignalDrop
  cgTSRoleNone,      // 16 SpontTrans
  cgTSRoleNone,      // 17 ContSignal
  cgTSRoleNone,      // 18 ServiceRequest
  cgTSRoleNone,      // 19 ServiceFinish
  cgTSRoleNone,      // 20 ServiceStart
  cgTSRoleNone,      // 21 ServiceInterrupt
  cgTSRoleNone,      // 22 TimerSet
  cgTSRoleNone,      // 23 TimerReset
  cgTSRoleNone,      // 24 TimerFire
  cgTSRoleReceiver,  // 25 TimerRemove
  cgTSRoleNone,      // 26 ProcedureCall
  cgTSRoleNone,      // 27 ProcedureReturn
  cgTSRoleNone,      // 28 Deadlock
  cgTSRoleNone,      // 29 Zyklus (Start)
  cgTSRoleNone,      // 30 Zyklus (Ende)
  cgTSRoleNone       // 31 Dummy
};

const char *event_role_names[5] =
{
  "scTSRoleNone",
  "scTSRoleSender",
  "scTSRoleReceiver",
  "scTSRoleParent",
  "scTSRoleOffspring"
};
