#ifndef _CGBASICTYPES_H_
#define _CGBASICTYPES_H_

#define CG_VERSION "1.7.0"
#define CG_DATE    "$Date: 1998/02/10 12:33:42 $"

/**************************************/
/* Informationen zum Debugging:       */
/**************************************/

#define CG_DEBUG_INCLUDE "dmalloc.h"
#define CG_DEBUG_LIB     "dmalloc"
#define CG_DEBUG_FILE    "dmalloc"
#define CG_DEBUG_DEFINES "-DDEBUG -DDMALLOC_FUNC_CHECK"

/******************************************/
/* Konstanten zur Steuerung des Tracings: */
/******************************************/
 
#define CG_TRACE_TYPE_NAME_NONE   "none"
#define CG_TRACE_TYPE_NAME_MSC    "msc"
#define CG_TRACE_TYPE_NAME_PEV    "pev"
#define CG_TRACE_TYPE_NAME_SENSOR "sensor"
#define CG_TRACE_TYPE_NAME_SOCKET "socket"

#define CG_TRACE_VARIABLE "tracing"

/************************************************/
/* Konstanten zur Steuerung der Write-Funktion: */
/************************************************/

enum
{
  CG_WRITE_H,
  CG_WRITE_CC,
  CG_WRITE_INCLUDE,
  CG_WRITE_ENUM,
  CG_WRITE_PARAM_STRUCT,
  CG_WRITE_PARAM,
  CG_WRITE_TYPE_H,
  CG_WRITE_TYPE_CC,
  CG_WRITE_TYPE_ENUM,
  CG_WRITE_TYPE_INIT,
  CG_WRITE_TYPE_FINISH,
  CG_WRITE_INIT,
  CG_WRITE_FINISH,
  CG_WRITE_DEFINITION,
  CG_WRITE_FORMAL_PARAM_H,
  CG_WRITE_FORMAL_PARAM_CC,
  CG_WRITE_ACTUAL_PARAM,
  CG_WRITE_SPEED_H,
  CG_WRITE_SPEED_CC,
  CG_WRITE_CREATE,
  CG_WRITE_DESTROY,
  CG_WRITE_SCHEDULE,
  CG_WRITE_LITERAL_LIST,
  CG_WRITE_OPERATOR_LIST,
  CG_WRITE_INPUT_SET_H,
  CG_WRITE_INPUT_SET_CC,
  CG_WRITE_SAVE_SET_H,
  CG_WRITE_SAVE_SET_CC,
  CG_WRITE_SET_ENABLING_CONDITION,
  CG_WRITE_SET_CONTINIOUS_SIGNALS,
  CG_WRITE_STATE,
  CG_WRITE_INPUT_LIST,
  CG_WRITE_NONE_LIST,
  CG_WRITE_TRANSITION_INIT,
  CG_WRITE_TRANSITION_CC,
  CG_WRITE_IDENTIFIER,
  CG_WRITE_SAVE,
  CG_WRITE_RESTORE,
  CG_WRITE_SIZE,
  CG_WRITE_HISTORY_SIZE,
  CG_WRITE_UPDATE,
  CG_WRITE_FRIEND
};

/**********************************/
/* Datentypen des Codegenerators: */
/**********************************/
 
 typedef unsigned short CGPos;

/********************************************************/
/* Prefixe fuer die verschiedenen SDL-Hierarchieebenen: */
/********************************************************/
 
#define PREFIX_SYSTEM             "sys_"
#define PREFIX_BLOCK              "blk_"
#define PREFIX_BLOCKSUBSTRUCTURE  "bsub_"
#define PREFIX_PROCESS            "pcs_"
#define PREFIX_PROCEDURE          "pcd_"

/*****************************/
/* Dateinamensbestandteile : */
/*****************************/
 
#define FILE_MAIN      "Main"
#define FILE_MAKE      "Makefile"
#define FILE_TEMPLATES "Templates"
#define FILE_MSC_TRACE "Output"

#define SUFFIX_INC        ".new"
#define SUFFIX_H          ".h"
#define SUFFIX_H_INC      ".h.new"
#define SUFFIX_C          ".cpp"
#define SUFFIX_C_INC      ".cpp.new"
#define SUFFIX_O          ".o"
#define SUFFIX_MSC_TRACE  ".mpr"
#define SUFFIX_PEV        ".pev"
#define SUFFIX_PEV_REPORT ".rep"

/**************************************/
/* Prefix fuer Environment-Variablen: */
/**************************************/
 
#define PREFIX_QUEST_ENV "QPAR_"

/*****************************************************/
/* Namensbestandteile von Variablen und Konstanten : */
/*****************************************************/
 
#define PREFIX_ATOM "atom_"
#define PREFIX_VARIABLE "v_"
#define PREFIX_SENSOR "sensor_"
#define PREFIX_INPUT_SET "input_set_"
#define PREFIX_SAVE_SET "save_set_"
#define PREFIX_SPONTANOUS_SET "spontanous_set_"
#define PREFIX_SYNONYM "syn_"
#define PREFIX_FIELD "field_"
#define PREFIX_DATA_TYPE "DataType_"
#define PREFIX_SIGNAL_TYPE "SigType_"
#define PREFIX_SIGNAL_TYPE_VARIABLE "sig_type_"
#define PREFIX_SIGNAL_TYPE_ID "sig_type_id_"
#define PREFIX_SIGNAL_PARAM_TYPE "SigParam_"
#define PREFIX_SIGNAL_PARAM_VARIABLE "sigparam_"
#define PREFIX_SIGNAL_FIELD "sig_field_"
#define PREFIX_TIMER_TYPE "TimType_"
#define PREFIX_TIMER_TYPE_VARIABLE "tim_type_"
#define PREFIX_TIMER_TYPE_ID "tim_type_id_"
#define PREFIX_TIMER_PARAM_TYPE "TimParam_"
#define PREFIX_TIMER_PARAM_VARIABLE "timparam_"
#define PREFIX_TIMER_FIELD "tim_field_"
#define PREFIX_PROCEDURE "pcd_"
#define PREFIX_PROCEDURE_ID "pcd_id_"
#define PREFIX_PROCEDURE_TYPE "PcdType_"
#define PREFIX_PROCEDURE_TYPE_VARIABLE "pcd_type_"
#define PREFIX_PROCEDURE_TYPE_ID "pcd_type_id_"
#define PREFIX_PROCEDURE_PARAM_VARIABLE "pcd_formal_param_"
#define PREFIX_PROCEDURE_PARAM_TYPE "PcdFormalParam_"
#define PREFIX_STATE_TYPE_VARIABLE "state_type_"
#define PREFIX_STATE_TYPE_ID "state_type_id_"
#define PREFIX_PROCESS "pcs_"
#define PREFIX_PROCESS_ID "pcs_id_"
#define PREFIX_PROCESS_TYPE "PcsType_"
#define PREFIX_PROCESS_TYPE_VARIABLE "pcs_type_"
#define PREFIX_PROCESS_TYPE_ID "pcs_type_id_"
#define PREFIX_PROCESS_PARAM_TYPE "PcsFormalParam_"
#define PREFIX_PROCESS_PARAM_VARIABLE "pcs_formal_param_"
#define PREFIX_MACHINE "mach_"
#define PREFIX_MACHINE_ID "mach_id_"
#define PREFIX_MACHINE_SPEED "mach_speed_"
#define PREFIX_MACHINE_SERVICE_TYPE_ID "mach_service_type_id_"
#define PREFIX_MACHINE_SERVICE_TYPE_VARIABLE "mach_service_type_"
#define PREFIX_LABEL "label_"

/**************************************************/
/* Standard-Compiler-Optionen fuer das generierte */
/* Makefile falls die entsprechenden Umgebungs-   */
/* variablen nicht gesetzt sind.                  */
/**************************************************/
 
#define CG_STANDARD_COMPILER_FLAGS    "-c"
#define CG_STANDARD_COMPILER_DEBUG    "-g"
#define CG_STANDARD_LINKER_FLAGS      "-o"
#ifdef macintosh
  #define CG_STANDARD_OUTPUT_DIR      ":"
#else
  #define CG_STANDARD_OUTPUT_DIR      "."
#endif
#define CG_STANDARD_LIBS              "-lm"
#define CG_STANDARD_SCL_LIB           "-lSCL"
#define CG_STANDARD_SCL_LIB_LONG      "libSCL.a"
#define CG_STANDARD_TL_LIB            "-lTL"
#define CG_STANDARD_TL_LIB_LONG       "libTL.a"
#define CG_STANDARD_THREAD_LIB        "-lpthread"
#define CG_STANDARD_PEV_LIBS          "-lPEV"
#define CG_STANDARD_PEV_LIBS_LONG     "libPEV.a"
#define CG_STANDARD_X11_LIB           "-lX11"
#define CG_STANDARD_SOCKET_LIBS       "-lsocket -lnsl"

/*****************************************************/
/* SCL-Befehle und -Namen:                           */
/*****************************************************/
 
#define CG_NAME_SCL_ARRAY                "SCArray"
#define CG_NAME_SCL_BOOLEAN              "SCBoolean"
#define CG_NAME_SCL_BYTE                 "SCByte"
#define CG_NAME_SCL_CALL                 "Call"
#define CG_NAME_SCL_CHARACTER            "SCCharacter"
#define CG_NAME_SCL_CHARSTRING           "SCCharstring"
#define CG_NAME_SCL_CREATE               "Create"
#define CG_NAME_SCL_DECISION             "SCDecision"
#define CG_NAME_SCL_DURATION             "SCDuration"
#define CG_NAME_SCL_INPUT                "Input"
#define CG_NAME_SCL_INTEGER              "SCInteger"
#define CG_NAME_SCL_MACHINE              "SCMachine"
#define CG_NAME_SCL_NATURAL              "SCNatural"
#define CG_NAME_SCL_NO_PROCESS_ID        "kSCNoProcessID"
#define CG_NAME_SCL_OUTPUT               "Output"
#define CG_NAME_SCL_PID                  "SCPid"
#define CG_NAME_SCL_PROCESS              "SCProcess"
#define CG_NAME_SCL_PROCESS_ID           "SCProcessID"
#define CG_NAME_SCL_PROCESS_TYPE         "SCProcessType"
#define CG_NAME_SCL_RAND_BERNOULLI       "SCRandBernoulli"
#define CG_NAME_SCL_RAND_CONST           "SCRandConst"
#define CG_NAME_SCL_RAND_EMPIRICAL       "SCRandEmpirical"
#define CG_NAME_SCL_RAND_ERLANG          "SCRandErlang"
#define CG_NAME_SCL_RAND_LONG            "SCRandLong"
#define CG_NAME_SCL_RAND_NEGEXP          "SCRandNegExp"
#define CG_NAME_SCL_RAND_NORMAL          "SCRandNormal"
#define CG_NAME_SCL_RAND_POISSON         "SCRandPoisson"
#define CG_NAME_SCL_RAND_UNIFORM         "SCRandUniform"
#define CG_NAME_SCL_REAL                 "SCReal"
#define CG_NAME_SCL_REQUEST              "Request"
#define CG_NAME_SCL_REQUEST_TYPE         "SCRequestType"
#define CG_NAME_SCL_RESETSETTIMER        "ResetTimer"
#define CG_NAME_SCL_SCHEDULER            "SCScheduler"
#define CG_NAME_SCL_SETTIMER             "SetTimer"
#define CG_NAME_SCL_SIGNAL_ID            "SCSignalID"
#define CG_NAME_SCL_SIGNAL_TYPE          "SCSignalType"
#define CG_NAME_SCL_STATE_TYPE           "SCStateType"
#define CG_NAME_SCL_STOP                 "Stop"
#define CG_NAME_SCL_SYNTYPE              "SCSyntype"
#define CG_NAME_SCL_SYSTEM_THREAD        "SCSystemThread"
#define CG_NAME_SCL_TIME                 "SCTime"
#define CG_NAME_SCL_TIMER_TYPE           "SCTimerType"
#define CG_NAME_SCL_TRACE                "SCTrace"
#define CG_NAME_SCL_UNLIMITED_PROCESS    "kSCUnlimitedProcesses"

/*****************************************************/
/* SCL-Sensor-Namen:                                 */
/*****************************************************/

#define CG_NAME_SENSOR_COUNTER                     "SCSensorCounter"
#define CG_NAME_SENSOR_FREQUENCY                   "SCSensorDynFreq"
#define CG_NAME_SENSOR_TALLY                       "SCSensorTally"

#define CG_NAME_SENSOR_PCS_FREQ_SIGNAL_RECEIVE     "SCSensorSignalReceiveFreq"
#define CG_NAME_SENSOR_PCS_FREQ_SIGNAL_DROP        "SCSensorSignalDropFreq"
#define CG_NAME_SENSOR_PCS_FREQ_SIGNAL_CONSUME     "SCSensorSignalConsumeFreq"
#define CG_NAME_SENSOR_PCS_FREQ_SIGNAL_SAVE        "SCSensorSignalSaveFreq"
#define CG_NAME_SENSOR_PCS_FREQ_SIGNAL_LOSE        "SCSensorSignalLoseFreq"
#define CG_NAME_SENSOR_PCS_FREQ_SIGNAL_SEND        "SCSensorSignalSendFreq"
#define CG_NAME_SENSOR_PCS_FREQ_REQUEST            "SCSensorRequestFreq"
#define CG_NAME_SENSOR_PCS_FREQ_STATE              "SCSensorStateFreq"
#define CG_NAME_SENSOR_PCS_COUNT_SIGNAL_RECEIVE    "SCSensorSignalReceiveCount"
#define CG_NAME_SENSOR_PCS_COUNT_SIGNAL_DROP       "SCSensorSignalDropCount"
#define CG_NAME_SENSOR_PCS_COUNT_SIGNAL_CONSUME    "SCSensorSignalConsumeCount"
#define CG_NAME_SENSOR_PCS_COUNT_SIGNAL_SAVE       "SCSensorSignalSaveCount"
#define CG_NAME_SENSOR_PCS_COUNT_SIGNAL_LOSE       "SCSensorSignalLoseCount"
#define CG_NAME_SENSOR_PCS_COUNT_SIGNAL_SEND       "SCSensorSignalSendCount"
#define CG_NAME_SENSOR_PCS_COUNT_REQUEST           "SCSensorRequestCount"
#define CG_NAME_SENSOR_PCS_COUNT_STATE             "SCSensorStateCount"
#define CG_NAME_SENSOR_PCS_QUEUE_LENGTH            "SCSensorSignalQueueLength"
#define CG_NAME_SENSOR_PCS_QUEUE_CONTENT           "SCSensorSignalQueueContent"
#define CG_NAME_SENSOR_PCS_WAITTIME_SIGNAL         "SCSensorSignalTime"
#define CG_NAME_SENSOR_PCS_WAITTIME_STATE          "SCSensorStateTime"
#define CG_NAME_SENSOR_PCS_RESPONSETIME_REQUEST    "SCSensorRequestTime"
#define CG_NAME_SENSOR_PCS_TRANSMISSIONTIME_SIGNAL "SCSensorSignalTime"
#define CG_NAME_SENSOR_PCS_FREQ_TRANSITION         "SCSensorTransitionFreq"
#define CG_NAME_SENSOR_PCS_COUNT_TRANSITION        "SCSensorTransitionCount"

#define CG_NAME_SENSOR_MACH_FREQ_REQUEST        "SCSensorRequestFreq"
#define CG_NAME_SENSOR_MACH_COUNT_REQUEST       "SCSensorRequestCount"
#define CG_NAME_SENSOR_MACH_QUEUE_LENGTH        "SCSensorWaitingQueueLength"
#define CG_NAME_SENSOR_MACH_QUEUE_CONTENT       "SCSensorRequestQueueContent"
#define CG_NAME_SENSOR_MACH_UTILIZATION         "SCSensorUtilization"
#define CG_NAME_SENSOR_MACH_WAITTIME_REQUEST    "SCSensorRequestTime"
#define CG_NAME_SENSOR_MACH_THROUGHPUT          "SCSensorRequestThroughput"
#define CG_NAME_SENSOR_MACH_SERVICETIME_REQUEST "SCSensorRequestTime"

#define CG_NAME_SENSOR_SYS_FREQ_SIGNAL          "SCSensorGlobalSignalFreq"
#define CG_NAME_SENSOR_SYS_FREQ_REQUEST         "SCSensorGlobalRequestFreq"
#define CG_NAME_SENSOR_SYS_FREQ_PROCESS_CREATE  "SCSensorProcessFreq"
#define CG_NAME_SENSOR_SYS_FREQ_PROCESS_STOP    "SCSensorProcessFreq"
#define CG_NAME_SENSOR_SYS_COUNT_SIGNAL         "SCSensorGlobalSignalCount"
#define CG_NAME_SENSOR_SYS_COUNT_REQUEST        "SCSensorGlobalRequestCount"
#define CG_NAME_SENSOR_SYS_COUNT_PROCESS_CREATE "SCSensorProcessCount"
#define CG_NAME_SENSOR_SYS_COUNT_PROCESS_STOP   "SCSensorProcessCount"
#define CG_NAME_SENSOR_SYS_LIFETIME_PROCESS     "SCSensorProcessTime"

enum CGEventRoles // vgl. SCL: SCTraceSensor.h
{
  cgTSRoleNone = 0,
  cgTSRoleSender,
  cgTSRoleReceiver,
  cgTSRoleParent,
  cgTSRoleOffspring
};

/*****************************************************/
/* SCL-Event-Namen:                                  */
/*****************************************************/

#define CG_NAME_EVENT_SCHEDULER_INIT   "scTraceSchedulerInit"
#define CG_NAME_EVENT_SCHEDULER_STOP   "scTraceSchedulerStop"
#define CG_NAME_EVENT_TIME_CHANGE      "scTraceTimeChange"
#define CG_NAME_EVENT_PROCESS_CREATE   "scTraceProcessCreate"
#define CG_NAME_EVENT_PROCESS_STOP     "scTraceProcessStop"
#define CG_NAME_EVENT_MACHINE_CREATE   "scTraceMachineCreate"
#define CG_NAME_EVENT_MACHINE_STOP     "scTraceMachineStop"
#define CG_NAME_EVENT_STATE_CHANGE     "scTraceStateChange"
#define CG_NAME_EVENT_SIGNAL_SEND      "scTraceSignalSend"
#define CG_NAME_EVENT_SIGNAL_NOT_SENT  "scTraceSignalNotSent"
#define CG_NAME_EVENT_SIGNAL_RECEIVE   "scTraceSignalReceive"
#define CG_NAME_EVENT_SIGNAL_LOSE      "scTraceSignalLose"
#define CG_NAME_EVENT_SIGNAL_CONSUME   "scTraceSignalConsume"
#define CG_NAME_EVENT_SIGNAL_SAVE      "scTraceSignalSave"
#define CG_NAME_EVENT_SIGNAL_DROP      "scTraceSignalDrop"
#define CG_NAME_EVENT_SPONT_TRANS      "scTraceSpontTrans"
#define CG_NAME_EVENT_CONT_SIGNAL      "scTraceContSignal"
#define CG_NAME_EVENT_SERVICE_REQUEST  "scTraceServiceRequest"
#define CG_NAME_EVENT_SERVICE_FINISH   "scTraceServiceFinish"
#define CG_NAME_EVENT_SERVICE_START    "scTraceServiceStart"
#define CG_NAME_EVENT_SERVICE_INTERRUPT "scTraceServiceInterrupt"
#define CG_NAME_EVENT_TIMER_SET        "scTraceTimerSet"
#define CG_NAME_EVENT_TIMER_RESET      "scTraceTimerReset"
#define CG_NAME_EVENT_TIMER_FIRE       "scTraceTimerFire"
#define CG_NAME_EVENT_TIMER_REMOVE     "scTraceTimerRemove"
#define CG_NAME_EVENT_PROCEDURE_CALL   "scTraceProcedureCall"
#define CG_NAME_EVENT_PROCEDURE_RETURN "scTraceProcedureReturn"
#define CG_NAME_EVENT_DEADLOCK         "scTraceDeadlock"
#define CG_NAME_EVENT_CYCLE_START      "scTraceCycleStart"
#define CG_NAME_EVENT_CYCLE_END        "scTraceCycleEnd"

/*****************************************************/
/* SCL-Skript-Befehle und -Namen:                    */
/*****************************************************/

#define CG_NAME_PEV_SYSTEM_SPECIFICATION          "Specification  "
#define CG_NAME_PEV_SYSTEM_EXPERIMENT             "Experiment     "
#define CG_NAME_PEV_SYSTEM_REPORT_FILE            "Report         "
#define CG_NAME_PEV_SYSTEM_CURVE_POINTS           "CurvePoints    "
#define CG_NAME_PEV_SYSTEM_SCALE_ADAPTION         "ScaleAdaption  "
#define CG_NAME_PEV_SYSTEM_DEFAULT_INTERVAL       "DefaultInterval"
#define CG_NAME_PEV_SENSORS                       "SensorCreation:"
#define CG_NAME_PEV_SENSOR_MACH_QUEUE_LEN         "MachineQLen         "
#define CG_NAME_PEV_SENSOR_MACH_QUEUE_LEN_FREQ    "MachineQLenFreq     "
#define CG_NAME_PEV_SENSOR_MACH_REQUEST_FREQ      "MachineInReqFreq    "
#define CG_NAME_PEV_SENSOR_MACH_REQUEST_WAIT_TIME "MachineReqWaitTime  "
#define CG_NAME_PEV_SENSOR_MACH_UTILIZATION       "MachineUtilization  "
#define CG_NAME_PEV_SENSOR_PROC_QUEUE_LEN         "ProcessQLen         "
#define CG_NAME_PEV_SENSOR_PROC_QUEUE_LEN_FREQ    "ProcessQLenFreq     "
#define CG_NAME_PEV_SENSOR_PROC_NUMBER            "ProcessNumber       "
#define CG_NAME_PEV_SENSOR_PROC_STATE_FREQ        "ProcessStateFreq    "
#define CG_NAME_PEV_SENSOR_PROC_SIGNAL_IN_FREQ    "ProcessInSigFreq    "
#define CG_NAME_PEV_SENSOR_PROC_SIGNAL_OUT_FREQ   "ProcessOutSigFreq   "
#define CG_NAME_PEV_SENSOR_PROC_SIGNAL_WAIT_TIME  "ProcessSigWaitTime  "
#define CG_NAME_PEV_SENSOR_PROC_REQUEST_FREQ      "ProcessOutReqFreq   "
#define CG_NAME_PEV_SENSOR_GLOBAL_SIGNAL_FREQ     "GlobalSigFreq"
#define CG_NAME_PEV_SENSOR_GLOBAL_REQUEST_FREQ    "GlobalReqFreq"
#define CG_NAME_PEV_SENSOR_ACTIVITY               "SimpleActivity     "
#define CG_NAME_PEV_SENSOR_EVENT                  "SimpleEvent        "
#define CG_NAME_PEV_DISPLAYS                      "DisplayCreation:"
#define CG_NAME_PEV_DISPLAY_CURVES                "Curves"
#define CG_NAME_PEV_DISPLAY_FIXED_CURVES          "FixedCurves"
#define CG_NAME_PEV_DISPLAY_GANTT                 "Gantt"
#define CG_NAME_PEV_DISPLAY_FREQ                  "Freqs"

/***************************************************/
/* Numerische Konstanten fuer die Codegenerierung: */
/***************************************************/
 
enum CGNewLine
{
  CG_NO_EOL = 0,
  CG_WITH_EOL
};

#define CG_MAX_STRING 128
#define CG_LINE_LEN 80U
#define CG_BEG_COM_POS 49
#define CG_END_COM_POS 78
#define CG_MAX_PATH 200
#define CG_LINE_BREAK_POS 60

/************************************************/
/* Zeichen-Konstanten fuer die Codegenerierung: */
/************************************************/
 
#define CG_HEAD_CHAR '*'
#define CG_BLANK_CHAR  ' '
#ifdef macintosh
  #define CG_EOL_CHAR '\r'
  #define CG_PATH_SEPARATOR_CHAR ':'
#else
  #define CG_EOL_CHAR '\n'
  #define CG_PATH_SEPARATOR_CHAR '/'
#endif
#define CG_EOS_CHAR '\0'

/***********************************************/
/* String-Konstanten fuer die Codegenerierung: */
/***********************************************/
 
#define CG_BEG_COM_STRING "/*"
#define CG_END_COM_STRING "*/"
#define CG_LINE_COM " // "
#ifdef macintosh
  #define CG_PATH_SEPARATOR_STRING ":"
  #define CG_PATH_SCL_DIR ""
  #define CG_PATH_PEV_DIR ""
  #define CG_PATH_TL_DIR ""
  #define CG_EOL_STRING "\r"
#else
  #define CG_PATH_SEPARATOR_STRING "/"
  #define CG_PATH_SCL_DIR "SCL/"
  #define CG_PATH_PEV_DIR "PEV/"
  #define CG_PATH_TL_DIR "TL/"
  #define CG_EOL_STRING "\n"
#endif
#define CG_EOS_STRING "\0"

/*********************************************************************/
/* Ereignisse des QSDL-Systems (immer mit SCTraceTypes.h abgleichen) */
/*********************************************************************/
 
enum CGTraceAction
{
  cgTraceUndefined    = 0,    // Dummy fuer 0
  cgTraceSchedulerInit,       //  1 Init-Befehl fuer Scheduler
  cgTraceSchedulerStop,       //  2 Stop-Befehl fuer Scheduler
  cgTraceTimeChange,          //  3 Simulationszeit hat sich geaendert
  cgTraceProcessCreate,       //  4 Prozess wird erzeugt
  cgTraceProcessStop,         //  5 Prozess wird zerstoert
  cgTraceMachineCreate,       //  6 Maschine wird erzeugt
  cgTraceMachineStop,         //  7 Maschine wird zerstoert
  cgTraceStateChange,         //  8 Zustandswechsel
  cgTraceSignalSend,          //  9 Output an einen anderen Prozess
  cgTraceSignalNotSend,       // 10 Output an einen anderen Prozess
  cgTraceSignalReceive,       // 11 Prozess hat ein Signal empfangen
  cgTraceSignalLose,          // 12 Signal ist verlorengegangen (Queue-Ueberlauf)
  cgTraceSignalConsume,       // 13 Signal wurde konsumiert
  cgTraceSignalSave,          // 14 Signal wurde zwischengespeichert (save)
  cgTraceSignalDrop,          // 15 Signal wurde verworfen (implizite Konsumierung)
  cgTraceSpontTrans,          // 16 Spontantransition (input none) hat gefeuert
  cgTraceContSignal,          // 17 Continuous Signal hat gefeuert
  cgTraceServiceRequest,      // 18 Request von Macghine empfangen
  cgTraceServiceFinish,       // 19 Request ist beendet, Prozess wird freigegeben
  cgTraceServiceStart,        // 20 Start der Bearbeitung eines Requests
  cgTraceServiceInterrupt,    // 21 Unterbrechung der Bearbeitung eines Requests
  cgTraceTimerSet,            // 22 Timer wurde gesetzt
  cgTraceTimerReset,          // 23 Timer wurde geloescht
  cgTraceTimerFire,           // 24 Timer wurde ausgeloest
  cgTraceTimerRemove,         // 25 Timer in InputQueue geloescht
  cgTraceProcedureCall,       // 26 Prozeduraufruf
  cgTraceProcedureReturn,     // 27 Prozedurende
  cgTraceDeadlock,            // 28 Totaler Deadlock
  cgTraceCycleStart,          // 29 Zyklus (Start)
  cgTraceCycleEnd,            // 30 Zyklus (Ende)
  cgTraceMax                  // 31 Dummy fuer letzten Wert
};


#endif // ifndef _CGBASICTYPES_H_
