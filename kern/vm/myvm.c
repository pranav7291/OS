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
#include <spl.h>


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
	for(unsigned i = 0; i<first_free_addr; i++) {
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
	spinlock_init(&coremap_spinlock);
}

/* Allocate/free some kernel-space virtual pages */
vaddr_t alloc_kpages(unsigned npages) {

	spinlock_acquire(&coremap_spinlock);

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
//			memset((void *)PADDR_TO_KVADDR(((i)*PAGE_SIZE)),0,PAGE_SIZE);
			for (unsigned j = 1; j < npages; j++) {
				coremap[i + j].state = FIXED;
				coremap[i + j].size = 1;
//				memset((void *)PADDR_TO_KVADDR(((i + j)*PAGE_SIZE)),0,PAGE_SIZE);
			}
			usedBytes = usedBytes + PAGE_SIZE*npages;
			spinlock_release(&coremap_spinlock);
			vaddr_t returner = PADDR_TO_KVADDR(PAGE_SIZE*i);
			return returner;
		}
	}
	spinlock_release(&coremap_spinlock);
	return 0;
}

void free_kpages(vaddr_t addr) {

	//todo free the memory
	spinlock_acquire(&coremap_spinlock);
	paddr_t paddr = KVADDR_TO_PADDR(addr);
	int i = paddr/PAGE_SIZE;

	int temp = coremap[i].size;
	for (int j = i; j < i + temp; j++) {
		coremap[j].state = FREE;
		coremap[j].size = 1;
	}

	usedBytes = usedBytes - temp * PAGE_SIZE;
	//iterate over the coremap to find the address
	spinlock_release(&coremap_spinlock);
	return;
}

paddr_t page_alloc() {

	spinlock_acquire(&coremap_spinlock);

	for (unsigned i = first_free_addr; i < noOfPages; i++){
		if (coremap[i].state == FREE){
			coremap[i].state = DIRTY;
			coremap[i].size = 1;
			memset((void *) ((PADDR_TO_KVADDR(i * PAGE_SIZE)) /*& PAGE_FRAME*/),0,PAGE_SIZE);

			usedBytes = usedBytes + PAGE_SIZE;
			spinlock_release(&coremap_spinlock);
			paddr_t returner = PAGE_SIZE*i;
			return returner;
		}
	}

	spinlock_release(&coremap_spinlock);
	return 0;
}

void page_free(paddr_t paddr) {

	//todo free the memory
	spinlock_acquire(&coremap_spinlock);
	int i = paddr/PAGE_SIZE;

	coremap[i].state = FREE;
	coremap[i].size = 1;

	usedBytes = usedBytes - PAGE_SIZE;
	spinlock_release(&coremap_spinlock);
	return;
}

unsigned
int coremap_used_bytes() {
	return usedBytes;
}

void vm_tlbshootdown_all(void) {
	//panic("myvm tried to do tlb shootdown?!\n");
	spinlock_acquire(&coremap_spinlock);
	int x = splhigh();
	for (int i = 0; i < NUM_TLB; i++){
		tlb_write(TLBHI_INVALID(i),TLBLO_INVALID(),i);
	}
	splx(x);
	spinlock_release(&coremap_spinlock);
}

void vm_tlbshootdown(const struct tlbshootdown *ts) {
	uint32_t lo, hi;
	int tlb_ind;
	spinlock_acquire(&coremap_spinlock);
	tlb_ind = ts->tlb_indicator;
	int x = splhigh();
	tlb_read(&hi, &lo, tlb_ind);
	if(lo & TLBLO_VALID){
		//todo remove entry from coremap maybe
	}
	tlb_write(TLBHI_INVALID(tlb_ind),TLBLO_INVALID(),tlb_ind);
	splx(x);
	spinlock_release(&coremap_spinlock);
}

int vm_fault(int faulttype, vaddr_t faultaddress) {

	//1. Check if the fault address is valid -

//	vaddr_t vbase, vtop;
//	bool fheap = false;
//	bool fstack = false;
//	bool fregion = false;
//	struct region *fault_reg = NULL;
	struct addrspace *as = curproc->p_addrspace;
//	struct region *reg = as->region;
//	if (faultaddress >= as->heap_bottom && faultaddress < as->heap_top) {
//		//fault address is in heap
//		fheap = true;
//	} else if (faultaddress >= as->stack_ptr
//			&& faultaddress <= (vaddr_t) 0x80000000) {
//		//fault address is in heap
//		fstack = true;
//	} else { //search regions
//		while (reg != NULL) {
//			vbase = reg->base_vaddr;
//			vtop = vbase + (PAGE_SIZE * reg->num_pages);
//			if (faultaddress >= vbase && faultaddress < vtop) {
////				fault_reg = reg;
//				fregion  = true;
//				break;
//			}
//			reg = reg->next;
//		}
//	}
//	if (!(fregion | fheap | fstack)) {
//		return EFAULT;
//	}

	//2. Check if the operation is valid by checking the page permission
	//first check if present in the page table, if not, create page

	unsigned mask_for_first_10_bits = 0xFFC00000;
	unsigned first_10_bits = faultaddress & mask_for_first_10_bits;
	first_10_bits = first_10_bits >> 22;

	unsigned mask_for_second_10_bits = 0x003FF000;
	unsigned next_10_bits = faultaddress & mask_for_second_10_bits;
	next_10_bits = next_10_bits >> 12;
	paddr_t paddr;
	int tlb_index = -1;
	if (as->pte[first_10_bits] != NULL) {
		//look for second level
		paddr_t paddr_temp = as->pte[first_10_bits][next_10_bits].ppn;
		if (paddr_temp == 0) {//if not null, you get your page table entry here.
			paddr = page_alloc();
			if (paddr == 0) {
				return EFAULT; //out of pages
			}
//			spinlock_acquire(as->pte[first_10_bits]->ptelock);
			as->pte[first_10_bits][next_10_bits].ppn = paddr;
			as->pte[first_10_bits][next_10_bits].vpn = faultaddress;
//			spinlock_release(as->pte[first_10_bits]->ptelock);
			//random tlb write
		} /*else {
			//random tlb write
			panic("pranav");
		}*/
	} else {
		//	panic("whooaaaa");
		as->pte[first_10_bits] = kmalloc(sizeof(struct PTE) * 1024);
		for(int i=0; i<1024;i++) {
			as->pte[first_10_bits][i].ppn = 0;
		}
		paddr_t paddr = page_alloc();
		if (paddr == 0) {
			return EFAULT; //out of pages
		}
//		spinlock_acquire(as->pte[first_10_bits]->ptelock);
		as->pte[first_10_bits][next_10_bits].ppn = paddr;
		as->pte[first_10_bits][next_10_bits].vpn = faultaddress;
//		spinlock_release(as->pte[first_10_bits]->ptelock);
		//todo set permissions also
//		tlb_random(paddr, faultaddress);
		//random write
	}
	if (faulttype == VM_FAULT_READ || faulttype == VM_FAULT_WRITE) {
		//random write
		int x = splhigh();
		paddr_t phy_page_no = as->pte[first_10_bits][next_10_bits].ppn;
		tlb_random((faultaddress & PAGE_FRAME ), ((phy_page_no & PAGE_FRAME)| TLBLO_VALID));
		splx(x);
	} else if (faulttype == VM_FAULT_READONLY){
		int x = splhigh();
		tlb_index = tlb_probe(faultaddress & PAGE_FRAME, 0);
		paddr_t phy_page_no = as->pte[first_10_bits][next_10_bits].ppn;
		tlb_write((faultaddress & PAGE_FRAME), ((phy_page_no & PAGE_FRAME) | TLBLO_DIRTY | TLBLO_VALID), tlb_index);
		splx(x);
	}

	return 0;
}
