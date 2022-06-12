#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

sig_atomic_t sigint_cnt = 0;
sig_atomic_t need_to_abort = 0;

static void
sigint_handler(int signum) {
    ++sigint_cnt;
}

static void
sigterm_handler(int signum) {
    need_to_abort = 1;
}

void sigint_handler_initialisation() {
    struct sigaction action_init;
    memset(&action_init, 0, sizeof(action_init));
    action_init.sa_handler = sigint_handler;
    action_init.sa_flags = SA_RESTART;
    sigaction(SIGINT, &action_init, NULL);
}

void sigterm_handler_initialisation() {
    struct sigaction action_init;
    memset(&action_init, 0, sizeof(action_init));
    action_init.sa_handler = sigterm_handler;
    action_init.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &action_init, NULL);
}

int main() {
    sigint_handler_initialisation();
    sigterm_handler_initialisation();

    printf("%d\n", getpid());
    fflush(stdout);

    while (!need_to_abort) {
        pause();
    }
    printf("%d\n", sigint_cnt);

    return 0;
}
