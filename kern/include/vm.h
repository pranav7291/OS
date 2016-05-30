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

#ifndef _VM_H_
#define _VM_H_

/*
 * VM system-related definitions.
 *
 * You'll probably want to add stuff here.
 */

#include <machine/vm.h>
#include <spinlock.h>


/* Fault-type arguments to vm_fault() */
#define VM_FAULT_READ        0    /* A read was attempted */
#define VM_FAULT_WRITE       1    /* A write was attempted */
#define VM_FAULT_READONLY    2    /* A write to a readonly page was attempted*/

#define MYVM_STACKPAGES 1024

#define SWAPDISK_SIZE 8192

#define FREE 1
#define DIRTY 2
#define CLEAN 3
#define FIXED 4

struct coremap_entry* coremap;
struct spinlock coremap_spinlock;
struct spinlock tlb_spinlock;
paddr_t first;
paddr_t last;
unsigned first_free_addr;
unsigned num_pages;
unsigned usedBytes;
bool swapping;
unsigned swapdisk_index;	//to store current index of swapdisk ptr
struct vnode *swapdisk_vnode;
struct lock *paging_lock;
struct bitmap *swapdisk_bitmap;
unsigned clock_pte_ptr;

//char *helper1[1000];
//char *helper2[2000];
//int h1index;
//int h2index;

int coremap_size;
int no_of_coremap_entries;
size_t num_swappages;

//added by sammokka
struct coremap_entry {
	size_t size;
	int state; //0 for clean, 1 for dirty, 2 for free
	bool busy;
	struct PTE *pte_ptr;
	bool clock;
	int cpu_num;
};

/* Initialization function */
void vm_bootstrap(void);

/* Fault handling function called by trap code */
int vm_fault(int faulttype, vaddr_t faultaddress);

/* Allocate/free kernel heap pages (called by kmalloc/kfree) */
vaddr_t alloc_kpages(unsigned npages);
void free_kpages(vaddr_t addr);

paddr_t page_alloc(struct PTE *pte);
void page_free(paddr_t paddr);

/*
 * Return amount of memory (in bytes) used by allocated coremap pages.  If
 * there are ongoing allocations, this value could change after it is returned
 * to the caller. But it should have been correct at some point in time.
 */
unsigned int coremap_used_bytes(void);

/* TLB shootdown handling called from interprocessor_interrupt */
void vm_tlbshootdown_all(void);
void vm_tlbshootdown(const struct tlbshootdown *);
void vm_tlbshootdownvaddr(vaddr_t vaddr);
void swapdisk_init(void);
int evict(void);
void swapout(vaddr_t swapaddr, paddr_t paddr);
void swapin(vaddr_t swapaddr, paddr_t paddr);

#endif /* _VM_H_ */
