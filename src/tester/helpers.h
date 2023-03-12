#ifndef __HELPERS_H
#define __HELPERS_H

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

#define CAT(x, y) __BASIC_CAT(x, y)
#define __BASIC_CAT(x, y) x##y
#define STR(x) __BASIC_STR(x)
#define __BASIC_STR(x) #x

#define PROFILE_FUNCTION(function, data, time_delta)    \
do                                                      \
{                                                       \
    pid_t child = 0;                                    \
    SPAWN_CHILD(function, data, child);                 \
    int64_t result = collect_child(child);              \
    assert(result >= 0);                                \
    (time_delta) = (uint32_t)result;                    \
} while (0)

#define SPAWN_CHILD(function, data, pid) do\
{\
    if (((pid) = fork()) == 0) { function (data); exit(0); }\
} while (0)

/**
 * @brief
 * Wait for a child process to terminate
 *
 * @param[in] pid The child process id
 * @return time in milliseconds the child process took to terminate or -1 on error
 * 
 * @todo Maybe use a timeout
 */
__always_inline
int64_t collect_child(pid_t pid)
{
    rusage resources = {};
    int status = 0;
    if (pid != wait4(pid, &status, 0, &resources)) return -1;   // error
    return resources.ru_utime.tv_usec / 1000;
}

#endif
