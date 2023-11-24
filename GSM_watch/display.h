#ifndef display_h
#define display_h

void displayInit(short int b);
void drawAnimatedSplash(void);
void drawStaticSplash(void);
void drawCentreString(const char *buf, int x, int y, int s);
void drawWarningScreen(void);
void drawCharingScreen(void);
void drawBanner(void);
void drawIdleScreen(void);
void drawNetworkScreen(void);
void drawIncomingCallScreen(void);
void drawInCallScreen(void);
void drawDebugMode1Screen(void);
void drawDebugMode2Screen(char c);
void generateNewScreenCoordinates(short int a, short int b);
void forceScreenToSleep(void);
void releaseScreenFromSleep(void);

#endif
