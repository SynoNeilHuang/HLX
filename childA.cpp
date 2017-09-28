#include <iostream>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

using namespace std;

static void singalHandler(int sig, siginfo_t *siginfo, void *context) {
    int inputSize;
    int buf;
    read(STDIN_FILENO, &inputSize, sizeof(int));
    cout << "[ChildA] Random Numbers Received From Pipe: ";
    for (int i = 0 ; i < inputSize ; ++i) {
	read(STDIN_FILENO, &buf, sizeof(int));
	cout << buf << " ";
    }
    cout << endl;
    fflush(stdout);
}

int main() {
    struct sigaction INTact;
    memset(&INTact, '\0', sizeof(INTact));
    INTact.sa_sigaction = &singalHandler;
    INTact.sa_flags = SA_SIGINFO;

    cout << "[ChildA] ChildA Process started" << endl;
    if (sigaction(SIGINT, &INTact, NULL) < 0) {
	cerr << "[ChildA] Sigaction Failed" << endl;
	exit(1);
    }

    while(true) {
	sleep(1);
    }

    return 0;
}
