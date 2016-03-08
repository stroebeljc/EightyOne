#include "Wavload_.h"
#include "WavCInterface.h"

int GetEarState()
{
        return(WavLoad->GetEarState());
}

void WavClockTick(int TStates)
{
        WavLoad->ClockTick(TStates);
}

void WavStop()
{
        WavLoad->Stop();
}

void WavStart()
{
        WavLoad->Start();
}

