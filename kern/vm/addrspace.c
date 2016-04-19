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
	as->pte = (struct PTE **) kmalloc(sizeof(struct PTE **) * 1024);
	for (int i = 0; i < 1024; i++){
		as->pte + i = NULL;
	}
	as->region = NULL;
	as->stack_ptr = (vaddr_t)0x80000000;
	as->heap_bottom = (vaddr_t)0;
	as->heap_top = (vaddr_t)0;

	return as;
}

int
as_copy(struct addrspace *old_addrspace, struct addrspace **ret)
{
	struct addrspace *new_as;

	new_as = as_create();
	if (new_as==NULL) {
		return ENOMEM;
	}

	/*
	 * Write this.
	 */

	//Copy regions
	struct region *old_region = old_addrspace->region;
	struct region *temp, *newreg;

	while (old_region != NULL) {
		if (new_as->region == NULL) {
			new_as->region = (struct region *) kmalloc(sizeof(struct region));
			new_as->region->next = NULL;
			newreg = new_as->region;
		} else {
			for (temp = new_as->region; temp->next != NULL; temp = temp->next);
			newreg = (struct region *) kmalloc(sizeof(struct region));
			temp->next = newreg;
		}
		newreg->start_vaddr = old_region->start_vaddr;
		newreg->size = old_region->size;
		newreg->permission = old_region->permission;
		newreg->old_permission = old_region->old_permission;
		newreg->next = NULL;

		old_region = old_region->next;
	}

//	//Copy pages
//	int result;
//	result = as_prepare_load(newas);	//COW
//	if (result){
//		as_destroy(newas);
//		return ENOMEM;
//	}
//
//	struct PTE *pte_new, *pte_old;
//	pte_new = newas->pages;
//	pte_old = old->pages;
//
//	while (pte_old != NULL) {
//		memmove((void *) PADDR_TO_KVADDR(pte_new->ppn),
//				(const void *) PADDR_TO_KVADDR(pte_old->ppn), PAGE_SIZE);
//		pte_new = pte_new->next;
//		pte_old = pte_old->next;
//	}

//Copy pte
	struct PTE **oldpte = old_addrspace->pte;
	struct PTE **newpte = new_as->pte;
	for (int i = 0; i < 1024; i++) {
		if (oldpte[i] != NULL) {
			oldpte[i] = (struct PTE *) kmalloc(sizeof(struct PTE *)); //second level kmalloc
			for (int j = 0; j < 1024; j++) {
				if (oldpte[i][j] != NULL) {
					newpte[i][j] = kmalloc(sizeof(struct PTE)); //struct kmalloc
					newpte[i][j]->vpn = oldpte[i][j]->vpn;
					newpte[i][j]->permission = oldpte[i][j]->permission;
					newpte[i][j]->ppn = oldpte[i][j]->ppn;
					newpte[i][j]->referenced = oldpte[i][j]->referenced;
					newpte[i][j]->state = oldpte[i][j]->state;
					newpte[i][j]->valid = oldpte[i][j]->valid;
				}
			}
		}
	}
	*ret = new_as;
	return 0;
}

void
as_destroy(struct addrspace *as)
{
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

		//		struct PTE *pte = as->pages;
		//		struct PTE *temp;
		//		while (pte != NULL){
		//			temp = pte;
		//			pte = pte->next;
		//			//write free page function
		//			kfree(temp);
		//		}

		struct PTE **pte = as->pte;
		for (int i = 0; i < 1024; i++) {
			if (pte[i] != NULL) {
				for (int j = 0; j < 1024; j++) {
					if (pte[i][j] != NULL) {
						kfree(pte[i][j]);	//kfree PTE
					}
				}
				kfree(pte[i]);	//kfree second level
			}
		}
		kfree(pte);	//kfree first level
	}

	kfree(as);
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
	/*
	 * Write this.
	 */

	(void)as;
	(void)vaddr;
	(void)memsize;
	(void)readable;
	(void)writeable;
	(void)executable;
	return ENOSYS;
}

int
as_prepare_load(struct addrspace *as)
{
	/*
	 * Write this.
	 */

	(void)as;
	return 0;
}

int
as_complete_load(struct addrspace *as)
{
	/*
	 * Write this.
	 */

	(void)as;
	return 0;
}

int
as_define_stack(struct addrspace *as, vaddr_t *stackptr)
{
	/*
	 * Write this.
	 */

	(void)as;

	/* Initial user-level stack pointer */
	*stackptr = USERSTACK;

	return 0;
}

