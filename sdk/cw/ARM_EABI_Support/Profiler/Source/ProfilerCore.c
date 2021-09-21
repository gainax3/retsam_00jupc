/*
 *  ProfilerCore.c
 *
 *  Copyright (c) 1993-2003 Metrowerks Inc.  All rights reserved.
 *	METROWERKS CONFIDENTIAL
 *
 *	Based on work by Steve Nicholai & Neil Trautman
 *	ratwell wuz here
 *
 */

// NO MP SUPPORT

// Added next macro
#define USEOLDSUMMARYMETHOD 0

#include <limits.h>
#include <stddef.h>
#include <stdio.h>

#include "Profiler.h"
#include "ProfilerIntern.h"

#include <stdlib.h>
#include <pool_alloc.h>

// NO COMPATIBILITYMODE

#if SHAREDLIBRARY
	#pragma export list ProfilerInit
	#pragma export list ProfilerTerm
	#pragma export list ProfilerSetStatus
	#pragma export list ProfilerGetStatus
	#pragma export list ProfilerGetDataSizes
	#pragma export list ProfilerDump
	#pragma export list ProfilerClear
	#pragma export list ProfilerCreateThread
	#pragma export list ProfilerDeleteThread
	#pragma export list ProfilerGetMainThreadRef
	#pragma export list ProfilerSwitchToThread
	
	// NO COMPATIBILITYMODE

	#pragma export list __PROFILE_ENTRY
	#pragma export list __PROFILE_EXIT

	// NO MP SUPPORT
#endif

// bmarine1: EXCEPTIONSUPPORT set to 0 (is this right?)
#define EXCEPTIONSUPPORT 0

// bmarine1: added SWAP(x) to be compatible with the way the IDE views the timeInterval */
// the higher part of the long long must ALWAYS come first in memory 
#ifdef LITTLE_ENDIAN
  #define SWAP(x) (timeInterval)( ( ( (x) & 0xFFFFFFFF00000000ull ) >> 32 ) | ( ( (x) & 0xFFFFFFFFull ) << 32 ) )
#else
  #define SWAP(x) x
#endif

/* ======================== Private Structures ====================== */

/* the memFunctionInfo structure is used to maintain function information
   in memory.  Its information is copied to a functionInfo structure to
   be written to disk.
 */


/* The InsertFunction structure stores a function that will be inserted into the
   list of functions (only in summary collection mode)
 */
typedef struct insertFunction {
	struct QElem				*qLink;
	short						qType;
	memFunctionInfo				info;
} insertFunction, *insertFunctionPtr;

#define INSERTQUEUESIZE	10
#define PROFILERQUEUETYPE	777		/* just pick any number greater than 128? */

// NO 68K
void __profileexit(void);


/* A semaphore 1 is cleared, and 0 is set. */
 
#define kInitialSemaphoreValue	1

#ifdef __cplusplus
extern "C" {
#endif

// NO 68K

extern "C" void getTime( timeIntervalPtr );
extern "C" long getTimerResolution( void );

void stopTimeBase(ProfilerGlobalsPtr globals)
{
	
	
}

static long FetchAndAdd(long *value, int increment = 1)
{
	return *value += increment;
} // FetchAndAdd

static asm void InsertIntoList(register void **head, register int offset, register void *newNode)
{
// Exactly how atomic should this function be ?
// Must it be written an ASM or is a C implementation enough?
// For now the function is written in ASM just to keep the original signature
// The function must implement this sequence:
//   newNode -> next = framePtr -> fct -> children;
//   framePtr -> fct -> children = newNode;

  ldr    r3, [ r0, #0 ]                // r3 = framePtr -> fct -> children
  str    r3, [ r1, r2 ]                // *( NewNode + offset ) = r3
  str    r2, [ r0, #0 ]                // framePtr -> fct -> children = newNode
  #ifdef THUMB_MODE
    bx     lr
  #else
    mov    pc, lr
  #endif
} 


/* the uninterruptable add time doesn't need to be fully uninterruptable,
   it just needs to handle two add times happening at "same" time.  This
   means that each half of the add time could be done indivisibly, but
   will an allowed division in between the two halves.
 */

// NO 68K
//void UninterruptableAddTime(timeIntervalPtr value, timeIntervalPtr dest);

inline void UninterruptableAddTime(timeIntervalPtr value, timeIntervalPtr dest)
{
	*dest += *value;
}

#ifdef __cplusplus
}
#endif

/* ========================== Local Variables ======================= */

static inline BucketGroup *hash(HashTable *table, char *functionName)
{
	int		index = (((unsigned long) functionName) >> 2) & (kHashTableSize - 1);
	
	return &table->table[index];
}

static ProfilerGlobals		profilerGlobals;



/* =========================== Private Macros ======================= */

#define max(a, b)	((a) > (b) ? (a) : (b))

/* ========================= Private Functions ====================== */

static long my_strlen(char *s)
{
	char *p = s;
	while (*p++)
		;
	return  p - s;
} // my_strlen

/* ------------------------------------------------------------------
 * GetGlobalsAddress
 *
 * On PowerPC globals are stored in a global variable.
 *
 * On 68k, globals are stored in the code.  This is done to support
 * all code models.
 * -----------------------------------------------------------------*/

ProfilerGlobalsPtr GetGlobalsAddress(void)
{
	return &profilerGlobals;
} /* GetGlobalsAddress */

// NO 68K

// NO COMPATIBILITYMODE
// NO 68K

// NO MP SUPPORT

// NO MP SUPPORT

#if USETHREADHOOKS
/* ------------------------------------------------------------------
 * FindThread
 *
 * Given a Thread Manager threadID, find the thread structure for that
 * threadID.
 * -----------------------------------------------------------------*/

static ProfilerThreadPtr FindThread(ProfilerGlobalsPtr globals, ThreadID threadID)
{
	QElemPtr	q = globals->threadQueue.qHead;
	ProfilerThreadPtr	threadPtr;
	
	while (q != NULL)
	{
		threadPtr = (ProfilerThreadPtr)((Ptr)q - offsetof(ProfilerThread, queueElement));
		if (threadPtr->threadID == threadID)
			return threadPtr;
		q = q->qLink;
	} // while not at end of list
	
	// not on the list, allocate a new one and add it to the list??
	ASSERT(0);
	
	return NULL;
} // FindThread

/* ------------------------------------------------------------------
 * DebuggerDisposeThreadCallback
 *
 * This notification routine is registered with the Thread Manager
 * and is called when a thread is disposed.
 * -----------------------------------------------------------------*/

static pascal void DebuggerDisposeThreadCallback(ThreadID threadDeleted)
{
	ProfilerGlobalsPtr globals = GetGlobalsAddress();
	ProfilerThreadPtr threadPtr = FindThread(globals, threadDeleted);
	if (threadPtr != NULL)
	{
		threadPtr->deleteMe = true;
	}
} // DebuggerDisposeThreadCallback

/* ------------------------------------------------------------------
 * DebuggerThreadSchedulerCallback
 *
 * This notification routine is registered with the Thread Manager
 * and is called when a new thread is scheduled to execute.
 * The profiler has to allocate the time to each thread.  So it needs
 * to know when threads start and stop.
 * -----------------------------------------------------------------*/

static pascal ThreadID DebuggerThreadSchedulerCallback(SchedulerInfoRecPtr schedulerInfo)
{
	ThreadID	curThread;
	OSErr error = GetCurrentThread(&curThread);

	ProfilerGlobalsPtr globals = GetGlobalsAddress();
	ProfilerThreadPtr  currentThread = globals->threadStack;
	ProfilerThreadPtr threadPtr = FindThread(globals, schedulerInfo->SuggestedThreadID);
	ProfilerSwitchToThread((ProfilerThreadRef) threadPtr);
	if (currentThread->deleteMe)
	{
		ProfilerDeleteThread((ProfilerThreadRef) threadPtr);
	}

	return schedulerInfo->SuggestedThreadID;	// don't change the scheduler decision
} // DebuggerThreadSchedulerCallback
#endif

/* ------------------------------------------------------------------
 * fillInFunction
 *
 * Fill in a fuction table entry with initial (blank) data.
 * -----------------------------------------------------------------*/

static void fillInFunction(functionInfoPtr function, char *functionName, frameInfoPtr framePtr)
{
	function->count = 0L;
	zeroTime(&function->functionOnly);
	zeroTime(&function->functionAndChildren);
	maxTime(&function->min);
	zeroTime(&function->max);
	function->children = NULL;
	if (framePtr)
	{
		InsertIntoList((void **) &framePtr->function->children, offsetof(functionInfo, next), function);
	}
	else
		function->next = NULL;
	function->stackDepth = 0L;
	function->recursDepth = 0L;

	function->name = functionName;		// Do name last so interrupt routines don't find it until
										// it's filled out.										
} // fillInFunction

#if !USEOLDSUMMARYMETHOD
/* ------------------------------------------------------------------
 * FindInBucket
 *
 * find the spot in a summary function table.
 * -----------------------------------------------------------------*/

static Bucket *FindInBucket(ProfilerGlobalsPtr globals, BucketGroup *bucketGroup, char *functionName)
{
	BucketGroup *prev;

	ASSERT(bucketGroup != NULL);
	
	while (bucketGroup != NULL)
	{
		Bucket		*bucket = &bucketGroup->buckets[0];
		int			i = 0;

		while (i++ < kBucketGroupSize)
		{
			if (!bucket->name)   //@@@ this and the set below need to be atomic
			{
				if (globals->header.numRecords < globals->maxFunctions)
				{
					bucket->name = functionName;
					long where = FetchAndAdd(&globals->header.numRecords);
					bucket->function = &globals->functionBufferPtr[where];
					fillInFunction(bucket->function, functionName, NULL);
					return bucket;		
				}
				else
					return NULL;

			}// if at the end of the list
			
			if (bucket->name == functionName)
			{
				return bucket;
			} // if it's the one
			bucket++;
		} // if not looked at every bucket in the group
		prev = bucketGroup;
		bucketGroup = bucketGroup->next;
	} // while not at the end of group list
	
	//@@@ The linking into the list needs to be atomic with the "while (bucketGroup != NULL)" above
	// add a new bucket group
	BucketGroup *bg = (BucketGroup *)FetchAndAdd((long *)&globals->hashTable.additions, sizeof(BucketGroup));
	if (bg < globals->hashTable.lastAddition)
	{
		if (globals->header.numRecords < globals->maxFunctions)
		{
			prev->next = bg;
			bg->next = NULL;
			Bucket *bucket = &bg->buckets[0];
			bucket->name = functionName;
			long where = FetchAndAdd(&globals->header.numRecords);
			bucket->function = &globals->functionBufferPtr[where];
			fillInFunction(bucket->function, functionName, NULL);
			return bucket;
		}
		else
			return NULL;
	}
	else
		return NULL;		// no more room in the table
} // FindInBucket

/* ------------------------------------------------------------------
 * findInHashTable
 *
 * find the spot in a summary function table.
 * -----------------------------------------------------------------*/

static Boolean findInHashTable(ProfilerGlobalsPtr globals, char *functionName, functionInfoPtr *where)
{
	BucketGroup *bucketGroup = hash(&globals->hashTable, functionName);
	Bucket		*bucket = FindInBucket(globals, bucketGroup, functionName);
	
	if (bucket)
	{
		*where = bucket->function;
		return true;
	}
	
	return false;
} // findInHashTable

#else /* USEOLDSUMMARYMETHOD */
/* ------------------------------------------------------------------
 * findInTable
 *
 * find the spot in a summary function table.
 * -----------------------------------------------------------------*/

static Boolean findInTable(ProfilerGlobalsPtr globals, char *functionName, functionInfoPtr *where)
{
	long				lo = 0;
	long				hi = globals->header.numRecords + 1;
	long				mid;
	functionInfoPtr		function;
	ptrdiff_t 			cmp;
	
	function = globals->functionBufferPtr;
	while (lo < hi)
	{
		mid = (lo + hi - 1) >> 1;
		function = &globals->functionBufferPtr[mid];
		cmp = function->name - functionName;
		if (cmp == 0)
		{
			*where = function;
			return true;
		}
		else if (cmp > 0)
		{
			hi = mid;
		}
		else
			lo = mid + 1;
	}
	
	*where = &globals->functionBufferPtr[lo];
	return false;
} // findInTable

/* ------------------------------------------------------------------
 * InsertAt
 *
 * find the spot in a summary function table.
 * -----------------------------------------------------------------*/

static Boolean InsertAt(ProfilerGlobalsPtr globals, functionInfoPtr function)
{
	frameInfoPtr	f;

	if (globals->header.numRecords < globals->maxFunctions)
	{
		globals->header.numRecords++;
		BlockMoveData(function, function + 1, (globals->header.numRecords) * sizeof(functionInfo) - ((Ptr)function - (Ptr)globals->functionBufferPtr));
		
		/* Update pointers in stack that point to moved functions */
		
		//@@@@ for all threads
		for (f = globals->threadStack->frameBuffer; f <= globals->threadStack->stackTop; f++)
			if (f->function >= function)
				f->function++;
	}
	else
	{
		globals->header.overflowFunctions++;
		return false;
	}
	
	return true;
} // InsertAt

#endif /* USEOLDSUMMARYMETHOD */

/* ------------------------------------------------------------------
 * findFunction
 *
 * This function calculates which bucket the function should be placed in.  It has
 * two different techniques based on which collectionMethod is in use.
 * 
 * The collectDetailed method uses a tree data structure to store the time in
 * a function based on where it is called from.
 * 
 * The collectSummary method uses a simple list to track the time spent in each
 * function.
 * -----------------------------------------------------------------*/

static functionInfoPtr findFunction(ProfilerGlobalsPtr globals, frameInfoPtr framePtr, char *functionName)
{
	functionInfoPtr		function;
	Boolean				foundit = false;
	
	if (globals->header.method == collectDetailed)
	{
		function = framePtr->function;	
		if (function)
			function = function->children;

		while (function)
		{
			if (function->name == functionName)
			{
				foundit = true;
				break;
			}
			function = function->next;
		}

		if (!foundit)
		{
			if (globals->header.numRecords < globals->maxFunctions)
			{
				long where = FetchAndAdd(&globals->header.numRecords);
				function = &globals->functionBufferPtr[where];
			}
			else
			{
				globals->header.overflowFunctions++;
				return NULL;
			}
		}
	}
	else /* collectSummary */
	{
//@@@ until I get this call fully MP enabled, grab a semaphore here.

#if !USEOLDSUMMARYMETHOD
		foundit = findInHashTable(globals, functionName, &function);
		if (!foundit)
		{
			globals->header.overflowFunctions++;
			return NULL;
		}
#else
		insertFunctionPtr		p;

		foundit = findInTable(globals, functionName, &function);
		if (!foundit)
		{
			if (TestAndSetSemaphore(&globals->addSemaphore))
			{
				if (!InsertAt(globals, function))
				{
					ClearSemaphore(&globals->addSemaphore);
					return NULL;
				}
/* uncomment the following line to test the interruptablity of the algorithm. */				
//				findFunction(NULL, "testing async");
				
				/* pull everything from insertQueue, need to handle duplicates here */

				p = (insertFunctionPtr) globals->insertQueue.qHead;
				while (p)
				{
					Boolean tempFoundIt;
					functionInfoPtr	tempFunction;
					
					Dequeue((QElemPtr) p, &globals->insertQueue);
					
					tempFoundIt = findInTable(globals, p->info.name, &tempFunction);
					
					if (!tempFoundIt)
					{
						if (!InsertAt(globals, tempFunction))
						{
							ClearSemaphore(&globals->addSemaphore);
							return NULL;
						}
						*tempFunction = p->info;
					}
					
					Enqueue((QElemPtr) p, &globals->emptyQueue);
					p = (insertFunctionPtr) globals->insertQueue.qHead;
				}
				ClearSemaphore(&globals->addSemaphore);
			}
			else
			{	/* put on the insert queue */
#if DEBUG
				DebugStr("\pput on insert queue");
#endif
				p = (insertFunctionPtr) globals->emptyQueue.qHead;
				if (p)
				{
					if (Dequeue((QElemPtr) p, &globals->emptyQueue) == noErr)
					{
						function = &p->info;
						fillInFunction(function, functionName, NULL);
						Enqueue((QElemPtr) p, &globals->insertQueue);
					}
#if DEBUG
					else
						DebugStr("\pemptyQueue Dequeue failed");
#endif
				}
#if DEBUG
				else
					DebugStr("\pemptyQueue empty");
#endif
			}
		}
//@@@ and release the semaphore here
#endif /*!USEOLDSUMMARYMETHOD */
	}

	if (!foundit)
	{
		fillInFunction(function, functionName, (globals->header.method == collectDetailed) ? framePtr : NULL);
	}

	return function;
} // findFunction

/* ------------------------------------------------------------------
 * SetupInitialTables
 *
 * Setup the tables and the header to clear out any data that has been
 * collected.
 * -----------------------------------------------------------------*/

static void SetupInitialTables(ProfilerGlobalsPtr globals)
{
	globals->functionBufferPtr = (functionInfoPtr) globals->functionBufferPtr;
	
	if (globals->header.method == collectDetailed)		/* must initialize a dummy root on stack */
	{									/* and connect it to a dummy function    */
		functionInfoPtr	function;
		
		globals->header.numRecords = 0;
		function = &globals->functionBufferPtr[0];
#if TARGET_RT_MAC_CFM
		function->name = "fake main";
#else
		function->name = globals->fakemain;
#endif
		function->count = 0;
		zeroTime(&function->functionOnly);
		zeroTime(&function->functionAndChildren);
		zeroTime(&function->min);
		maxTime(&function->max);
		function->next = NULL;
		function->children = NULL;
	}
	else
		globals->header.numRecords = -1;
} // SetupInitialTables

/* ------------------------------------------------------------------
 * ProfilerDeleteAllThreads
 *
 * Delete all currently active profiler parallel stack structures.
 * There is one for every thread.  This doesn't delete the ones
 * on the task queue.
 * -----------------------------------------------------------------*/

static void ProfilerDeleteAllThreads()
{
} // ProfilerDeleteAllThreads

/* ------------------------------------------------------------------
 * FindInOffsetTable
 *
 * Check to see if the name is the same as one that has come before.  This
 * checks to see if the name is at the same address, not if the name is the
 * same.  This way different static functions with the same name will have
 * different name table entries.
 * -----------------------------------------------------------------*/

static long FindInOffsetTable(ProfilerGlobalsPtr globals, NameOffsetEntryPtr nameOffsetPtr, char *name, long newOffset)
{
	NameOffsetEntryPtr	hashPosition = &nameOffsetPtr[(unsigned long)name % globals->nameOffsetSize];
	NameOffsetEntryPtr	endPosition = &nameOffsetPtr[globals->nameOffsetSize];
	NameOffsetEntryPtr	checkPosition = hashPosition;
	
	while (true)
	{
		if (checkPosition->name == name)
		{
			return checkPosition->offset;
		} // if found this name
		if (checkPosition->name == NULL)
		{
			checkPosition->name = name;
			checkPosition->offset = newOffset;
			return -1L;  // indicate that it's not shared
		} // if position is empty
		checkPosition++;
		if (checkPosition == endPosition)
		{
			checkPosition = nameOffsetPtr;
			continue;
		} // if at end, move to beginning
		if (checkPosition == hashPosition)
		{
			ASSERT(checkPosition != hashPosition);	// table is not full
			break;
		} // if got back to starting position, table is full
	} // look for function
		
	return -1L;
} // FindInOffsetTable

enum {
//	kWriteBufferSize = 32768 + 511
	kWriteBufferSize = 1024 * 4 + 511
};

/* ------------------------------------------------------------------
 * InitializeWriteBuffer
 *
 * Initialize a structure for buffered writing.  This makes up for the
 * poor buffering of the MacOS filesystem.
 * -----------------------------------------------------------------*/

static OSErr InitializeWriteBuffer(WriteBufferType *buffer, short refNum, long startPosition)
{
	OSErr	error;

	buffer->buffer = AllocateBuffer(kWriteBufferSize, &error);
	if (error == 0)
	{
		buffer->bufferPtr = buffer->buffer;
		buffer->writePosition = 0L;
		
		buffer->refNum = refNum;
		buffer->fileOffset = startPosition;
	}
	
	return error;
} // InitializeWriteBuffer

/* ------------------------------------------------------------------
 * WriteToBuffer
 *
 * Put the bytes in the buffer.
 * -----------------------------------------------------------------*/

static OSErr WriteToBuffer(WriteBufferType *buffer, void *info, short size)
{
	OSErr	error = noErr;
	
	if (buffer->writePosition + size > kWriteBufferSize)
		error = FlushWriteBuffer(buffer, false);
	
	if (error == noErr)
	{
		short	i;
		char	*p = (char *)info;
		char	*q = (char *)buffer->bufferPtr + buffer->writePosition;
		
		for (i = 0; i < size; i++)
			*q++ = *p++;
		buffer->writePosition += size;
	}
	
	return error;
} // WriteToBuffer

/* ------------------------------------------------------------------
 * WriteData
 *
 * The file is open already, write the data out to the file.
 * -----------------------------------------------------------------*/

OSErr WriteData(ProfilerGlobalsPtr globals, short refNum, memFunctionInfo *functionBase,
	 long limit, long *nameSizeParam, long functOffset, long diskOffset)
{
	OSErr				error = noErr;
	long				i;
	diskFunctionInfoV3	fInfo;
	functionInfoPtr		fp;
	long				Shared = -1L;
	NameOffsetEntryPtr	nameOffsetPtr;
	long				nameOffset = 0L;
	WriteBufferType		functions;
	WriteBufferType		names;
    
	nameOffsetPtr = (NameOffsetEntryPtr)globals->nameOffsetPtr;

	error = InitializeWriteBuffer(&functions, refNum, functOffset);
	if (error == noErr)
	{
		error = InitializeWriteBuffer(&names, refNum, diskOffset);
		if (error == noErr)
		{
			if (globals->header.method == collectSummary)
				i = 0;	/* don't skip first element */
			else
				i = 1;	/* collect detailed - skip the first element, fake main */
			
			for (; error == noErr && i < limit; i++)
			{
				fp = &globals->functionBufferPtr[i];
							
			    // to be removed: for debugging only
//				printf( "Function name: %s\n", fp -> name );
//				printf( "         time: %lld\n", fp -> functionOnly );
//				printf( "         with children: %lld\n\n", fp -> functionAndChildren );
		
				if (globals->header.method == collectDetailed)
				{
					Shared = FindInOffsetTable(globals, nameOffsetPtr, fp->name, nameOffset);
				} // 
				
				if (Shared != -1L)
				{
					ASSERT(Shared >= 0 && Shared <= nameOffset);
					fInfo.nameOffset = Shared;
				}
				else
				{
					fInfo.nameOffset = nameOffset;
				}
					
				fInfo.count = fp->count;
				fInfo.functionOnly = SWAP( fp->functionOnly );
				fInfo.functionAndChildren = SWAP( fp->functionAndChildren );
				fInfo.min = SWAP( fp->min );
				fInfo.max = SWAP( fp->max );
				fInfo.next = fp->next ? (fp->next - functionBase) : -1L;
				ASSERT(fInfo.next == -1L || (fInfo.next > i && fInfo.next < limit));
				fInfo.children = fp->children ? (fp->children - functionBase) : -1L;
				ASSERT(fInfo.children == -1L || (fInfo.children > i && fInfo.children < limit));
				fInfo.stackSpace = fp->stackDepth;
			
				error = WriteToBuffer(&functions, &fInfo, sizeof(fInfo));
				if (error == noErr && Shared == -1L)
				{
					long	size;

					size = my_strlen(fp->name);
					error = WriteToBuffer(&names, fp->name, size);
					nameOffset += size;
				}
			}
			error = FlushWriteBuffer(&functions, true);
			if (error == noErr)
				error = FlushWriteBuffer(&names, true);
		}
	}
	*nameSizeParam = nameOffset;
	return error;
} // WriteData

/* ------------------------------------------------------------------
 * CalcFinalTime
 *
 * Calculate the amount of time the function took. 
 * -----------------------------------------------------------------*/

// NO MP SUPPORT
void CalcFinalTime(ProfilerThreadPtr threadPtr, ProfilerGlobalsPtr globals,
						  timeInterval *overheadStart, functionInfoPtr function)
{
	timeInterval	temp;
	frameInfoPtr	localStackTop = threadPtr->stackTop;		


// gcm start changes.
	// remove the time offset for thread switching
	if (cmpTime(&threadPtr->offsetTime, overheadStart))
		subTime(&temp, overheadStart, &threadPtr->offsetTime);
	else
		zeroTime(&temp);		

//	temp = (overheadStart - stackTop->overheadTime) - stackTop->startTime;
	if (cmpTime(&localStackTop->startTime, &temp))
		subTime(&temp, &temp, &localStackTop->startTime);
	else
		zeroTime(&temp);		    		
		
// gcm end changes.

	if (cmpTime(&localStackTop->overheadTime, &temp))
		subTime(&temp, &temp, &localStackTop->overheadTime);
	else
		zeroTime(&temp);		

	// if ((summary && recursDepth == 0) || detailed)  DeMorgan's Law into:
	if (--function->recursDepth == 0 || globals->header.method == collectDetailed)		
	{
	//	function->functionAndChildren += temp;
		UninterruptableAddTime(&temp, &function->functionAndChildren);
	}
	
	//	if (stackTop->childrenTime < temp)
	if (cmpTime(&localStackTop->childrenTime, &temp))
	{
	//	temp -= stackTop->childrenTime;
		subTime(&temp, &temp, &localStackTop->childrenTime);
	}
	else
		zeroTime(&temp);

	if (cmpTime(&temp, &function->min))
		function->min = temp;
	if (cmpTime(&function->max, &temp))
		function->max = temp;

//	function->functionOnly += temp;
	UninterruptableAddTime(&temp, &function->functionOnly);
	
//	stackTop[-1].childrenTime += temp + stackTop->childrenTime;
	addTime(&temp, &temp, &localStackTop->childrenTime);
	UninterruptableAddTime(&temp, &localStackTop[-1].childrenTime);

	ASSERT(localStackTop >= threadPtr->frameBuffer && localStackTop < threadPtr->stackEnd);

	// NO MP SUPPORT
	(*globals->getCurrentTime)(&temp);

//	stackTop[-1].overheadTime += (temp - overheadStart) + overHead + stackTop->overheadTime;
	subTime(&temp, &temp, overheadStart);
	addTime(&temp, &temp, &globals->getTimeOverHead);
	addTime(&temp, &temp, &localStackTop->overheadTime);
	addTime(&localStackTop[-1].overheadTime, &localStackTop[-1].overheadTime, &temp);
} // CalcFinalTime

/* ========================= Public Functions ======================= */

/* ------------------------------------------------------------------
 * ProfilerInit
 *
 * Turns on the Profiler and starts the underlying timebase.  This 
 * should be the first function called by the user.
 * -----------------------------------------------------------------*/

OSErr ProfilerInit(short /*ProfilerCollectionMethod*/ method, short /*ProfilerTimeBase*/ timeBase, long numFunctions, short stackDepth)
{
	OSErr			err = 0;
	Size			size;
	// long			result;
	long resolution;

	ProfilerGlobalsPtr globals = GetGlobalsAddress();
	
	if (globals->functionBufferPtr != NULL)
		return -1;		// already inited

	ASSERT(sizeof(globals->header) == 512);
	ASSERT(sizeof(diskFunctionInfoV2) == 48);
	ASSERT(sizeof(diskFunctionInfoV3) == 52);

	globals->threadQueue.qHead = NULL;
	globals->threadQueue.qTail = NULL;
	
	if (true /*TrapAvailable(_Gestalt)*/)
	{
		// err = Gestalt(gestaltFSAttr, &result);
		if (true /*err == noErr && (result & (1 << gestaltHasFSSpecCalls))*/)
		{
			// ensure the header is zeroed
			{
				long *p = (long *)&globals->header;
				long *end = (long *)((char *)&globals->header + sizeof(globals->header));
				
				while (p < end)
					*p++ = 0L;
			}
		
			/* allocate buffers */

			if (method == collectDetailed)
			{
				globals->maxFunctions = (long)numFunctions * 12L;
			}
			else if (method == collectSummary)
			{
				globals->maxFunctions = (long)numFunctions;	
			}
			else
				return -1;
		
			size = (globals->maxFunctions + 2) * sizeof(functionInfo);
			globals->header.method = method;
		
			globals->addSemaphore = kInitialSemaphoreValue;
			
			// NO MP SUPPORT
			/* start timebase */
			
			// err = startTimeBase(globals, &timeBase);
			globals->getCurrentTime = &getTime;

			if (err == noErr)
			{
				globals->functionBufferPtr = (functionInfoPtr)AllocateBuffer(size, &err);
				if (err)
				{
					stopTimeBase(globals);
					return err;
				}
				
				globals->nameOffsetSize = globals->maxFunctions + (globals->maxFunctions >> 3) + 1;
				
				if (method == collectSummary)
				{
#if !USEOLDSUMMARYMETHOD
					long numBucketGroups = (numFunctions / kBucketGroupSize);
					numBucketGroups -= (kHashTableSize / 2);
					if (numBucketGroups > 0)
					{
						globals->queueHandle = AllocateBuffer(numBucketGroups * sizeof(BucketGroup), &err);
						if (err)
						{
							stopTimeBase(globals);
							ReleaseBuffer(&(Handle)globals->functionBufferPtr);
							return err;
						}
						globals->hashTable.additions = (BucketGroup*) *globals->queueHandle;
						globals->hashTable.lastAddition = globals->hashTable.additions + (numBucketGroups - 1);
					}
#else
					globals->queueHandle = AllocateBuffer(INSERTQUEUESIZE * sizeof(insertFunction), &err);
					if (err)
					{
						stopTimeBase(globals);
						ReleaseBuffer(&(Handle)globals->functionBufferHandle);
						return err;
					}
					else
					{
						short		i;
						insertFunctionPtr	p;
						
						p = (insertFunctionPtr) *globals->queueHandle;
						for (i = 0; i < INSERTQUEUESIZE; i++)
						{
							Enqueue((QElemPtr)&p[i], &globals->emptyQueue);
						}
					}
#endif
				}

				globals->header.signature = profileFileSignature;
				globals->header.version = profileCurrentVersion;
				
				globals->header.processor = profilePowerPCProcessor;
				globals->header.method = method;
				globals->header.timebase = timeBase;
				globals->header.initialNameTableSize = numFunctions;
				globals->header.initialStackTableSize = stackDepth;
				
				SetupInitialTables(globals);
				
				{
					ProfilerThreadRef threadRef;
					
					err = ProfilerCreateMainThread(stackDepth, &threadRef);
				}
				if (err != noErr)
				{
					stopTimeBase(globals);
					ReleaseBuffer(&(Handle)globals->functionBufferPtr);
					return err;
				} // if able to create main thread

				globals->profilerStatus = true;
				// NO 68K
			} // if started timebase
		} // if has FSSpec calls
	} // if has gestalt
	
	// added by bmarine1 for timer resolution
	resolution = getTimerResolution();
	if( resolution > 10 )
	  resolution /= 10;
	else
	  resolution = 1;
	globals->header.tbConversionFactor = resolution;	
	
	return err;
} // ProfilerInit

/* ------------------------------------------------------------------
 * ProfilerTerm
 *
 * Turn off the profiler and stop the timebase.  This 
 * should be the last function called by the user.
 * -----------------------------------------------------------------*/

void ProfilerTerm(void)
{
	ProfilerGlobalsPtr globals = GetGlobalsAddress();

	globals->profilerStatus = false;

	ReleaseBuffer(&(Handle)globals->functionBufferPtr);
	globals->functionBufferPtr = NULL;
	ProfilerDeleteAllThreads();
	stopTimeBase(globals);
	if (globals->queueHandle)
	{
		ReleaseBuffer(&globals->queueHandle);
	}
	// NO MP SUPPORT

} // ProfilerTerm

/* ------------------------------------------------------------------
 * ProfilerSetStatus
 *
 * Turn on and off the profiler.  This does not stop the timebase,
 * only the recording of data.
 *
 * @@@ There is an open issue in the API, does this make sense in
 * an MP world?  Or should the status be per thread/task?
 * -----------------------------------------------------------------*/

void ProfilerSetStatus(short on)
{
	ProfilerGlobalsPtr globals = GetGlobalsAddress();
	timeInterval			pauseOverhead;
	
	if (globals->profilerStatus != on)
	{
		globals->profilerStatus = on;
	
		if (on)
		{
			(*globals->getCurrentTime)(&pauseOverhead);
			subTime(&pauseOverhead, &pauseOverhead, &globals->startPause);
			addTime(&globals->threadStack->stackTop->overheadTime, &globals->threadStack->stackTop->overheadTime, &pauseOverhead);
		}
		else
		{
			(*globals->getCurrentTime)(&globals->startPause);
		}
	} // if the status is different
} // ProfilerSetStatus

/* ------------------------------------------------------------------
 * ProfilerGetStatus
 *
 * Return the current status of the profiler.  Is it on or off?
 *
 * @@@ There is an open issue in the API, does this make sense in
 * an MP world?  Or should the status be per thread/task?
 * -----------------------------------------------------------------*/

short ProfilerGetStatus(void)
{
	ProfilerGlobalsPtr globals = GetGlobalsAddress();

	return globals->profilerStatus;
} // ProfilerGetStatus


/* ------------------------------------------------------------------
 * ProfilerGetDataSizes
 *
 * Return the current amount of data used by the profiler.
 * -----------------------------------------------------------------*/

void ProfilerGetDataSizes(long *functionSize, long *stackSize)
{
	ProfilerGlobalsPtr globals = GetGlobalsAddress();

	*functionSize = globals->header.numRecords;
	*stackSize = globals->threadStack->stackMaxPoint - globals->threadStack->frameBuffer;
} // ProfilerGetDataSizes

/* ------------------------------------------------------------------
 * ProfilerClear
 *
 * Clear out the profile buffer.  This does not stop the recording of
 * new information.
 * -----------------------------------------------------------------*/

void ProfilerClear(void)
{
	ProfilerGlobalsPtr globals = GetGlobalsAddress();
	ProfilerThreadRef  threadRef;
	
	ProfilerDeleteAllThreads();
	SetupInitialTables(globals);
	ProfilerCreateMainThread(globals->header.initialStackTableSize, &threadRef);
} // ProfilerClear

/* ------------------------------------------------------------------
 * ProfilerSwitchToThread
 *
 * Notify the profiler that the current thread is being switched out,
 * and a new thread is being activated.
 * -----------------------------------------------------------------*/

pascal void ProfilerSwitchToThread(ProfilerThreadRef thread)
{
	ProfilerGlobalsPtr globals = GetGlobalsAddress();
	ProfilerThreadPtr	threadPtr = (ProfilerThreadPtr) thread;
	
	ASSERT(threadPtr->threadSignature == ThreadSignature);
	
	if (globals->getCurrentTime)
	{
		timeInterval	offset;
		
		globals->threadStack->hasBeenSwitchedOut = true;
		(*globals->getCurrentTime)(&globals->threadStack->stopTime);
		
		if (threadPtr->hasBeenSwitchedOut)
		{
		//	offset = currentTime - threadPtr->stopTime;
			subTime(&offset, &globals->threadStack->stopTime, &threadPtr->stopTime);
		//  threadPtr->offsetTime += offset
			addTime(&threadPtr->offsetTime, &threadPtr->offsetTime, &offset);
		} // if it was previously stopped
		globals->threadStack = threadPtr;
	} // if the profiler is initialized
} // ProfilerSwitchToThread

/* ------------------------------------------------------------------
 * ProfilerGetMainThreadRef
 *
 * Get a ThreadRef for the main thread.  This is necessary for
 * threaded applications.
 * -----------------------------------------------------------------*/

pascal ProfilerThreadRef ProfilerGetMainThreadRef()
{
	ProfilerGlobalsPtr globals = GetGlobalsAddress();
	return globals->mainThreadRef;
} // ProfilerGetMainThreadRef

 /* ------------------------------------------------------------------
 * ProfilerCreateThread
 *
 * Create a ThreadRef for a new thread.
 * -----------------------------------------------------------------*/

pascal OSErr ProfilerCreateThread(short stackSize, long byteStackSize, ProfilerThreadRef *threadRef)
{
	ProfilerGlobalsPtr globals = GetGlobalsAddress();

	// NO MP SUPPORT
//	ProfilerThreadPtr threadPtr = (ProfilerThreadPtr) NewPtrClear(sizeof(ProfilerThread) + sizeof(frameInfo) * stackSize);
	ProfilerThreadPtr threadPtr = (ProfilerThreadPtr) calloc(sizeof(ProfilerThread) + sizeof(frameInfo) * stackSize, 1);
	
	if (threadPtr)
	{
		*threadRef = (unsigned long)threadPtr;
//		Enqueue(&threadPtr->queueElement, &globals->threadQueue);
	
	/* initialize the thread specific globals */
		threadPtr->threadSignature = ThreadSignature;
		threadPtr->hasBeenSwitchedOut = false;
		threadPtr->maxStackDepth = stackSize;
		threadPtr->stackEnd = threadPtr->frameBuffer + threadPtr->maxStackDepth;
		threadPtr->stackMaxPoint = threadPtr->frameBuffer;

		threadPtr->stackTop = threadPtr->frameBuffer;
		threadPtr->deleteMe = false;
	/* Initialize the first entry in the thread stack*/
		zeroTime(&threadPtr->stackTop->overheadTime);
		zeroTime(&threadPtr->stopTime);
		zeroTime(&threadPtr->offsetTime);
		threadPtr->stackTop->returnAddress = (void *)0x7fffffff;
	
		if (globals->header.method == collectDetailed)
		{
			threadPtr->stackTop->function = &globals->functionBufferPtr[0];
		} // hook into "fake main" if detailed

	/* mark these as not set up, __PROFILE will set them up then */
		threadPtr->stackTop->stackPointer = nil;
		threadPtr->stackBase = nil;
		threadPtr->stackBottom = nil;
		threadPtr->stackSize = byteStackSize;
	} // if created the memory ok
	
	// NO MP SUPPORT

	return 0;
} // ProfilerCreateThread

pascal OSErr ProfilerCreateMainThread(short stackSize, ProfilerThreadRef *threadRef)
{
#if TARGET_API_MAC_CARBON
	long theResult;
#endif

	ProfilerGlobalsPtr globals = GetGlobalsAddress();
	OSErr	error = ProfilerCreateThread(stackSize, 0L, threadRef);
	ProfilerThreadPtr threadPtr = (ProfilerThreadPtr) *threadRef;
	
//	threadPtr->stackTop->stackPointer = LMGetCurStackBase();
	threadPtr->stackTop->stackPointer = (Ptr)0x7FFFFFFF;
	threadPtr->stackTop->stackPointer = (Ptr)0x0;

// 	threadPtr->stackBase = LMGetCurStackBase();
	threadPtr->stackBase = (Ptr)0x7FFFFFFF;
	threadPtr->stackBase = (Ptr)0x0;
	threadPtr->stackBottom = 0;

	if (error == noErr)
	{
		globals->threadStack = threadPtr;
		globals->mainThreadRef = *threadRef;
		// NO MP SUPPORT
	} // if created thread ok

	return error;
} // ProfilerCreateMainThread

/* ========================= Compatibility  ======================= */

// NO COMPATIBILITYMODE

/* ===================== Data Collection Routines =================== */

#pragma mark -

/* ------------------------------------------------------------------
 * __PROFILE_ENTRY
 *
 * This function is called at the beginning of every function.
 * -----------------------------------------------------------------*/
// This function is not used for ARM 
// The ARM ABI does not define an explicit stack layout (and thus no back link)
/*static asm unsigned long* GetStackBacklink()
{
  mov    r0, #0
  bx     lr
}*/

typedef struct{

	long m_version;
	long m_enabled;
	long m_entryCount;
	long m_entryPosition;
	long m_bufferSize;
	unsigned long* m_AddressBuffer;
	unsigned long long* m_TimeBuffer;

}CWTraceLibraryBuffer;

CWTraceLibraryBuffer gCWTraceLibraryBuffer = {1, 0, 0, 0, 0, NULL};

void AddTraceEntry(unsigned long retAddr)
{
	if (gCWTraceLibraryBuffer.m_enabled && gCWTraceLibraryBuffer.m_AddressBuffer)
	{
		if (gCWTraceLibraryBuffer.m_entryPosition >= gCWTraceLibraryBuffer.m_bufferSize)
		{
			gCWTraceLibraryBuffer.m_entryPosition = 0;
		}
		gCWTraceLibraryBuffer.m_AddressBuffer[gCWTraceLibraryBuffer.m_entryPosition] = retAddr;
		unsigned long long tbTime;
		getTime(&tbTime);
		gCWTraceLibraryBuffer.m_TimeBuffer[gCWTraceLibraryBuffer.m_entryPosition] = tbTime;
		gCWTraceLibraryBuffer.m_entryCount++;
		gCWTraceLibraryBuffer.m_entryPosition++;
	}
}

long TraceInit(long traceBufferSize)
{
	if (gCWTraceLibraryBuffer.m_AddressBuffer)
		return -1;
	
	gCWTraceLibraryBuffer.m_AddressBuffer = (unsigned long*)malloc(traceBufferSize * sizeof(unsigned long));
	gCWTraceLibraryBuffer.m_TimeBuffer = (unsigned long long*)malloc(traceBufferSize * sizeof(unsigned long long));
	
	if (gCWTraceLibraryBuffer.m_AddressBuffer && gCWTraceLibraryBuffer.m_TimeBuffer)
	{
		gCWTraceLibraryBuffer.m_bufferSize = traceBufferSize;
		gCWTraceLibraryBuffer.m_entryCount = 0;
		gCWTraceLibraryBuffer.m_entryPosition = 0;
	}
	else
		return -1;
	
	return 0;
}

long TraceTerm()
{
	if (!gCWTraceLibraryBuffer.m_AddressBuffer)
		return -1;
	
	free(gCWTraceLibraryBuffer.m_AddressBuffer);
	free(gCWTraceLibraryBuffer.m_TimeBuffer);
	
	gCWTraceLibraryBuffer.m_bufferSize = 0;
	gCWTraceLibraryBuffer.m_entryCount = 0;
	gCWTraceLibraryBuffer.m_entryPosition = 0;
	gCWTraceLibraryBuffer.m_enabled = 0;
	
	return 0;
}

long TraceClear()
{
	if (!gCWTraceLibraryBuffer.m_AddressBuffer)
		return -1;
	
	gCWTraceLibraryBuffer.m_entryCount = 0;
	gCWTraceLibraryBuffer.m_entryPosition = 0;
	
	return 0;
}

long TraceSetStatus(long enabled)
{
	long wasEnabled = gCWTraceLibraryBuffer.m_enabled;
	gCWTraceLibraryBuffer.m_enabled = enabled;
	return wasEnabled;
}

// Added the second parameter that is the value of sp in the profiled function
// at the time of the call to __PROFILE_ENTRY; the profiled function has the 
// following stack layout:
// SP --> R0
//    --> LR
// Also removed pascal calling convention
/* pascal */ void __cPROFILE_ENTRY(char *functionName, int *sp)
{
	// The next line was modified to get the value of LR
	unsigned long retAddr = sp[ 1 ];
	
	ProfilerGlobalsPtr globals = GetGlobalsAddress();
	timeInterval	overheadStart, startTime, temp;
	functionInfoPtr	function;
	frameInfoPtr	localStackTop = nil;
	ProfilerThreadPtr	threadStack;

	// Temporary, for debugging only
	// printf( "Entry: name = %s return address = %08X\n", functionName, retAddr );
	AddTraceEntry(retAddr);

	//PPCLinkFramePtr	framePtr = GetLinkageFrame();
	void 			**returnAddress = (void 			**)retAddr; // &framePtr->savedLR;

#if EXCEPTIONSUPPORT
	void			*prev;
#endif

	if (
		// NO 68K
		globals->profilerStatus)
	{
		// NO MP SUPPORT
		(*globals->getCurrentTime)(&overheadStart);
		threadStack = globals->threadStack;

		if (!threadStack->stackBase)
		{
			threadStack->stackTop->stackPointer = returnAddress + 4;	/* make sure it's above */
			threadStack->stackBase = (Ptr)returnAddress;
			threadStack->stackBottom = (Ptr)returnAddress - threadStack->stackSize;
		} /* if in a thread and not fully set up */

#if EXCEPTIONSUPPORT
		prev = threadStack->stackTop->stackPointer;
		while (returnAddress <= threadStack->frameBuffer->stackPointer && returnAddress >= prev && threadStack->stackTop > threadStack->frameBuffer)	/* a longjmp() or exception */
		{
			// NO MP SUPPORT
			CalcFinalTime(threadStack, globals, &overheadStart, threadStack->stackTop->function);

			threadStack->stackTop--;
			
			ASSERT(threadStack->stackTop->function >= globals->functionBufferPtr && threadStack->stackTop->function <= globals->functionBufferPtr + globals->header.numRecords + 1);
			prev = threadStack->stackTop->stackPointer;
		}
#endif
		if (threadStack->stackTop < threadStack->stackEnd)
		{
			ASSERT(threadStack->stackTop >= threadStack->frameBuffer && threadStack->stackTop < threadStack->stackEnd);
			function = findFunction(globals, threadStack->stackTop, functionName);
			if (!function)
				return;

			globals->tempOverflowStack = 0;

			function->count++;
			function->recursDepth++;
			threadStack->stackTop++;
			localStackTop = threadStack->stackTop;

			if (localStackTop > threadStack->stackMaxPoint)
				threadStack->stackMaxPoint = localStackTop;

			if (returnAddress < (void *)threadStack->stackBase && returnAddress > (void *)threadStack->stackBottom)
			{
				long	tempStackSpace = threadStack->stackBase - (Ptr)returnAddress;
				if (function->stackDepth == 0 || tempStackSpace > function->stackDepth)
				{
					function->stackDepth = tempStackSpace;
				} /* if this is a new or lower stackdepth */
			} /* if stack pointer is in the initial stack */

			localStackTop->stackPointer = returnAddress;
			localStackTop->returnAddress = NULL;

			localStackTop->function = function;
			zeroTime(&localStackTop->childrenTime);

			// NO MP SUPPORT
			(*globals->getCurrentTime)(&startTime);

			/* The overhead is in callee function's time, so remove it there */
			
		// stackTop.overheadTime = startTime - overheadStart + getTimeOverHead;

			if (cmpTime(&overheadStart, &startTime))
				subTime(&temp, &startTime, &overheadStart);
			else
				zeroTime(&temp);
			addTime(&localStackTop->overheadTime, &temp, &globals->getTimeOverHead);

		// Subtract time that the thread's already been asleep. gcm
			if (cmpTime(&globals->threadStack->offsetTime, &startTime))
				subTime(&startTime, &startTime, &globals->threadStack->offsetTime);
			else
				zeroTime(&startTime);
			localStackTop->startTime = startTime;
		}
		else
		{
			globals->tempOverflowStack++;
			globals->header.overflowStack = max(globals->tempOverflowStack, globals->header.overflowStack);
		}
	}
//@@@	return localStackTop;
} // __PROFILE_ENTRY

// Modified __PROFILE_ENTRY to call __cPROFILE_ENTRY
extern "C" asm void __PROFILE_ENTRY()
{
  stmdb  sp!, { r1-r3, lr }
  add    r1, sp, #16                   // r1 point to the old sp
  bl     __cPROFILE_ENTRY
  #ifdef THUMB_MODE
    ldmia  sp!, { r1-r3, lr }
    bx     lr
  #else
    ldmia  sp!, { r1-r3, pc }
  #endif
}

/* ------------------------------------------------------------------
 * __PROFILE_ENTRY
 *
 * The real entry point for 68k.
 * -----------------------------------------------------------------*/

// NO 68K


/* ------------------------------------------------------------------
 * __cPROFILE_EXIT
 *
 * This function is called at the end of each function being profiled.
 * -----------------------------------------------------------------*/

// The parameter of this function is the value of sp in the profiled function
// at the time of the call to __PROFILE_EXIT; the profiled function has the 
// following stack layout:
// SP --> LR
// Also removed pascal calling convention 
static /* pascal */ void __cPROFILE_EXIT(void *stackPointer)
{
	ProfilerGlobalsPtr globals = GetGlobalsAddress();
	timeInterval	overheadStart;
	ProfilerThreadPtr	threadStack;

	unsigned long* spPtr = (unsigned long*)stackPointer;
	// Modified next line to reflect the value of LR for ARM
	unsigned long retAddr = spPtr[0];
	// Temporary, for debugging only
    // printf( "exit: return address = %08X\n", retAddr );
	AddTraceEntry(retAddr);

	if (
		// NO 68K
		stackPointer != NULL
// JWW - The following test is very strange... it causes crashes if ProfilerTerm() has been called
// because the FindTask() function will not ever find anything - it will return NULL.
//#if !USEMP
		&& globals->getCurrentTime
//#endif
		)		/* if the profiler hasn't been terminated */
	{
		// NO MP SUPPORT
		threadStack = globals->threadStack;
		(*globals->getCurrentTime)(&overheadStart);

		if ( threadStack->stackTop > threadStack->frameBuffer)
		{
			ASSERT(threadStack->stackTop->function >= globals->functionBufferPtr && threadStack->stackTop->function <= globals->functionBufferPtr + globals->header.numRecords + 1);

			while (stackPointer <= threadStack->frameBuffer->stackPointer && stackPointer >= threadStack->stackTop[-1].stackPointer && threadStack->stackTop > (threadStack->frameBuffer + 1))	/* a longjmp() or exception */
			{
				// NO MP SUPPORT
				CalcFinalTime(threadStack, globals, &overheadStart, threadStack->stackTop->function);

				threadStack->stackTop--;
				
				ASSERT(threadStack->stackTop->function >= globals->functionBufferPtr && threadStack->stackTop->function <= globals->functionBufferPtr + globals->header.numRecords + 1);
			}

			ASSERT(threadStack->stackTop > threadStack->frameBuffer);
			// NO MP SUPPORT
			CalcFinalTime(threadStack, globals, &overheadStart, threadStack->stackTop->function);

			threadStack->stackTop--;
		}
	}
} /* __cPROFILE_EXIT */


/* ------------------------------------------------------------------
 * __PROFILE_EXIT
 *
 * The real entry point that is called at the end of each function.
 * -----------------------------------------------------------------*/

extern "C" asm void __PROFILE_EXIT(void)
{
  // Modified for ARM
  stmdb  sp!, { r0-r3, lr }
  add    r0, sp, #20                   // SP of the calling function
  bl     __cPROFILE_EXIT
  #ifdef THUMB_MODE
    ldmia  sp!, { r0-r3, lr }
    bx     lr
  #else
    ldmia  sp!, { r0-r3, pc }
  #endif
} 

// NO 68K
/* ===================== MP Helper routines =================== */


// NO MP SUPPORT
