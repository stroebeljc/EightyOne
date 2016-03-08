#ifndef _IECBUS_H
#define _IECBUS_H

#ifdef __cplusplus
extern "C" {
#endif
extern void IECAssertReset(int DeviceNo);
extern void IECAssertATN(int DeviceNo);
extern void IECAssertClock(int DeviceNo);
extern void IECAssertData(int DeviceNo);

extern void IECReleaseReset(int DeviceNo);
extern void IECReleaseATN(int DeviceNo);
extern void IECReleaseClock(int DeviceNo);
extern void IECReleaseData(int DeviceNo);

extern int IECIsReset(void);
extern int IECIsATN(void);
extern int IECIsClock(void);
extern int IECIsData(void);

extern void IECReset(void);
extern void IECClockTick(int ts);
#ifdef __cplusplus
}
#endif

#endif

