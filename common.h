#ifndef __COMMON__
#define __COMMON__
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>

#define READ_END  0
#define WRITE_END 1
#define SHM_MAX_SIZE 1024
key_t SHM_KEY = 12345;



#endif /* __COMMON__ */
