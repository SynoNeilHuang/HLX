#ifndef __COMMON__
#define __COMMON__
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <iostream>

using namespace std;

#define READ_END  0
#define WRITE_END 1
#define SHM_MAX_SIZE 1024

key_t SHM_KEY = 12345;
key_t A_SEM_KEY = 56789;
key_t B_SEM_KEY = 44555;
key_t M_SEM_KEY = 33444;
key_t LOG1_SEM_KEY = 22344;
key_t LOG2_SEM_KEY = 22333;

void sem_wait(int& semid) {
    struct sembuf sb = {0, -1, 0};
    if (semop(semid, &sb, 1) == -1) {
	cerr << "[manParent] lock failed" << endl;
	exit(1);
    }
}

void sem_signal(int& semid) {
    struct sembuf sb = {0, 1, 0};
    if (semop(semid, &sb, 1) == -1) {
	cerr << "[manParent] lock failed" << endl;
	exit(1);
    }
}

#endif /* __COMMON__ */
