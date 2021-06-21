/*
 *  ProfilerARMIO.c
 *
 *  Copyright (c) 2003 Metrowerks Inc.  All rights reserved.
 *	METROWERKS CONFIDENTIAL
 *
 *	Port these routines to other platforms
 *	ratwell wuz here
 *
 */

#include "ProfilerIntern.h"
#include <stdlib.h>
#include <pool_alloc.h>
#include <string.h>
//Symbol gCWProfileLibraryBuffer was declared as static, due to which this
//was not getting exposed to debugger. Removed static qualifier ...Dinesh
CWProfileLibraryBuffer gCWProfileLibraryBuffer = {1, 0, 0, 0, 0};

void CWProfileDataReady()
{
	gCWProfileLibraryBuffer.m_dataWaiting = gCWProfileLibraryBuffer.m_dataWaiting;
}

void WriteBackBuffer(unsigned long bufferAddress, unsigned long bufferSize, long dataWaiting, bool writeAtBeginning)
{
	gCWProfileLibraryBuffer.m_version = 1;
	gCWProfileLibraryBuffer.m_dataWaiting = dataWaiting;
	gCWProfileLibraryBuffer.m_writeAtBeginning = writeAtBeginning;
	gCWProfileLibraryBuffer.m_writeBufferAddress = bufferAddress;
	gCWProfileLibraryBuffer.m_writeBufferLength = bufferSize;
	CWProfileDataReady();
	gCWProfileLibraryBuffer.m_dataWaiting = 0;
}

/* ------------------------------------------------------------------
* ProfilerDump
*
* Dump the current buffer of profile information to the given file.
* If it exists, append and increment a number to the filename.  
* This does not clear the profile information.
* -----------------------------------------------------------------*/

OSErr ProfilerDump(char *filename)
{
	ProfilerGlobalsPtr globals = GetGlobalsAddress();
	OSErr		err = noErr;
	short		refNum;
	long 		count = 2;
	Boolean		saveProfilerStatus = globals->profilerStatus;
	long		diskOffset;
	long		nameOffset;
	long		functOffset;
	long		limit;
	
	strcpy(gCWProfileLibraryBuffer.m_writeBufferName, filename);
	globals->nameOffsetPtr = AllocateBuffer(globals->nameOffsetSize * sizeof(NameOffsetEntry), &err);
	if (err)
	{
		return err;
	} // if couldn't create handle
	{
		long	i = globals->nameOffsetSize;
		NameOffsetEntryPtr	p = (NameOffsetEntryPtr)globals->nameOffsetPtr;
		while (i--)
		{
			(p++)->name = 0;
		} // zero out all name entries
	}
	/* the profiler needs to be turned off here because completion routines
	   could still be called.  We don't want them changing the values while
	   we are writing them out.
	 */
	globals->profilerStatus = false;

//	GetDateTime(&globals->header.dateSaved);
	
	//@@@need total overhead from all threads
	globals->header.totalOverhead = globals->threadStack->frameBuffer->overheadTime;
		
	/* the rest of the header is already set up */
	
//	if (err == fnfErr)
	{	
//		err = FSpCreate(&spec, profileFileSignature, profileFileType, 0);
		
//		if (err == noErr)
		{
//			err = FSpOpenDF(&spec, fsRdWrPerm, &refNum);
			
//			if (err == noErr)
			{
				long		size;
				memFunctionInfo	*functionBase;
				
				nameOffset = 0L;
				globals->header.nameSize = 0L;
				if (globals->header.method == collectSummary)
				{
					functionBase = globals->functionBufferPtr;
					globals->header.detailedRootOffset = 0;
					globals->header.numRecords++;
					limit = globals->header.numRecords;
				}
				else
				{
					functionBase = &globals->functionBufferPtr[1];
					globals->header.detailedRootOffset = globals->functionBufferPtr[0].children - functionBase;
					limit = globals->header.numRecords + 1;
				} // collectDetailed

				diskOffset = sizeof(globals->header) + globals->header.numRecords * sizeof(diskFunctionInfoV3);
				functOffset = sizeof(globals->header);
				globals->header.nameBaseOffset = diskOffset;
				
				size = sizeof(globals->header);
//				err = FSWrite(refNum, &size, (Ptr) &globals->header);
				WriteBackBuffer((unsigned long) &globals->header, size, kProfileDataReady, true);
				
				if (err == noErr)
				{
					ASSERT(size == sizeof(globals->header));

					err = WriteData(globals, refNum, functionBase, limit, &nameOffset, functOffset, diskOffset);					
					if (err == noErr)		/* rewrite the header with nameSize filled in */
					{
						globals->header.nameSize = nameOffset;
#if 0
						
						pb.ioParam.ioRefNum = refNum;
						pb.ioParam.ioBuffer = (Ptr) &globals->header;
						pb.ioParam.ioPosMode = fsFromStart;
						pb.ioParam.ioPosOffset = 0;
						pb.ioParam.ioReqCount = sizeof(globals->header);
						err = PBWriteSync(&pb);
#endif
						WriteBackBuffer((unsigned long) &globals->header, sizeof(globals->header), kProfileDataDone, true);

					} // if wrote the data, fix up the nameOffset in the header
					if (err == noErr)
					{
						size = sizeof(globals->header) + 
								globals->header.numRecords * sizeof(diskFunctionInfoV3) +
								nameOffset;
						// err = SetEOF(refNum, size);
					} // if fixed up the nameOffset ok, make sure the file size is correct
				} // if wrote the header
				
				if (globals->header.method == collectSummary)	/* put it back */
					globals->header.numRecords--;

				//FSClose(refNum);		/* ignore close errors to report prev error */
			} // if opened the file for writing
			//if (err != noErr)
//				FSpDelete(&spec);
		} // if created the file
	} // if the file didn't exist
	
	ReleaseBuffer(&(Handle)globals->nameOffsetPtr);

	globals->profilerStatus = saveProfilerStatus;
	
	return err;
} // ProfilerDump

/* ------------------------------------------------------------------
 * FlushWriteBuffer
 *
 * Move bytes from the buffer to the disk.  If this isn't the last
 * write, then only write up to the last disk block boundary.  It's
 * much more efficent to write in whole disk blocks.
 * -----------------------------------------------------------------*/

OSErr FlushWriteBuffer(WriteBufferType *buffer, Boolean flush)
{
	OSErr	error = noErr;
	short			i;
	long			writeLength;
	long			preserveLength = 0;
	
	if (buffer->writePosition > 0)
	{

		if (flush)
		{
			writeLength = buffer->writePosition;
		} // write everything
		else
		{
			writeLength = ((buffer->fileOffset + buffer->writePosition) & 0xfffffe00) - buffer->fileOffset;
			preserveLength = buffer->writePosition - writeLength;
		} // write up to a 512 byte block boundary

		// error = PBWriteSync(&pb);

		WriteBackBuffer((unsigned long)buffer->bufferPtr, writeLength, kProfileDataReady, false);

		if (error == noErr)	
		{
			buffer->fileOffset += writeLength;
			if (preserveLength)
			{
				memcpy((Ptr)buffer->bufferPtr, (Ptr)buffer->bufferPtr + writeLength, preserveLength);
			} // if stuff left over in buffer, copy it to beginning
			buffer->writePosition = preserveLength;
		} // if did write
	}
	
	return error;
} // FlushWriteBuffer