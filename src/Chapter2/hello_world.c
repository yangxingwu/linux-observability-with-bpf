#include <stdio.h>
#include <unistd.h>
#include "hello_world.skel.h"

int main(int argc, char **argv)
{
    struct hello_world_bpf *skel = NULL;
    int err;

    skel = hello_world_bpf__open_and_load();
    if (!skel) {
        fprintf(stderr, "Failed to open and load BPF skeleton\n");
        return 1;
    }

    err = hello_world_bpf__attach(skel);
    if (err) {
        fprintf(stderr, "Failed to attach BPF skeleton\n");
        goto cleanup;
    }

    while (1) {
        sleep(5);
    }

cleanup:
    hello_world_bpf__destroy(skel);
    return -err;
}
