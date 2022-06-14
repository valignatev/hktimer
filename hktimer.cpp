#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

const char known_names[2][14] = {
    "Hollow_Knight",
    "Hollow Knight"
};


struct Process {
    const char* name = "Process not found";
    pid_t pid = 0;
    void* base_address;
};

// PID - process id - is a number on Linux. Check if the candidate looks like one
// and return it. Return 0 if the candidate doesn't look like PID
int parse_pid(char* candidate, int* pid) {
    if (sscanf(candidate, "%d", pid) < 1) {
        return false;
    }
    return true;
}

int main() {
    DIR *proc_dir;
    FILE *pid_stat;
    dirent *entry;
    char stat_path[30];
    char process_name[250];
    Process process = {};

    if ((proc_dir = opendir("/proc")) == NULL) {
        printf("can't get the process list\n");
        exit(1);
    }

    int pid;
    while ((entry = readdir(proc_dir)) != NULL) {
        if (!parse_pid(entry->d_name, &pid)) {
            continue;
        }

        // Kernel reserves first 300 pids anyway, no reason to even look at them
        if (pid <= 300) {
            continue;
        }

        sprintf(stat_path, "/proc/%d/stat", pid);
        pid_stat = fopen(stat_path, "r");
        // procfs pid stat starts with pid, then whitespace and a program name in 
        // parenthesis. Read and ignore up until the opening (
        fscanf(pid_stat, "%*d (");
        // And then everything up until the closing ) goes into the process name
        // fscanf is kinda ugly, but I don't want to invest into better base layer for now
        // to avoid it
        fscanf(pid_stat, "%[^)]", process_name);

        for (unsigned int i = 0; i < sizeof(known_names); i++) {
            auto known_name = known_names[i];
            if (strcmp(known_name, process_name) == 0) {
                process.pid = (pid_t)pid;
                process.name = known_name;
                goto stop_search;
            }
        }

    }

stop_search:
    fclose(pid_stat);
    closedir(proc_dir);
    printf("Hook attempt result: %d -> %s\n", process.pid, process.name);
    return 0;
}
