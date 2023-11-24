#ifndef sim_h
#define sim_h

void simInit(void);
void simInitSerialOnly(void);
void loopSIM(void);
void requestDataFromSIMTimerUpdate(void);
void resetSimModule(void);
void switchOffSimModule(void);
void switchOnSimModule(void);
void forceSimModuleToSleep(void);
void releaseSimModuleFromSleep(void);
void getSIMStatus(char* result);
void getSIMOperatorName(char* result);
void getSIMSignalQuality(char* result);
void getSIMCaller(char* result);
void requestSIMServiceParams(void);
char* returnRAWResultsBuffer(void);
void callFirstNumber(void);
void callSecondNumber(void);
void answerCall(void);
void hangupCall(void);

#endif
