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
        Equal,
        LessThan,
        GreaterThan,
        NotEqual,
        Range
};

struct breakpoint
{
        breakpoint(){}
        breakpoint(int addr, BreakpointType type)
        {
                Addr = addr;
                Argument = 0xFFFF;
                Type = type;
                Condition = Equal;
                Permanent = true;
        }

        bool hit(int curAddr, BreakpointType reqType)
        {
                if (Type != reqType) return false;

                int maskedCurAddr = (reqType == BP_TSTATES) ? curAddr : (curAddr & Mask);

                switch (Condition)
                {
                        case Equal:
                                return maskedCurAddr == Addr;

                        case LessThan:
                                return maskedCurAddr < Addr;

                        case GreaterThan:
                                return (maskedCurAddr > Addr) && (maskedCurAddr <= zx81.RAMTOP);

                        case Range:
                                return (curAddr >= Addr) && (curAddr <= EndAddr);

                        case NotEqual:
                                return maskedCurAddr != Addr;
                }

                return false;
        }

        int Addr;
        union
        {
                int Argument;
                int EndAddr;
                int Count;
                int Mask;
        };
        BreakpointType Type;
        BreakpointCondition Condition;
        bool Permanent;
};

#endif // __breakpoint_h

