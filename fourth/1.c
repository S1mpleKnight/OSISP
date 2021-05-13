//14. 1->2   2->(3,4,5)   4->6   3->7   5->8
//6. 1->(2,3) SIGUSR1   3->4 SIGUSR2   4->(5,6,7) SIGUSR1  7->8 SIGUSR1   8->1 SIGUSR2

#include <stdlib.h>
#include <wait.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#define ZERO_FILE "zero_pid.txt"
#define FIRST_FILE "first_pid.txt"
#define SECOND_FILE "second_pid.txt"
#define THIRD_FILE "third_pid.txt"
#define FOURTH_FILE "fourth_pid.txt"
#define FIFTH_FILE "fifth_pid.txt"
#define SIXTH_FILE "sixth_pid.txt"
#define SEVENTH_FILE "seventh_pid.txt"
#define EIGHTH_FILE "eighth_pid.txt"
//#define DEBUG

const char *FILES[] = 
{
 ZERO_FILE,
 FIRST_FILE,
 SECOND_FILE,
 THIRD_FILE,
 FOURTH_FILE,
 FIFTH_FILE,
 SIXTH_FILE, 
 SEVENTH_FILE, 
 EIGHTH_FILE
};

char* prog_name;
int cur_proc = 0;
int USR1_count = 0;
int USR2_count = 0;
int recieved_by_1_proc = 0;

int ready7 = 0;
int ready6 = 0;
int ready3 = 0;

void clear_storage() {
    for (int i = 0; i < 9; i++) {
        errno = 0;
        if (remove(FILES[i]) == -1) {
            fprintf(stderr, "%d: %s: %s: %s\n", getpid(), prog_name, strerror(errno), FILES[i]);
        }
    }
}

void store_getpid(int pid, char *filename) {
    FILE *file = fopen(filename, "w+");
    if (file == NULL) {
        fprintf(stderr, "%d: %s: %s: %s\n", getpid(), prog_name, strerror(errno), filename);
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%d", pid);
    if (fclose(file) == EOF) {
        fprintf(stderr, "%d: %s: %s: %s\n", getpid(), prog_name, strerror(errno), filename);
        exit(EXIT_FAILURE);
    }
}

int take_pid(char *filename) {
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        fprintf(stderr, "%d: %s: %s: %s\n", getpid(), prog_name, strerror(errno), filename);
        exit(EXIT_FAILURE);
    }
    char buf[64];
    fgets(buf, 64, file);
    errno = 0;
    long pid = strtol(buf, NULL, 10);
    if (errno != 0) {
        fprintf(stderr, "%d: %s: %s: %s\n", getpid(), prog_name, strerror(errno), filename);
        exit(EXIT_FAILURE);
    }
    if (fclose(file) == EOF) {
        fprintf(stderr, "%d: %s: %s: %s\n", getpid(), prog_name, strerror(errno), filename);
        exit(EXIT_FAILURE);
    }
    return (int) pid;
}

long time_millis() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_usec % 1000;
}

void sigh_action(int signal, void (*function)(int)) {
    struct sigaction* action;
    action = (struct sigaction*) calloc(1, sizeof(struct sigaction));
    action->sa_handler = function;
    sigaction(signal, action, 0);
}

void USR1_recieve() {
    USR1_count++;
    fprintf(stdout, "%d %d %d received SIGUSR1 %ld\n", cur_proc, getpid(), getppid(), time_millis());
    fflush(stdout);
}

void USR2_recieve() {
    USR2_count++;
    fprintf(stdout, "%d %d %d received SIGUSR2 %ld\n", cur_proc, getpid(), getppid(), time_millis());
    fflush(stdout);
}

void TERM_recieve() {
    fprintf(stdout, "%d %d terminated after %d SIGUSR1 and %d SIGUSR2\n", getpid(), getppid(), USR1_count, USR2_count);
    fflush(stdout);
}

void USR1_send(int processNumber, int pid) {
    fprintf(stdout, "%d %d %d sent SIGUSR1 %ld\n", processNumber, getpid(), getppid(), time_millis());
    fflush(stdout);
    kill(pid, SIGUSR1);
}

void USR2_send(int processNumber, int pid) {
    fprintf(stdout, "%d %d %d sent SIGUSR1 %ld\n", processNumber, getpid(), getppid(), time_millis());
    fflush(stdout);
    kill(pid, SIGUSR2);
}

void USR1_send_group(int processNumber, int group) {
    fprintf(stdout, "%d %d %d sent SIGUSR2 %ld\n", processNumber, getpid(), getppid(), time_millis());
    fflush(stdout);
    killpg(group, SIGUSR1);
}

void TERM_8_proc_handler(int sig_num) {
    TERM_recieve();
    exit(EXIT_SUCCESS);
}

void TERM_7_proc_handler(int sig_num) {
    TERM_recieve();
    exit(EXIT_SUCCESS);
}

void TERM_6_proc_handler(int sig_num) {
    TERM_recieve();
    exit(EXIT_SUCCESS);
}

void TERM_5_proc_handler(int sig_num) {
    int eightPid = take_pid(EIGHTH_FILE );
    kill(eightPid, SIGTERM);
    wait(0);
    TERM_recieve();
    exit(EXIT_SUCCESS);
}

void TERM_4_proc_handler(int sig_num) {
    int sixthPid = take_pid(SIXTH_FILE );
    kill(sixthPid, SIGTERM);
    wait(0);
    TERM_recieve();
    exit(EXIT_SUCCESS);
}

void TERM_3_proc_handler(int sig_num) {
    int seventhPid = take_pid(SEVENTH_FILE );
    kill(seventhPid, SIGTERM);
    wait(0);
    TERM_recieve();
    exit(EXIT_SUCCESS);
}

void TERM_2_proc_handler(int sig_num) {
    int thirdPid = take_pid(THIRD_FILE );
    int forthPid = take_pid(FOURTH_FILE );
    int fifthPid = take_pid(FIFTH_FILE );
    kill(thirdPid, SIGTERM);
    kill(forthPid, SIGTERM);
    kill(fifthPid, SIGTERM);
    for (int i = 0; i < 3; i++) {
        wait(0);
    }
    TERM_recieve();
    exit(EXIT_SUCCESS);
}

void USR1_8_proc_handler(int sig_num) {
    USR1_recieve();
    int firstPid = take_pid(FIRST_FILE );
    USR2_send(cur_proc, firstPid);
}

void USR1_7_proc_handler(int sig_num) {
    while (ready7 == 0) {}
    ready7 = 0;
    USR1_recieve();
    int eightPid = take_pid(EIGHTH_FILE );
    USR1_send(cur_proc, eightPid);
}

void USR2_7_proc_handler(int signal) {
    USR2_count++;
    ready7 = 1;
}

void USR1_6_proc_handler(int sig_num) {
    while (ready6 == 0) {}
    ready6 = 0;
    USR1_recieve();
    int seventhPid = take_pid(SEVENTH_FILE );
    kill(seventhPid, SIGUSR2);
}

void USR2_6_proc_handler(int signal) {
    USR2_count++;
    ready6 = 1;
}

void USR1_5_proc_handler(int sig_num) {
    USR1_recieve();
    int sixthPid = take_pid(SIXTH_FILE );
    kill(sixthPid, SIGUSR2);
}

void USR2_4_proc_handler(int sig_num) {
    USR2_recieve();
    int fifthPid = take_pid(FIRST_FILE );
    USR1_send_group(cur_proc, fifthPid);
}

void USR2_3_proc_handler(int sig_num) {
    USR2_count++;
    ready3 = 1;
}

void USR1_3_proc_handler() {
    while (ready3 == 0) {}
    ready3 = 0;
    USR1_recieve();
    int forthPid = take_pid(FOURTH_FILE );
    USR2_send(cur_proc, forthPid);
}

void USR1_2_proc_handler(int signal) {
    USR1_recieve();
    int thirdPid = take_pid(THIRD_FILE );
    kill(thirdPid, SIGUSR2);
}

void USR2_1_proc_handler(int signal) {
    USR2_recieve();
    recieved_by_1_proc++;

    int secondPid = take_pid(SECOND_FILE );
    if (recieved_by_1_proc== 101) {
        kill(secondPid, SIGTERM);
        wait(0);
        TERM_recieve();
        exit(EXIT_SUCCESS);
    }

    USR1_send_group(cur_proc, secondPid);
}

void start_8_proc() {
    cur_proc = 8;

    sigh_action(SIGUSR1, USR1_8_proc_handler);
    sigh_action(SIGTERM, TERM_8_proc_handler);

#ifdef DEBUG
    int zerothPid = take_pid(ZERO_FILE );
    char psTreeCommand[64] = { 0 };
    sprintf(psTreeCommand, "pstree -p %d", zerothPid);
    system(psTreeCommand);
#endif

    store_getpid(getpid(), EIGHTH_FILE );

    if (setpgid(getpid(), getpid()) == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
    }

    while (1) {
        pause();
    }
}

void start_7_proc() {
    cur_proc = 7;

    sigh_action(SIGUSR1, USR1_7_proc_handler);
    sigh_action(SIGUSR2, USR2_7_proc_handler);
    sigh_action(SIGTERM, TERM_7_proc_handler);

    store_getpid(getpid(), SEVENTH_FILE );

    while (1) {
        pause();
    }
}

void start_6_proc() {
    cur_proc = 6;

    sigh_action(SIGUSR1, USR1_6_proc_handler);
    sigh_action(SIGUSR2, USR2_6_proc_handler);
    sigh_action(SIGTERM, TERM_6_proc_handler);

    store_getpid(getpid(), SIXTH_FILE );

    while (1) {
        pause();
    }
}

void start_5_proc() {
    cur_proc = 5;

    sigh_action(SIGUSR1, USR1_5_proc_handler);
    sigh_action(SIGTERM, TERM_5_proc_handler);

    struct stat buf;
    while (lstat(SEVENTH_FILE , &buf) == -1)
        continue;

    int eightPid = fork();
    if (eightPid == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
        exit(EXIT_FAILURE);
    } else if (eightPid == 0) {
        start_8_proc();
    }

    store_getpid(getpid(), FIFTH_FILE );

    while (1) {
        pause();
    }
}

void start_4_proc() {
    cur_proc = 4;

    sigh_action(SIGUSR2, USR2_4_proc_handler);
    sigh_action(SIGTERM, TERM_4_proc_handler);

    int sixthPid = fork();
    if (sixthPid == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
        exit(EXIT_FAILURE);
    } else if (sixthPid == 0) {
        start_6_proc();
    }

    store_getpid(getpid(), FOURTH_FILE );

    if (setpgid(getpid(), getpid()) == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
    }

    while (1) {
        pause();
    }
}

void start_3_proc() {
    cur_proc = 3;

    sigh_action(SIGUSR2, USR2_3_proc_handler);
    sigh_action(SIGUSR1, USR1_3_proc_handler);
    sigh_action(SIGTERM, TERM_3_proc_handler);

    struct stat buf;
    while (lstat(SIXTH_FILE , &buf) == -1)
        continue;

    int seventhPid = fork();
    if (seventhPid == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
        exit(EXIT_FAILURE);
    } else if (seventhPid == 0) {
        start_7_proc();
    }

    store_getpid(getpid(), THIRD_FILE );

    while (1) {
        pause();
    }
}

void start_2_proc() {
    cur_proc = 2;

    sigh_action(SIGUSR1, USR1_2_proc_handler);
    sigh_action(SIGTERM, TERM_2_proc_handler);

    int third_pid = fork();
    if (third_pid == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
        exit(EXIT_FAILURE);
    } else if (third_pid == 0) {
        start_3_proc();
    }

    int forthPid = fork();
    if (forthPid == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
        exit(EXIT_FAILURE);
    } else if (forthPid == 0) {
        start_4_proc();
    }

    int fifthPid = fork();
    if (fifthPid == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
        exit(EXIT_FAILURE);
    } else if (fifthPid == 0) {
        start_5_proc();
    }

    store_getpid(getpid(), SECOND_FILE );

    if (setpgid(getpid(), getpid()) == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
    }
    if (setpgid(third_pid, getpid()) == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
    }

    while (1) {
        pause();
    }
}

void start_1_proc() {
    cur_proc = 1;

    sigh_action(SIGUSR2, USR2_1_proc_handler);

    if (setpgid(getpid(), getpid()) == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
    }

    int second_pid = fork();
    if (second_pid == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
        exit(EXIT_FAILURE);
    } else if (second_pid == 0) {
        start_2_proc();
    }

    store_getpid(getpid(), FIRST_FILE );

    struct stat buf;
    while (lstat(EIGHTH_FILE , &buf) == -1)
        continue;

    int zero_pid = take_pid(ZERO_FILE );
    if (setpgid(getpid(), zero_pid) == -1) {
        fprintf(stderr, "%d: %s: %s\n", getpid(), prog_name, strerror(errno));
    }

    USR1_send_group(cur_proc, second_pid);

    while (1) {
        pause();
    }
}

int main(int argc, char* argv[]) {
    prog_name = (char *) calloc(15, sizeof(char));
    strcpy(prog_name, strrchr(argv[0], '/') + 1);
    if (argc != 1) {
        fprintf(stderr, "%s: Error: Wrong amount of arguments.\n", prog_name);
        return 1;
    }
    store_getpid(getpid(),ZERO_FILE );
    int firstPid = fork();
    switch (firstPid) {
        case -1: {
            fprintf(stderr, "%s: %s\n", prog_name, strerror(errno));
            break;
        }
        case 0: {
            start_1_proc();
            break;
        }
        default: {
            wait(0);
        }
    }
    clear_storage();
    return 0;
}

