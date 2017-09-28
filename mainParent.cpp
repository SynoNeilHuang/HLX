#include <stdlib.h>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <csignal>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "common.h"

int sema_init(int& semid, key_t &key, int nsem, int initValue) {
    union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
    }arg;

    if ( -1 == (semid = semget(key, nsem, 0666 | IPC_CREAT))) {
	cerr << "[mainParent] semget failed: " << strerror(errno) << endl;
	return -1;
    }
    arg.val = initValue;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
	cerr << "[mainParent] semctl failed" << endl;
	return -1;
    }
    return 0;
}

int main() {
    int shmid = 0;
    int semidA, semidB, semidM, semidL1, semidL2;

    if ( 0 > sema_init(semidA, A_SEM_KEY, 1, 0)) {
	cerr << "[mainParent] semaphore init failed" << endl;
	exit(1);
    }

    if ( 0 > sema_init(semidB, B_SEM_KEY, 1, 0)) {
	cerr << "[mainParent] semaphore init failed" << endl;
	exit(1);
    }

    if ( 0 > sema_init(semidM, M_SEM_KEY, 1, 1)) {
	cerr << "[mainParent] semaphore init failed" << endl;
	exit(1);
    }

    if ( 0 > sema_init(semidL1, LOG1_SEM_KEY, 1, 0)) {
	cerr << "[mainParent] semaphore init failed" << endl;
	exit(1);
    }

    if ( 0 > sema_init(semidL2, LOG2_SEM_KEY, 1, 0)) {
	cerr << "[mainParent] semaphore init failed" << endl;
	exit(1);
    }
    cout << "[mainParent] Main Process Started" << endl;

    shmid = shmget(SHM_KEY, SHM_MAX_SIZE * sizeof(int), IPC_CREAT);

    if (shmid < 0) {
	cout << "[mainParent] shm failed : " << errno << endl;
	exit(1);
    }

    int *array;
    array = (int *)shmat(shmid, 0, 0);

    int PipeFd[2];

    if (pipe(PipeFd)) {
	cerr << "[mainParent] Data Pipe create failed!!";
	exit(1);
    }

    int input = 0;
    cout << "[mainParent] ChildA Process Created" << endl;
    fflush(stdout);
    pid_t a_pid = fork();
    switch(a_pid) {
	case -1: {
		     cout << "[mainParent] Create Child A Failed" << endl;
		     exit(1);
		 }
	case 0: {
		    close(STDIN_FILENO);
		    dup2(PipeFd[READ_END],STDIN_FILENO);
		    close(PipeFd[WRITE_END]);

		    char *argv[1] = {(char*)0};
		    execvp("ChildA",argv);
		    break;
		}
	default: {
		     close(PipeFd[READ_END]);
		     break;
		 }
    }
    cout << "[mainParent] ChildB Process Created" << endl;
    fflush(stdout);
    pid_t b_pid = fork();
    switch(b_pid) {
	case -1: {
		     cout << "[mainParent] Create Child B Failed" << endl;
		     exit(1);
		 }
	case 0: {
		    close(PipeFd[WRITE_END]);

		    char *argv[1] = {(char*)0};
		    execvp("ChildB",argv);
		    break;
		}
	default: {
		     break;
		 }
    }
    while (true) {

	sem_wait(semidM);
	sem_wait(semidL2);
	cout << "[mainParent] Enter a positive integer or 0 to exit : ";
	cin >> input;
	sem_signal(semidL2);

	if (0 == input) {
	    cout << "[mainParent] Process Waits" << endl;
	    shmdt(array);
	    shmctl(shmid, IPC_RMID, NULL);
	    semctl(semidA, 0, IPC_RMID, NULL);
	    semctl(semidB, 0, IPC_RMID, NULL);
	    semctl(semidM, 0, IPC_RMID, NULL);
	    semctl(semidL1, 0, IPC_RMID, NULL);
	    semctl(semidL2, 0, IPC_RMID, NULL);
	    kill(a_pid, SIGTERM);
	    cout << "[mainParent] Process Exits" << endl;
	    return 0;
	} else if (0 > input) {
	    continue;
	} else {
	    cout << "[mainParent] Generating " << input << " random integers : ";
	    memset(array, 0, input+1);
	    array[0] = input;
	    srand(time(NULL));

	    for (int i = 0 ; i < input ; ++i) {
		array[i+1] = (rand() % 51 + 50);
		cout << array[i+1] << " ";
	    }
	    cout << endl;

	    for (int i = 0 ; i < array[0] + 1 ; ++i) {
		write(PipeFd[WRITE_END], &array[i], sizeof(int));
	    }
	}
	sem_signal(semidA);
    }
}
