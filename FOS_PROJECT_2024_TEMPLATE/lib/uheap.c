#include <inc/lib.h>
#define saad_size NUM_OF_UHEAP_PAGES

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
uint32 indexing[saad_size];

void* malloc(uint32 size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #12] [3] USER HEAP [USER SIDE] - malloc()
	// Write your code here, remove the panic and write your code
	uint32 omar_totalPagesToAllocate = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
	uint32 omar_availablePagesCount = 0;
	if (size <= DYN_ALLOC_MAX_BLOCK_SIZE) {

		return alloc_block_FF(size);

	}

	int addressArrayIndex = 0;
	uint32 omar_startAddr = myEnv->block_user_hard_limit + PAGE_SIZE;
	for (uint32 omar_endAddr = omar_startAddr + PAGE_SIZE;
			omar_endAddr < USER_HEAP_MAX; omar_endAddr += PAGE_SIZE) {
		if (indexing[addressArrayIndex] != 0) {
			omar_startAddr = omar_endAddr;
			omar_availablePagesCount = 0;
		} else {
			omar_availablePagesCount++;
		}
		addressArrayIndex++;
		if (omar_availablePagesCount != omar_totalPagesToAllocate)
			continue;

		uint32 total = omar_startAddr + (omar_availablePagesCount * PAGE_SIZE);
		if (total < USER_HEAP_MAX) {
			int i = addressArrayIndex - omar_availablePagesCount;
			while (i < addressArrayIndex) {
				indexing[i] = omar_startAddr;
				i++;
			}
			sys_allocate_user_mem(omar_startAddr,
					omar_totalPagesToAllocate * PAGE_SIZE);
			return (uint32*) omar_startAddr;
		} else {
			return NULL;
		}
	}

	return NULL;
	//panic("malloc() is not implemented yet...!!");
	return NULL;
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy

}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #14] [3] USER HEAP [USER SIDE] - free()
	// Write your code here, remove the panic and write your code
	if (virtual_address < (void*) USER_HEAP_MAX
			&& virtual_address
					>= (void*) (myEnv->block_user_hard_limit + PAGE_SIZE)) {
		bool saad_isAddressFound = 0;
		int index = 0;
		uint32 saad_allocatedPages = 0;
		while (index < saad_size) {
			int match = (indexing[index] == (uint32) virtual_address);

			switch (match) {
			case 1:
				saad_allocatedPages++;
				indexing[index] = 0;
				saad_isAddressFound = 1;
				break;

			default:
				if (saad_isAddressFound) {
					if ((void*) indexing[index] != virtual_address)
						break;
				}
				break;
			}

			index++;
		}

		uint32 addressPointer = (uint32) virtual_address;

		sys_free_user_mem(addressPointer, saad_allocatedPages);
	} else if (virtual_address >= (void*) USER_HEAP_START) {
		if (virtual_address > (void*) myEnv->block_user_hard_limit)
			return;
		free_block(virtual_address);
	}
	//panic("free() is not implemented yet...!!");
}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
	//panic("smalloc() is not implemented yet...!!");
	uint32 totalPagesToAllocate = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
	uint32 availablePagesCount = 0;
	int addressArrayIndex = 0;
	uint32 startAddr = myEnv->block_user_hard_limit + PAGE_SIZE;
	for (uint32 endAddr = startAddr + PAGE_SIZE; endAddr < USER_HEAP_MAX;
			endAddr += PAGE_SIZE) {
		if (indexing[addressArrayIndex] != 0) {
			startAddr = endAddr;
			availablePagesCount = 0;
		} else {
			availablePagesCount++;
		}
		addressArrayIndex++;
		if (availablePagesCount != totalPagesToAllocate)
			continue;

		uint32 total = startAddr + (availablePagesCount * PAGE_SIZE);
		if (total < USER_HEAP_MAX) {
			int i = addressArrayIndex - availablePagesCount;
			while (i < addressArrayIndex) {
				indexing[i] = startAddr;
				i++;
			}
			int ret = sys_createSharedObject(sharedVarName, size, isWritable,
					(uint32*) startAddr);
			if (ret == E_SHARED_MEM_EXISTS) {
				return NULL;
			} else if (ret == E_NO_SHARE) {
				return NULL;
			}
			return (uint32*) startAddr;

		}

	}
	return NULL;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()
	// Write your code here, remove the panic and write your code
	//panic("sget() is not implemented yet...!!");
	uint32 size = sys_getSizeOfSharedObject(ownerEnvID, sharedVarName);
	if (size == E_SHARED_MEM_NOT_EXISTS) {
		return NULL;
	}
	uint32 totalPagesToAllocate = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
	uint32 availablePagesCount = 0;
	int addressArrayIndex = 0;
	uint32 startAddr = myEnv->block_user_hard_limit + PAGE_SIZE;
	for (uint32 endAddr = startAddr + PAGE_SIZE; endAddr < USER_HEAP_MAX;
			endAddr += PAGE_SIZE) {
		if (indexing[addressArrayIndex] != 0) {
			startAddr = endAddr;
			availablePagesCount = 0;
		} else {
			availablePagesCount++;
		}
		addressArrayIndex++;
		if (availablePagesCount != totalPagesToAllocate)
			continue;

		uint32 total = startAddr + (availablePagesCount * PAGE_SIZE);
		if (total < USER_HEAP_MAX) {
			int i = addressArrayIndex - availablePagesCount;
			while (i < addressArrayIndex) {
				indexing[i] = startAddr;
				i++;
			}
			int ret = sys_getSharedObject(ownerEnvID, sharedVarName,
					(uint32*) startAddr);
			if (ret == E_SHARED_MEM_NOT_EXISTS) {
				return NULL;
			}
			return (uint32*) startAddr;

		}

	}
	return NULL;
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [USER SIDE] - sfree()
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");
}


//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//[PROJECT]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}


//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");

}
void shrink(uint32 newSize)
{
	panic("Not Implemented");

}
void freeHeap(void* virtual_address)
{
	panic("Not Implemented");

}
