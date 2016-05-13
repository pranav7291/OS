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
#include <kern/stat.h>
#include <vnode.h>
#include <vfs.h>
#include <bitmap.h>
#include <uio.h>

struct coremap_entry* coremap;
struct bitmap *swapdisk_bitmap;
struct vnode *swapdisk_vnode;

void vm_bootstrap(void) {
	last = ram_getsize();
	first = ram_getfirstfree();

	//pages below first (between first and 0) is already occupied.
	//todo for part 2 - allocate all pages between 0 and first to kernel
	//for the
	num_pages = last/PAGE_SIZE;
	coremap_size = num_pages * sizeof(struct coremap_entry);


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
		coremap[i].vaddr = PADDR_TO_KVADDR(i); //have to store the virtual address here
		coremap[i].pte_ptr = 0;
		coremap[i].busy = 1;
	}
	for (unsigned i = first_free_addr; i < num_pages; i++) {
		coremap[i].state = FREE;
		coremap[i].size = 1;
		coremap[i].vaddr = PADDR_TO_KVADDR(i); //have to store the virtual address here
		coremap[i].pte_ptr = 0;
		coremap[i].busy = 0;
	}
	spinlock_init(&coremap_spinlock);
	spinlock_init(&tlb_spinlock);
}

/* Allocate/free some kernel-space virtual pages */
vaddr_t alloc_kpages(unsigned npages) {
	int page_ind;
	vaddr_t returner;

	spinlock_acquire(&coremap_spinlock);

//check for a free space
	for (unsigned i = first_free_addr; i < num_pages; i++) {
		int flag = 1;
		if (coremap[i].state == FREE && ((i + npages) < num_pages)) {
			//free space found. Check if next nPages are also free
			for (unsigned j = 0; j < npages; j++) {
				if (coremap[i + j].state == FREE) { ///1 is free
				} else {//next nPages are not free. break to start of outer for loop
					flag = 0;
					break;
				}
			}
			if (flag == 0) {
				continue;
			}
			//npages of free space found.
			coremap[i].state = FIXED;
			coremap[i].busy = 0;
			coremap[i].size = npages;
			for (unsigned j = 1; j < npages; j++) {
				coremap[i + j].state = FIXED;
				coremap[i + j].busy = 0;
				coremap[i + j].size = 1;
				coremap[i + j].pte_ptr = NULL;
			}
			bzero((void *) PADDR_TO_KVADDR(i * PAGE_SIZE), PAGE_SIZE * npages);

			usedBytes = usedBytes + PAGE_SIZE * npages;
			spinlock_release(&coremap_spinlock);
			returner = PADDR_TO_KVADDR(PAGE_SIZE * i);
			return returner;
		}
	}
	//todo no page found. write logic for swapout
	if (swapping) {
		page_ind = evict();
		coremap[page_ind].state = FIXED;
		coremap[page_ind].busy = 0;
		coremap[page_ind].size = npages;
		for (unsigned j = 1; j < npages; j++) {
			coremap[page_ind + j].state = FIXED;
			coremap[page_ind + j].busy = 0;
			coremap[page_ind + j].size = 1;
			coremap[page_ind + j].pte_ptr = NULL;
		}
		bzero((void *) PADDR_TO_KVADDR(page_ind * PAGE_SIZE),
				PAGE_SIZE * npages);
		usedBytes = usedBytes + PAGE_SIZE * npages;

		spinlock_release(&coremap_spinlock);

		returner = PADDR_TO_KVADDR(PAGE_SIZE * page_ind);
		return returner;
	} else {
		spinlock_release(&coremap_spinlock);
		return 0;
	}
}

void free_kpages(vaddr_t addr) {

	//todo free the memory
	spinlock_acquire(&coremap_spinlock);
	paddr_t paddr = KVADDR_TO_PADDR(addr) & PAGE_FRAME;
	int i = paddr/PAGE_SIZE;
	int index = 0;
	int temp = coremap[i].size;
	for (int j = i; j < i + temp; j++) {
		vm_tlbshootdownvaddr(addr + (index * PAGE_SIZE));
		coremap[j].state = FREE;
		coremap[j].size = 1;
		coremap[j].busy = 0;
		index++;
	}

	usedBytes = usedBytes - temp * PAGE_SIZE;
	//iterate over the coremap to find the address
	spinlock_release(&coremap_spinlock);
	return;
}

paddr_t page_alloc(struct PTE *pte) {
	int page_ind;
	spinlock_acquire(&coremap_spinlock);

	for (unsigned i = first_free_addr; i < num_pages; i++) {
		if (coremap[i].state == FREE && coremap[i].busy == 0) {
			coremap[i].state = DIRTY;
			coremap[i].size = 1;
			coremap[i].busy = 1;
			coremap[i].pte_ptr = pte;

			bzero((void *) PADDR_TO_KVADDR(i * PAGE_SIZE), PAGE_SIZE);

			usedBytes = usedBytes + PAGE_SIZE;
			spinlock_release(&coremap_spinlock);
			paddr_t returner = PAGE_SIZE * i;
			return returner;
		}
	}
	//todo no page found. write logic for swapout
	if (swapping) {
		page_ind = evict();
		coremap[page_ind].state = DIRTY;
		coremap[page_ind].size = 1;
		coremap[page_ind].busy = 1;
		coremap[page_ind].pte_ptr = pte;

		bzero((void *) PADDR_TO_KVADDR(page_ind * PAGE_SIZE), PAGE_SIZE);

		usedBytes = usedBytes + PAGE_SIZE;
		spinlock_release(&coremap_spinlock);
		paddr_t returner = PAGE_SIZE * page_ind;
		return returner;
	} else {
		spinlock_release(&coremap_spinlock);
		return 0;
	}
}

void page_free(paddr_t paddr) {

	//todo free the memory
	spinlock_acquire(&coremap_spinlock);
	vm_tlbshootdownvaddr(PADDR_TO_KVADDR(paddr));
	int i = paddr/PAGE_SIZE;

	coremap[i].state = FREE;
	coremap[i].size = 1;
	coremap[i].busy = 0;

	usedBytes = usedBytes - PAGE_SIZE;
	spinlock_release(&coremap_spinlock);
	return;
}

void swapdisk_init(void){
	struct stat swapdisk_stat;
	char *swapdisk = kstrdup("lhd0raw:");
	int result;
	swapping = false;

	result = vfs_open(swapdisk, O_RDWR, 0, &swapdisk_vnode);
	if (result) {
		swapping = false;
	}
	if (swapping) {
		VOP_STAT(swapdisk_vnode, &swapdisk_stat);
		num_swappages = swapdisk_stat.st_size / PAGE_SIZE;
		//todo swap lock init here
		swapdisk_bitmap = bitmap_create(SWAPDISK_SIZE);
		KASSERT(swapdisk_bitmap != NULL);
	}
}

void swapout(vaddr_t vaddr, paddr_t paddr){
	int result;
	struct iovec iov;
	struct uio ku;
	vaddr_t kva=PADDR_TO_KVADDR(paddr);

	uio_kinit(&iov, &ku, (char *)kva, PAGE_SIZE, vaddr, UIO_WRITE);
	result=VOP_WRITE(swapdisk_vnode, &ku);
	if (result) {
		panic("\nSwapout error:%d", result);
	}
}

void swapin(vaddr_t vaddr, paddr_t paddr){
	int result;
	struct iovec iov;
	struct uio ku;
	vaddr_t kva=PADDR_TO_KVADDR(paddr);

	uio_kinit(&iov, &ku, (char *)kva, PAGE_SIZE, vaddr, UIO_READ);
	result=VOP_READ(swapdisk_vnode, &ku);
	if (result) {
		panic("\nSwapin error:%d", result);
	}
}

int evict(){
	int victim, rand_num, flag;
	flag = 0;
	while (flag == 0) {
		rand_num = random();
		victim = rand_num % num_pages;
		if ((coremap[victim].state == DIRTY || coremap[victim].state == CLEAN)) {
			flag = 1;
		}
	}
	if(flag == 0){
		return -1;
	}
	paddr_t paddr = (PAGE_SIZE * victim) + first_free_addr;
	//todo shoot down from the TLB. Check this
//	vm_tlbshootdownvaddr(coremap[victim].vaddr);
	if (coremap[victim].state == DIRTY){
		//todo set the VA for all pages
		swapout(coremap[victim].vaddr, paddr);
	}

	spinlock_acquire(&coremap_spinlock);
	coremap[victim].state = CLEAN;
	coremap[victim].vaddr = 0;
	spinlock_release(&coremap_spinlock);
	return victim;
}

unsigned
int coremap_used_bytes() {
	return usedBytes;
}

void vm_tlbshootdown_all(void) {
	spinlock_acquire(&tlb_spinlock);
	int x = splhigh();
	for (int i = 0; i < NUM_TLB; i++){
		tlb_write(TLBHI_INVALID(i),TLBLO_INVALID(),i);
	}
	splx(x);
	spinlock_release(&tlb_spinlock);
}

void vm_tlbshootdown(const struct tlbshootdown *ts) {
	uint32_t lo, hi;
	int tlb_ind;
	spinlock_acquire(&tlb_spinlock);
	tlb_ind = ts->tlb_indicator;
	int x = splhigh();

	tlb_read(&hi, &lo, tlb_ind);
	int i = tlb_probe(hi,0);
	if(i >= 0){
		tlb_write(TLBHI_INVALID(tlb_ind),TLBLO_INVALID(),i);
	}
	splx(x);
	spinlock_release(&tlb_spinlock);
}

void vm_tlbshootdownvaddr(vaddr_t vaddr) {
	uint32_t lo, hi;
	spinlock_acquire(&tlb_spinlock);
	int x = splhigh();
	int i=tlb_probe(vaddr & PAGE_FRAME, 0);
	if(i >= 0)
	{
		tlb_read(&hi, &lo, i);
		tlb_write(TLBHI_INVALID(i),TLBLO_INVALID(),i);
	}
	splx(x);
	spinlock_release(&tlb_spinlock);
}

int vm_fault(int faulttype, vaddr_t faultaddress) {

	//1. Check if the fault address is valid -
	vaddr_t stack_top, stack_bottom;
	vaddr_t vbase, vtop;
	bool fheap = false;
	bool fstack = false;
	bool fregion = false;
//	KASSERT(faultaddress!=0x4009C0);
//	struct region *fault_reg = NULL;
	struct addrspace *as = curproc->p_addrspace;
	struct region *reg = as->region;
	stack_top = USERSTACK;
	stack_bottom = USERSTACK - MYVM_STACKPAGES * PAGE_SIZE;
	if (faultaddress >= as->heap_bottom && faultaddress < as->heap_top) {
		//fault address is in heap
		fheap = true;
	} else if ((faultaddress >= stack_bottom) && (faultaddress < stack_top)) {
		//fault address is in stack
		fstack = true;
	} else { //search regions
		while (reg != NULL) {
			vbase = reg->base_vaddr;
			vtop = vbase + (PAGE_SIZE * reg->num_pages);
			if (faultaddress >= vbase && faultaddress < vtop) {
//				fault_reg = reg;
				fregion  = true;
				break;
			}
			reg = reg->next;
		}
	}
	if (!(fregion || fheap || fstack)) {
		return EFAULT;
	}

	//2. Check if the operation is valid by checking the page permission
	//first check if present in the page table, if not, create page

	int found = 0;
	int tlb_index = -1;
	struct PTE *curr, *last;

	if (as->pte == NULL) {
		found = 1;
		as->pte = kmalloc(sizeof(struct PTE));
		if(as->pte == NULL){
			return ENOMEM;
		}
		as->pte->ppn = page_alloc(as->pte);
		//todo write swapin somewhere here
		if(as->pte->ppn == (vaddr_t)0){
			return ENOMEM;
		}
		as->pte->vpn = faultaddress & PAGE_FRAME;
		as->pte->next = NULL;
		curr = as->pte;
	} else {
		//if the first pte is the required pte
		for (curr = as->pte; curr != NULL; curr = curr->next){
			if(curr->vpn == (faultaddress & PAGE_FRAME)){
				found = 1;
				break;
			}
		}
	}
	if (found == 0) {
		//vaddr not found. kmalloc and add to tlb
		curr = kmalloc(sizeof(struct PTE));
		if(curr == NULL){
			return ENOMEM;
		}
		curr->ppn = page_alloc(curr);
		if(curr->ppn == (vaddr_t)0){
			return ENOMEM;
		}
		curr->vpn = faultaddress & PAGE_FRAME;
		curr->next = NULL;
		for (last = as->pte; last->next != NULL; last = last->next);
		last->next = curr;
	}

	if (faulttype == VM_FAULT_READ || faulttype == VM_FAULT_WRITE) {
		//random write
		if((curr->state == DISK) && (swapping == true)){
			swapin(curr->vpn, curr->ppn);
			curr->state = MEM;
		}
		spinlock_acquire(&tlb_spinlock);
		int x = splhigh();
		tlb_index = tlb_probe(faultaddress & PAGE_FRAME, 0);
		if(tlb_index < 0){
			paddr_t phy_page_no = curr->ppn;
			tlb_random((faultaddress & PAGE_FRAME ), ((phy_page_no & PAGE_FRAME)| TLBLO_VALID));
		}
		splx(x);
		spinlock_release(&tlb_spinlock);
	} else if (faulttype == VM_FAULT_READONLY){
		spinlock_acquire(&tlb_spinlock);
		int x = splhigh();
		tlb_index = tlb_probe(faultaddress & PAGE_FRAME, 0);
		if(tlb_index >= 0){
			paddr_t phy_page_no = curr->ppn;
			tlb_write((faultaddress & PAGE_FRAME), ((phy_page_no & PAGE_FRAME) | TLBLO_DIRTY | TLBLO_VALID), tlb_index);
		}
		splx(x);
		spinlock_release(&tlb_spinlock);
	}

	return 0;
}
