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

#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <spl.h>
#include <cpu.h>
#include <spinlock.h>
#include <proc.h>
#include <current.h>
#include <mips/tlb.h>
#include <addrspace.h>
#include <vm.h>

/*
 * Dumb MIPS-only "VM system" that is intended to only be just barely
 * enough to struggle off the ground. You should replace all of this
 * code while doing the VM assignment. In fact, starting in that
 * assignment, this file is not included in your kernel!
 *
 * NOTE: it's been found over the years that students often begin on
 * the VM assignment by copying dumbvm.c and trying to improve it.
 * This is not recommended. dumbvm is (more or less intentionally) not
 * a good design reference. The first recommendation would be: do not
 * look at dumbvm at all. The second recommendation would be: if you
 * do, be sure to review it from the perspective of comparing it to
 * what a VM system is supposed to do, and understanding what corners
 * it's cutting (there are many) and why, and more importantly, how.
 */

/* under dumbvm, always have 72k of user stack */
/* (this must be > 64K so argument blocks of size ARG_MAX will fit) */
#define DUMBVM_STACKPAGES    18

/*
 * Wrap ram_stealmem in a spinlock.
 */
//static struct spinlock stealmem_lock = SPINLOCK_INITIALIZER;
struct coremap_entry coremap[];

static struct spinlock *allock_lock;

paddr_t first;
paddr_t last;
int noOfPages;

int entry_count;
paddr_t lowest_available = 0;
void vm_bootstrap(void) {
	first = ram_getfirstfree();
	last = ram_getsize();

	//pages below first (between first and 0) is already occupied.
	//todo for part 2 - allocate all pages between 0 and first to kernel
	//for the
	first = first + noOfPages * sizeof(coremap); //change first to the address after the coremap allocation
	noOfPages = last/PAGE_SIZE;
	entry_count = (last - first)/PAGE_SIZE;
//	paddr_t temp = first;
	coremap = (struct coremap_entry *) PADDR_TO_KVADDR(first);
	for (int i = 0; i < entry_count; i++) {
		coremap[i].state = 2;
		coremap[i].size = 1;
		coremap[i].addr = 0;
		//what will address field have? todo
	}
	spinlock_init(allock_lock);
}



/* Allocate/free some kernel-space virtual pages */
vaddr_t alloc_kpages(unsigned npages) {

	spinlock_acquire(allock_lock);
//check for a free space
	for (int i = 0; i < noOfPages; i++) {
		if (coremap[i].state != 1) {
			//free space found. Check if next nPages are also free
			for (int j = 0; j < npages; j++) {
				if (coremap[i + j].state == 1) { ///1 is free
					continue;
				}
				break; //next nPages are not free.Go back to outer loop
			}


		}
	}





}

void free_kpages(vaddr_t addr) {

	//todo free the memory
	spinlock_acquire(allock_lock);
	paddr_t paddr = KVADDR_TO_PADDR(addr);

	//iterate over the coremap to find the address
	for (int i = 0; i < 200; i++) {
		if(coremap[i]!=NULL) {
			if(coremap[i].addr==paddr && coremap[i].state != 1) {
				coremap[i].addr = 1;
				spinlock_release(allock_lock);
				return;
			}
		}
	}
	spinlock_release(allock_lock);
	return;
}

unsigned
int coremap_used_bytes() {

	/* dumbvm doesn't track page allocations. Return 0 so that khu works. */

	return 0;
}

void vm_tlbshootdown_all(void) {
	panic("myvm tried to do tlb shootdown?!\n");
}

void vm_tlbshootdown(const struct tlbshootdown *ts) {
	(void) ts;
	panic("myvm tried to do tlb shootdown?!\n");
}

int vm_fault(int faulttype, vaddr_t faultaddress) {
	(void) faulttype;
	(void) faultaddress;
	return 0;
}
