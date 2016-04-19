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
#include <proc.h>
#include <current.h>
#include <mips/tlb.h>
#include <addrspace.h>
#include <vm.h>

struct coremap_entry* coremap;

void vm_bootstrap(void) {
	last = ram_getsize();
	first = ram_getfirstfree();

	//pages below first (between first and 0) is already occupied.
	//todo for part 2 - allocate all pages between 0 and first to kernel
	//for the
	noOfPages = last/PAGE_SIZE;
	coremap_size = noOfPages * sizeof(struct coremap_entry);


//	no_of_coremap_entries = (last - first)/PAGE_SIZE;


	coremap = (struct coremap_entry *) PADDR_TO_KVADDR(first);
	first = first + coremap_size; //change first to the address after the coremap allocation
	first = ROUNDUP(first, PAGE_SIZE);

//	paddr_t temp = first;

	first_free_addr = first / PAGE_SIZE;
	usedBytes = 0;
	for(unsigned i = 0; i>first_free_addr; i++) {
		coremap[i].state = FIXED;
		coremap[i].size = 1;
		coremap[i].addr = 0; //have to store the virtual address here
	}
	for (unsigned i = first_free_addr; i < noOfPages; i++) {
		coremap[i].state = FREE;
		coremap[i].size = 1;
		coremap[i].addr = 0; //have to store the virtual address here
		//what will address field have? todo
	}
	spinlock_init(&allock_lock);
}

/* Allocate/free some kernel-space virtual pages */
vaddr_t alloc_kpages(unsigned npages) {

	spinlock_acquire(&allock_lock);

//check for a free space
	for (unsigned i = first_free_addr; i < noOfPages; i++) {
		int flag = 1;
		if (coremap[i].state != DIRTY && coremap[i].state != FIXED) {
			//free space found. Check if next nPages are also free
			for (unsigned j = 0; j < npages; j++) {
				if (coremap[i + j].state != DIRTY && coremap[i + j].state != FIXED) { ///1 is free

				} else {
					flag = 0;
					break; //next nPages are not free. break to start of outer loop
				}
			}
			if (flag == 0) {
				continue;
			}
			//npages of free space found.
			coremap[i].state = FIXED;
			coremap[i].size = npages;
			for (unsigned j = 1; j < npages; j++) {
				coremap[i + j].state = FIXED;
				coremap[i + j].size = 1;
			}
			usedBytes = usedBytes + PAGE_SIZE*npages;
			spinlock_release(&allock_lock);
			vaddr_t returner = PADDR_TO_KVADDR(PAGE_SIZE*i);
			return returner;
		}
	}
	spinlock_release(&allock_lock);
	return 0;
}

void free_kpages(vaddr_t addr) {

	//todo free the memory
	spinlock_acquire(&allock_lock);
	paddr_t paddr = KVADDR_TO_PADDR(addr);
	int i = paddr/PAGE_SIZE;

	int temp = coremap[i].size;
	for (int j = i; j < i + temp; j++) {
		coremap[j].state = FREE;
		coremap[j].size = 1;
	}

	usedBytes = usedBytes - temp * PAGE_SIZE;
	//iterate over the coremap to find the address
	spinlock_release(&allock_lock);
	return;
}

paddr_t page_alloc() {

	spinlock_acquire(&allock_lock);

	for (unsigned i = first_free_addr; i < noOfPages; i++){
		if (coremap[i].state == FREE){
			coremap[i].state = DIRTY;
			coremap[i].size = 1;
			usedBytes = usedBytes + PAGE_SIZE;
			spinlock_release(&allock_lock);
			paddr_t returner = PAGE_SIZE*i;
			return returner;
		}
	}

	spinlock_release(&allock_lock);
	return 0;
}

void page_free(paddr_t paddr) {

	//todo free the memory
	spinlock_acquire(&allock_lock);
	int i = paddr/PAGE_SIZE;

//	int temp = coremap[i].size;
//	for (int j = i; j < i + temp; j++) {
	coremap[i].state = FREE;
	coremap[i].size = 1;
	}

	usedBytes = usedBytes - PAGE_SIZE;
	spinlock_release(&allock_lock);
	return;
}

unsigned
int coremap_used_bytes() {

	/* dumbvm doesn't track page allocations. Return 0 so that khu works. */

	return usedBytes;
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
