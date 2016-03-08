#ifndef _WAVCINTERFACE
#define _WAVCINTERFACE

#ifdef __cplusplus
extern "C" {
#endif

int GetEarState();
void WavClockTick(int TStates);
void WavStop();
void WavStart();
#ifdef __cplusplus
}
#endif

#endif
