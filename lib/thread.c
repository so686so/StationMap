#include "thread.h"


/******************************** Mutex ********************************/
void MUTEX_Init( MUTEX_T* pmutex )
{
	pthread_mutex_init( &pmutex->mutex, NULL );
	pmutex->bEnable = TRUE;
}

/*
 * wait forever until mutex released
 */
void MUTEX_Enter( MUTEX_T* pmutex )
{
	if( pmutex->bEnable == TRUE ) {
		pthread_mutex_lock( &pmutex->mutex );
	}
}

/*
 * release mutex
 */
void MUTEX_Leave( MUTEX_T* pmutex )
{
	 if( pmutex->bEnable == TRUE ) {
		pthread_mutex_unlock(&pmutex->mutex);
	}
}

void MUTEX_Term( MUTEX_T* pmutex )
{
	if( pmutex->bEnable == TRUE ) {
		pthread_mutex_destroy( &pmutex->mutex );
	}
	pmutex->bEnable = FALSE;
}


/************************** Thread Funcion **************************/
static void* ThreadProc( void* arg )
{
	Thread_t* pTh = (Thread_t*)arg;

	if( pTh == NULL ) {
		return 0;
	}

	if( pTh->initial != NULL ) {
        TRACE( "Thread Initial\n" );
		pTh->initial( pTh->initialArg );
	}

	if( pTh->run != NULL ) {
		pTh->run( pTh->runArg );
	}

	if( pTh->final ) {
        TRACE( "Thread Final\n" );
		pTh->final( pTh->finalArg );
	}

	return NULL;
}

Thread_t* TH_CreateThread()
{
	Thread_t* pTh = ( Thread_t* )malloc( sizeof( Thread_t ) );

	if( pTh == NULL ) {
		return NULL;
	}

	pTh->initial    = NULL;
	pTh->initialArg = NULL;

	pTh->run        = NULL;
	pTh->runArg     = NULL;

	pTh->final      = NULL;
	pTh->finalArg   = NULL;

	pTh->m_bActive  = FALSE;

	return pTh;
}

int TH_Start( Thread_t* pTh )
{
	if( pTh->m_bActive == FALSE ){
        if( pthread_create( &pTh->m_hThread, NULL, ThreadProc,(void*)pTh ) == -1 ) {
            ERROR_TRACE( "[Thread]thread cannot create\n" );
            return -1;
        }
		pTh->m_bActive = TRUE;
	}
	return 0;
}

void TH_SetInitialCallback( Thread_t* pTh, void(*Initial)(void*), void* arg )
{
	pTh->initial    = Initial;
	pTh->initialArg = arg;
}

void TH_SetFinalCallback( Thread_t* pTh, void(*Final)(void*), void* arg )
{
	pTh->final      = Final;
	pTh->finalArg   = arg;
}

void TH_SetRunCallback( Thread_t* pTh, void(*Run)(void*), void* arg )
{
	pTh->run    = Run;
	pTh->runArg = arg;
}

void TH_Terminate( Thread_t* pTh )
{
	if( pTh->m_bActive == TRUE ) {
        TRACE( "===================== Thread Jonin =====================\n" );
		pthread_join( pTh->m_hThread, NULL );
		pTh->m_bActive = FALSE;
	}
	free( pTh );
}

BOOL TH_IsRunning( Thread_t* pTh )
{
	return pTh->m_bActive;
}


/************************** wait condional **************************/
void WAIT_Init( WAIT_COND* pwait )
{
	pwait->bEvent     = FALSE;
	pwait->bTerminate = FALSE;
	pwait->bWait      = FALSE;

	pthread_mutex_init( &pwait->mutex, NULL );
	pthread_cond_init( &pwait->event, NULL );
}

void WAIT_SetEvent( WAIT_COND* pwait )
{
    pthread_mutex_lock( &pwait->mutex );
	if( pwait->bTerminate == TRUE ) {
        pthread_mutex_unlock( &pwait->mutex );
		return;
	}

	if( pwait->bEvent == FALSE ) {
		pwait->bEvent = TRUE;
		pthread_cond_signal( &pwait->event );
	}
    pthread_mutex_unlock( &pwait->mutex );
}

// usec 1000000 => 1 sec
int WAIT_WaitforEvent( WAIT_COND* pwait, unsigned int usec )
{
	int nRet = EVENT_WAIT_OCCUR;

	if( pwait->bTerminate == TRUE ) {
		return EVENT_WAIT_TERMINATE;
	}

    pthread_mutex_lock( &pwait->mutex );
	if( pwait->bEvent == FALSE ) {
        pwait->bWait = TRUE;

		if( usec == INFITE_WAIT ) {
			// wait infinitly
			pthread_cond_wait( &pwait->event, &pwait->mutex );
		}
		else {
			// wait time out
			int ret;
			struct timeval  now;
			struct timespec timeout;

			gettimeofday( &now, NULL );

			now.tv_usec += (usec % 1000000);

			if( now.tv_usec >= 1000000 ) {
				now.tv_usec -= 1000000;
				now.tv_sec  += 1;
			}

			timeout.tv_sec  = now.tv_sec + ( usec / 1000000 );
			timeout.tv_nsec = now.tv_usec * 1000;

			ret = pthread_cond_timedwait( &pwait->event, &pwait->mutex, &timeout );

			if( ret == ETIMEDOUT ) {
				nRet = EVENT_WAIT_TIME_OUT;
			}
		}
		pwait->bWait = FALSE;
	}
	pwait->bEvent = FALSE;

	if( pwait->bTerminate == TRUE ) {
		nRet = EVENT_WAIT_TERMINATE;
	}
    pthread_mutex_unlock( &pwait->mutex );

	return nRet;
}


void WAIT_Term( WAIT_COND* pwait )
{
    pthread_mutex_lock( &pwait->mutex );
	pwait->bTerminate = TRUE;
	pwait->bEvent = TRUE;
	pthread_cond_signal( &pwait->event );
    pthread_mutex_unlock( &pwait->mutex );

	pthread_mutex_destroy( &pwait->mutex );
	pthread_cond_destroy( &pwait->event );
}