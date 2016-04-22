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
#include <addrspace.h>
#include <vm.h>
#include <proc.h>

/*
 * Note! If OPT_DUMBVM is set, as is the case until you start the VM
 * assignment, this file is not compiled or linked or in any way
 * used. The cheesy hack versions in dumbvm.c are used instead.
 */

struct addrspace *
as_create(void)
{
	struct addrspace *as;

	as = kmalloc(sizeof(struct addrspace));
	if (as == NULL) {
		return NULL;
	}

	/*
	 * Initialize as needed.
	 */
	//as->pages = NULL;
	as->pte =  kmalloc(sizeof(struct PTE *) * 1024);
	if(as->pte==NULL) {
		kfree(as);
		return NULL;
	}
//	for (int i = 0; i < 1024; i++){
//		as->pte[i] = NULL;
//	}
//	if (as->pte[0] == NULL) {
//		panic("Sammokka 1");
//	}
//	if (as->pte[1] == NULL) {
//		panic("Sammokka 2");
//	}
	as->region = NULL;
	as->stack_ptr = (vaddr_t) 0x80000000;
	as->heap_bottom = (vaddr_t) 0;
	as->heap_top = (vaddr_t) 0;

	return as;
}

int
as_copy(struct addrspace *old_addrspace, struct addrspace **ret)
{
	struct addrspace *new_as;

	new_as = as_create();
	if (new_as==NULL) {
		as_destroy(new_as);
		return ENOMEM;
	}

	//Copy regions
	struct region *old_region = old_addrspace->region;
	struct region *temp, *newreg;

	while (old_region != NULL) {
		if (new_as->region == NULL) {
			new_as->region = (struct region *) kmalloc(sizeof(struct region));
			if (new_as->region == NULL) {
				as_destroy(new_as);
				return ENOMEM;
			}
			new_as->region->next = NULL;
			newreg = new_as->region;
		} else {
			for (temp = new_as->region; temp->next != NULL; temp = temp->next);
			newreg = (struct region *) kmalloc(sizeof(struct region));
			if(newreg==NULL) {
				as_destroy(new_as);
				return ENOMEM;
			}
			temp->next = newreg;
		}
		newreg->base_vaddr = old_region->base_vaddr;
		newreg->num_pages = old_region->num_pages;
		newreg->permission = old_region->permission;
		newreg->old_permission = old_region->old_permission;
		newreg->next = NULL;

		old_region = old_region->next;
	}

	//copy PTEs
	struct PTE **oldpte = old_addrspace->pte;
	struct PTE **newpte = new_as->pte;
	for (int i = 0; i < 1024; i++) {
		if (oldpte[i] != NULL) {
			spinlock_acquire(oldpte[i]->ptelock);
			newpte[i] = kmalloc(sizeof(struct PTE) * 1024); //second level kmalloc
			if (newpte[i] == NULL) {
				spinlock_release(oldpte[i]->ptelock);
				as_destroy(new_as);
				return ENOMEM;
			}
			for (int j = 0; j < 1024; j++) {
				if (oldpte[i][j].ppn != 0) {
					if (newpte[i] == NULL) {
						as_destroy(new_as);
						return ENOMEM;
					}
					spinlock_init(newpte[i][j].ptelock);
					newpte[i][j].vpn = oldpte[i][j].vpn;
					newpte[i][j].ppn = page_alloc();
					memmove((void *) PADDR_TO_KVADDR(newpte[i][j].ppn), (const void *) PADDR_TO_KVADDR(oldpte[i][j].ppn), PAGE_SIZE);
					newpte[i][j].permission = oldpte[i][j].permission;
					newpte[i][j].referenced = oldpte[i][j].referenced;
					newpte[i][j].state = oldpte[i][j].state;
					newpte[i][j].valid = oldpte[i][j].valid;
				}
			}
		spinlock_release(oldpte[i]->ptelock);
		}
	}

	*ret = new_as;
	return 0;
}

void
as_destroy(struct addrspace *as) {
	/*
	 * Clean up as needed.
	 */
	if (as != NULL) {
		struct region *reg = as->region;
		struct region *temp1;
		while (reg != NULL) {
			temp1 = reg;
			reg = reg->next;
			kfree(temp1);
		}

		struct PTE **pte = as->pte;
		for (int i = 0; i < 1024; i++) {
			if (pte[i] != NULL) {
				vm_tlbshootdown_all();
				spinlock_cleanup(pte[i]->ptelock);
				page_free(pte[i]->ppn & PAGE_FRAME);
				kfree(pte[i]);	//kfree second level
			}
		}
		kfree(pte);	//kfree first level
		pte = NULL;
		kfree(as);
	}
}

void
as_activate(void)
{
	int i, spl;
	struct addrspace *as;

	as = proc_getas();
	if (as == NULL) {
		return;
	}

	/* Disable interrupts on this CPU while frobbing the TLB. */
	spl = splhigh();

	for (i=0; i<NUM_TLB; i++) {
		tlb_write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
	}

	splx(spl);
}

void
as_deactivate(void)
{
	/*
	 * Write this. For many designs it won't need to actually do
	 * anything. See proc.c for an explanation of why it (might)
	 * be needed.
	 */
}

/*
 * Set up a segment at virtual address VADDR of size MEMSIZE. The
 * segment in memory extends from VADDR up to (but not including)
 * VADDR+MEMSIZE.
 *
 * The READABLE, WRITEABLE, and EXECUTABLE flags are set if read,
 * write, or execute permission should be set on the segment. At the
 * moment, these are ignored. When you write the VM system, you may
 * want to implement them.
 */
int
as_define_region(struct addrspace *as, vaddr_t vaddr, size_t memsize,
		 int readable, int writeable, int executable)
{
	//Aligning the region
	memsize += vaddr & ~(vaddr_t) PAGE_FRAME;
	vaddr &= PAGE_FRAME;
	memsize = (memsize + PAGE_SIZE - 1) & PAGE_FRAME;

	size_t num_pages;
	num_pages = memsize / PAGE_SIZE;

	struct region *reg_end;

	if(as->region == NULL){
		as->region = (struct region *) kmalloc(sizeof(struct region));
		if (as->region == NULL) {
			as_destroy(as);
			return ENOMEM;
		}
		as->region->next = NULL;
		reg_end = as->region;
	} else {
		for (reg_end = as->region; reg_end->next != NULL;
				reg_end = reg_end->next)
			;
		reg_end->next = (struct region *) kmalloc(sizeof(struct region));
		if (reg_end->next == NULL) {
			as_destroy(as);
			return ENOMEM;
		}
		reg_end = reg_end->next;
		reg_end->next = NULL;
	}
	reg_end->num_pages = num_pages;
	reg_end->permission = 7 & (readable | writeable | executable);
	reg_end->base_vaddr = vaddr;
	as->heap_bottom = (vaddr + (PAGE_SIZE * num_pages))&PAGE_FRAME;
	as->heap_top = as->heap_bottom;

	//todo do I create heap and stack here???

	return 0;
}

int
as_prepare_load(struct addrspace *as)
{
	struct region * regionitr;
	regionitr = as->region;
	while(regionitr != NULL){
		regionitr->old_permission = regionitr->permission;
		regionitr->permission = 7 & (010 | 001);	//write + execute
		regionitr = regionitr->next;
	}
	//todo which page table entries do I setup here?? page table entries for each region
	return 0;
}

int
as_complete_load(struct addrspace *as)
{
	struct region * regionitr;
	regionitr = as->region;
	while(regionitr != NULL){
		regionitr->permission = regionitr->old_permission;
		regionitr = regionitr->next;
	}
	//todo invalidate TLB entries ???
	return 0;
}

int
as_define_stack(struct addrspace *as, vaddr_t *stackptr)
{
	/* Initial user-level stack pointer */
	*stackptr = USERSTACK;
	as->stack_ptr = USERSTACK;
	return 0;
}

//void free_as(struct addrspace *as) {
//	if (as == NULL) {
//		return;
//	}
//	struct region *reg = as->region;
//	struct region *temp1;
//	while (reg != NULL) {
//		temp1 = reg;
//		reg = reg->next;
//		kfree(temp1);
//	}
//
//}
