#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"
struct spinlock spin;
uint8 spin_lock=1;
//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC
int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
	//TODO: [PROJECT'24.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator
	// Write your code here, remove the panic and write your code
	//panic("initialize_kheap_dynamic_allocator() is not implemented yet...!!");
	block_start = daStart;
	block_sbrk = daStart + initSizeToAllocate;
	block_hard_limit = daLimit;
	uint32 adrOfMyKerHeapStrt = block_start;


while (adrOfMyKerHeapStrt  < block_sbrk) {
	    struct FrameInfo *framWillBeMapped = NULL;

	    if (allocate_frame(&framWillBeMapped) == 0) {


	        map_frame(ptr_page_directory, framWillBeMapped, adrOfMyKerHeapStrt , PERM_PRESENT | PERM_WRITEABLE);

	        framWillBeMapped->va = adrOfMyKerHeapStrt ;
	    } else {
	        return E_NO_MEM;
	    }

	    adrOfMyKerHeapStrt  += PAGE_SIZE;
	}

		initialize_dynamic_allocator(daStart, initSizeToAllocate);
		return 0;
}

void* sbrk(int numOfPages)
{
	/* numOfPages > 0: move the segment break of the kernel to increase the size of its heap by the given numOfPages,
	 * 				you should allocate pages and map them into the kernel virtual address space,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * numOfPages = 0: just return the current position of the segment break
	 *
	 * NOTES:
	 * 	1) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted
	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, return -1
	 */

	//MS2: COMMENT THIS LINE BEFORE START CODING==========
	//return (void*)-1 ;
	//====================================================

	//TODO: [PROJECT'24.MS2 - #02] [1] KERNEL HEAP - sbrk
	// Write your code here, remove the panic and write your code
	//panic("sbrk() is not implemented yet...!!");
	if (numOfPages == 0) {
	    return (void*) block_sbrk;
	}

	if (numOfPages < 0) {
	    return (void*) -1;
	}

	if (numOfPages > 0) {
	    uint32 new_block_brk = block_sbrk + (numOfPages * PAGE_SIZE);

	    if (new_block_brk < block_hard_limit) {
	        uint32 prev_brk = block_sbrk;
	        uint32 addrWill_Iterate_on_It = block_sbrk;

	        while (addrWill_Iterate_on_It< new_block_brk) {
	            struct FrameInfo *tmp_freme = NULL;
	            int tempRetreveVariaable = allocate_frame(&tmp_freme);

	            if (tmp_freme != NULL) {
	                map_frame(ptr_page_directory, tmp_freme,addrWill_Iterate_on_It, PERM_PRESENT | PERM_WRITEABLE);
	                tmp_freme->va = addrWill_Iterate_on_It;
	            } else {
	                return (void*) -1;
	            }

	            addrWill_Iterate_on_It += PAGE_SIZE;
	        }

	        block_sbrk = new_block_brk;
	        return (void*) prev_brk;
	    }
	        return (void*) -1;

	}

	return (void*) -1;

}

//TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator

void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT'24.MS2 - #03] [1] KERNEL HEAP - kmalloc
	// Write your code here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");
	if(spin_lock)
	{
		init_spinlock(&spin, "spinlock initlization");
		spin_lock=0;
	}

	uint8 helding_spinLock = holding_spinlock(&spin);
	if (!helding_spinLock)
	{
		acquire_spinlock(&spin);
	}
	if (size == 0) {
		release_spinlock(&spin);
		return NULL;
	}
	uint32 start_address = 0;
	uint32 contiguous_pages = 0;

	if (!isKHeapPlacementStrategyFIRSTFIT()){
		release_spinlock(&spin);
		return NULL;
	}

	if (size > DYN_ALLOC_MAX_BLOCK_SIZE) {
		uint32 required_pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;

		for (uint32 current_address = block_hard_limit + PAGE_SIZE;
				current_address < KERNEL_HEAP_MAX; current_address +=
				PAGE_SIZE)
				{

			if (contiguous_pages == required_pages) {

				return allocate_pages(start_address, contiguous_pages);
			}

			if (contiguous_pages == 0) {
				start_address = current_address;
			}


			uint32 *page_table = NULL;
			struct FrameInfo *frame_info = get_frame_info(ptr_page_directory,
					current_address, &page_table);
			if (frame_info != NULL) contiguous_pages = 0;
			else contiguous_pages++;

		}
	} else if (size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
		release_spinlock(&spin);
		return alloc_block_FF(size);
	}
	release_spinlock(&spin);
	return NULL;

	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy

}
void* allocate_pages(uint32 start_address, uint32 page_count) {
    uint32 current_address = start_address;

    for (int remaining_pages = page_count; remaining_pages > 0; remaining_pages--) {
        struct FrameInfo *frame_info = NULL;
        int allocation_status = allocate_frame(&frame_info);

        if (frame_info != NULL) {
            map_frame(ptr_page_directory, frame_info, current_address, PERM_WRITEABLE);
            uint32 *page_table = NULL;
            get_frame_info(ptr_page_directory, current_address, &page_table)->va = current_address;
            current_address += PAGE_SIZE;
        } else {
        	release_spinlock(&spin);
            panic("No Memory Space");
        }
    }

    uint32 *page_table = NULL;
    get_frame_info(ptr_page_directory, start_address, &page_table)->size = page_count;
    release_spinlock(&spin);
    return (void*)start_address;
}
void kfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");
		if(spin_lock)
		{
			init_spinlock(&spin, "spinlock initlization");
			spin_lock=0;
		}
		uint8 helding_spinLock = holding_spinlock(&spin);
		if (!helding_spinLock)
		{
			acquire_spinlock(&spin);
		}
	 	uint32 va = (uint32)virtual_address;
	    uint32* page_table_ret = NULL;
	    struct FrameInfo* frame_info = get_frame_info(ptr_page_directory, va, &page_table_ret);

	    if (frame_info == NULL) {
	    	release_spinlock(&spin);
	        //panic("invalid address");
	    	return;
	    }

	    while (1) {
	        if (virtual_address >= (void*)KERNEL_HEAP_START && virtual_address < (void*)block_sbrk)
	        {
	            free_block(virtual_address);
	            release_spinlock(&spin);
	            break;
	        }

	        if (virtual_address >= (void*)(block_hard_limit + PAGE_SIZE) &&
	            virtual_address < (void*)KERNEL_HEAP_MAX) {
	            uint32 frames_to_free = frame_info->size;

	            while (frames_to_free > 0) {
	                unmap_frame(ptr_page_directory, va);
	                va += PAGE_SIZE;
	                frames_to_free--;
	            }

	            break;
	        }
	        release_spinlock(&spin);
	        return;
	        //panic("kfree() invalid virtual address: Out of kernel heap range!");
	    }
	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #05] [1] KERNEL HEAP - kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");
		uint32 vartualADDRESS = (uint32) virtual_address % PAGE_SIZE;
		uint32 *ptrPagetble = NULL;
		struct FrameInfo *poitr = get_frame_info(ptr_page_directory,(uint32) virtual_address, &ptrPagetble );
		if (poitr != NULL) {
			return (uint32) to_physical_address(poitr) + vartualADDRESS;
		}
		return 0;
	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT'24.MS2 - #06] [1] KERNEL HEAP - kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");
	uint32 offest_that_added_to_virtal_aDD = (uint32) physical_address % PAGE_SIZE;
		struct FrameInfo *PointerThatPointFrameINMEMory = to_frame_info((uint32) physical_address);

		if (PointerThatPointFrameINMEMory != NULL && PointerThatPointFrameINMEMory->va != 0) {
			return PointerThatPointFrameINMEMory->va + offest_that_added_to_virtal_aDD;
		}
		return 0;
	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}
//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, if moved to another loc: the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT'24.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc
	// Write your code here, remove the panic and write your code
	return NULL;
	panic("krealloc() is not implemented yet...!!");
}
