#ifndef KEYPAD_H
#define KEYPAD_H

enum State
{
        AwaitingPollRequest,
        AwaitingPollRequestEnd,
        AwaitingScanRequest,
        AwaitingReadKeypadID,
        AwaitingReadRowFlag,
        AwaitingReadRowBits
};

enum Substate
{
        AwaitingReadBitSync,
        AwaitingReadBitSyncEnd,
        AwaitingReadBit,
        AwaitingReadBitEnd
};

class CKeypad
{
public:
        CKeypad();
        void Write(int registerByte);
        int Read();
        void ClockTick(int ts);
        void Reset();

private:
        static const int CKeypad::KeypadSocketOutLineLevelLow;
        static const int CKeypad::KeypadSocketOutLineLevelHigh;
        static const int CKeypad::KeypadSocketInLineLevelLow;
        static const int CKeypad::KeypadSocketInLineLevelHigh;

        State state;
        Substate substate;
        int row;
        int keypadSocketOut;
        int keypadSocketIn;
        int stateTimeoutTCycles;
        int bitsToSend;
        int keyRowBits[5];
        int keyBitsToSend;

        void HandleRowRead();
        int FetchRowKeys();
        int FetchRowKeys_Keyboard();
};

extern CKeypad Keypad;

#endif
