// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* Standard library */
#include <sys/types.h>

/* Local headers */
#include <systemCalls.h>
#include <kernelTypes.h>
#include <graphics.h>
#include <keyboard.h>
#include <time.h>
#include <memoryManager.h>
#include <lib.h>
#include <process.h>
#include <scheduler.h>
#include <pipe.h>
#include <semaphores.h>

typedef size_t (*TSyscallHandlerFunction)(size_t rdi, size_t rsi, size_t rdx, size_t r10, size_t r8);

static ssize_t sys_read_handler(int fd, char* buffer, size_t count) {
    return prc_handleReadFd(sch_getCurrentPID(), fd, buffer, count);
}

static ssize_t sys_write_handler(int fd, const char* buffer, size_t count) {
    return prc_handleWriteFd(sch_getCurrentPID(), fd, buffer, count);
}

static int sys_close_handler(int fd) {
    return prc_unmapFd(sch_getCurrentPID(), fd);
}

static int sys_clearScreen_handler() {
    if (!prc_isForeground(sch_getCurrentPID()))
        return 1;

    scr_clear();
    return 0;
}

static unsigned long sys_millis_handler() {
    return TICKS_TO_MILLISECONDS(rtc_getElapsedTicks());
}

static int sys_time_handler(char* buffer) {
    rtc_getCurrentTimeString(buffer);
    return 0;
}

static int sys_date_handler(char* buffer) {
    rtc_getCurrentDateString(buffer);
    return 0;
}

static void* sys_malloc_handler(size_t size) {
    return prc_handleMalloc(sch_getCurrentPID(), size);
}

static int sys_free_handler(void* ptr) {
    return prc_handleFree(sch_getCurrentPID(), ptr);
}

static void* sys_realloc_handler(void* ptr, size_t size) {
    return prc_handleRealloc(sch_getCurrentPID(), ptr, size);
}

static int sys_memState_handler(TMemoryState* memoryState) {
    return mm_getState(memoryState);
}

static TPid sys_getPid_handler() {
    return sch_getCurrentPID();
}

static TPid sys_createProcess_handler(int stdinMapFd, int stdoutMapFd, int stderrMapFd, const TProcessCreateInfo* createInfo) {
    TPid callerPid = sch_getCurrentPID();
    TPid newPid = prc_create(createInfo);

    if (newPid < 0)
        return newPid;

    if (stdinMapFd < 0)
        kbd_mapToProcessFd(newPid, STDIN);
    else
        prc_dupFd(callerPid, newPid, stdinMapFd, STDIN);

    if (stdoutMapFd < 0)
        scr_mapToProcessFd(newPid, STDOUT, &GRAY);
    else
        prc_dupFd(callerPid, newPid, stdoutMapFd, STDOUT);

    if (stderrMapFd < 0)
        scr_mapToProcessFd(newPid, STDERR, &ORANGE);
    else
        prc_dupFd(callerPid, newPid, stderrMapFd, STDERR);

    return newPid;
}

static int sys_exit_handler() {
    prc_kill(sch_getCurrentPID());
    sch_yieldProcess();
    return 1;
}

static int sys_blockProcess_handler(TPid pid) {
    int result = sch_blockProcess(pid);
    if (pid == sch_getCurrentPID())
        sch_yieldProcess();
    return result;
}

static int sys_unblockProcess_handler(TPid pid) {
    return sch_unblockProcess(pid);
}

static void sys_yield_handler() {
    sch_yieldProcess();
}

static int sys_killProcess_handler(TPid pid) {
    int result = prc_kill(pid);
    if (pid == sch_getCurrentPID())
        sch_yieldProcess();
    return result;
}

static int sys_nice_handler(TPid pid, TPriority priority) {
    return sch_setProcessPriority(pid, priority);
}

static int sys_listProcesses_handler(TProcessInfo* array, int maxProcesses) {
    return prc_listProcesses(array, maxProcesses);
}

static int sys_waitpid_handler(TPid pid) {
    TPid currentPid = sch_getCurrentPID();
    if (pid == currentPid || prc_unblockOnKilled(currentPid, pid))
        return 1;

    sch_blockProcess(currentPid);
    sch_yieldProcess();
    return 0;
}

static int sys_pipe_handler(int pipefd[2]) {
    TPid pid = sch_getCurrentPID();

    TPipe pipe;
    int readFd = -1, writeFd = -1;

    if ((pipe = pipe_create()) < 0 || (readFd = pipe_mapToProcessFd(pid, -1, pipe, 1, 0)) < 0 || (writeFd = pipe_mapToProcessFd(pid, -1, pipe, 0, 1)) < 0) {
        if (readFd >= 0)
            prc_unmapFd(pid, readFd);
        if (pipe >= 0)
            pipe_free(pipe);
        return 1;
    }

    pipefd[0] = readFd;
    pipefd[1] = writeFd;
    return 0;
}

static int sys_openPipe_handler(const char* name, int pipefd[2]) {
    TPid pid = sch_getCurrentPID();

    TPipe pipe;
    int readFd = -1, writeFd = -1;

    if ((pipe = pipe_open(name)) < 0 || (readFd = pipe_mapToProcessFd(pid, -1, pipe, 1, 0)) < 0 || (writeFd = pipe_mapToProcessFd(pid, -1, pipe, 0, 1)) < 0) {
        if (readFd >= 0)
            prc_unmapFd(pid, readFd);
        if (pipe >= 0)
            pipe_free(pipe);
        return 1;
    }

    pipefd[0] = readFd;
    pipefd[1] = writeFd;
    return 0;
}

static int sys_unlinkPipe_handler(const char* name) {
    return pipe_unlink(name);
}

static int sys_listPipes_handler(TPipeInfo* array, int maxPipes) {
    return pipe_listPipes(array, maxPipes);
}

static TSem sys_openSem_handler(const char* name, unsigned int value) {
    return sem_open(name, value);
}

static int sys_closeSem_handler(TSem sem) {
    return sem_close(sem);
}

static int sys_postSem_handler(TSem sem) {
    return sem_post(sem);
}

static int sys_waitSem_handler(TSem sem) {
    return sem_wait(sem);
}

static int sys_listSemaphores_handler(TSemaphoreInfo* array, int maxSemaphores) {
    return sem_listSemaphores(array, maxSemaphores);
}

static TSyscallHandlerFunction syscallHandlers[] = {
    /* I/O syscalls */
    /* 0x00 */ (TSyscallHandlerFunction)sys_read_handler,
    /* 0x01 */ (TSyscallHandlerFunction)sys_write_handler,
    /* 0x02 */ (TSyscallHandlerFunction)sys_close_handler,
    /* 0x03 -> 0x0F*/ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    /* Graphics-related syscalls */
    /* 0x10 */ (TSyscallHandlerFunction)sys_clearScreen_handler,
    /* 0x11 -> 0x1F */ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    /* Time & date syscalls*/
    /* 0x20 */ (TSyscallHandlerFunction)sys_millis_handler,
    /* 0x21 */ (TSyscallHandlerFunction)sys_time_handler,
    /* 0x22 */ (TSyscallHandlerFunction)sys_date_handler,
    /* 0x23 -> 0x2F */ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    /* Memory management syscalls */
    /* 0x30 */ (TSyscallHandlerFunction)sys_malloc_handler,
    /* 0x31 */ (TSyscallHandlerFunction)sys_free_handler,
    /* 0x32 */ (TSyscallHandlerFunction)sys_realloc_handler,
    /* 0x33 */ (TSyscallHandlerFunction)sys_memState_handler,
    /* 0x34 -> 0x3F */ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    /* Process-related syscalls */
    /* 0x40 */ (TSyscallHandlerFunction)sys_getPid_handler,
    /* 0x41 */ (TSyscallHandlerFunction)sys_createProcess_handler,
    /* 0x42 */ (TSyscallHandlerFunction)sys_exit_handler,
    /* 0x43 */ (TSyscallHandlerFunction)sys_blockProcess_handler,
    /* 0x44 */ (TSyscallHandlerFunction)sys_unblockProcess_handler,
    /* 0x45 */ (TSyscallHandlerFunction)sys_yield_handler,
    /* 0x46 */ (TSyscallHandlerFunction)sys_killProcess_handler,
    /* 0x47 */ (TSyscallHandlerFunction)sys_nice_handler, // ((nice))
    /* 0x48 */ (TSyscallHandlerFunction)sys_listProcesses_handler,
    /* 0x49 */ (TSyscallHandlerFunction)sys_waitpid_handler,
    /* 0x4A -> 0x4F */ NULL, NULL, NULL, NULL, NULL, NULL,

    /* Pipe syscalls */
    /* 0x50 */ (TSyscallHandlerFunction)sys_pipe_handler,
    /* 0x51 */ (TSyscallHandlerFunction)sys_openPipe_handler,
    /* 0x52 */ (TSyscallHandlerFunction)sys_unlinkPipe_handler,
    /* 0x53 */ (TSyscallHandlerFunction)sys_listPipes_handler,
    /* 0x54 -> 0x5F */ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    /* Semaphore syscalls */
    /* 0x60 */ (TSyscallHandlerFunction)sys_openSem_handler,
    /* 0x61 */ (TSyscallHandlerFunction)sys_closeSem_handler,
    /* 0x62 */ (TSyscallHandlerFunction)NULL,
    /* 0x63 */ (TSyscallHandlerFunction)sys_postSem_handler,
    /* 0x64 */ (TSyscallHandlerFunction)sys_waitSem_handler,
    /* 0x65 */ (TSyscallHandlerFunction)sys_listSemaphores_handler
};

size_t sysCallDispatcher(size_t rdi, size_t rsi, size_t rdx, size_t r10, size_t r8, size_t rax) {
    TSyscallHandlerFunction handler;
    if (rax < (sizeof(syscallHandlers) / sizeof(syscallHandlers[0])) && (handler = syscallHandlers[rax]) != NULL)
        return handler(rdi, rsi, rdx, r10, r8);
    return -1;
}