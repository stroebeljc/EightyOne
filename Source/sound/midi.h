#ifndef MIDI_H
#define MIDI_H

class CMidi
{
public:
        CMidi();
        void WriteBit(int Bit);
        void ClockTick(int ts);
        void Write(int Byte);
        void Start(void);
        void Stop(void);
        int Device;

private:
        int MidiSerialCount;
        int MidiByte;
        unsigned char MidiBuffer[1024];
        int MidiBufferLen;
        void *outHandle;     
};

extern CMidi Midi;


#endif
