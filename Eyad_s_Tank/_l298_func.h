#ifndef _l298_func_h
#define _l298_func_h

void l298_init(void);
void l298_moveF(int);
void l298_moveB(int);
void l298_moveR(int);
void l298_moveL(int);
void l298_moveFR(int);
void l298_moveFL(int);
void l298_moveBR(int);
void l298_moveBL(int);
void l298_stop(void);

#endif
