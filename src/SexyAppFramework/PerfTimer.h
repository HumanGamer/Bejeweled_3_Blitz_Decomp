#pragma once

#include "Common.h"

struct PerfRecord;

namespace Sexy
{
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    class PerfTimer
    {
    protected:
        LARGE_INTEGER mStart;
        double mDuration;
        bool mRunning;

        void CalcDuration();

    public:
        PerfTimer();
        void Start();
        void Stop();

        double GetDuration();

        static __int64 GetCPUSpeed(); // in Hz
        static int GetCPUSpeedMHz();
    };

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    class SexyPerf
    {
    public:
        static void BeginPerf(bool measurePerfOverhead = false);
        static void EndPerf();
        static bool IsPerfOn();

        static bool IsPerfRecording();

        static void StartTiming(const char *theName);
        static void StopTiming(const char *theName);

        static void ClearFrameInfo();

        static std::string GetResults();
    };

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    class SexyAutoPerf
    {
    public:
        const char *mName;
        bool mIsStarted;

        SexyAutoPerf(const char *theName) : mName(theName), mIsStarted(true) { SexyPerf::StartTiming(theName); }
        SexyAutoPerf(const char *theName, bool doStart) : mIsStarted(doStart), mName(theName)
        {
            if(doStart)
                SexyPerf::StartTiming(theName);
        }

        ~SexyAutoPerf() { Stop(); }

        void Start()
        {
            if(!mIsStarted)
            {
                mIsStarted = true;
                SexyPerf::StartTiming(mName);
            }
        }

        void Stop()
        {
            if(mIsStarted)
            {
                SexyPerf::StopTiming(mName);
                mIsStarted = false;
            }
        }
    };

    class PerfJournal
    {
    public:
        struct MemItem
        {
            const char* mName;
            int64 mTime;
            uint mStart;
        };

        struct DiskNode
        {
            float mDurationMS;
            unsigned __int16 mScopeIndex;
            unsigned __int16 mParentNodeRel;
            uint mCallCount;
        };

        struct DiskFrame
        {
            double mAppSeconds;
            float mDurationMS;
            uint mNodeFirst;
            unsigned __int16 mNodeCount;
            unsigned __int16 mReserved;
        };

        struct DiskStackItem
        {
            int mNodeIndex;
            double mTime;
        };

        struct Scope
        {
            const char* mName;
            double mTotalMS;
            float mLongestCallMS;
            uint mTotalCallCount;
        };

        // TODO: What the heck happened here? Is this normal?
        struct Save
        {
            struct __l2
            {
                struct Header
                {
                    unsigned int mMagic;
                    unsigned int mVersion;
                    unsigned int mScopeCount;
                    unsigned int mScopeOffset;
                    unsigned int mFrameCount;
                    unsigned int mFrameOffset;
                    unsigned int mNodeCount;
                    unsigned int mNodeOffset;
                };
            };
        };

        // TODO: Typedefs for these
        std::vector<MemItem> mMemItems;
        std::vector<DiskNode> mDiskNodes;
        std::vector<DiskFrame> mDiskFrames;
        std::vector<DiskStackItem> mDiskNodeStack;
        std::vector<Scope> mScopes;
        std::map<const char*, int> mScopeNameToIndexMap;
        std::map<std::pair<int, int>, int> mNodeScopeToChildNodeMap;
        HANDLE mThread;
        mutable __int64 mBaseTime;
        int mStackDepth;
        bool mChangeFrame;

    protected:
        int GetScopeIndex(const char* theName);
        void Flush();

    public:
        PerfJournal();
        ~PerfJournal();

        void AddItem(const PerfRecord* item);
        void Save(const char* filename);
    };
}

#pragma warning(push)
#pragma warning(disable:4005)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//#define SEXY_PERF_ENABLED
#if defined(SEXY_PERF_ENABLED) && !defined(RELEASEFINAL)

#define SEXY_PERF_BEGIN(theName) SexyPerf::StartTiming(theName)
#define SEXY_PERF_END(theName) SexyPerf::StopTiming(theName)
#define SEXY_AUTO_PERF_MULTI(theName,theSuffix) SexyAutoPerf anAutoPerf##theSuffix(theName)
#define SEXY_AUTO_PERF_2(theName,theSuffix) SEXY_AUTO_PERF_MULTI(theName,theSuffix)
#define SEXY_AUTO_PERFL(theName) SEXY_AUTO_PERF_2(theName,__LINE__) // __LINE__ doesn't work correctly if Edit-and-Continue (/ZI) is enabled
#define SEXY_AUTO_PERF(theName) SEXY_AUTO_PERF_2(theName,UNIQUE)

#define SEXY_PERF_BEGIN_COND(theName,theCond) if(theCond) SexyPerf::StartTiming(theName)
#define SEXY_PERF_END_COND(theName,theCond) if(theCond) SexyPerf::StopTiming(theName)
#define SEXY_AUTO_PERF_MULTI_COND(theName,theSuffix,theCond) SexyAutoPerf anAutoPerf##theSuffix(theName,theCond);
#define SEXY_AUTO_PERF_COND_2(theName,theSuffix,theCond) SEXY_AUTO_PERF_MULTI_COND(theName,theSuffix,theCond);
#define SEXY_AUTO_PERF_CONDL(theName) SEXY_AUTO_PERF_COND_2(theName,__LINE__,theCond)
#define SEXY_AUTO_PERF_COND(theName) SEXY_AUTO_PERF_COND_2(theName,UNIQUE,theCond)

#else

#define SEXY_PERF_BEGIN(theName)
#define SEXY_PERF_END(theName)
#define SEXY_AUTO_PERF_MULTI(theName,theSuffix)
#define SEXY_AUTO_PERF(theName)

#define SEXY_PERF_BEGIN_COND(theName,theCond)
#define SEXY_PERF_END_COND(theName,theCond)
#define SEXY_AUTO_PERF_MULTI_COND(theName,theSuffix,theCond)
#define SEXY_AUTO_PERF_COND(theName)

#endif

#pragma warning(pop)