#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include "type.h"
#include "trace.h"

/*
    Code for define LinkedList about subway & station
*/

// FWD - Clockwise
// RVS - CounterClockwise

/* ==== Station Define ============================================================================ */
#define MAIN_CIRCLE_STATION_NUM 43  // 주요 순환선 지하철 역 수
#define FWD_SUBLINE_STATION_NUM 4   // 정방향 분기점 지하철 역 수 (시계 방향)
#define RVS_SUBLINE_STATION_NUM 4   // 역방향 분기점 지하철 역 수 (반시계 방향)

#define FWD_START_STATION_NAME  "왕십리"
#define RVS_START_STATION_NAME  "신도림"

enum __STATION_KIND__
{
    MAIN_CIRCLE_LINE = 0,
    FORWARD_SUBLINE,
    REVERSE_SUBLINE,
};


/* ==== Subway Define ============================================================================= */
#define MAX_SUBWAY_CNT              22
#define FWD_SUBWAY_IN_SUBLINE       3
#define RVS_SUBWAY_IN_SUBLINE       4


/* ==== Time Define ================================================================================ */
#define TRAVEL_TIME_ON_RAIL         2
#define WAIT_WHEN_NEXT_OCCUPY       1
#define WAIT_WHEN_FRONT_START       8

#define WAIT_WHEN_WITHE_STATION     3
#define WAIT_WHEN_TAGUEK_STATION    5


/* ==== ETC ======================================================================================== */
#define NAME_LEN                    30


/* ==== Structure : Station ======================================================================== */
typedef struct stInfo {
    char  name[NAME_LEN];
    BOOL  isTaeguek;
    BOOL  isSubLine;
    BOOL  isBranched;
    void *branchNode;
} stInfo;

typedef struct stNode {
    stInfo         info;
    struct stNode *prev;
    struct stNode *next;
} stNode, stList;


/* ==== Structure : Subway ======================================================================== */
typedef struct swInfo {
// Fixed
    int  subwayID;
    BOOL bClockwise;

// Variable
    int  remainTime;
    BOOL bWait;
    BOOL bOnRail;
    
// for subLine
    BOOL bGoSubLine;
    BOOL bTurn;

// The current station where it is stopped, or the next station when it is running
    stNode *curStation;

// Log each subway
    int  stationCount;
    int  runTime;
} swInfo;

typedef struct swList {
    swInfo         info;
    struct swList *link;
} swNode, swList;


/* ==== Function Declaration ======================================================================== */
stList *CreateStationList( void );
void    DestroyStationList( stList *targetHead );

stList *GetStationByName( char *stName );
stList *GetStartStation( BOOL bClockWise );

BOOL isClockWiseSubLine( stList *target );
int  GetDistance( stList *src, stList *dst, BOOL bClockWise );

void CreateSubwayList( void );
void DestroySubwayList( void );

void SortSubwayNode( void );
void RunOneTime( void );

stList *GetNextStation( swList *target );

swList *GetSubwayListHead_FWD( void );
swList *GetSubwayListHead_RVS( void );

#endif