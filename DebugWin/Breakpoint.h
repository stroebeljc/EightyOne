#ifndef __breakpoint_h
#define __breakpoint_h

#include "zx81config.h"

enum BreakpointType
{
        BP_EXE,
        BP_RD,
        BP_WR,
        BP_IN,
        BP_OUT,
        BP_TSTATES
};

enum BreakpointCondition
{
        LessThan,
        Equal,
        GreaterThan,
        InRange
};

struct breakpoint
{
        breakpoint(){}
        breakpoint(int addr, BreakpointType type)
        {
                Addr = addr;
                AddrHi = addr;
                Type = type;
                Condition = Equal;
                Permanent = true;
                Count = 1;
        }

        bool hit(int curAddr, BreakpointType reqType)
        {
                if (Type != reqType) return false;

                switch (Condition)
                {
                        case Equal:
                                return curAddr == Addr;

                        case LessThan:
                                return curAddr < Addr;

                        case GreaterThan:
                                return curAddr > Addr && curAddr <= zx81.RAMTOP;

                        case InRange:
                                return curAddr >= Addr && AddrHi > curAddr;
                }

                return false;
        }

        int Addr;
        int AddrHi;
        BreakpointType Type;
        BreakpointCondition Condition;
        bool Permanent;
        int Count;
};

#endif // __breakpoint_h

