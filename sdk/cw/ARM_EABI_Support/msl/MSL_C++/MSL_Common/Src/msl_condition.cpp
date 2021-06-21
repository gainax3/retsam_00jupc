/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/09 17:02:27 $
 * $Revision: 1.10.2.2 $
 */

// msl_condition.cpp

//  The algorithms for condition::wait and condition::notify_(one/all) for _MSL_USE_MPTASKS
//  and _MSL_USE_WINTHREADS were derived closely from a newsgroup post on
//  comp.programming.threads by Alexander Terekhov, subject:  A theoretical question
//  on synchronization, on april 27, 2001.  This is what Alexander labels "Algorithm 8a".

#include <msl_condition>
#include <msl_mutex>
#include <limits>

#ifdef __MWERKS__
#pragma options align=native
#endif

#ifdef _MSL_FORCE_ENUMS_ALWAYS_INT
	#if _MSL_FORCE_ENUMS_ALWAYS_INT
		#pragma enumsalwaysint on
	#else
		#pragma enumsalwaysint off
	#endif
#endif  // _MSL_FORCE_ENUMS_ALWAYS_INT

#ifdef _MSL_FORCE_ENABLE_BOOL_SUPPORT
	#if _MSL_FORCE_ENABLE_BOOL_SUPPORT
		#pragma bool on
	#else
		#pragma bool off
	#endif
#endif  // _MSL_FORCE_ENABLE_BOOL_SUPPORT

#ifndef _MSL_NO_CPP_NAMESPACE
	namespace Metrowerks {
#else
	#ifndef Metrowerks
		#define Metrowerks
	#endif
#endif  // _MSL_NO_CPP_NAMESPACE

#ifdef _MSL_USE_MPTASKS

condition::condition()
	:	n_waiters_gone_(0),
		n_waiters_blocked_(0),
		n_waiters_to_unblock_(0)
{
	if (MPCreateSemaphore(1, 1, &block_lock_))
		detail::throw_thread_resource_error();
	if (MPCreateSemaphore((unsigned long)_STD::numeric_limits<long>::max(), 0, &block_queue_))
	{
		MPDeleteSemaphore(block_lock_);
		detail::throw_thread_resource_error();
	}
	if (MPCreateCriticalRegion(&unblock_lock_))
	{
		MPDeleteSemaphore(block_queue_);
		MPDeleteSemaphore(block_lock_);
		detail::throw_thread_resource_error();
	}
}

condition::~condition()
{
	MPDeleteCriticalRegion(unblock_lock_);
	MPDeleteSemaphore(block_queue_);
	MPDeleteSemaphore(block_lock_);
}

void
condition::notify_one()
{
	int n_signals_to_issue = 0;
	MPEnterCriticalRegion(unblock_lock_, kDurationForever);
	if (n_waiters_to_unblock_ != 0)
	{
		if (n_waiters_blocked_ != 0)
		{
			n_signals_to_issue = 1;
			++n_waiters_to_unblock_;
			--n_waiters_blocked_;
		}
	}
	else if (n_waiters_gone_ < n_waiters_blocked_)
	{
		MPWaitOnSemaphore(block_lock_, kDurationForever);
		if (n_waiters_gone_ != 0)
		{
			n_waiters_blocked_ -= n_waiters_gone_;
			n_waiters_gone_ = 0;
		}
		n_signals_to_issue = n_waiters_to_unblock_ = 1;
		--n_waiters_blocked_;
	}
	MPExitCriticalRegion(unblock_lock_);
	if (n_signals_to_issue)
		MPSignalSemaphore(block_queue_);
}

void
condition::notify_all()
{
	int n_signals_to_issue = 0;
	MPEnterCriticalRegion(unblock_lock_, kDurationForever);
	if (n_waiters_to_unblock_ != 0)
	{
		if (n_waiters_blocked_ != 0)
		{
			n_signals_to_issue = n_waiters_blocked_;
			n_waiters_to_unblock_ += n_signals_to_issue;
			n_waiters_blocked_ = 0;
		}
	}
	else if (n_waiters_gone_ < n_waiters_blocked_)
	{
		MPWaitOnSemaphore(block_lock_, kDurationForever);
		if (n_waiters_gone_ != 0)
		{
			n_waiters_blocked_ -= n_waiters_gone_;
			n_waiters_gone_ = 0;
		}
		n_signals_to_issue = n_waiters_to_unblock_ = n_waiters_blocked_;
		n_waiters_blocked_ = 0;
	}
	MPExitCriticalRegion(unblock_lock_);
	for (; n_signals_to_issue > 0; --n_signals_to_issue)
		MPSignalSemaphore(block_queue_);
}

void
condition::prepare_wait()
{
	MPWaitOnSemaphore(block_lock_, kDurationForever);
	++n_waiters_blocked_;
	MPSignalSemaphore(block_lock_);
}

void
condition::execute_wait()
{
	MPWaitOnSemaphore(block_queue_, kDurationForever);
	MPEnterCriticalRegion(unblock_lock_, kDurationForever);
	int n_signals_was_left = n_waiters_to_unblock_;
	int n_waiters_was_gone = n_waiters_gone_;
	if (n_signals_was_left != 0)
	{
		if (--n_waiters_to_unblock_ == 0)
		{
			if (n_waiters_blocked_ != 0)
			{
				MPSignalSemaphore(block_lock_);
				n_signals_was_left = 0;
			}
			else if (n_waiters_was_gone != 0)
				n_waiters_gone_ = 0;
		}
	}
	else if (++n_waiters_gone_ == _STD::numeric_limits<int>::max()/2)
	{
		MPWaitOnSemaphore(block_lock_, kDurationForever);
		n_waiters_blocked_ -= n_waiters_gone_;
		MPSignalSemaphore(block_lock_);
		n_waiters_gone_ = 0;
	}
	MPExitCriticalRegion(unblock_lock_);
	if (n_signals_was_left == 1)
	{
		for (; n_waiters_was_gone > 0; --n_waiters_was_gone)
			MPWaitOnSemaphore(block_queue_, kDurationForever);
		MPSignalSemaphore(block_lock_);
	}
}

#ifndef _MSL_NO_TIME_SUPPORT

int
condition::prepare_timed_wait(const universal_time& unv_time)
{
	MPWaitOnSemaphore(block_lock_, kDurationForever);
	++n_waiters_blocked_;
	MPSignalSemaphore(block_lock_);
	int duration = 0;
	universal_time c;
	if (c < unv_time)
	{
		elapsed_time e = unv_time - c;
		if (e.sec_ > 0)
			duration = e.sec_ * 1000;
		else if (e.nsec_ > 0)
			duration = -e.nsec_ / 1000;
	}
	return duration;
}

bool
condition::execute_timed_wait(int duration)
{
	bool timed_out = MPWaitOnSemaphore(block_queue_, duration) == kMPTimeoutErr;
	MPEnterCriticalRegion(unblock_lock_, kDurationForever);
	int n_signals_was_left = n_waiters_to_unblock_;
	int n_waiters_was_gone = n_waiters_gone_;
	if (n_signals_was_left != 0)
	{
		if (timed_out)
		{
			if (n_waiters_blocked_ != 0)
				--n_waiters_blocked_;
			else
				++n_waiters_gone_;
		}
		if (--n_waiters_to_unblock_ == 0)
		{
			if (n_waiters_blocked_ != 0)
			{
				MPSignalSemaphore(block_lock_);
				n_signals_was_left = 0;
			}
			else if (n_waiters_was_gone != 0)
				n_waiters_gone_ = 0;
		}
	}
	else if (++n_waiters_gone_ == _STD::numeric_limits<int>::max()/2)
	{
		MPWaitOnSemaphore(block_lock_, kDurationForever);
		n_waiters_blocked_ -= n_waiters_gone_;
		MPSignalSemaphore(block_lock_);
		n_waiters_gone_ = 0;
	}
	MPExitCriticalRegion(unblock_lock_);
	if (n_signals_was_left == 1)
	{
		for (; n_waiters_was_gone > 0; --n_waiters_was_gone)
			MPWaitOnSemaphore(block_queue_, kDurationForever);
		MPSignalSemaphore(block_lock_);
	}
	return timed_out;
}

#endif  // _MSL_NO_TIME_SUPPORT

#endif  // _MSL_USE_MPTASKS

#ifdef _MSL_USE_WINTHREADS

condition::condition()
	:	n_waiters_gone_(0),
		n_waiters_blocked_(0),
		n_waiters_to_unblock_(0)
{
	block_lock_ =  CreateSemaphore(0, 1, 1, 0);
	if (block_lock_ == 0)
		detail::throw_thread_resource_error();
	block_queue_ = CreateSemaphore(0, 0, _STD::numeric_limits<long>::max(), 0);
	if (block_queue_ == 0)
	{
		CloseHandle(block_lock_);
		detail::throw_thread_resource_error();
	}
	InitializeCriticalSection(&unblock_lock_);
}

condition::~condition()
{
	DeleteCriticalSection(&unblock_lock_);
	CloseHandle(block_queue_);
	CloseHandle(block_lock_);
}

void
condition::notify_one()
{
	int n_signals_to_issue = 0;
	EnterCriticalSection(&unblock_lock_);
	if (n_waiters_to_unblock_ != 0)
	{
		if (n_waiters_blocked_ != 0)
		{
			n_signals_to_issue = 1;
			++n_waiters_to_unblock_;
			--n_waiters_blocked_;
		}
	}
	else if (n_waiters_gone_ < n_waiters_blocked_)
	{
		WaitForSingleObject(block_lock_, INFINITE);
		if (n_waiters_gone_ != 0)
		{
			n_waiters_blocked_ -= n_waiters_gone_;
			n_waiters_gone_ = 0;
		}
		n_signals_to_issue = n_waiters_to_unblock_ = 1;
		--n_waiters_blocked_;
	}
	LeaveCriticalSection(&unblock_lock_);
	if (n_signals_to_issue > 0)
		ReleaseSemaphore(block_queue_, n_signals_to_issue, 0);
}

void
condition::notify_all()
{
	int n_signals_to_issue = 0;
	EnterCriticalSection(&unblock_lock_);
	if (n_waiters_to_unblock_ != 0)
	{
		if (n_waiters_blocked_ != 0)
		{
			n_signals_to_issue = n_waiters_blocked_;
			n_waiters_to_unblock_ += n_signals_to_issue;
			n_waiters_blocked_ = 0;
		}
	}
	else if (n_waiters_gone_ < n_waiters_blocked_)
	{
		WaitForSingleObject(block_lock_, INFINITE);
		if (n_waiters_gone_ != 0)
		{
			n_waiters_blocked_ -= n_waiters_gone_;
			n_waiters_gone_ = 0;
		}
		n_signals_to_issue = n_waiters_to_unblock_ = n_waiters_blocked_;
		n_waiters_blocked_ = 0;
	}
	LeaveCriticalSection(&unblock_lock_);
	if (n_signals_to_issue > 0)
		ReleaseSemaphore(block_queue_, n_signals_to_issue, 0);
}

void
condition::prepare_wait()
{
	WaitForSingleObject(block_lock_, INFINITE);
	++n_waiters_blocked_;
	ReleaseSemaphore(block_lock_, 1, 0);
}

void
condition::execute_wait()
{
	WaitForSingleObject(block_queue_, INFINITE);
	EnterCriticalSection(&unblock_lock_);
	int n_signals_was_left = n_waiters_to_unblock_;
	int n_waiters_was_gone = n_waiters_gone_;
	if (n_signals_was_left != 0)
	{
		if (--n_waiters_to_unblock_ == 0)
		{
			if (n_waiters_blocked_ != 0)
			{
				ReleaseSemaphore(block_lock_, 1, 0);
				n_signals_was_left = 0;
			}
			else if (n_waiters_was_gone != 0)
				n_waiters_gone_ = 0;
		}
	}
	else if (++n_waiters_gone_ == _STD::numeric_limits<int>::max()/2)
	{
		WaitForSingleObject(block_lock_, INFINITE);
		n_waiters_blocked_ -= n_waiters_gone_;
		ReleaseSemaphore(block_lock_, 1, 0);
		n_waiters_gone_ = 0;
	}
	LeaveCriticalSection(&unblock_lock_);
	if (n_signals_was_left == 1)
	{
		for (; n_waiters_was_gone > 0; --n_waiters_was_gone)
			WaitForSingleObject(block_queue_, INFINITE);
		ReleaseSemaphore(block_lock_, 1, 0);
	}
}

#ifndef _MSL_NO_TIME_SUPPORT

DWORD
condition::prepare_timed_wait(const universal_time& unv_time)
{
	WaitForSingleObject(block_lock_, INFINITE);
	++n_waiters_blocked_;
	ReleaseSemaphore(block_lock_, 1, 0);
	DWORD milliseconds = 0;
	universal_time c;
	if (c < unv_time)
	{
		elapsed_time e = unv_time - c;
		milliseconds = e.sec_ * 1000 + e.nsec_ / 1000000;
	}
	return milliseconds;
}

bool
condition::execute_timed_wait(DWORD milliseconds)
{
	bool timed_out = WaitForSingleObject(block_queue_, milliseconds) == WAIT_TIMEOUT;
	EnterCriticalSection(&unblock_lock_);
	int n_signals_was_left = n_waiters_to_unblock_;
	int n_waiters_was_gone = n_waiters_gone_;
	if (n_signals_was_left != 0)
	{
		if (timed_out)
		{
			if (n_waiters_blocked_ != 0)
				--n_waiters_blocked_;
			else
				++n_waiters_gone_;
		}
		if (--n_waiters_to_unblock_ == 0)
		{
			if (n_waiters_blocked_ != 0)
			{
				ReleaseSemaphore(block_lock_, 1, 0);
				n_signals_was_left = 0;
			}
			else if (n_waiters_was_gone != 0)
				n_waiters_gone_ = 0;
		}
	}
	else if (++n_waiters_gone_ == _STD::numeric_limits<int>::max()/2)
	{
		WaitForSingleObject(block_lock_, INFINITE);
		n_waiters_blocked_ -= n_waiters_gone_;
		ReleaseSemaphore(block_lock_, 1, 0);
		n_waiters_gone_ = 0;
	}
	LeaveCriticalSection(&unblock_lock_);
	if (n_signals_was_left == 1)
	{
		for (; n_waiters_was_gone > 0; --n_waiters_was_gone)
			WaitForSingleObject(block_queue_, INFINITE);
		ReleaseSemaphore(block_lock_, 1, 0);
	}
	return timed_out;
}

#endif  // _MSL_NO_TIME_SUPPORT

#endif  // _MSL_USE_WINTHREADS

#ifndef _MSL_NO_CPP_NAMESPACE
	} // namespace Metrowerks
#endif

// hh 030616 Created
