#ifndef __OS_MUTEX_H
#define __OS_MUTEX_H

#include <CommProto/architecture/os/include_defines.h>
#include <CommProto/architecture/os/mutex-config.h>

COM_EXTERN_C
/**
   initialize the mutex.
*/
void mutex_init(mutex_t* mutex);
/**
   Lock this mutex prevent any other thread from accessing the same resource.
*/
void mutex_lock(mutex_t* mutex);
/**
   Unlock the mutex to allow other threads to access the resource.
*/
void mutex_unlock(mutex_t* mutex);
/**
   Destroy the mutex.
*/
void mutex_destroy(mutex_t* mutex);

END_EXTERN_C

#endif // __OS_MUTEX_H
