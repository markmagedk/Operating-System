/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
__inline__ uint32 get_block_size(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (*curBlkMetaData) & ~(0x1);
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
__inline__ int8 is_free_block(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (~(*curBlkMetaData) & 0x1) ;
}

//===========================
// 3) ALLOCATE BLOCK:
//===========================

void *alloc_block(uint32 size, int ALLOC_STRATEGY)
{
	void *va = NULL;
	switch (ALLOC_STRATEGY)
	{
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list)
{
	cprintf("=========================================\n");
	struct BlockElement* blk ;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(size: %d, isFree: %d)\n", get_block_size(blk), is_free_block(blk)) ;
	}
	cprintf("=========================================\n");

}
//
////********************************************************************************//
////********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

bool is_initialized = 0;
//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================
void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (initSizeOfAllocatedSpace % 2 != 0) initSizeOfAllocatedSpace++; //ensure it's multiple of 2
		if (initSizeOfAllocatedSpace == 0)
			return ;
		is_initialized = 1;
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #04] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("initialize_dynamic_allocator is not implemented yet");
	//Your Code is Here...
	LIST_INIT(&freeBlocksList);
		struct BlockElement* first_free_block;
		first_free_block = (struct BlockElement*) (daStart + 2 * sizeof(int));

		uint32* begining_block = (uint32*) daStart;
		*begining_block = 1;
		uint32* end_block = (uint32*) (daStart + initSizeOfAllocatedSpace
				- sizeof(int));
		*end_block = 1;
		set_block_data(first_free_block, initSizeOfAllocatedSpace - 4 * sizeof(int),
				0);

		LIST_INSERT_HEAD(&freeBlocksList, first_free_block);
}
//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================
void set_block_data(void* va, uint32 totalSize, bool isAllocated)
{
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("set_block_data is not implemented yet");
	//Your Code is Here...
	if (totalSize % 2 != 0)
			totalSize++;
		uint32 *header = (uint32*) (va - sizeof(int));
		uint32 *footer = (uint32*) (va + totalSize);
		if (isAllocated) {
			*header = (totalSize + 1 + (2 * sizeof(int)));
			*footer = (totalSize + 1 + (2 * sizeof(int)));
		} else {
			*header = (totalSize + 2 * sizeof(int));
			*footer = (totalSize + 2 * sizeof(int));
		}
}


//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *alloc_block_FF(uint32 size)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (size % 2 != 0) size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE ;
		if (!is_initialized)
		{
			uint32 required_size = size + 2*sizeof(int) /*header & footer*/ + 2*sizeof(int) /*da begin & end*/ ;
			uint32 da_start = (uint32)sbrk(ROUNDUP(required_size, PAGE_SIZE)/PAGE_SIZE);
			uint32 da_break = (uint32)sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_FF is not implemented yet");
	//Your Code is Here...
	if (size == 0) {
				return NULL;
			} else {
				uint32 required_size = size + 2 * sizeof(int);
				bool find = 0;
				uint32 *start_ptr = NULL;
				uint32 *end_ptr = NULL;
				uint32 remain_size = 0;
				uint32 first_fit_free_block_size = 0;
				struct BlockElement *it;

				LIST_FOREACH(it , &freeBlocksList)
				{
					uint32 *curr_free_block_size = (uint32 *) it;
					curr_free_block_size--;

					if (required_size <= *curr_free_block_size) {
						find = 1;
						start_ptr = (uint32*) it;
						end_ptr = (uint32*) ((uint32) start_ptr + required_size);
						first_fit_free_block_size = *curr_free_block_size;
						break;
					}
				}

				if (find) {
					remain_size = first_fit_free_block_size - required_size;

					if (remain_size >= 2 * DYN_ALLOC_MIN_BLOCK_SIZE) {
						set_block_data(start_ptr, size, 1);
						set_block_data(end_ptr, remain_size - (2 * sizeof(int)), 0);

						struct BlockElement *new_free_block =
								(struct BlockElement *) end_ptr;
						LIST_INSERT_AFTER(&freeBlocksList, it, new_free_block);
						LIST_REMOVE(&freeBlocksList, it);

						return (void *) start_ptr;
					} else {
						set_block_data(start_ptr,
								first_fit_free_block_size - 2 * sizeof(int), 1);
						LIST_REMOVE(&freeBlocksList, it);
						return (void *) start_ptr;
					}
			}
			else
			{
				uint32 s_break = (uint32)sbrk(0);

				 s_break = (uint32) sbrk(ROUNDUP(size, PAGE_SIZE)/PAGE_SIZE);


				if (s_break == -1) {

					return NULL;
				} else {
					uint32* daEnd = (uint32*) (s_break + PAGE_SIZE - sizeof(int));
					*daEnd = 1;
					set_block_data((void*) s_break, PAGE_SIZE-2*sizeof(int), 1);
					free_block((void*) s_break);
					return alloc_block_FF(size);
				}

			}
		}
			return NULL;
}
//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_BF is not implemented yet");
	//Your Code is Here...
	if (size % 2 != 0)
			size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE;
		if (size == 0) {
				return NULL;
			} else {
				uint32 required_size = size + 2 * sizeof(int);
				bool find = 0;
				uint32 *start_ptr = NULL;
				uint32 *end_ptr = NULL;
				uint32 remain_size = 0;
				uint32 first_fit_free_block_size = 0;
				struct BlockElement *it;
				uint32 best_size = 4294967295;
				LIST_FOREACH(it , &freeBlocksList)
				{
					uint32 *curr_free_block_size = (uint32 *) it;
					curr_free_block_size--;

					if (required_size <= *curr_free_block_size) {
						if(*curr_free_block_size-required_size<best_size)
						{
							best_size=*curr_free_block_size-required_size;
							find = 1;
							start_ptr = (uint32*) it;
						    end_ptr = (uint32*) ((uint32) start_ptr + required_size);
						    first_fit_free_block_size = *curr_free_block_size;
						}

					}
				}
				struct BlockElement* best_fit_block=(struct BlockElement*)start_ptr;

				if (find) {
					remain_size = first_fit_free_block_size - required_size;

					if (remain_size >= 2 * DYN_ALLOC_MIN_BLOCK_SIZE) {
						set_block_data(start_ptr, size, 1);
						set_block_data(end_ptr, remain_size - (8), 0);

						struct BlockElement *new_free_block =
								(struct BlockElement *) end_ptr;
						LIST_INSERT_AFTER(&freeBlocksList, best_fit_block, new_free_block);
						LIST_REMOVE(&freeBlocksList, best_fit_block);

						return (void *) start_ptr;
					} else {
						set_block_data(start_ptr,
						first_fit_free_block_size - 2 * sizeof(int), 1);
						LIST_REMOVE(&freeBlocksList, best_fit_block);
						return (void *) start_ptr;
					}
				} else {
					void* s_break = sbrk(required_size);
					if (s_break == (void*) -1) {
						return NULL;
					}
					return s_break;
				}
			}
			return NULL;
}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	//TODO: [PROJECT'24.MS1 - #07] [3] DYNAMIC ALLOCATOR - free_block
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("free_block is not implemented yet");
	//Your Code is Here...
	uint32 *header = (uint32 *) va;
		header--;
		if (*header % 2 == 0) {
			return;
		}
		*header = *header - 1;
		uint32 Size=*header;

		uint32 *footer = (uint32 *) va;
		footer=(uint32*)(Size +(uint32)footer-2*sizeof(int));
		*footer = *footer - 1;

		uint32 *next_header = (uint32 *) va;
		next_header=(uint32 *)(Size +(uint32) next_header-sizeof(int));
		uint32 *prev_footer = (uint32 *) va;
		prev_footer =(uint32 *)((uint32)(prev_footer)- 2*sizeof(int));

		if (*prev_footer == 1 && *next_header == 1) {
			LIST_INSERT_HEAD(&freeBlocksList, (struct BlockElement * )va);
		} else if (*prev_footer == 1 && *next_header % 2 != 0) {
			LIST_INSERT_HEAD(&freeBlocksList, (struct BlockElement * )va);
		} else if (*prev_footer % 2 == 0 && *next_header == 1) {
			set_block_data((struct BlockElement *) (va - *prev_footer),
					*prev_footer + *header - (8), 0);

		} else if (*prev_footer % 2 == 0 && *next_header % 2 != 0) {
			set_block_data((struct BlockElement *) (va - *prev_footer),
					*prev_footer + *header - (8), 0);

		} else if (*prev_footer % 2 != 0 && *next_header % 2 == 0) {

			uint32 block_size = *header;
			uint32 next_block_size = *next_header;
			uint32 total_size_curr_next_block = block_size + next_block_size;

			uint32 *next_footer = (uint32 *) va;
			next_footer += total_size_curr_next_block / sizeof(uint32) - 2;
			*header = total_size_curr_next_block;
			*next_footer = total_size_curr_next_block;

			uint32 *ptr = next_header;
			ptr++;

			if (*prev_footer == 1) {
				LIST_INSERT_HEAD(&freeBlocksList, (struct BlockElement * )va);
				LIST_REMOVE(&freeBlocksList, (struct BlockElement * )ptr);
			} else {
				LIST_INSERT_BEFORE(&freeBlocksList, (struct BlockElement * )ptr,
						(struct BlockElement * )va);
				LIST_REMOVE(&freeBlocksList, (struct BlockElement * )ptr);
			}

			*footer = 0;
			*next_header = 0;
		} else if (*prev_footer % 2 == 0 && *next_header % 2 == 0) {

			uint32 *ptr = next_header;
			ptr++;

			set_block_data((struct BlockElement *) (va - *prev_footer),
					*prev_footer + *header + *next_header - (8), 0);

			LIST_REMOVE(&freeBlocksList, (struct BlockElement * )ptr);

		} else if (*prev_footer % 2 != 0 && *next_header % 2 != 0) {

			uint32 *ptr = next_header;
			uint32 size = 0;

			while (*ptr % 2 != 0 || *ptr == 1) {
				size = *ptr - 1;
				ptr += size / sizeof(uint32);
				if (size == 0) {
					break;
				}
			}

			if (*ptr == 1) {
				LIST_INSERT_TAIL(&freeBlocksList, (struct BlockElement * )va);
			} else {
				ptr++;
				LIST_INSERT_BEFORE(&freeBlocksList, (struct BlockElement * )ptr,
						(struct BlockElement * )va);
			}
		}
		return;
}

//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size)
{
	//TODO: [PROJECT'24.MS1 - #08] [3] DYNAMIC ALLOCATOR - realloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("realloc_block_FF is not implemented yet");
	//Your Code is Here...
	if(va == NULL && new_size == 0)
			return NULL;

		if(new_size == 0)
		{
			free_block(va);
			return NULL;
		}
		if (new_size % 2 != 0)
			new_size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (new_size < DYN_ALLOC_MIN_BLOCK_SIZE)
			new_size = DYN_ALLOC_MIN_BLOCK_SIZE;
		if(va == NULL)
			return (void *)alloc_block_FF(new_size);


		uint32 resize_block = new_size + 2 * sizeof(int);

		if(resize_block == get_block_size(va))
			return (void *)va;

		if(resize_block < get_block_size(va))
		{
			uint32 remain_size = get_block_size(va) - resize_block;
			if(remain_size < 2 * DYN_ALLOC_MIN_BLOCK_SIZE)
			{
				return (void *)va;
			}
			else
			{
				uint32 *new_block_va =  (uint32 *)((uint32)va + resize_block);
				set_block_data(va , new_size , 1);
				set_block_data(new_block_va , remain_size - 2 * sizeof(int) , 1);
				free_block(new_block_va);
				return (void *)va;
			}
		}
		else
		{
			uint32* next_block_va = (uint32*)((uint32)va + get_block_size(va));
			uint32 total_size_curr_next_block = get_block_size(va) + get_block_size(next_block_va);

			if (is_free_block(next_block_va)&& resize_block <= total_size_curr_next_block)
			{
				if (total_size_curr_next_block- resize_block< 2* DYN_ALLOC_MIN_BLOCK_SIZE)
				{
					set_block_data(va, get_block_size(va) - 2 * sizeof(int), 0);
					set_block_data(next_block_va,get_block_size(next_block_va) - 2 * sizeof(int), 0);
					set_block_data(va, total_size_curr_next_block- 2 * sizeof(int), 1);
					LIST_REMOVE(&freeBlocksList, (struct BlockElement * ) next_block_va);
					return (void *) va;
				}
				else
				{
					uint32 *new_block_va = (uint32 *) ((uint32) va + resize_block);
					set_block_data(va, new_size, 1);
					set_block_data(new_block_va,
							(total_size_curr_next_block - resize_block)
									- 2 * sizeof(int), 0);
					LIST_INSERT_BEFORE(&freeBlocksList,
							(struct BlockElement * )next_block_va,
							(struct BlockElement * ) new_block_va);
					LIST_REMOVE(&freeBlocksList,
							(struct BlockElement * ) next_block_va);
					return (void *) va;
				}
			}
			else
			{
				void *alocate_block = alloc_block_FF(new_size);
				if (alocate_block  == NULL)
				{
					return NULL;
				}
				memcpy(alocate_block, va, get_block_size(va)- 2 * sizeof(int));
				free_block(va);
			    return (void *)alocate_block;
			}
		}

		return NULL;
}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
//=========================================
// [7] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size)
{
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [8] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}
