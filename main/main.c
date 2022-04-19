#include "linked_list.h"
#include "key.h"
#include "thread.h"
#include "basic.h"
#include "integrity_key.h"
#include "show_map.h"

static Thread_t* g_pThRunStation = NULL;
static MUTEX_T   g_mtxIntegrityKey;
static WAIT_COND g_waitRunStation;

static BOOL initMain( KEY *pKey );
static void quitMain( KEY *pKey );

#define MAX_MENU_COUNT 6
static void RunStationThread( void* lp )
{
    KEY *pKey = (KEY *)lp;

    stList *head = CreateStationList();
    CreateSubwayList();

    swList *swHead_FWD = GetSubwayListHead_FWD();
    swList *swHead_RVS = GetSubwayListHead_RVS();

    swList *curPoint = NULL;

    BOOL bShowClockWise = TRUE;
    BOOL bThreadFinish  = FALSE;

    SM_ARG smArg = { MENU_SUBWAY_CUR_POS, 0, TRUE };

    struct timeval tmStart;
    struct timeval tmDone;
    struct timeval tmInterval;

    memset(&tmStart,    0, sizeof(tmStart));
    memset(&tmDone,     0, sizeof(tmDone));
    memset(&tmInterval, 0, sizeof(tmInterval));

    gettimeofday( &tmStart, NULL );

    int count       = 0;
    int inputKey    = KEY_NON_INPUT;
    int menuCursor  = 0;
    int waitRet     = EVENT_WAIT_TIME_OUT;

    system("clear");

	do {
        waitRet  = WAIT_WaitforEvent( &g_waitRunStation, 500000 );
        inputKey = GetKeyValueIntegrity( pKey );

        switch (inputKey)
        {
        case KEY_TAP:
            bShowClockWise = !bShowClockWise;
            break;

        case KEY_ESC:
            bThreadFinish = TRUE;
            break;

        case KEY_UP:
            if( menuCursor == 0 ) {
                menuCursor = MAX_MENU_COUNT;
            } else {
                menuCursor--;
            }
            break;

        case KEY_DOWN:
            if( menuCursor == MAX_MENU_COUNT ) {
                menuCursor = 0;
            } else {
                menuCursor++;
            }
            break;
        
        default:
            break;
        }

        gettimeofday( &tmDone, NULL );
        tmInterval.tv_sec = tmDone.tv_sec - tmStart.tv_sec;

        if( tmInterval.tv_sec >= 1 ) {
            gettimeofday( &tmStart, NULL );
            count += tmInterval.tv_sec;

            for( int i = 0; i < tmInterval.tv_sec; i++ ) {
                SortSubwayNode();
                RunOneTime();
            }
            system("clear");
        }
        else if( waitRet == EVENT_WAIT_OCCUR ) {
            system("clear");
        }

        smArg.bClockwise = bShowClockWise;
        smArg.nMenu      = menuCursor;
        smArg.nTimeCount = count;
        curPoint         = (smArg.bClockwise) ? swHead_FWD : swHead_RVS;

        showBorderLine();
        showSelectMap( menuCursor );
        showMainMap( head, curPoint, &smArg );
        jumpTo(0, 17);

        fflush(stdout);
	} while( bThreadFinish != TRUE );

    DestroyStationList( head );
    DestroySubwayList();
	DestroyIntegrityKey( pKey );
	WAIT_Term( &g_waitRunStation );
}

static BOOL initMain( KEY *pKey )
{
	WAIT_Init( &g_waitRunStation );

    g_pThRunStation = TH_CreateThread();

	if( g_pThRunStation == NULL ) {
        ERROR_TRACE("TH_CreateThread Failed.\n");
        quitMain( pKey );
		return FALSE;
	}

	TH_SetRunCallback( g_pThRunStation, RunStationThread, pKey );

    if( TH_Start( g_pThRunStation ) == -1 ) {
        ERROR_TRACE("TH_SetRunCallback Failed.\n");
        quitMain( pKey );
		return FALSE;
    }
    return TRUE;
}

static void quitMain( KEY *pKey )
{
    TH_Terminate( g_pThRunStation );
    DestroyIntegrityKey( pKey );
}

int main()
{
    KEY *pKey = CreateIntegrityKey( &g_mtxIntegrityKey );
    BOOL bExit = FALSE;

    if( initMain( pKey ) == FALSE ) {
        return -1;
    }

    while( bExit != TRUE ) {
        SetKeyValueIntegrity( pKey, key());
        WAIT_SetEvent( &g_waitRunStation );

        if( pKey->nKeyValue == KEY_ESC )
            bExit = TRUE;
    }
    quitMain( pKey );

    return 0;
}