#ifndef _CGDECISION_H_
#define _CGDECISION_H_

#include <DS/DSDecision.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGDecision : public DSDecision
{                           
  public:
    CGDecision(DSObject* =     NULL,     // Vaterobjekt
               DSString* =     NULL,     // optionales Label
               DSExpression* = NULL,     // Question (<question>)
               DSRangeList* = NULL,      // Answer (<answer>)
               DSTransitionList* = NULL, // Transitionsliste
               DSTransition* = NULL);    // Else-Part (<else part>)
    CGDecision(DSObject*,
               DSString&,
               DSExpression* = NULL,     // Question (<question>)
               DSRangeList* = NULL,      // Answer (<answer>)
               DSTransitionList* = NULL, // Transitionsliste
               DSTransition* = NULL);    // Else-Part (<else part>)
    CGDecision(DSObject*,
               const char *,
               DSExpression* = NULL,     // Question (<question>)
               DSRangeList* = NULL,      // Answer (<answer>)
               DSTransitionList* = NULL, // Transitionsliste
               DSTransition* = NULL);    // Else-Part (<else part>)
   ~CGDecision(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  protected:
    DSResult WriteCondition(CGWriter *, DSExpression *, DSRange *) const;
    DSResult WriteIf(CGWriter *) const; 
    DSResult WriteSwitch(CGWriter *) const; 
    DSResult WriteLabel(CGWriter *) const; 
};

#endif
