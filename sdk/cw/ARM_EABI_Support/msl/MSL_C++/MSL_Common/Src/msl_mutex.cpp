/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/09 17:02:29 $
 * $Revision: 1.11.2.2 $
 */

// msl_mutex.cpp

#include <msl_mutex>
#include <msl_condition>
#include <msl_thread>

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

detail::defer_lock_type defer_lock;
detail::try_lock_type try_to_lock;

#ifdef _MSL_USE_PTHREADS

#ifndef _MSL_NO_TIME_SUPPORT

timed_mutex::timed_mutex()
	: locked_(false)
{
	if (pthread_mutex_init(&m_, 0))
		detail::throw_thread_resource_error();
	if (pthread_cond_init(&c_, 0))
	{
		pthread_mutex_destroy(&m_);
		detail::throw_thread_resource_error();
	}
}

timed_mutex::~timed_mutex()
{
	pthread_cond_destroy(&c_);
	pthread_mutex_destroy(&m_);
}

void
timed_mutex::lock()
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	while (locked_)
		pthread_cond_wait(&c_, &m_);
	locked_ = true;
	pthread_mutex_unlock(&m_);
}

bool
timed_mutex::try_lock()
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (!locked_)
		locked_ = true;
	bool result = locked_;
	pthread_mutex_unlock(&m_);
	return result;
}

bool
timed_mutex::timed_lock(const universal_time& unv_time)
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	int cond = 0;
	while (locked_)
	{
		cond = pthread_cond_timedwait(&c_, &m_, (const timespec*)&unv_time);
		if (cond)
			break;
	}
	if (!locked_)
	{
		locked_ = true;
		cond = 0;
	}
	pthread_mutex_unlock(&m_);
	return cond == 0;
}

void
timed_mutex::unlock()
{
	if (pthread_mutex_lock(&m_) || !locked_)
		detail::throw_lock_error();
	locked_ = false;
	pthread_mutex_unlock(&m_);
	pthread_cond_broadcast(&c_);
}

void
timed_mutex::wait_prefix(state&)
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	locked_ = false;
	pthread_cond_broadcast(&c_);
}

void
timed_mutex::wait_suffix(const state&)
{
	while (locked_)
		pthread_cond_wait(&c_, &m_);
	locked_ = true;
	pthread_mutex_unlock(&m_);
}

#endif  // _MSL_NO_TIME_SUPPORT

#ifdef PTHREAD_MUTEX_RECURSIVE

recursive_mutex::recursive_mutex()
{
	pthread_mutexattr_t attr;
	if (pthread_mutexattr_init(&attr))
		detail::throw_thread_resource_error();
	if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) ||
	    pthread_mutex_init(&m_, &attr))
	{
		pthread_mutexattr_destroy(&attr);
		detail::throw_thread_resource_error();
	}
	pthread_mutexattr_destroy(&attr);
}

#else   // PTHREAD_MUTEX_RECURSIVE

recursive_mutex::recursive_mutex()
	:	count_(0)
{
	if (pthread_mutex_init(&m_, 0))
		detail::throw_thread_resource_error();
	if (pthread_cond_init(&c_, 0))
	{
		pthread_mutex_destroy(&m_);
		detail::throw_thread_resource_error();
	}
}

recursive_mutex::~recursive_mutex()
{
	pthread_cond_destroy(&c_);
	pthread_mutex_destroy(&m_);
}

void
recursive_mutex::lock()
{
	pthread_t self = pthread_self();
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (count_ > 0 && pthread_equal(self, owner_))
		++count_;
	else
	{
		while (count_ > 0)
			pthread_cond_wait(&c_, &m_);
		count_ = 1;
		owner_ = self;
	}
	pthread_mutex_unlock(&m_);
}

void
recursive_mutex::unlock()
{
	pthread_t self = pthread_self();
	if (pthread_mutex_lock(&m_) || count_ == 0 || !pthread_equal(self, owner_))
		detail::throw_lock_error();
	bool notify = --count_ == 0;
	pthread_mutex_unlock(&m_);
	if (notify)
		pthread_cond_broadcast(&c_);
}

void
recursive_mutex::wait_prefix(state& st)
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	st.count_ = count_;
	st.owner_ = owner_;
	count_ = 0;
	pthread_cond_broadcast(&c_);
}

void
recursive_mutex::wait_suffix(const state& st)
{
	while (count_ > 0)
		pthread_cond_wait(&c_, &m_);
	count_ = st.count_;
	owner_ = st.owner_;
	pthread_mutex_unlock(&m_);
}

#endif  // PTHREAD_MUTEX_RECURSIVE

#ifdef PTHREAD_MUTEX_RECURSIVE

recursive_try_mutex::recursive_try_mutex()
{
	pthread_mutexattr_t attr;
	if (pthread_mutexattr_init(&attr))
		detail::throw_thread_resource_error();
	if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) ||
	    pthread_mutex_init(&m_, &attr))
	{
		pthread_mutexattr_destroy(&attr);
		detail::throw_thread_resource_error();
	}
	pthread_mutexattr_destroy(&attr);
}

#else  // PTHREAD_MUTEX_RECURSIVE

recursive_try_mutex::recursive_try_mutex()
	:	count_(0)
{
	if (pthread_mutex_init(&m_, 0))
		detail::throw_thread_resource_error();
	if (pthread_cond_init(&c_, 0))
	{
		pthread_mutex_destroy(&m_);
		detail::throw_thread_resource_error();
	}
}

recursive_try_mutex::~recursive_try_mutex()
{
	pthread_cond_destroy(&c_);
	pthread_mutex_destroy(&m_);
}

void
recursive_try_mutex::lock()
{
	pthread_t self = pthread_self();
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (count_ > 0 && pthread_equal(self, owner_))
		++count_;
	else
	{
		while (count_ > 0)
			pthread_cond_wait(&c_, &m_);
		count_ = 1;
		owner_ = self;
	}
	pthread_mutex_unlock(&m_);
}

bool
recursive_try_mutex::try_lock()
{
	pthread_t self = pthread_self();
	bool owns = false;
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (count_ == 0)
	{
		owner_ = self;
		++count_;
		owns = true;
	}
	else if (pthread_equal(self, owner_))
	{
		++count_;
		owns = true;
	}
	pthread_mutex_unlock(&m_);
	return owns;
}

void
recursive_try_mutex::unlock()
{
	pthread_t self = pthread_self();
	if (pthread_mutex_lock(&m_) || count_ == 0 || !pthread_equal(self, owner_))
		detail::throw_lock_error();
	bool notify = --count_ == 0;
	pthread_mutex_unlock(&m_);
	if (notify)
		pthread_cond_broadcast(&c_);
}

void
recursive_try_mutex::wait_prefix(state& st)
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	st.count_ = count_;
	st.owner_ = owner_;
	count_ = 0;
	pthread_cond_broadcast(&c_);
}

void
recursive_try_mutex::wait_suffix(const state& st)
{
	while (count_ > 0)
		pthread_cond_wait(&c_, &m_);
	count_ = st.count_;
	owner_ = st.owner_;
	pthread_mutex_unlock(&m_);
}

#endif  // PTHREAD_MUTEX_RECURSIVE

#ifndef _MSL_NO_TIME_SUPPORT

recursive_timed_mutex::recursive_timed_mutex()
	:	count_(0)
{
	if (pthread_mutex_init(&m_, 0))
		detail::throw_thread_resource_error();
	if (pthread_cond_init(&c_, 0))
	{
		pthread_mutex_destroy(&m_);
		detail::throw_thread_resource_error();
	}
}

recursive_timed_mutex::~recursive_timed_mutex()
{
	pthread_cond_destroy(&c_);
	pthread_mutex_destroy(&m_);
}

void
recursive_timed_mutex::lock()
{
	pthread_t self = pthread_self();
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (count_ > 0 && pthread_equal(self, owner_))
		++count_;
	else
	{
		while (count_ > 0)
			pthread_cond_wait(&c_, &m_);
		count_ = 1;
		owner_ = self;
	}
	pthread_mutex_unlock(&m_);
}

bool
recursive_timed_mutex::try_lock()
{
	pthread_t self = pthread_self();
	bool owns = false;
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (count_ == 0)
	{
		owner_ = self;
		++count_;
		owns = true;
	}
	else if (pthread_equal(self, owner_))
	{
		++count_;
		owns = true;
	}
	pthread_mutex_unlock(&m_);
	return owns;
}

bool
recursive_timed_mutex::timed_lock(const universal_time& unv_time)
{
	pthread_t self = pthread_self();
	bool owns = false;
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (count_ == 0)
	{
		owner_ = self;
		++count_;
		owns = true;
	}
	else if (pthread_equal(self, owner_))
	{
		++count_;
		owns = true;
	}
	else
	{
		while (count_ > 0)
		{
			if (pthread_cond_timedwait(&c_, &m_, (const timespec*)&unv_time))
				break;
		}
		if (count_ == 0)
		{
			owner_ = self;
			++count_;
			owns = true;
		}
	}
	pthread_mutex_unlock(&m_);
	return owns;
}

void
recursive_timed_mutex::unlock()
{
	pthread_t self = pthread_self();
	if (pthread_mutex_lock(&m_) || count_ == 0 || !pthread_equal(self, owner_))
		detail::throw_lock_error();
	bool notify = --count_ == 0;
	pthread_mutex_unlock(&m_);
	if (notify)
		pthread_cond_broadcast(&c_);
}

void
recursive_timed_mutex::wait_prefix(state& st)
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	st.count_ = count_;
	st.owner_ = owner_;
	count_ = 0;
	pthread_cond_broadcast(&c_);
}

void
recursive_timed_mutex::wait_suffix(const state& st)
{
	while (count_ > 0)
		pthread_cond_wait(&c_, &m_);
	count_ = st.count_;
	owner_ = st.owner_;
	pthread_mutex_unlock(&m_);
}

#endif  // _MSL_NO_TIME_SUPPORT

#endif  // _MSL_USE_PTHREADS

#ifdef _MSL_USE_MPTASKS

#ifndef _MSL_NO_TIME_SUPPORT

bool
timed_mutex::timed_lock(const elapsed_time& elps_time)
{
	int dt = 0;
	if (elps_time.sec_ > 0)
		dt = elps_time.sec_ * 1000;
	else if (elps_time.nsec_ > 0)
		dt = -elps_time.nsec_ / 1000;
	int result = MPEnterCriticalRegion(m_, dt);
	if (result != 0 && result != kMPTimeoutErr)
		detail::throw_lock_error();
	return result == 0;
}

bool
recursive_timed_mutex::timed_lock(const elapsed_time& elps_time)
{
	int dt = 0;
	if (elps_time.sec_ > 0)
		dt = elps_time.sec_ * 1000;
	else if (elps_time.nsec_ > 0)
		dt = -elps_time.nsec_ / 1000;
	int result = MPEnterCriticalRegion(m_, dt);
	if (result != 0 && result != kMPTimeoutErr)
		detail::throw_lock_error();
	return result == 0;
}

#endif  // _MSL_NO_TIME_SUPPORT

#endif // _MSL_USE_MPTASKS

#ifdef _MSL_USE_WINTHREADS

#ifndef _MSL_NO_TIME_SUPPORT

bool
timed_mutex::timed_lock(const elapsed_time& elps_time)
{
	DWORD dt = (DWORD)(elps_time.sec_ * 1000 + elps_time.nsec_ / 1000);
	DWORD result = WaitForSingleObject(m_, dt);
	if (result != WAIT_OBJECT_0 && result != WAIT_TIMEOUT)
		detail::throw_lock_error();
	return result == WAIT_OBJECT_0;
}

bool
recursive_timed_mutex::timed_lock(const elapsed_time& elps_time)
{
	DWORD dt = (DWORD)(elps_time.sec_ * 1000 + elps_time.nsec_ / 1000);
	DWORD result = WaitForSingleObject(m_, dt);
	if (result != WAIT_OBJECT_0 && result != WAIT_TIMEOUT)
		detail::throw_lock_error();
	return result == WAIT_OBJECT_0;
}

#endif  // _MSL_NO_TIME_SUPPORT

#endif  // _MSL_USE_WINTHREADS

#ifndef _MSL_NO_CONDITION

rw_mutex::rw_mutex()
	:	activity_(0)
{
}

void
rw_mutex::lock_sharable()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
	unsigned n_readers;
	private_lock lk(mut_, defer_lock);
loop:
	asm	{lwarx    a, 0, p}
	n_readers = a & max_readers_;
	if ((a & write_entered_) || n_readers == max_readers_)
	{
		if (!lk.locked())
			lk.lock();
		a |= entry_sleeping_;
		asm
		{
			stwcx.   a, 0, p
			bne-     loop		
		}
		entry_.wait(lk);
		goto loop;
	}
	++n_readers;
	a &= ~max_readers_;
	a |= n_readers;
	asm
	{
		stwcx.   a, 0, p
		bne-     loop
		isync
	}
#else  // __POWERPC__
	private_lock lk(mut_);
	while ((activity_ & write_entered_) || (activity_ & max_readers_) == max_readers_)
		entry_.wait(lk);
	unsigned t = (activity_ & max_readers_) + 1;
	activity_ &= ~max_readers_;
	activity_ |= t;
#endif  // __POWERPC__
}

bool
rw_mutex::try_lock_sharable()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
	unsigned n_readers;
loop:
	asm	{lwarx    a, 0, p}
	n_readers = a & max_readers_;
	if (a & write_entered_ || n_readers == max_readers_)
		return false;
	++n_readers;
	a &= ~max_readers_;
	a |= n_readers;
	asm
	{
		stwcx.   a, 0, p
		bne-     loop
		isync
	}
	return true;
#else  // __POWERPC__
	private_lock lk(mut_, Metrowerks::try_to_lock);
	unsigned n_readers = activity_ & max_readers_;
	if (lk.locked() && !(activity_ & write_entered_) && n_readers != max_readers_)
	{
		++n_readers;
		activity_ &= ~max_readers_;
		activity_ |= n_readers;
		return true;
	}
	return false;
#endif  // __POWERPC__
}

void
rw_mutex::unlock_sharable()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
	unsigned n_readers;
	private_lock lk(mut_, defer_lock);
loop:
	asm	{lwarx    a, 0, p}
	n_readers = (a & max_readers_) - 1;
	a &= ~max_readers_;
	a |= n_readers;
	if (n_readers == 0 && (a & fw_sleeping_))
	{
		if (!lk.locked())
			lk.lock();
		a &= ~fw_sleeping_;
		asm
		{
			stwcx.   a, 0, p
			bne-     loop
		}
		lk.unlock();
		first_wait_.notify_one();
	}
	else if (n_readers == max_readers_ - 1 && (a & entry_sleeping_))
	{
		if (!lk.locked())
			lk.lock();
		a &= ~entry_sleeping_;
		asm
		{
			stwcx.   a, 0, p
			bne-     loop
		}
		lk.unlock();
		entry_.notify_all();
	}
	else
	{
		asm
		{
			stwcx.   a, 0, p
			bne-     loop
		}
	}
#else  // __POWERPC__
	private_lock lk(mut_);
	unsigned n_readers = (activity_ & max_readers_) - 1;
	unsigned write_entered = activity_ & write_entered_;
	activity_ &= ~max_readers_;
	activity_ |= n_readers;
	if (n_readers == 0)
		first_wait_.notify_one();
	else if (n_readers == max_readers_ - 1)
		entry_.notify_all();
#endif  // __POWERPC__
}

void
rw_mutex::lock_upgradable()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
	unsigned n_readers;
	private_lock lk(mut_, defer_lock);
loop:
	asm	{lwarx    a, 0, p}
	n_readers = a & max_readers_;
	if ((a & (write_entered_ | upgradable_entered_)) || n_readers == max_readers_)
	{
		if (!lk.locked())
			lk.lock();
		a |= entry_sleeping_;
		asm
		{
			stwcx.   a, 0, p
			bne-     loop		
		}
		entry_.wait(lk);
		goto loop;
	}
	++n_readers;
	a &= ~max_readers_;
	a |= upgradable_entered_ | n_readers;
	asm
	{
		stwcx.   a, 0, p
		bne-     loop
		isync
	}
#else  // __POWERPC__
	private_lock lk(mut_);
	while ((activity_ & (write_entered_ | upgradable_entered_)) ||
	       (activity_ & max_readers_) == max_readers_)
		entry_.wait(lk);
	unsigned t = (activity_ & max_readers_) + 1;
	activity_ &= ~max_readers_;
	activity_ |= upgradable_entered_ | t;
#endif  // __POWERPC__
}

bool
rw_mutex::try_lock_upgradable()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
	unsigned n_readers;
loop:
	asm	{lwarx    a, 0, p}
	n_readers = a & max_readers_;
	if ((a & (write_entered_ | upgradable_entered_)) || n_readers == max_readers_)
		return false;
	++n_readers;
	a &= ~max_readers_;
	a |= upgradable_entered_ | n_readers;
	asm
	{
		stwcx.   a, 0, p
		bne-     loop
		isync
	}
	return true;
#else  // __POWERPC__
	private_lock lk(mut_, Metrowerks::try_to_lock);
	unsigned n_readers = activity_ & max_readers_;
	if (lk.locked() && !(activity_ & (write_entered_ | upgradable_entered_)) &&
	    n_readers != max_readers_)
	{
		++n_readers;
		activity_ &= ~max_readers_;
		activity_ |= upgradable_entered_ | n_readers;
		return true;
	}
	return false;
#endif  // __POWERPC__
}

void
rw_mutex::unlock_upgradable()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
	unsigned n_readers;
	private_lock lk(mut_, defer_lock);
loop:
	asm	{lwarx    a, 0, p}

	n_readers = (a & max_readers_) - 1;
	a &= ~(upgradable_entered_ | max_readers_);
	a |= n_readers;
	if (a & entry_sleeping_)
	{
		if (!lk.locked())
			lk.lock();
		a &= ~entry_sleeping_;
		asm
		{
			stwcx.   a, 0, p
			bne-     loop
		}
		lk.unlock();
		entry_.notify_all();
	}
	else
	{
		asm
		{
			stwcx.   a, 0, p
			bne-     loop
		}
	}
#else  // __POWERPC__
	private_lock lk(mut_);
	unsigned n_readers = (activity_ & max_readers_) - 1;
	activity_ &= ~(upgradable_entered_ | max_readers_);
	activity_ |= n_readers;
	entry_.notify_all();
#endif  // __POWERPC__
}

void
rw_mutex::lock()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
	private_lock lk(mut_, defer_lock);
loop1:
	asm	{lwarx    a, 0, p}
	if (a & (write_entered_ | upgradable_entered_))
	{
		if (!lk.locked())
			lk.lock();
		a |= entry_sleeping_;
		asm
		{
			stwcx.   a, 0, p
			bne-     loop1		
		}
		entry_.wait(lk);
		goto loop1;
	}
	a |= write_entered_;
	asm
	{
		stwcx.   a, 0, p
		bne-     loop1
	}
	if (a & max_readers_)
	{
loop2:
		asm	{lwarx    a, 0, p}
		if (a & max_readers_)
		{
			if (!lk.locked())
				lk.lock();
			a |= fw_sleeping_;
			asm
			{
				stwcx.   a, 0, p
				bne-     loop2
			}
			first_wait_.wait(lk);
			goto loop2;
		}
		asm
		{
			stwcx.   a, 0, p
			bne-     loop2
		}
	}
	asm {isync}
#else  // __POWERPC__
	private_lock lk(mut_);
	while (activity_ & (write_entered_ | upgradable_entered_))
		entry_.wait(lk);
	activity_ |= write_entered_;
	while (activity_ & max_readers_)
		first_wait_.wait(lk);
#endif  // __POWERPC__
}

bool
rw_mutex::try_lock()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
loop:
	asm	{lwarx    a, 0, p}
	if (a & (write_entered_ | max_readers_))
		return false;
	a |= write_entered_;
	asm
	{
		stwcx.   a, 0, p
		bne-     loop
		isync
	}
	return true;
#else  // __POWERPC__
	private_lock lk(mut_, Metrowerks::try_to_lock);
	if (lk.locked() && activity_ == 0)
	{
		activity_ = write_entered_;
		return true;
	}
	return false;
#endif  // __POWERPC__
}

void
rw_mutex::unlock()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
	asm
	{
		machine all
		sync
loop:
		lwarx    a, 0, p
	}
	if (a & entry_sleeping_)
	{
		private_lock lk(mut_);
		activity_ = 0;
		lk.unlock();
		entry_.notify_all();
	}
	else
	{
		a = 0;
		asm
		{
			stwcx.   a, 0, p
			bne-     loop
		}
	}
#else  // __POWERPC__
	private_lock lk(mut_);
	activity_ = 0;
	entry_.notify_all();
#endif  // __POWERPC__
}

void
rw_mutex::unlock_and_lock_sharable()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
	asm
	{
		machine all
		sync
loop:
		lwarx    a, 0, p
	}
	a &= ~write_entered_;
	a |= 1;
	if (a & entry_sleeping_)
	{
		private_lock lk(mut_);
		a &= ~entry_sleeping_;
		activity_ = a;
		lk.unlock();
		entry_.notify_all();
	}
	else
	{
		asm
		{
			stwcx.   a, 0, p
			bne-     loop
		}
	}
#else  // __POWERPC__
	private_lock lk(mut_);
	activity_ = 1;
	entry_.notify_all();
#endif  // __POWERPC__
}

void
rw_mutex::unlock_and_lock_upgradable()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
	asm
	{
		machine all
		sync
loop:
		lwarx    a, 0, p
	}
	a &= ~write_entered_;
	a |= upgradable_entered_ | 1;
	if (a & entry_sleeping_)
	{
		private_lock lk(mut_);
		a &= ~entry_sleeping_;
		activity_ = a;
		lk.unlock();
		entry_.notify_all();
	}
	else
	{
		asm
		{
			stwcx.   a, 0, p
			bne-     loop
		}
	}
#else  // __POWERPC__
	private_lock lk(mut_);
	activity_ = upgradable_entered_ | 1;
	entry_.notify_all();
#endif  // __POWERPC__
}

void
rw_mutex::unlock_upgradable_and_lock_sharable()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
	private_lock lk(mut_, defer_lock);
loop:
	asm {lwarx    a, 0, p}
	a &= ~upgradable_entered_;
	if (a & entry_sleeping_)
	{
		if (!lk.locked())
			lk.lock();
		a &= ~entry_sleeping_;
		asm
		{
			stwcx.   a, 0, p
			bne-     loop
		}
		lk.unlock();
		entry_.notify_all();
	}
	else
	{
		asm
		{
			stwcx.   a, 0, p
			bne-     loop
		}
	}
#else  // __POWERPC__
	private_lock lk(mut_);
	activity_ &= ~upgradable_entered_;
	entry_.notify_all();
#endif  // __POWERPC__
}

void
rw_mutex::unlock_upgradable_and_lock()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
	private_lock lk(mut_, defer_lock);
loop:
	asm	{lwarx    a, 0, p}
	unsigned n_readers = (a & max_readers_) - 1;
	a &= ~(upgradable_entered_ | max_readers_);
	a |= write_entered_ | n_readers;
	asm
	{
		stwcx.   a, 0, p
		bne-     loop
	}
	if (n_readers)
	{
loop2:
		asm	{lwarx    a, 0, p}
		if (a & max_readers_)
		{
			if (!lk.locked())
				lk.lock();
			a |= fw_sleeping_;
			asm
			{
				stwcx.   a, 0, p
				bne-     loop2
			}
			first_wait_.wait(lk);
			goto loop2;
		}
		asm
		{
			stwcx.   a, 0, p
			bne-     loop2
		}
	}
#else  // __POWERPC__
	private_lock lk(mut_);
	unsigned n_readers = (activity_ & max_readers_) - 1;
	activity_ &= ~(upgradable_entered_ | max_readers_);
	activity_ |= write_entered_ | n_readers;
	while (activity_ & max_readers_)
		first_wait_.wait(lk);
#endif  // __POWERPC__
}

bool
rw_mutex::try_unlock_upgradable_and_lock()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
loop:
	asm	{lwarx    a, 0, p}
	if ((a & max_readers_) != 1)
		return false;
	a &= ~(upgradable_entered_ | max_readers_);
	a |= write_entered_;
	asm
	{
		stwcx.   a, 0, p
		bne-     loop
	}
	return true;
#else  // __POWERPC__
	private_lock lk(mut_, Metrowerks::try_to_lock);
	if (lk.locked() && (activity_ & max_readers_) == 1)
	{
		activity_ &= ~(upgradable_entered_ | max_readers_);
		activity_ |= write_entered_;
		return true;
	}
	return false;
#endif  // __POWERPC__
}

bool
rw_mutex::try_unlock_sharable_and_lock_upgradable()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
loop:
	asm	{lwarx    a, 0, p}
	if (a & (write_entered_ | upgradable_entered_))
		return false;
	a |= upgradable_entered_;
	asm
	{
		stwcx.   a, 0, p
		bne-     loop
	}
	return true;
#else  // __POWERPC__
	private_lock lk(mut_, Metrowerks::try_to_lock);
	if (lk.locked() && !(activity_ & (write_entered_ | upgradable_entered_)))
	{
		activity_ |= upgradable_entered_;
		return true;
	}
	return false;
#endif  // __POWERPC__
}

bool
rw_mutex::try_unlock_sharable_and_lock()
{
#ifdef __POWERPC__
	register volatile unsigned* const p = &activity_;
	register unsigned a;
loop:
	asm	{lwarx    a, 0, p}
	if ((a & (write_entered_ | upgradable_entered_)) || (a & max_readers_) != 1)
		return false;
	a &= ~max_readers_;
	a |= write_entered_;
	asm
	{
		stwcx.   a, 0, p
		bne-     loop
	}
	return true;
#else  // __POWERPC__
	private_lock lk(mut_, Metrowerks::try_to_lock);
	if (lk.locked() && !(activity_ & (write_entered_ | upgradable_entered_)) && (activity_ & max_readers_) == 1)
	{
		activity_ &= ~max_readers_;
		activity_ |= write_entered_;
		return true;
	}
	return false;
#endif  // __POWERPC__
}

bool
rw_mutex::invariants() const
{
	unsigned a = activity_;
	if ((a & upgradable_entered_) != 0 && (a & max_readers_) == 0)
		return false;
	if ((a & upgradable_entered_) != 0 && (a & write_entered_) != 0)
		return false;
	return true;
}

#endif  // _MSL_NO_CONDITION

#ifndef _MSL_NO_CPP_NAMESPACE
	} // namespace Metrowerks
#endif

// hh 030616 Created
// hh 031202 Added rw_mutex
