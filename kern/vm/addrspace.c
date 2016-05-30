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
#include <synch.h>
#include <bitmap.h>

struct addrspace *
as_create(void) {
	struct addrspace *as;

	as = kmalloc(sizeof(struct addrspace));
	if (as == NULL) {
		return NULL;
	}

	as->pte = NULL;
	as->pte_last = NULL;
	as->region = NULL;
	as->stack_ptr = USERSTACK;
	as->heap_bottom = (vaddr_t) 0;
	as->heap_top = (vaddr_t) 0;

	return as;
}

int as_copy(struct addrspace *old_addrspace, struct addrspace **ret) {
//	if(swapping){
//		lock_acquire(paging_lock);
//	}
	struct addrspace *new_as;
	new_as = as_create();
	if (new_as == NULL) {
		return ENOMEM;
	}

	//Copy regions
	struct region *old_region = old_addrspace->region;
	struct region *temp, *newreg;

	while (old_region != NULL) {
		if (new_as->region == NULL) {
			new_as->region = (struct region *) kmalloc(sizeof(struct region));
			if (new_as->region == NULL) {
				return ENOMEM;
			}
			new_as->region->next = NULL;
			newreg = new_as->region;
		} else {
			for (temp = new_as->region; temp->next != NULL; temp = temp->next)
				;
			newreg = (struct region *) kmalloc(sizeof(struct region));
			if (newreg == NULL) {
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

	struct PTE *old_pte_itr = old_addrspace->pte;
	struct PTE *new_pte;
	struct PTE *temp1;

	while (old_pte_itr != NULL) {
		if (new_as->pte == NULL) {
			new_as->pte = (struct PTE *) kmalloc(sizeof(struct PTE));
			if (new_as->pte == NULL) {
				return ENOMEM;
			}
			if (swapping) {
				new_as->pte->pte_lock = lock_create("pte_lock");
				lock_acquire(new_as->pte->pte_lock);
				lock_acquire(old_pte_itr->pte_lock);
			}
			new_as->pte->next = NULL;
			new_pte = new_as->pte;
			if (swapping) {
				new_as->pte_last = new_as->pte;
			}
		} else {
			if (!swapping) {
				for (temp1 = new_as->pte; temp1->next != NULL;
						temp1 = temp1->next)
					;
			}
			new_pte = (struct PTE *) kmalloc(sizeof(struct PTE));
			if (new_pte == NULL) {
				return ENOMEM;
			}
			if (swapping) {
				new_pte->pte_lock = lock_create("pte_lock");
				lock_acquire(new_pte->pte_lock);
				lock_acquire(old_pte_itr->pte_lock);
				new_as->pte_last->next = new_pte;
				new_as->pte_last = new_pte;
			}
			if (!swapping) {
				temp1->next = new_pte;
			}
		}
		new_pte->vpn = old_pte_itr->vpn;
		new_pte->permission = old_pte_itr->permission;
		new_pte->state = DISK;
//		if (swapping) {
//			unsigned x;
//			if (!bitmap_alloc(swapdisk_bitmap, &x)) {
//				new_pte->swapdisk_pos = x * PAGE_SIZE;
//			} else {
//				panic("\nRan out of swapdisk");
//			}
//		}
		swapdisk_index++;
		new_pte->swapdisk_pos = swapdisk_index * PAGE_SIZE;
		new_pte->next = NULL;

		if (swapping) {
//			KASSERT(old_pte_itr->state == MEM);
//			KASSERT(old_pte_itr->ppn != (paddr_t)0);
			if (old_pte_itr->state == DISK) {
//				lock_acquire(old_pte_itr->pte_lock);
				old_pte_itr->ppn = page_alloc(old_pte_itr);
				spinlock_acquire(&coremap_spinlock);
				coremap[(old_pte_itr->ppn / PAGE_SIZE)].busy = 1;
				spinlock_release(&coremap_spinlock);

				swapin(old_pte_itr->swapdisk_pos, old_pte_itr->ppn);

				spinlock_acquire(&coremap_spinlock);
				coremap[(old_pte_itr->ppn / PAGE_SIZE)].state = CLEAN;
				coremap[(old_pte_itr->ppn / PAGE_SIZE)].clock = true;
				spinlock_release(&coremap_spinlock);
				old_pte_itr->state = MEM;
//				lock_release(old_pte_itr->pte_lock);
			}
//			vm_tlbshootdownvaddr(old_pte_itr->vpn);
			swapout(new_pte->swapdisk_pos, old_pte_itr->ppn);
			new_pte->state = DISK;
			spinlock_acquire(&coremap_spinlock);
			coremap[(old_pte_itr->ppn / PAGE_SIZE)].busy = 0;
			spinlock_release(&coremap_spinlock);

			new_pte->ppn = (paddr_t) 0;
		} else {
			new_pte->ppn = page_alloc(new_pte);
			if (new_pte->ppn == (paddr_t) 0) {
				return ENOMEM;
			}
			memmove((void *) PADDR_TO_KVADDR(new_pte->ppn),
					(const void *) PADDR_TO_KVADDR(old_pte_itr->ppn),
					PAGE_SIZE);
		}
		if (swapping) {
			lock_release(old_pte_itr->pte_lock);
			lock_release(new_pte->pte_lock);
		}
		old_pte_itr = old_pte_itr->next;
	}

	new_as->heap_bottom = old_addrspace->heap_bottom;
	new_as->heap_top = old_addrspace->heap_top;
	new_as->stack_ptr = old_addrspace->stack_ptr;

//	if(swapping){
//		lock_release(paging_lock);
//	}

	*ret = new_as;
	return 0;
}

void as_destroy(struct addrspace *as) {
//	if(swapping){
//		lock_acquire(paging_lock);
//	}

	if (as != NULL) {
		vm_tlbshootdown_all();

		struct region *reg = as->region;
		struct region *temp1;
		while (reg != NULL) {
			temp1 = reg;
			reg = reg->next;
			kfree(temp1);
		}

		struct PTE *pte_itr = as->pte;
		struct PTE *temp2;
		while (pte_itr != NULL) {
			if (swapping) {
				lock_acquire(pte_itr->pte_lock);
				if (pte_itr->state == MEM) {
					page_free(pte_itr->ppn);
				}
//				KASSERT(bitmap_isset(swapdisk_bitmap, (pte_itr->swapdisk_pos / PAGE_SIZE)));
//				bitmap_unmark(swapdisk_bitmap,(pte_itr->swapdisk_pos / PAGE_SIZE));
			} else {
				page_free(pte_itr->ppn);
			}
			temp2 = pte_itr;

			if (swapping) {
				lock_release(temp2->pte_lock);
				lock_destroy(temp2->pte_lock);
			}
			pte_itr = pte_itr->next;
			kfree(temp2);
		}
		kfree(as);
	}
//	if(swapping){
//		lock_release(paging_lock);
//	}
}

void as_activate(void) {
	int i, spl;
	struct addrspace *as;

	as = proc_getas();
	if (as == NULL) {
		return;
	}

	/* Disable interrupts on this CPU while frobbing the TLB. */
	spl = splhigh();

	for (i = 0; i < NUM_TLB; i++) {
		tlb_write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
	}

	splx(spl);
}

void as_deactivate(void) {
}

int as_define_region(struct addrspace *as, vaddr_t vaddr, size_t memsize,
		int readable, int writeable, int executable) {
	//Aligning the region
	memsize += vaddr & ~(vaddr_t) PAGE_FRAME;
	vaddr &= PAGE_FRAME;
	memsize = (memsize + PAGE_SIZE - 1) & PAGE_FRAME;

	size_t num_pages;
	num_pages = memsize / PAGE_SIZE;

	struct region *reg_end;

	if (as->region == NULL) {
		as->region = (struct region *) kmalloc(sizeof(struct region));
		if (as->region == NULL) {
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
			return ENOMEM;
		}
		reg_end = reg_end->next;
		reg_end->next = NULL;
	}
	reg_end->num_pages = num_pages;
	reg_end->permission = 7 & (readable | writeable | executable);
	reg_end->base_vaddr = vaddr;
	as->heap_bottom = (vaddr + (PAGE_SIZE * num_pages)); //&PAGE_FRAME;
	as->heap_top = as->heap_bottom;

	return 0;
}

int as_prepare_load(struct addrspace *as) {
	struct region * regionitr;
	regionitr = as->region;
	while (regionitr != NULL) {
		regionitr->old_permission = regionitr->permission;
		regionitr->permission = 7 & (010 | 001);	//write + execute
		regionitr = regionitr->next;
	}
	return 0;
}

int as_complete_load(struct addrspace *as) {
	struct region * regionitr;
	regionitr = as->region;
	while (regionitr != NULL) {
		regionitr->permission = regionitr->old_permission;
		regionitr = regionitr->next;
	}
	return 0;
}

int as_define_stack(struct addrspace *as, vaddr_t *stackptr) {
	/* Initial user-level stack pointer */
	*stackptr = USERSTACK;	// - (MYVM_STACKPAGES * PAGE_SIZE);
	as->stack_ptr = USERSTACK;	// - (MYVM_STACKPAGES * PAGE_SIZE);
	return 0;
}
