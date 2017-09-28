#include <iostream>
#include <stdlib.h>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <csignal>
#include "common.h"

using namespace std;

int main() {
    int shmid = 0;
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
	return -1;
    }

    int input = 0;
    int wstatus;
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

	    cout << "[mainParent] ChildA Process Created" << endl;
	    char *argv[1] = {(char*)0};
	    execvp("ChildA",argv);
	    break;
		}
	default: {
	    close(PipeFd[READ_END]);
	    waitpid(a_pid, &wstatus, WNOHANG);
	    break;
		 }
    }

    while (true) {
	cout << "[mainParent] Enter a positive integer or 0 to exit : ";
	cin >> input;
	if (0 == input) {
	    shmdt(array);
	    shmctl(shmid, IPC_RMID, NULL);
	    cout << "[mainParent] Process Waits" << endl;
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

	    kill(a_pid, SIGINT);
	}
    }
}
