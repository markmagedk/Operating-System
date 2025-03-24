#include <inc/memlayout.h>
#include "shared_memory_manager.h"

#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/queue.h>
#include <inc/environment_definitions.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/syscall.h>
#include "kheap.h"
#include "memory_manager.h"

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//
struct Share* get_share(int32 ownerID, char* name);

//===========================
// [1] INITIALIZE SHARES:
//===========================
//Initialize the list and the corresponding lock
void sharing_init()
{
#if USE_KHEAP
	LIST_INIT(&AllShares.shares_list) ;
	init_spinlock(&AllShares.shareslock, "shares lock");
#else
	panic("not handled when KERN HEAP is disabled");
#endif
}

//==============================
// [2] Get Size of Share Object:
//==============================
int getSizeOfSharedObject(int32 ownerID, char* shareName)
{
	//[PROJECT'24.MS2] DONE
	// This function should return the size of the given shared object
	// RETURN:
	//	a) If found, return size of shared object
	//	b) Else, return E_SHARED_MEM_NOT_EXISTS
	//
	struct Share* ptr_share = get_share(ownerID, shareName);
	if (ptr_share == NULL)
		return E_SHARED_MEM_NOT_EXISTS;
	else
		return ptr_share->size;

	return 0;
}

//===========================================================


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
//===========================
// [1] Create frames_storage:
//===========================
// Create the frames_storage and initialize it by 0
inline struct FrameInfo** create_frames_storage(int numOfFrames)
{
	//TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_frames_storage()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_frames_storage is not implemented yet");
	//Your Code is Here...
	struct FrameInfo **Same7_frames = (struct FrameInfo**) kmalloc(
			numOfFrames * sizeof(struct FrameInfo*));
	uint32 i = 0;
	while (i < numOfFrames) {
		Same7_frames[i] = NULL;
		i++;
	}
	if (Same7_frames == NULL) {
		return NULL;
	}
	return Same7_frames;

}

//=====================================
// [2] Alloc & Initialize Share Object:
//=====================================
//Allocates a new shared object and initialize its member
//It dynamically creates the "framesStorage"
//Return: allocatedObject (pointer to struct Share) passed by reference
struct Share* create_share(int32 ownerID, char* shareName, uint32 size, uint8 isWritable)
{
	//TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_share is not implemented yet");
	//Your Code is Here...
	unsigned int Same7_sizeOf_kmalloc = sizeof(struct Share);
	struct Share* Same7_share = (struct Share*) kmalloc(Same7_sizeOf_kmalloc);
	Same7_share->ownerID = ownerID;
	strcpy(Same7_share->name, shareName);
	Same7_share->size = size;
	Same7_share->isWritable = isWritable;
	Same7_share->references = (int32) 1;
	Same7_share->ID = (int32) ((uint32) Same7_share & 0x7FFFFFFF);

	int numOfFrames = ROUNDUP(size,PAGE_SIZE) / PAGE_SIZE;
	Same7_share->framesStorage = create_frames_storage(numOfFrames);
	if (Same7_share == NULL) {
		kfree((void*) Same7_share);
		return NULL;
	}
	return Same7_share;

}

//=============================
// [3] Search for Share Object:
//=============================
//Search for the given shared object in the "shares_list"
//Return:
//	a) if found: ptr to Share object
//	b) else: NULL
struct Share* get_share(int32 ownerID, char* name)
{
	//TODO: [PROJECT'24.MS2 - #17] [4] SHARED MEMORY - get_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("get_share is not implemented yet");
	//Your Code is Here...
	struct Share* Same7forEachPtr = NULL;
	struct Share* Same7Share = NULL;
	LIST_FOREACH(Same7forEachPtr,&AllShares.shares_list)
	{
		if (strcmp(name, Same7forEachPtr->name) == 0
				&& Same7forEachPtr->ownerID == ownerID) {
			Same7Share = Same7forEachPtr;
			break;
		}
	}
	if (Same7Share == NULL) {
		return NULL;
	}

	return Same7Share;

}

//=========================
// [4] Create Share Object:
//=========================
int createSharedObject(int32 ownerID, char* shareName, uint32 size, uint8 isWritable, void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #19] [4] SHARED MEMORY [KERNEL SIDE] - createSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("createSharedObject is not implemented yet");
	//Your Code is Here...

	struct Env* myenv = get_cpu_proc(); //The calling environment
	acquire_spinlock(&(AllShares.shareslock));
	struct Share * object = get_share(ownerID, shareName);
	if (object != NULL) {
		release_spinlock(&(AllShares.shareslock));
		return E_SHARED_MEM_EXISTS;
	}
	struct Share* mark_newShare = create_share(ownerID, shareName, size,
			isWritable);

	if (mark_newShare == NULL) {
		release_spinlock(&(AllShares.shareslock));
		return E_NO_SHARE;
	}
	uint32 sizeToAllocate = ROUNDUP(size, PAGE_SIZE);
	uint32 numOfFrames = sizeToAllocate / PAGE_SIZE;
	for (uint32 i = 0; i < numOfFrames; i++) {
		struct FrameInfo* frame = NULL;
		int result = allocate_frame(&frame);

		map_frame(myenv->env_page_directory, frame,
				((uint32) virtual_address + i * PAGE_SIZE),
				PERM_USER | PERM_WRITEABLE);

		mark_newShare->framesStorage[i] = frame;
	}
	LIST_INSERT_TAIL(&AllShares.shares_list, mark_newShare);
	release_spinlock(&(AllShares.shareslock));
	return mark_newShare->ID;
}


//======================
// [5] Get Share Object:
//======================
int getSharedObject(int32 ownerID, char* shareName, void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #21] [4] SHARED MEMORY [KERNEL SIDE] - getSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("getSharedObject is not implemented yet");
	//Your Code is Here...

	struct Env* myenv = get_cpu_proc(); //The calling environment
	acquire_spinlock(&(AllShares.shareslock));
	struct Share* omar_sharedObj = get_share(ownerID, shareName);
	if (omar_sharedObj == NULL) {
		release_spinlock(&(AllShares.shareslock));
		return E_SHARED_MEM_NOT_EXISTS;
	}
	struct FrameInfo** frames = omar_sharedObj->framesStorage;
	uint32 va = (uint32) virtual_address;

	int numFrames = ROUNDUP(omar_sharedObj->size, PAGE_SIZE) / PAGE_SIZE;
	for (int i = 0; i < numFrames; i++) {
		int perm = PERM_USER | PERM_PRESENT;
		if (omar_sharedObj->isWritable) {
			perm = perm | PERM_WRITEABLE;
		}
		int result = map_frame(myenv->env_page_directory, frames[i], va, perm);
		if (result < 0) {
			release_spinlock(&(AllShares.shareslock));
			return result;
		}
		va += PAGE_SIZE;
	}
	omar_sharedObj->references++;
	release_spinlock(&(AllShares.shareslock));
	return omar_sharedObj->ID & 0x7FFFFFFF;
}

//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//==========================
// [B1] Delete Share Object:
//==========================
//delete the given shared object from the "shares_list"
//it should free its framesStorage and the share object itself
void free_share(struct Share* ptrShare)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - free_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("free_share is not implemented yet");
	//Your Code is Here...

}
//========================
// [B2] Free Share Object:
//========================
int freeSharedObject(int32 sharedObjectID, void *startVA)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - freeSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("freeSharedObject is not implemented yet");
	//Your Code is Here...

}
