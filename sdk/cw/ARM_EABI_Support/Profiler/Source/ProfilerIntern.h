/*
  The internal defintions for the Profiler.
  
  These structures are shared between the collection library and the viewer application.
  
  Copyright © 1993-1994, 2003 Metrowerks Inc.   All rights reserved.

 */

#ifndef __PROFILERINTERN__
#define __PROFILERINTERN__

#include "Profiler.h"

#if DEBUG
#undef DEBUG
#endif

#if DEBUG
	#define _STR(x)	_VAL(x)
	#define _VAL(x)	#x
	/* Don't use !(condition) in the following because it will cause = vs == errors to not get warnings from the compiler. */
	//#define ASSERT(condition)	{ if (condition) {} else DebugStr("\p" "ASSERT: " #condition " " __FILE__ "(" __func__ "): " _STR(__LINE__)); }
	#define ASSERT(condition)	{ if (condition) {} else DebugStr("\p" "ASSERT: " #condition " " __FILE__ "(" __func__ "): " _STR(__LINE__)); }
#else
	#define ASSERT(a)	(void)0;
#endif

/* These are used to identify files created by the profiler. */

#define profileFileType 'proF'
#define profileFileSignature 'proF'

enum {
	profile68kProcessor = 1,
	profilePowerPCProcessor = 2,
	profilex86Processor	= 3				// RA/9/21/98 Unused
};

enum {
	profileOriginalVersion = 0,			/* release on DR/2 CD */
	profilerDeadVersion = 1,			/* release on DR/3 CD - not supported */
	profileVersion2 = 2,				/* adds detailRootOffset to header */
	profilerStackSpaceVersion = 3,		/* adds stackspace to records */
	profilerVarColumnWidthsVersion = 4,	/* saves variable column widths in header */
	profileCurrentVersion = 4
};

typedef char *                          Ptr;
typedef Ptr                           Handle;
// typedef bool                           Boolean;
typedef unsigned char                   Boolean;
typedef long							OSErr;
typedef long							Size;

const long noErr = 0;

#define nil NULL


struct QElem {
	struct QElem			*qLink;
	short			qType;
	short			qData[1];
};
typedef struct QElem QElem;

typedef QElem *QElemPtr;

struct QHdr {
	short			qFlags;
	QElemPtr			qHead;
	QElemPtr			qTail;
};
typedef struct QHdr QHdr;

typedef QHdr *QHdrPtr;

/* this structure is used on all machines. */

//typedef struct timeInterval
//{
//	unsigned long	hi;
//	unsigned long	lo;
//} timeInterval, *timeIntervalPtr;

typedef  unsigned long long timeInterval;
typedef unsigned long long *timeIntervalPtr;

typedef struct {
	Handle			buffer;
	void			*bufferPtr;
	long			writePosition;
	short			refNum;
	long			fileOffset;
//	ParamBlockRec	pb;
} WriteBufferType;

typedef struct memFunctionInfo
{
	char			*name;
	long			count;
	timeInterval	functionOnly;
	timeInterval	functionAndChildren;
	timeInterval	min;
	timeInterval	max;
	struct memFunctionInfo	*next;
	struct memFunctionInfo	*children;
	long			stackDepth;
	long			recursDepth;
} memFunctionInfo, *memFunctionInfoPtr;

typedef memFunctionInfo		functionInfo, *functionInfoPtr;

/* the frameInfo structure is used to track function invocations */

typedef struct frameInfo
{
	functionInfoPtr	function;
	void			*returnAddress;
	timeInterval	startTime;
	timeInterval	childrenTime;
	timeInterval	overheadTime;
#if USESROUTINEDESCRIPTORS
	void			*savedTOC;
#endif
	void			*stackPointer;
} frameInfo, *frameInfoPtr;

typedef struct ProfilerThread
{
	frameInfoPtr		stackTop;
	long				threadSignature;	// const ThreadSignature
	QElem				queueElement;
	Boolean				hasBeenSwitchedOut;
	Boolean				active;
	Boolean				deleteMe;
	Boolean				unused;
	frameInfoPtr		stackEnd;
	frameInfoPtr		stackMaxPoint;
	long				maxStackDepth;
	timeInterval		stopTime;
	timeInterval		offsetTime;
	Ptr					stackBase;
	Ptr					stackBottom;
	long				stackSize;
	// NO MP SUPPORT
	//ThreadID			threadID;
	frameInfo			frameBuffer[1];
} ProfilerThread, *ProfilerThreadPtr;

typedef long	Semaphore;

#define kHashTableSize   512

typedef struct 
{
	char			*name;
	functionInfoPtr	function;
}Bucket;

#define kBucketGroupSize 4

struct BucketGroup;
struct BucketGroup
{
	Bucket		buckets[kBucketGroupSize];
	BucketGroup	*next;
};

struct HashTable
{
	BucketGroup		table[kHashTableSize];
	BucketGroup		*additions;	//@@@@
	BucketGroup		*lastAddition;
};

typedef struct NameOffsetEntry
{
	char	*name;
	long	offset;
} NameOffsetEntry, *NameOffsetEntryPtr;

/* this structure is the header structure for the saved files. */

#define NUMRESERVEDV3 9

typedef struct profileHeaderV3
{
	long				signature;
	short				version;					/* version 1 adds detailedRootOffset */
	short				processor;					/* profile68kVersion or profilePPCVersion */
	long				numRecords;
	short				method;						/* CollectionMethod */
	short				timebase;					/* TimeBase */
	long				overflowFunctions;
	long				overflowStack;
	unsigned long		dateSaved;
	long				nameBaseOffset;				/* offset to name table in file */
	long				nameSize;					/* size of name table */
	timeInterval		totalOverhead;
	long				detailedRootOffset;
	long				initialNameTableSize;
	long				initialStackTableSize;
	long				tbConversionFactor;
	long				reserved[NUMRESERVEDV3];	/* 96 byte header */
} profileHeaderV3, *profileHeaderV3Ptr;

typedef struct profileHeader
{
	long				signature;
	short				version;					/* version 1 adds detailedRootOffset */
	short				processor;					/* profile68kVersion or profilePPCVersion */
	long				numRecords;
	short				method;						/* CollectionMethod */
	short				timebase;					/* TimeBase */
	long				overflowFunctions;
	long				overflowStack;
	unsigned long		dateSaved;
	long				nameBaseOffset;				/* offset to name table in file */
	long				nameSize;					/* size of name table */
	timeInterval		totalOverhead;
	long				detailedRootOffset;
	long				initialNameTableSize;
	long				initialStackTableSize;
	long				tbConversionFactor;
	short				columnWidths[16];
	long				reserved[105];				/* 512 byte header */
} profileHeader, *profileHeaderPtr;

typedef unsigned long ProfilerThreadRef;

typedef struct ProfilerGlobals
{
	Boolean				profilerStatus;

	Boolean				timeBaseStarted;
	void				(*getCurrentTime)(timeIntervalPtr time);
	timeInterval		getTimeOverHead;

//	Handle				functionBufferHandle;
	functionInfoPtr		functionBufferPtr;

//	Handle				nameOffsetHandle;
	Handle				nameOffsetPtr;
	long				nameOffsetSize;

	ProfilerThreadRef	mainThreadRef;
	ProfilerThreadPtr	threadStack;
	
	long				maxFunctions;

	long				tempOverflowStack;

	struct profileHeader	header;

	QHdr				emptyQueue;
	QHdr				insertQueue;
	Handle				queueHandle;

	Semaphore			addSemaphore;

	timeInterval		startPause;

	QHdr				threadQueue;
	Boolean				removeThreadProcs;
	// ProcessSerialNumber	theProcess;
	HashTable			hashTable;

	// NO MP SUPPORT
	
// these must be at the end for 68k
	long				myCheckGlobal;
	char				fakemain[10];
} ProfilerGlobals, *ProfilerGlobalsPtr;

/* this structure is the repeating section in the file following the header */

typedef struct diskFunctionInfoV2
{
	long			nameOffset;
	long			count;
	timeInterval	functionOnly;
	timeInterval	functionAndChildren;
	timeInterval	min;
	timeInterval	max;
	long			next;
	long			children;
} diskFunctionInfoV2, *diskFunctionInfoV2Ptr;

typedef struct diskFunctionInfoV3
{
	long			nameOffset;
	long			count;
	timeInterval	functionOnly;
	timeInterval	functionAndChildren;
	timeInterval	min;
	timeInterval	max;
	long			next;
	long			children;
	long			stackSpace;
} diskFunctionInfoV3, *diskFunctionInfoV3Ptr;


/* --------------------------- Math Support functions ------------------- */

inline void maxTime(timeIntervalPtr t)
{
	*t = 0xffffffffffffffff;
} // maxTime

#define ThreadSignature 'thrd'

#ifdef __cplusplus
	extern "C" {
#endif

// Core

ProfilerGlobalsPtr GetGlobalsAddress(void);
OSErr WriteData(ProfilerGlobalsPtr globals, short refNum, memFunctionInfo *functionBase,
	 long limit, long *nameSizeParam, long functOffset, long diskOffset);
void CalcFinalTime(ProfilerThreadPtr threadPtr, ProfilerGlobalsPtr globals,
						  timeInterval *overheadStart, functionInfoPtr function);
						  
// Lowlevel 64-bit timer support

inline void zeroTime(timeIntervalPtr t)
{
	*t = 0x0;
} // zeroTime


inline void addTime(timeIntervalPtr dest, timeIntervalPtr src1, timeIntervalPtr src2)
{
	*dest = *src1 + *src2;
}
inline void subTime(timeIntervalPtr dest, timeIntervalPtr src1, timeIntervalPtr src2)
{
	*dest = *src1 - *src2;
}

;
inline int cmpTime(timeIntervalPtr src1, timeIntervalPtr src2)
{
	if(*src1<*src2) 
		return 1;
	else
		return 0;
}

double timeIntervalToDouble(timeIntervalPtr t, ProfilerTimeBase tb);

// Memory support

Handle AllocateBuffer(Size size, OSErr *error);
void ReleaseBuffer(Handle *h);

// IO Support

OSErr FlushWriteBuffer(WriteBufferType *buffer, Boolean flush);

// Thread Support

pascal OSErr ProfilerCreateThread(short stackSize, long byteStackSize, ProfilerThreadRef *threadRef);
pascal void ProfilerDeleteThread(ProfilerThreadRef thread);
pascal OSErr ProfilerCreateMainThread(short stackSize, ProfilerThreadRef *threadRef);
// static pascal void DebuggerNewThreadCallback(ThreadID threadCreated);

// Timer Support

static void microsecondsGetTime(timeIntervalPtr t);
static void ticksGetTime(timeIntervalPtr t);
static void timeMgrGetTime(timeIntervalPtr t);
OSErr startTimeBase(ProfilerGlobalsPtr globals, short *tb);
void stopTimeBase(ProfilerGlobalsPtr globals);

#define kProfileBufferSymbolName "gCWProfileLibraryBuffer"

struct CWProfileLibraryBuffer {

	long m_version;
	
	long m_dataWaiting;
	long m_writeAtBeginning;
	unsigned long m_writeBufferAddress;
	unsigned long m_writeBufferLength;
	char m_writeBufferName[512];

};
// Constants for m_dataWaiting
const long kNoProfileData = 0;
const long kProfileDataReady = 1;
const long kProfileDataDone = 2;

//CWProfileLibraryBuffer gCWProfileLibraryBuffer = {1, 0, 0, 0, 0};
// Trap Support (temporary)

/*
static short NumToolboxTraps(void);
static TrapType GetTrapType(short theTrap);

#if TARGET_API_MAC_CARBON
	#define TrapAvailable(theTrap) TRUE
#else
	static Boolean TrapAvailable(short theTrap);
#endif
*/

#ifdef __cplusplus
	}
#endif

#endif /* __PROFILERINTERN__ */
