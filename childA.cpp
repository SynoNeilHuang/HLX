#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "common.h"

int main() {
    int semidA, semidB, semidM, semidL1;

    if (-1 == (semidA = semget(A_SEM_KEY, 1, 0))) {
	cerr << "[ChildA] semget failed" << endl;
	exit(1);
    }

    if (-1 == (semidB = semget(B_SEM_KEY, 1, 0))) {
	cerr << "[ChildA] semget failed" << endl;
	exit(1);
    }

    if (-1 == (semidM = semget(M_SEM_KEY, 1, 0))) {
	cerr << "[ChildA] semget failed" << endl;
	exit(1);
    }

    if (-1 == (semidL1 = semget(LOG1_SEM_KEY, 1, 0))) {
	cerr << "[ChildA] semget failed" << endl;
	exit(1);
    }

    cout << unitbuf << "[ChildA] ChildA Process started" << endl;
    sem_signal(semidL1);

    while(true) {
	sem_wait(semidA);
	int inputSize;
	int buf;
	vector<int> input;
	double result;

	read(STDIN_FILENO, &inputSize, sizeof(int));
	cout << "[ChildA] Random Numbers Received From Pipe: ";
	for (int i = 0 ; i < inputSize ; ++i) {
	    read(STDIN_FILENO, &buf, sizeof(int));
	    input.push_back(buf);
	}
	for(const auto& i:input) {
	    cout << i << " ";
	}
	cout << endl;

	sort(input.begin(), input.end());
	if (1 == (inputSize % 2)) {
	    result = input.at(inputSize/2);
	} else {
	    result = ((double)(input.at((inputSize/2)-1)) + (double)input.at(inputSize/2)) / 2;
	}

	cout << "[ChildA] Median : " << fixed << setprecision(8) << result << endl;
	sem_signal(semidB);

    }
    return 0;
}
