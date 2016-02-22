/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Synchronization primitives.
 * The specifications of the functions are in synch.h.
 */

#include <types.h>
#include <lib.h>
#include <spinlock.h>
#include <wchan.h>
#include <thread.h>
#include <current.h>
#include <synch.h>

#define MAX_READERS 30 //added by Sammokka
////////////////////////////////////////////////////////////
//
// Semaphore.

struct semaphore *
sem_create(const char *name, unsigned initial_count)
{
	struct semaphore *sem;

	sem = kmalloc(sizeof(*sem));
	if (sem == NULL) {
		return NULL;
	}

	sem->sem_name = kstrdup(name);
	if (sem->sem_name == NULL) {
		kfree(sem);
		return NULL;
	}

	sem->sem_wchan = wchan_create(sem->sem_name);
	if (sem->sem_wchan == NULL) {
		kfree(sem->sem_name);
		kfree(sem);
		return NULL;
	}

	spinlock_init(&sem->sem_lock);
	sem->sem_count = initial_count;

	return sem;
}

void
sem_destroy(struct semaphore *sem)
{
	KASSERT(sem != NULL);

	/* wchan_cleanup will assert if anyone's waiting on it */
	spinlock_cleanup(&sem->sem_lock);
	wchan_destroy(sem->sem_wchan);
	kfree(sem->sem_name);
	kfree(sem);
}

void
P(struct semaphore *sem)
{
	KASSERT(sem != NULL);

	/*
	 * May not block in an interrupt handler.
	 *
	 * For robustness, always check, even if we can actually
	 * complete the P without blocking.
	 */
	KASSERT(curthread->t_in_interrupt == false);

	/* Use the semaphore spinlock to protect the wchan as well. */
	spinlock_acquire(&sem->sem_lock);
	while (sem->sem_count == 0) {
		/*
		 *
		 * Note that we don't maintain strict FIFO ordering of
		 * threads going through the semaphore; that is, we
		 * might "get" it on the first try even if other
		 * threads are waiting. Apparently according to some
		 * textbooks semaphores must for some reason have
		 * strict ordering. Too bad. :-)
		 *
		 * Exercise: how would you implement strict FIFO
		 * ordering?
		 */
		wchan_sleep(sem->sem_wchan, &sem->sem_lock);
	}
	KASSERT(sem->sem_count > 0);
	sem->sem_count--;
	spinlock_release(&sem->sem_lock);
}

void
V(struct semaphore *sem)
{
	KASSERT(sem != NULL);

	spinlock_acquire(&sem->sem_lock);

	sem->sem_count++;
	KASSERT(sem->sem_count > 0);
	wchan_wakeone(sem->sem_wchan, &sem->sem_lock);

	spinlock_release(&sem->sem_lock);
}

////////////////////////////////////////////////////////////
//
// Lock.



struct lock *
lock_create(const char *name)
{
	struct lock *lock;

	lock = kmalloc(sizeof(*lock));
	if (lock == NULL) {
		return NULL;
	}

	lock->lk_name = kstrdup(name);
	if (lock->lk_name == NULL) {
		kfree(lock);
		return NULL;
	}

	// add stuff here as needed

	lock->lk_wchan = wchan_create(lock->lk_name);

//	if(lock->lk_wchan==NULL) {
//		kfree(lock->lk_name);
//		kfree(lock);
//		return NULL;
//	}

	spinlock_init(&lock->lk_spinlock);
	lock->lk_isLocked = false; //unlocked
	lock->holder = NULL; //no thread is holding it right now.
	//added - sammok

	return lock;
}

void
lock_destroy(struct lock *lock)
{
	KASSERT(lock != NULL);

	// add stuff here as needed TODO Sammok


	KASSERT(lock->holder==NULL); //assert that no thread is holding it

	wchan_destroy(lock->lk_wchan);

	kfree(lock->lk_name);
	kfree(lock);

}

void
lock_acquire(struct lock *lock) {
	// Write this

	KASSERT(lock!=NULL);

	//Adding - sammokka

	KASSERT(curthread->t_in_interrupt == false);//check if it's not an interrrupt

	spinlock_acquire(&lock->lk_spinlock);

	//Keep looping until lock is unlocked
	while (lock->lk_isLocked) {
		wchan_sleep(lock->lk_wchan, &lock->lk_spinlock);
	}
	lock->lk_isLocked = true;
	lock->holder = curthread;

	spinlock_release(&lock->lk_spinlock);
}

void
lock_release(struct lock *lock) {
	//added by sammokka
	KASSERT(lock!=NULL);
	spinlock_acquire(&lock->lk_spinlock);
	KASSERT(lock->lk_isLocked==true);
	lock->lk_isLocked = false;
	lock->holder = NULL;
	wchan_wakeone(lock->lk_wchan, &lock->lk_spinlock);
	spinlock_release(&lock->lk_spinlock);
}

bool lock_do_i_hold(struct lock *lock) {
	// Write this
	KASSERT(lock!=NULL);
	if (lock->holder == curthread) {
		return true; // dummy until code gets written
	} else {
		return false;
	}
}

////////////////////////////////////////////////////////////
//
// CV


struct cv *
cv_create(const char *name)
{
	struct cv *cv;

	cv = kmalloc(sizeof(*cv));
	if (cv == NULL) {
		return NULL;
	}

	cv->cv_name = kstrdup(name);
	if (cv->cv_name==NULL) {
		kfree(cv);
		return NULL;
	}

	// add stuff here as needed
	// added by pranavja

	cv->cv_wchan = wchan_create(cv->cv_name);
	if (cv->cv_wchan == NULL) {
		kfree(cv->cv_name);
		kfree(cv);
		return NULL;
	}

	spinlock_init(&cv->cv_spinlock);
	
	return cv;
}

void
cv_destroy(struct cv *cv)
{
	KASSERT(cv != NULL);

	// add stuff here as needed
	// added by pranavja
	spinlock_cleanup(&cv->cv_spinlock);
	wchan_destroy(cv->cv_wchan);
	
	kfree(cv->cv_name);
	kfree(cv);
}

void
cv_wait(struct cv *cv, struct lock *lock)
{
	// Write this
	// added by pranavja
	KASSERT(cv != NULL);
	KASSERT(lock!=NULL);
	KASSERT(curthread->t_in_interrupt == false);//check if it's not an interrrupt
	KASSERT(lock_do_i_hold(lock));
	
	spinlock_acquire(&cv->cv_spinlock);
	lock_release(lock);
	wchan_sleep(cv->cv_wchan, &cv->cv_spinlock);
	spinlock_release(&cv->cv_spinlock);
	
	lock_acquire(lock);
	

	//(void)cv;    // suppress warning until code gets written
	//(void)lock;  // suppress warning until code gets written
}

void
cv_signal(struct cv *cv, struct lock *lock)
{
	// Write this
	// added by pranavja
	KASSERT(cv != NULL);
	KASSERT(lock!=NULL);
	KASSERT(curthread->t_in_interrupt == false);//check if it's not an interrrupt
	KASSERT(lock_do_i_hold(lock));
	spinlock_acquire(&cv->cv_spinlock);
	
	wchan_wakeone(cv->cv_wchan, &cv->cv_spinlock);
	
	spinlock_release(&cv->cv_spinlock);
	//(void)cv;    // suppress warning until code gets written
	//(void)lock;  // suppress warning until code gets written
}

void
cv_broadcast(struct cv *cv, struct lock *lock)
{
	// Write this
	// added by pranavja
	KASSERT(cv != NULL);
	KASSERT(lock!=NULL);
	KASSERT(curthread->t_in_interrupt == false);//check if it's not an interrrupt
	KASSERT(lock_do_i_hold(lock));
	spinlock_acquire(&cv->cv_spinlock);
	
	wchan_wakeall(cv->cv_wchan, &cv->cv_spinlock);
	
	spinlock_release(&cv->cv_spinlock);

	//(void)cv;    // suppress warning until code gets written
	//(void)lock;  // suppress warning until code gets written
}


struct rwlock* rwlock_create(const char* name) {

	struct rwlock *rwlock;
	rwlock = kmalloc(sizeof(*rwlock));
	if (rwlock == NULL) {
		return NULL;
	}

	rwlock->rwlock_name = kstrdup(name);
	if (rwlock->rwlock_name == NULL) {
		kfree(rwlock);
		return NULL;
	}

	rwlock->rwlock_name = 	kstrdup(name);
	rwlock->rwlock_lock = lock_create(rwlock->rwlock_name);
	rwlock->rwlock_semaphore = sem_create(rwlock->rwlock_name, MAX_READERS);
	return rwlock;

}

void rwlock_destroy(struct rwlock* rwlock) {
	KASSERT(rwlock != NULL);
	sem_destroy(rwlock->rwlock_semaphore);
//	KASSERT(rwlock->rwlock_semaphore==NULL);
	lock_destroy(rwlock->rwlock_lock);
//	KASSERT(rwlock->rwlock_lock==NULL);
	kfree(rwlock->rwlock_name);
//	KASSERT(rwlock->rwlock_name==NULL);
	kfree(rwlock);
//	KASSERT(rwlock==NULL);
}

/**
 * 	 For acquiring a read lock
 * 	 1. acquire a lock
 * 	 2. acquire the resource using p
 * 	 3. release the lock
 */
void rwlock_acquire_read(struct rwlock *rwlock) {

	//Sammokka
	KASSERT(rwlock!=NULL);
	KASSERT(rwlock->rwlock_lock!=NULL);
	KASSERT(curthread->t_in_interrupt == false);

	//acquire a lock --? what kind of lock? wher does this lock come from? i think spinlock.
	//use rwlock->rwlock_spinlock? or rwlock->rwlock_sem->sem_spinlock?
	lock_acquire(rwlock->rwlock_lock);
	KASSERT(rwlock->rwlock_lock->lk_isLocked==true);

	//acquire resource using p?
	//using the semaphore?
	P(rwlock->rwlock_semaphore);

	//release the lock --> same as comment earlier
	lock_release(rwlock->rwlock_lock);

//	KASSERT(rwlock->rwlock_lock->lk_isLocked==false);

}

//release the resource using v
void rwlock_release_read(struct rwlock *rwlock) {
//	//Sammokka
	KASSERT(rwlock!=NULL);
	KASSERT(curthread->t_in_interrupt == false);
	KASSERT(rwlock->rwlock_semaphore->sem_count<MAX_READERS);


//	KASSERT(rwlock->rwlock_lock->lk_isLocked==true);
	V(rwlock->rwlock_semaphore);
}
/**
 * acquire the lock, P Max_reader types, release the lock
 */
void rwlock_acquire_write(struct rwlock *rwlock) {
	//sammokka

	KASSERT(rwlock!=NULL);
	KASSERT(curthread->t_in_interrupt == false);



	lock_acquire(rwlock->rwlock_lock);
	KASSERT(rwlock->rwlock_lock->lk_isLocked==true);

	for (int i = 0; i < MAX_READERS; i++) {
		P(rwlock->rwlock_semaphore);
	}
	lock_release(rwlock->rwlock_lock);
//	KASSERT(rwlock->rwlock_lock->lk_isLocked==false);
}

//release all resources
void rwlock_release_write(struct rwlock *rwlock){
	//sammokka

	KASSERT(rwlock!=NULL);
	KASSERT(curthread->t_in_interrupt == false);
	KASSERT(rwlock->rwlock_semaphore->sem_count==0);


	for (int i = 0; i < MAX_READERS; i++) {
		V(rwlock->rwlock_semaphore);
	}
}




