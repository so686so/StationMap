#include "show_map.h"
#include "basic.h"
#include <string.h>

static char *colorSubLine( char *msg, stInfo info )
{
    int setColor = 0;
    
    memset( msg, 0x00, MSG_LEN );
    
    if     ( info.isSubLine  )  { setColor = COLOR_SKY;    } 
    else if( info.isBranched )  { setColor = COLOR_PINK;   }
    else if( info.isTaeguek  )  { setColor = COLOR_YELLOW; }

    snprintf( msg, MSG_LEN, "\x1b[%dm%s\x1b[0m", setColor, info.name );

    return msg;
}

static void showMainLineEachSubway( int nMenu, swList *sNode, int lineNum )
{
    stList  *nextStation        = NULL;
    swInfo  curInfo             = sNode->info;
    char    printMsg[MSG_LEN]   = {'0',};
    int     realRunTime         = (curInfo.runTime > 0) ? curInfo.runTime : 0;

    nextStation = GetNextStation( sNode );
    lineNum    += MAIN_SHOW_MAP_START_ROW;

    jumpTo( MAIN_SHOW_MAP_START_COL, lineNum );
    printf("#%02d ", curInfo.subwayID);

    switch (nMenu)
    {
    case MENU_SUBWAY_CUR_POS:
        if( curInfo.bOnRail == TRUE ) {
            printf("(열차 이동 중... (%d))", curInfo.remainTime);
            jumpTo( MAIN_SHOW_MAP_START_COL + MAIN_SHOW_MAP_ARROW_POS, lineNum );
            printf(" -> %s", colorSubLine(printMsg, curInfo.curStation->info));    
        } else {
            printf("%s [대기중(%d)]", colorSubLine(printMsg, curInfo.curStation->info), curInfo.remainTime);    
            jumpTo( MAIN_SHOW_MAP_START_COL + MAIN_SHOW_MAP_ARROW_POS, lineNum );
            printf(" -> %s", colorSubLine(printMsg, nextStation->info));    
        }
        break;

    case MENU_STATION_HISTORY:
        printf("[ 총 경유 역 : %4d ] [ 총 운행 시간 : %02d H %02d M ]", 
                curInfo.stationCount, 
                COUNT_TO_HOUR(realRunTime), COUNT_TO_MIN(realRunTime) );
        break;

    default:
        break;
    }
}

static void showHeadLine( SM_ARG *arg )
{
    int Hour = ( 2 + ( arg->nTimeCount / 60 ) ) % 24;
    int Min  = arg->nTimeCount % 60;

    jumpTo( 3, 1 );
    printf("[ %02d : %02d ] [%s]", Hour, Min, (arg->bClockwise) ? "시계 방향" : "반시계 방향");
}

void showBorderLine()
{
    jumpTo( 0, 0 );
    printf("┌───────────────────────────────────────────────────────────────────┐    ┌───────────────────────────────────┐\n");
    printf("│                                                                   │    │                                   │\n");
    printf("│                                                                   │    │                                   │\n");
    printf("│                                                                   │    │                                   │\n");
    printf("│                                                                   │    │                                   │\n");
    printf("│                                                                   │    │                                   │\n");
    printf("│                                                                   │    │                                   │\n");
    printf("│                                                                   │    │                                   │\n");
    printf("│                                                                   │    └───────────────────────────────────┘\n");
    printf("│                                                                   │    ┌───────────────────────────────────┐\n");
    printf("│                                                                   │    │                                   │\n");
    printf("│                                                                   │    │                                   │\n");
    printf("│                                                                   │    │                                   │\n");
    printf("│                                                                   │    │                                   │\n");
    printf("│                                                                   │    └───────────────────────────────────┘\n");
    printf("└───────────────────────────────────────────────────────────────────┘        TAP : 방향 변경  ESC : 종료\n");
}

void showMainMap( stList *stHead, swList *swHead, SM_ARG *arg )
{
    swList *curPoint    = swHead;
    int     curLine     = 0;
    
    UNUSED(stHead);

    switch ( arg->nMenu )
    {
    case MENU_SUBWAY_CUR_POS:
    case MENU_STATION_HISTORY:
        showHeadLine( arg );
        while( curPoint != NULL ) {
            showMainLineEachSubway( arg->nMenu, curPoint, curLine );
            curPoint = curPoint->link;
            curLine++;
        }
        break;
    
    default:
        showHeadLine( arg );
        break;
    }
}

void showSelectMap( int nMenu )
{
    #define START_COL_T 80
    #define START_ROW_T 1
    #define ARROW_POS_T -3

    int currRow = SLCT_SHOW_MAP_START_ROW;

    jumpTo(SLCT_SHOW_MAP_START_COL, currRow++); printf("1. 지하철 별 현재 위치");
    jumpTo(SLCT_SHOW_MAP_START_COL, currRow++); printf("2. 지하철 별 총 이동 역 수");
    jumpTo(SLCT_SHOW_MAP_START_COL, currRow++); printf("3. 미구현");
    jumpTo(SLCT_SHOW_MAP_START_COL, currRow++); printf("4. 미구현");
    jumpTo(SLCT_SHOW_MAP_START_COL, currRow++); printf("5. 미구현");
    jumpTo(SLCT_SHOW_MAP_START_COL, currRow++); printf("6. 미구현");
    jumpTo(SLCT_SHOW_MAP_START_COL, currRow++); printf("7. 미구현");

    jumpTo(SLCT_SHOW_MAP_START_COL+SLCT_SHOW_MAP_ARROW_POS, SLCT_SHOW_MAP_START_ROW+nMenu); printf("▶ ");

    currRow += 2;

    jumpTo(SLCT_SHOW_MAP_START_COL, currRow++); printf("* 일반 역");
    jumpTo(SLCT_SHOW_MAP_START_COL, currRow++); printf("* \x1b[33m환승 역\x1b[0m");
    jumpTo(SLCT_SHOW_MAP_START_COL, currRow++); printf("* \x1b[35m분기 역\x1b[0m");
    jumpTo(SLCT_SHOW_MAP_START_COL, currRow++); printf("* \x1b[36m서브 역\x1b[0m");
}
