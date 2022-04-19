#ifndef _TRACE_H_
#define _TRACE_H_

/*
    Code for log management and tracing
*/

#include <stdio.h>

#define ERROR_TRACE(x) printf("[ %s -> %s ( %d Line ) ] %s", __FILE__, __func__, __LINE__, (x))

// #define DEBUG
#ifdef  DEBUG
    #define TRACE(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define TRACE(fmt, ...)
#endif

#endif