#include <Classes.hpp>
#include <SyncObjs.hpp>
#include <vector>

class TThreadPool;

class TTask {
private:
    int (*mFunc)(void *);
    void *mObject;
public:
    __fastcall TTask(int (*fcall)(void *), void *object) {
        mFunc = fcall;
        mObject = object;
    }

    void __fastcall Execute(void) {
        if (mFunc) mFunc(mObject);
    }
};

// Worker thread that pulls tasks from the pool
class TWorkerThread : public TThread {
private:
    TThreadPool* FPool;
protected:
    void __fastcall Execute();
public:
    __fastcall TWorkerThread(TThreadPool* APool);
};

// Thread pool manager
class TThreadPool {
private:
    std::vector<TWorkerThread*> FThreads;
    TThreadList* FTaskQueue;
    TCriticalSection* FTaskLock;
    HANDLE FWorkEvent; // Windows Event to signal new work
    bool FTerminating;

public:
    __fastcall TThreadPool(int MaxThreads);
    __fastcall ~TThreadPool();

    void __fastcall EnqueueTask(void* TaskData);
    void* __fastcall DequeueTask();
    bool __fastcall IsTerminating() { return FTerminating; }
    HANDLE __fastcall GetWorkEvent() { return FWorkEvent; }
};
 
