//---------------------------------------------------------------------------

#ifndef kbstatusH
#define kbstatusH
//---------------------------------------------------------------------------

extern void PCKeyDown(WORD key);
extern void PCKeyUp(WORD key);
extern void PCKeySetCursor(char left, char down, char right, char up, int shift);
extern void PCKeySetCTRL(char key);
extern void PCAllKeysUp(void);
extern void PCKbInit(void);
#endif
