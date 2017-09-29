#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sys/stat.h>
#include "common.h"

void* getShm() {
    int shmid;
    if (0 > (shmid = shmget(SHM_KEY, SHM_MAX_SIZE, 0666 | S_IRUSR))) {
	cerr << "[ChildB] get shm failed" << endl;
	exit(1);
    }
    return shmat(shmid, 0, 0);
}

int main() {
    int semidA, semidB, semidM, semidL1, semidL2;
    int shmid;

    if (-1 == (semidA = semget(A_SEM_KEY, 1, 0))) {
	cerr << "[ChildB] semget failed" << endl;
	exit(1);
    }
    if (-1 == (semidB = semget(B_SEM_KEY, 1, 0))) {
	cerr << "[ChildB] semget failed" << endl;
	exit(1);
    }

    if (-1 == (semidM = semget(M_SEM_KEY, 1, 0))) {
	cerr << "[ChildB] semget failed" << endl;
	exit(1);
    }

    if (-1 == (semidL1 = semget(LOG1_SEM_KEY, 1, 0))) {
	cerr << "[ChildB] semget failed" << endl;
	exit(1);
    }

    if (-1 == (semidL2 = semget(LOG2_SEM_KEY, 1, 0))) {
	cerr << "[ChildB] semget failed" << endl;
	exit(1);
    }

    int *array = (int *)getShm();

    sem_wait(semidL1);
    cout << unitbuf << "[ChildB] ChildB Process started" << endl;
    sem_signal(semidL2);


    while(true) {
	sem_wait(semidB);
	vector<int> input;
	double geoProduct=1;
	double result;

	int inputSize = array[0];
	for (int i = 0 ; i < inputSize ; ++i) {
	    input.push_back(array[i+1]);
	}

	cout << "[ChildB] Random Numbers Received From Shared Memory : ";
	for(const auto& i : input) {
	    cout << i << " ";
	    geoProduct *= i;
	}
	cout << endl;

	sort(input.begin(), input.end());
	cout << "[ChildB] Sorted Sequence: ";
	for(const auto& i : input) {
	    cout << i << " ";
	}
	cout << endl;

	cout << "[ChildB] Geometric Mean: ";
	result = pow(geoProduct, 1/(double)inputSize);
	cout << result << endl;
	sem_signal(semidM);
    }
    return 0;
}
