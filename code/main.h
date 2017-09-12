#if !defined(MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Mohamed Shazan $
   $Notice: All Rights Reserved. $
   ======================================================================== */
#include <windows.h>
#include "imgui.h"
#include "imgui.cpp"
#include "imgui_draw.cpp"
#include "imgui_demo.cpp"
#include "imgui_impl_sdl.cpp"
#include "sqlite_helper.h"
#include <stdio.h>
#include <assert.h>
#include <SDL.h>
#include <SDL_opengl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//
// NOTE: Compilers
//
    
#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif
    
#if !defined(COMPILER_LLVM)
#define COMPILER_LLVM 0
#endif

#if !COMPILER_MSVC && !COMPILER_LLVM
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#else
// TODO: Moar compilerz!!!
#undef COMPILER_LLVM
#define COMPILER_LLVM 1
#endif
#endif

#if COMPILER_MSVC
#include <intrin.h>
#elif COMPILER_LLVM
#include <x86intrin.h>
#else
#error SEE/NEON optimizations are not available for this compiler yet!!!!
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>
    
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef intptr_t intptr;
typedef uintptr_t uintptr;

typedef size_t memory_index;
    
typedef float real32;
typedef double real64;
    
typedef int8 s8;
typedef int8 s08;
typedef int16 s16;
typedef int32 s32;
typedef int64 s64;
typedef bool32 b32;
    
typedef uint8 u8;
typedef uint8 u08;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;

typedef real32 r32;
typedef real64 r64;
typedef real32 f32;
typedef real64 f64;
    
typedef uintptr_t umm;
typedef intptr_t smm;

typedef b32 b32x;
typedef u32 u32x;
    
#define U32FromPointer(Pointer) ((u32)(memory_index)(Pointer))
#define PointerFromU32(type, Value) (type *)((memory_index)Value)

#define OffsetOf(type, Member) (umm)&(((type *)0)->Member)
#define Assert(Expression) assert(Expression)
#define U16Max 65535
#define U32Max ((u32)-1)
#define F32Max FLT_MAX
#define F32Min -FLT_MAX
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#if !defined(internal)
#define internal static
#endif
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f
#define Tau32 6.28318530717958647692f

#if COMPILER_MSVC
#define CompletePreviousReadsBeforeFutureReads _ReadBarrier()
#define CompletePreviousWritesBeforeFutureWrites _WriteBarrier()
inline uint32 AtomicCompareExchangeUInt32(uint32 volatile *Value, uint32 New, uint32 Expected)
{
    uint32 Result = _InterlockedCompareExchange((long volatile *)Value, New, Expected);

    return(Result);
}
inline u64 AtomicExchangeU64(u64 volatile *Value, u64 New)
{
    u64 Result = _InterlockedExchange64((__int64 volatile *)Value, New);

    return(Result);
}
inline u64 AtomicAddU64(u64 volatile *Value, u64 Addend)
{
    // NOTE(casey): Returns the original value _prior_ to adding
    u64 Result = _InterlockedExchangeAdd64((__int64 volatile *)Value, Addend);

    return(Result);
}
inline u32 GetThreadID(void)
{
#if _M_IX86
    u8 *ThreadLocalStorage = (u8 *)__readfsdword(0x30);
#else
    u8 *ThreadLocalStorage = (u8 *)__readgsqword(0x30);
#endif
    u32 ThreadID = *(u32 *)(ThreadLocalStorage + 0x48);

    return(ThreadID);
}

#elif COMPILER_LLVM
// TODO(casey): Does LLVM have real read-specific barriers yet?
#define CompletePreviousReadsBeforeFutureReads asm volatile("" ::: "memory")
#define CompletePreviousWritesBeforeFutureWrites asm volatile("" ::: "memory")
inline uint32 AtomicCompareExchangeUInt32(uint32 volatile *Value, uint32 New, uint32 Expected)
{
    uint32 Result = __sync_val_compare_and_swap(Value, Expected, New);

    return(Result);
}
inline u64 AtomicExchangeU64(u64 volatile *Value, u64 New)
{
    u64 Result = __sync_lock_test_and_set(Value, New);

    return(Result);
}
inline u64 AtomicAddU64(u64 volatile *Value, u64 Addend)
{
    // NOTE(casey): Returns the original value _prior_ to adding
    u64 Result = __sync_fetch_and_add(Value, Addend);

    return(Result);
}
inline u32 GetThreadID(void)
{
    u32 ThreadID;
#if defined(__APPLE__) && defined(__x86_64__)
    asm("mov %%gs:0x00,%0" : "=r"(ThreadID));
#elif defined(__i386__)
    asm("mov %%gs:0x08,%0" : "=r"(ThreadID));
#elif defined(__x86_64__)
    asm("mov %%fs:0x10,%0" : "=r"(ThreadID));
#else
#error Unsupported architecture
#endif

    return(ThreadID);
}
#else
// TODO(casey): Other compilers/platforms??
#endif
    

struct ticket_mutex
{
    u64 volatile Ticket;
    u64 volatile Serving;
};

struct platform_work_queue;
#define PLATFORM_WORK_QUEUE_CALLBACK(name) void name(platform_work_queue *Queue, void *Data)
typedef PLATFORM_WORK_QUEUE_CALLBACK(platform_work_queue_callback);
    
typedef void platform_add_entry(platform_work_queue *Queue, platform_work_queue_callback *Callback, void *Data);
typedef void platform_complete_all_work(platform_work_queue *Queue);

struct platform_work_queue_entry
{
    platform_work_queue_callback *Callback;
    void *Data;
};

struct platform_work_queue
{
    uint32 volatile CompletionGoal;
    uint32 volatile CompletionCount;

    uint32 volatile NextEntryToWrite;
    uint32 volatile NextEntryToRead;
    HANDLE SemaphoreHandle;

    platform_work_queue_entry Entries[256];
};

struct win32_thread_startup
{
    platform_work_queue *Queue;
};

internal void
Win32AddEntry(platform_work_queue *Queue, platform_work_queue_callback *Callback, void *Data)
{
    // TODO(casey): Switch to InterlockedCompareExchange eventually
    // so that any thread can add?
    uint32 NewNextEntryToWrite = (Queue->NextEntryToWrite + 1) % ArrayCount(Queue->Entries);
    Assert(NewNextEntryToWrite != Queue->NextEntryToRead);
    platform_work_queue_entry *Entry = Queue->Entries + Queue->NextEntryToWrite;
    Entry->Callback = Callback;
    Entry->Data = Data;
    ++Queue->CompletionGoal;
    _WriteBarrier();
    Queue->NextEntryToWrite = NewNextEntryToWrite;
    ReleaseSemaphore(Queue->SemaphoreHandle, 1, 0);
}

internal bool32
Win32DoNextWorkQueueEntry(platform_work_queue *Queue)
{
    bool32 WeShouldSleep = false;

    uint32 OriginalNextEntryToRead = Queue->NextEntryToRead;
    uint32 NewNextEntryToRead = (OriginalNextEntryToRead + 1) % ArrayCount(Queue->Entries);
    if(OriginalNextEntryToRead != Queue->NextEntryToWrite)
    {
        uint32 Index = InterlockedCompareExchange((LONG volatile *)&Queue->NextEntryToRead,
                                                  NewNextEntryToRead,
                                                  OriginalNextEntryToRead);
        if(Index == OriginalNextEntryToRead)
        {
            platform_work_queue_entry Entry = Queue->Entries[Index];
            Entry.Callback(Queue, Entry.Data);
            InterlockedIncrement((LONG volatile *)&Queue->CompletionCount);
        }
    }
    else
    {
        WeShouldSleep = true;
    }

    return(WeShouldSleep);
}

internal void
Win32CompleteAllWork(platform_work_queue *Queue)
{
    while(Queue->CompletionGoal != Queue->CompletionCount)
    {
        Win32DoNextWorkQueueEntry(Queue);
    }

    Queue->CompletionGoal = 0;
    Queue->CompletionCount = 0;
}

DWORD WINAPI
ThreadProc(LPVOID lpParameter)
{
    win32_thread_startup *Thread = (win32_thread_startup *)lpParameter;
    platform_work_queue *Queue = Thread->Queue;

    u32 TestThreadID = GetThreadID();
    Assert(TestThreadID == GetCurrentThreadId());
    
    for(;;)
    {
        if(Win32DoNextWorkQueueEntry(Queue))
        {
            WaitForSingleObjectEx(Queue->SemaphoreHandle, INFINITE, FALSE);
        }
    }

//    return(0);
}

internal PLATFORM_WORK_QUEUE_CALLBACK(DoWorkerWork)
{
    char Buffer[256];
    wsprintf(Buffer, "Thread %u: %s\n", GetCurrentThreadId(), (char *)Data);
    OutputDebugStringA(Buffer);
}

internal void
Win32MakeQueue(platform_work_queue *Queue, uint32 ThreadCount, win32_thread_startup *Startups)
{
    Queue->CompletionGoal = 0;
    Queue->CompletionCount = 0;

    Queue->NextEntryToWrite = 0;
    Queue->NextEntryToRead = 0;

    uint32 InitialCount = 0;
    Queue->SemaphoreHandle = CreateSemaphoreEx(0,
                                               InitialCount,
                                               ThreadCount,
                                               0, 0, SEMAPHORE_ALL_ACCESS);
    for(uint32 ThreadIndex = 0;
        ThreadIndex < ThreadCount;
        ++ThreadIndex)
    {
        win32_thread_startup *Startup = Startups + ThreadIndex;
        Startup->Queue = Queue;

        DWORD ThreadID;
        HANDLE ThreadHandle = CreateThread(0, 0, ThreadProc, Startup, 0, &ThreadID);
        CloseHandle(ThreadHandle);
    }
}

struct ImageData{
    void* Data;
    GLuint ID;
    int w;
    int h;
    int n;
};

void LoadImage(char* Filename,ImageData *Result){

    // TODO: Fix BMP and 3 channel file(RGB) loading -- opengl rendering is flipped
    Result->Data = (stbi_load(Filename,&Result->w,&Result->h,&Result->n,0));
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &Result->ID);
    glBindTexture(GL_TEXTURE_2D, Result->ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(Result->n == 3)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Result->w, Result->h, 0, GL_RGB,GL_UNSIGNED_BYTE, Result->Data);
    if(Result->n == 4)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Result->w, Result->h, 0, GL_RGBA,GL_UNSIGNED_BYTE, Result->Data);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void FreeImage(ImageData *Image){
    stbi_image_free(Image->Data);
    Image = {};
}
#define MAIN_H
#endif
