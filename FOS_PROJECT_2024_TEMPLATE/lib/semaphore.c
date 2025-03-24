// User-level Semaphore

#include "inc/lib.h"

struct semaphore create_semaphore(char *semaphoreName, uint32 value) {

    struct __semdata *sharedSemData = (struct __semdata *)smalloc(semaphoreName, sizeof(struct __semdata), 1);

    sharedSemData->count = value;
    sharedSemData->lock = 0;
    strcpy(sharedSemData->name, semaphoreName);

    sys_init(&sharedSemData->queue);


    struct semaphore omar_semWrapper;
    omar_semWrapper.semdata = sharedSemData;

    return omar_semWrapper;

}
struct semaphore get_semaphore(int32 ownerEnvID, char* semaphoreName)
{
	//TODO: [PROJECT'24.MS3 - #03] [2] USER-LEVEL SEMAPHORE - get_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("get_semaphore is not implemented yet");
	//Your Code is Here...

	struct __semdata* sharedSemData = (struct __semdata*)sget(ownerEnvID, semaphoreName);

	struct semaphore omar_semWrapper;
	omar_semWrapper.semdata = sharedSemData;

	return omar_semWrapper;

}

void wait_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #04] [2] USER-LEVEL SEMAPHORE - wait_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("wait_semaphore is not implemented yet");
	//Your Code is Here...


	while(xchg(&(sem.semdata->lock),1) != 0) ;

	sem.semdata->count--;

	if(sem.semdata->count < 0){

		  sys_wait_semaphore(&(sem.semdata->queue),&(sem.semdata->lock));

	}

	sem.semdata->lock=0;
}

void signal_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #05] [2] USER-LEVEL SEMAPHORE - signal_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING

	while(xchg(&(sem.semdata->lock),1) != 0) ;
	sem.semdata->count++;

	if(sem.semdata->count <= 0){

		sys_signal_semaphore(&(sem.semdata->queue));
	}


	sem.semdata->lock=0;
}
int semaphore_count(struct semaphore sem)
{
	return sem.semdata->count;
}
