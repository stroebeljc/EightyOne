#ifndef __breakpoint_h
#define __breakpoint_h

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

        bool hit(int Addr, BreakpointType type)
        {
                if (type != Type) return false;
                switch (Condition)
                {
                        case Equal:
                                return false;

                        case LessThan:
                                return false;

                        case GreaterThan:
                                return false;
                                
                        case InRange:
                                return false;
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

