#include "ThreadPool.h"


                     // --- TWorkerThread Implementation ---
__fastcall TWorkerThread::TWorkerThread(TThreadPool* APool) 
    : TThread(true), FPool(APool) {
    FreeOnTerminate = true;
}

void __fastcall TWorkerThread::Execute() {
    while (!FPool->IsTerminating()) {
        // Wait for a task to be queued (timeout every 1s to check for termination)
        DWORD WaitResult = WaitForSingleObject(FPool->GetWorkEvent(), 1000);
        
        if (WaitResult == WAIT_OBJECT_0) {
            void* Task = FPool->DequeueTask();
            if (Task != NULL) {
                // Execute the task (cast to your specific function/object)
                // e.g., ((TMyTask*)Task)->Execute();
                ((TTask*)Task)->Execute();
            }
        }
    }
}

// --- TThreadPool Implementation ---
__fastcall TThreadPool::TThreadPool(int MaxThreads) : FTerminating(false) {
    FTaskQueue = new TThreadList();
    FTaskLock = new TCriticalSection();
    FWorkEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // Manual reset event

    for (int i = 0; i < MaxThreads; i++) {
        FThreads.push_back(new TWorkerThread(this));
        FThreads[i]->Resume();
    }
}

__fastcall TThreadPool::~TThreadPool() {
    FTerminating = true;
    SetEvent(FWorkEvent); // Wake up all threads to exit loop
    
    // Wait for all threads to terminate
    //for (size_t i = 0; i < FThreads.size(); i++) {
    //    FThreads[i]->WaitFor();
    //}

    CloseHandle(FWorkEvent);
    delete FTaskQueue;
    delete FTaskLock;
}

void __fastcall TThreadPool::EnqueueTask(void* TaskData) {
    FTaskLock->Acquire();
    try {
        FTaskQueue->Add(TaskData);
        SetEvent(FWorkEvent); // Signal to waiting workers
    } __finally {
        FTaskLock->Release();
    }
}

void* __fastcall TThreadPool::DequeueTask() {
    void* Task = NULL;
    FTaskLock->Acquire();
    try {
        TList* List = FTaskQueue->LockList();
        try {
            if (List->Count > 0) {
                Task = List->Items[0];
                List->Delete(0);
            }
        } __finally {
            FTaskQueue->UnlockList();
        }
    } __finally {
        FTaskLock->Release();
    }
    
    // If no more tasks, reset the event
    if (FTaskQueue->LockList()->Count == 0) {
        ResetEvent(FWorkEvent);
    }
    FTaskQueue->UnlockList();
    
    return Task;
}
