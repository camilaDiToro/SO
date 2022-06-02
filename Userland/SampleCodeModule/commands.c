// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* Local headers */
#include <commands.h>
#include <syscalls.h>
#include <userstdlib.h>
#include <processes.h>
#include <string.h>
#include <phylo.h>
#include <test.h>

static TCommand validCommands[] = {
    {runHelp, "help", "Displays a list of all available commands."},
    {runClear, "clear", "Clears the window."},
    {runEcho, "echo", "Prints the parameters passed to it to standard output."},
    {runTime, "time", "Displays the system's date and time."},
    {runDivideByZero, "dividezero", "Causes an exception by attempting to dividing by 0."},
    {runInvalidOp, "invalidop", "Causes an exception by attempting to run an invalid instruction."},
    {runMem, "mem", "Displays information about the system's memory."},
    {runPs, "ps", "Displays a list of all the currently running processes with their properties."},
    {runLoop, "loop", "Creates a process that prints it's PID once every 3 seconds."},
    {runKill, "kill", "Kills the process with the parameter-specified PID."},
    {runNice, "nice", "Changes the priority of the process with the parameter-specified PID."},
    {runBlock, "block", "Blocks the process with the given PID."},
    {runUnblock, "unblock", "Unblocks the process with the given PID."},
    {runSem, "sem", "Displays a list of all the currently active semaphores with their properties."},
    {runCat, "cat", "Creates a process that prints the standard input onto the standard output."},
    {runWc, "wc", "Creates a process that counts the newlines coming in from standard input and prints that number to standard output."},
    {runFilter, "filter", "Creates a process that filters the vowels received from standard input and prints them to standard output."},
    {runPipe, "pipe", "Displays a list of all the currently active pipes with their properties."},
    {runPhylo, "phylo", "Creates a process that simulates the Dining Philosophers problem."},
    {runNamedPipes, "namedpipes", "Runs a test for names pipes."},
    {runTestMM, "testmm", "Runs a test for memory manager."},
    {runTestAsync, "testasync", "Runs a synchronization test with multiple processes without semaphores."},
    {runTestSync, "testsync", "Runs a synchronization test with multiple processes with semaphores."},
    {runTestProcesses, "testprocesses", "Runs a test for processes."},
    {runTestPrio, "testprio", "Runs a test on process priorities."},
};

const TCommand* getCommandByName(const char* name) {
    for (int i = 0; i < (sizeof(validCommands) / sizeof(validCommands[0])); i++) {
        if (!strcmp(name, validCommands[i].name)) {
            return &validCommands[i];
        }
    }
    return NULL;
}

int runHelp(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    fprint(stdout, "The available commands are:");

    for (int i = 0; i < (sizeof(validCommands) / sizeof(validCommands[0])); i++)
        fprintf(stdout, "\n\t '%s' \t - %s", validCommands[i].name, validCommands[i].description);

    return 1;
}

int runClear(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    sys_clearScreen();
    return 1;
}

int runEcho(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    for (int i = 0; i < argc; i++) {
        if (i != 0)
            fputChar(stdout, ' ');
        fprintf(stdout, argv[i]);
    }

    return 1;
}

int runTime(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    char time[11];
    sys_time(time);
    fprintf(stdout, "Time: %s", time);

    char date[11];
    sys_date(date);
    fprintf(stdout, "\nDate: %s", date);

    fprintf(stdout, "\nMillis since startup: %u", (unsigned int)sys_millis());

    return 1;
}

int runDivideByZero(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    divideByZero();
    return 1;
}

int runInvalidOp(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    invalidOp();
    return 1;
}

int runMem(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TMemoryState memoryState;
    if (sys_memState(&memoryState)) {
        fprint(stderr, "Failed to get memory state.");
        return -1;
    }

    fprintf(stdout, "Memory Manager Type: %s\n", memoryState.type == NODE ? "NODE" : memoryState.type == BUDDY ? "BUDDY" : "UNKNOWN");
    fprintf(stdout, "Total memory: %u.\n", memoryState.total);
    fprintf(stdout, "Used: %u (%u%%).\n", memoryState.used, (memoryState.used * 100 / memoryState.total));
    fprintf(stdout, "Available: %u.\n", memoryState.total - memoryState.used);
    fprintf(stdout, "Total chunks: %u", memoryState.chunks);

    return 1;
}

int runPs(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessInfo array[16];
    int count = sys_listProcesses(array, 16);
    fprintf(stdout, "Listing %d process%s:", count, count == 1 ? "" : "es");

    for (int i = 0; i < count; i++) {
        const char* status = array[i].status == READY ? "READY" : array[i].status == RUNNING ? "RUNNING" : array[i].status == BLOCKED ? "BLOCKED" : array[i].status == KILLED ? "KILLED" : "UNKNOWN";

        fprintf(stdout, "\npid=%d, name=%s, stackEnd=%x, stackStart=%x, isForeground=%d, priority=%d, status=%s, rsp=%x",
               array[i].pid, array[i].name, array[i].stackEnd, array[i].stackStart, array[i].isForeground, array[i].priority, status, array[i].currentRSP);
    }

    return 1;
}

int runKill(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    if (argc != 1) {
        fprint(stderr, "Usage: kill [PID]");
        return 0;
    }

    TPid pidToKill = atoi(argv[0]);

    if (pidToKill == sys_getPid()) {
        fprint(stderr, "This shell does not commit suicide.");
        return 0;
    }

    int result = sys_killProcess(pidToKill);
    if (result == 0) {
        fprintf(stdout, "Sucessfully killed process with PID %d.", pidToKill);
        return 1;
    }

    fprintf(stderr, "Failed to kill process with PID %d. Error code: %d.", pidToKill, result);
    return 0;
}

int runNice(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    if (argc != 2) {
        fprint(stderr, "Usage: nice [PID] [PRIORITY]. Priority must be a number between -10 and 10.");
        return 0;
    }

    TPid pidToChange = atoi(argv[0]);
    TPriority newPriority = atoi(argv[1]);

    if (newPriority < MAX_PRIORITY || newPriority > MIN_PRIORITY) {
        fprint(stderr, "Invalid priority value. Must be in the range -10 to 10.");
        return 0;
    }

    int result = sys_nice(pidToChange, newPriority);
    if (result == 0) {
        fprintf(stdout, "Process %d has priority %d.\n", pidToChange, newPriority);
    } else {
        fprintf(stdout, "Failed modifying process' %d priority\n Error: %d.\n", pidToChange, result);
    }

    return 1;
}

int runBlock(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    if (argc != 1) {
        fprint(stderr, "Usage: block [PID]");
        return 0;
    }

    TPid pidToBlock = atoi(argv[1]);

    if (pidToBlock == sys_getPid()) {
        fprint(stderr, "You cannot block this shell!");
        return 0;
    }

    int result = sys_block(pidToBlock);
    if (result == 0) {
        fprintf(stdout, "Successfully blocked process with PID %d.", pidToBlock);
        return 1;
    }
    
    fprintf(stderr, "Failed to block process with PID %d. Error code: %d.", pidToBlock, result);
    return 0;
}

int runUnblock(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    if (argc != 1) {
        fprint(STDERR, "Usage: unblock [PID]");
        return 0;
    }

    TPid pidToUnblock = atoi(argv[0]);

    if (pidToUnblock == 0) {
        fprint(stderr, "Cannot unblock the shell.");
        return 0;
    }

    int result = sys_unblock(pidToUnblock);

    if (result == 0) {
        fprintf(stdout, "Successfully unblocked process with PID %d.", pidToUnblock);
        return 1;
    }
    
    fprintf(stdout, "Failed to unblock process with PID %d. Error code: %d.", pidToUnblock, result);
    return 0;
}

int runSem(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TSemaphoreInfo array[16];
    int count = sys_listSemaphores(array, 16);
    fprintf(stdout, "Listing %d semaphore%s:", count, count == 1 ? "" : "s");

    for (int i = 0; i < count; i++) {
        fprintf(stdout, "\nname=%s, value=%d, linkedProcesses=%d", array[i].name, array[i].value, array[i].linkedProcesses);

        fprintf(stdout, ", waitingProcesses={");
        for (int c = 0; array[i].waitingProcesses[c] >= 0; c++) {
            if (c != 0) {
                fprintf(stdout, ", ");
            }
            fprintf(stdout, "%d", array[i].waitingProcesses[c]);
        }
        fprintf(stdout, "}");
    }

    return 1;
}

int runPipe(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TPipeInfo array[16];
    int count = sys_listPipes(array, 16);
    fprintf(stdout, "Listing %d pipe%s:", count, count == 1 ? "" : "s");

    for (int i = 0; i < count; i++) {
        fprintf(stdout, "\nbytes=%u, readers=%u, writers=%u, name=%s", (unsigned int)array[i].remainingBytes, (unsigned int)array[i].readerFdCount,
               (unsigned int)array[i].writerFdCount, array[i].name);

        fprintf(stdout, ", readBlocked={");
        for (int c = 0; array[i].readBlockedPids[c] >= 0; c++) {
            if (c != 0) {
                fprintf(stdout, ", ");
            }
            fprintf(stdout, "%d", array[i].readBlockedPids[c]);
        }
        fprintf(stdout, "}");

        fprintf(stdout, ", writeBlocked={");
        for (int c = 0; array[i].writeBlockedPids[c] >= 0; c++) {
            if (c != 0) {
                fprintf(stdout, ", ");
            }
            fprintf(stdout, "%d", array[i].writeBlockedPids[c]);
        }
        fprintf(stdout, "}");
    }

    return 1;
}

int runCat(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo catInfo = {
        .name = "cat",
        .entryPoint = catProcess,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };

    *createdProcess = sys_createProcess(stdin, stdout, stderr, &catInfo);
    return *createdProcess >= 0;
}

int runWc(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo wcInfo = {
        .name = "wc",
        .entryPoint = wcProcess,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };

    *createdProcess = sys_createProcess(stdin, stdout, stderr, &wcInfo);
    return *createdProcess >= 0;
}

int runFilter(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo filterInfo = {
        .name = "filter",
        .entryPoint = filterProcess,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };

    *createdProcess = sys_createProcess(stdin, stdout, stderr, &filterInfo);
    return *createdProcess >= 0;
}

int runLoop(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo loopInfo = {
        .name = "loop",
        .entryPoint = loopProcess,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };

    *createdProcess = sys_createProcess(stdin, stdout, stderr, &loopInfo);
    return *createdProcess >= 0;
}

int runPhylo(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo phyloInfo = {
        .name = "phylo",
        .entryPoint = initPhyloProcess,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };

    *createdProcess = sys_createProcess(stdin, stdout, stderr, &phyloInfo);
    return *createdProcess >= 0;
}

int runNamedPipes(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo namedpipesInfo = {
        .name = "namedPipes",
        .entryPoint = namedPipesProcess,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };

    *createdProcess = sys_createProcess(stdin, stdout, stderr, &namedpipesInfo);
    return *createdProcess >= 0;
}

int runTestMM(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo pci = {
        .name = "testMM",
        .entryPoint = test_mm,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };

    *createdProcess = sys_createProcess(stdin, stdout, stderr, &pci);
    return *createdProcess >= 0;
}

int runTestAsync(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo pci = {
        .name = "testAsync",
        .entryPoint = test_async,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };

    *createdProcess = sys_createProcess(stdin, stdout, stderr, &pci);
    return *createdProcess >= 0;
}

int runTestSync(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo pci = {
        .name = "testSync",
        .entryPoint = test_sync,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };

    *createdProcess = sys_createProcess(stdin, stdout, stderr, &pci);
    return *createdProcess >= 0;
}

int runTestProcesses(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo pci = {
        .name = "testSync",
        .entryPoint = test_processes,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };

    *createdProcess = sys_createProcess(stdin, stdout, stderr, &pci);
    return *createdProcess >= 0;
}

int runTestPrio(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo pci = {
        .name = "testPrio",
        .entryPoint = test_prio,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };

    *createdProcess = sys_createProcess(stdin, stdout, stderr, &pci);
    return *createdProcess >= 0;
}