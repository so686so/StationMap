#include "linked_list.h"
#include "key.h"
#include "thread.h"
#include "basic.h"
#include "integrity_key.h"

static Thread_t* g_pThRunStation = NULL;
static MUTEX_T   g_MtxIntegrityKey;
static WAIT_COND g_waitRunStation;


static char *colorSubLine( char *msg, stInfo info )
{
    int  setColor = 0;
    
    if( info.isSubLine ) {
        setColor = 36;
    } 
    else if( info.isBranched ) {
        setColor = 35;
    }
    else if( info.isTaeguek ) {
        setColor = 33;
    }

    memset( msg, 0x00, 100 );
    snprintf(msg, 100, "\x1b[%dm%s\x1b[0m", setColor, info.name);

    return msg;
}

#define SUBWAY_CUR_POSITION         0
#define SUBWAY_TOTAL_STATION_COUNT  1
static void showLine( int nMenu, swList *sNode, int lineNum )
{
    #define START_COL 5
    #define START_ROW 2
    #define ARROW_POS 35

    stList *nextStation = NULL;
    swInfo curInfo      = sNode->info;

    nextStation = GetNextStation( sNode );

    lineNum += START_ROW;

    char colorMsg[100] = {'0',};

    switch (nMenu)
    {
    case SUBWAY_CUR_POSITION:
        jumpTo( START_COL, lineNum );
        printf("#%02d ", curInfo.subwayID);
        if( curInfo.bOnRail == TRUE ) {
            printf("(열차 이동 중... (%d))", curInfo.remainTime);
            jumpTo(START_COL+ARROW_POS, lineNum);
            printf(" -> %s\n", colorSubLine(colorMsg, curInfo.curStation->info));    
        } else {
            printf("%s [대기중(%d)]", colorSubLine(colorMsg, curInfo.curStation->info), curInfo.remainTime);    
            jumpTo(START_COL+ARROW_POS, lineNum);
            printf(" -> %s\n", colorSubLine(colorMsg, nextStation->info));    
        }        
        break;
    case SUBWAY_TOTAL_STATION_COUNT:
        {
            int realRunTime = (curInfo.runTime > 0) ? curInfo.runTime : 0;
            jumpTo( START_COL, lineNum );
            printf("#%02d ", curInfo.subwayID);
            printf("[ 총 경유 역 : %3d ] [ 총 운행 시간 : %02d Hour %02d Min ]", curInfo.stationCount, (realRunTime / 60)%24, realRunTime%60);
        }
        break;    
    default:
        break;
    }

}

static void TestShowMenu( int nValueMenuCursor )
{
    #define START_COL_T 80
    #define START_ROW_T 2
    #define ARROW_POS_T -3

    int currRow = START_ROW_T;

    jumpTo(START_COL_T, currRow++); printf("1. 지하철 별 현재 위치");
    jumpTo(START_COL_T, currRow++); printf("2. 지하철 별 총 이동 역 수");
    jumpTo(START_COL_T, currRow++); printf("3. 미구현");
    jumpTo(START_COL_T, currRow++); printf("4. 미구현");
    jumpTo(START_COL_T, currRow++); printf("5. 미구현");

    jumpTo(START_COL_T+ARROW_POS_T, START_ROW_T+ nValueMenuCursor); printf("▶ ");

    currRow += 2;

    jumpTo(START_COL_T, currRow++); printf("* 일반 역");
    jumpTo(START_COL_T, currRow++); printf("* \x1b[33m환승 역\x1b[0m");
    jumpTo(START_COL_T, currRow++); printf("* \x1b[35m분기 역\x1b[0m");
    jumpTo(START_COL_T, currRow++); printf("* \x1b[36m서브 역\x1b[0m");
}

#define MAX_MENU_COUNT 4
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

    struct timeval  tv_start;
    struct timeval  tv_done;
    struct timeval  tv_gap;

    memset(&tv_start, 0, sizeof(tv_start));
    memset(&tv_done , 0, sizeof(tv_done));
    memset(&tv_gap  , 0, sizeof(tv_gap));

    gettimeofday( &tv_start, NULL );

    int count       = 0;
    int inputKey    = KEY_NON_INPUT;
    int menuCursor  = 0;
    int line        = 0;
	do {
        WAIT_WaitforEvent( &g_waitRunStation, 200000 );
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

        gettimeofday( &tv_done, NULL );
        tv_gap.tv_sec = (tv_done.tv_sec) - (tv_start.tv_sec);

        if( tv_gap.tv_sec >= 1 ) {
            gettimeofday( &tv_start, NULL );
            count++;

            SortSubwayNode();
            RunOneTime();
        }

        system("clear");

        line = 0;
        if( bShowClockWise ) {
            jumpTo( 3, 1 );
            printf("[ %02d : %02d ] [시계 방향]\n", (2 + (count / 60))%24, count%60);
            curPoint = swHead_FWD;
        } 
        else {
            line = 0;
            jumpTo( 3, 1 );
            printf("[ %02d : %02d ] [반시계 방향]\n", (2 + (count / 60))%24, count%60);
            curPoint = swHead_RVS;
        }

        while( curPoint != NULL ) {
            showLine( menuCursor, curPoint, line );
            curPoint = curPoint->link;
            line++;
        }

        TestShowMenu( menuCursor );
        jumpTo(0, line+5);

        fflush(stdout);

	} while( bThreadFinish != TRUE );

    DestroyStationList( head );
    DestroySubwayList();
	DestroyIntegrityKey( pKey );
	WAIT_Term( &g_waitRunStation );
}

int main()
{
    KEY *pKey = CreateIntegrityKey( &g_MtxIntegrityKey );

	WAIT_Init( &g_waitRunStation );

    g_pThRunStation = TH_CreateThread();

	if( g_pThRunStation == NULL ) {
		return FALSE;
	}

	TH_SetRunCallback( g_pThRunStation, RunStationThread, pKey );
	TH_Start( g_pThRunStation );

    while( 1 ) {
        SetKeyValueIntegrity( pKey, key() );
        WAIT_SetEvent( &g_waitRunStation );

        if( pKey->nKeyValue == KEY_ESC ) 
            break;
    }

    TH_Terminate( g_pThRunStation );

    return 0;
}