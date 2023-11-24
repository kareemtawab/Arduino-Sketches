#ifndef system_h
#define system_h

void systemInit(void);
void systemRun(void);
void miscSecondHeartbeat(void);
void manageScreenBasedOnSIMStatusAndDebugMode(void);
void manageActionsBasedOnSIMStatus(void);
void resetSleepCounter(void);

#endif
