#ifndef __THREAD_H__
#define __THREAD_H__

#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include "type.h"
#include "trace.h"


/******************************** Mutex ********************************/
typedef struct _stMUTEX
{
	BOOL bEnable;
	pthread_mutex_t mutex;
} MUTEX_T, *PMUTEX_T;

void MUTEX_Init( MUTEX_T* pmutex );
void MUTEX_Enter( MUTEX_T* pmutex );
void MUTEX_Leave( MUTEX_T* pmutex );
void MUTEX_Term( MUTEX_T* pmutex );


/************************** Thread Funcion **************************/
typedef struct _tTHREAD {
	BOOL       m_bActive;
	pthread_t  m_hThread;

	void*      initialArg;
	void*      runArg;
	void*      finalArg;
	void       (*initial)( void* arg );
	void       (*run)( void* arg );
	void       (*final)( void* arg );
} Thread_t;

Thread_t* TH_CreateThread();
int  TH_Start( Thread_t* pTh );
void TH_Terminate( Thread_t* pTh );
BOOL TH_IsRunning(Thread_t *pTh);

void TH_SetInitialCallback( Thread_t* pTh, void(*Initial)(void*), void* arg );
void TH_SetFinalCallback( Thread_t* pTh, void(*Final)(void*), void* arg );
void TH_SetRunCallback( Thread_t* pTh, void(*Run)(void*), void* arg );


/************************** wait condional **************************/
#define INFITE_WAIT				0
#define EVENT_WAIT_TIME_OUT		0
#define EVENT_WAIT_OCCUR		1
#define EVENT_WAIT_TERMINATE	2

typedef struct _tWAIT_COND {
	BOOL            bEvent;
	BOOL            bTerminate;
	BOOL            bWait;
	pthread_mutex_t mutex;
	pthread_cond_t  event;
} WAIT_COND;

void WAIT_Init( WAIT_COND* pwait );
void WAIT_SetEvent( WAIT_COND* pwait );
int  WAIT_WaitforEvent( WAIT_COND* pwait, unsigned int millisec );
void WAIT_Term( WAIT_COND* pwait );


#endif