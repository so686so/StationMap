#include <malloc.h>
#include <string.h>
#include "linked_list.h"
#include "station_info.h"

stList *stHead      = NULL;
swList *swHead_FWD  = NULL;
swList *swHead_RVS  = NULL;

static stList *CreatePartialStationList( int StationKind )
{
    stList *head        = NULL;

    stList *curNode     = NULL;
    stList *preNode     = NULL;

    stInfo *infoTable   = NULL;
    int     stationNum  = 0;

    switch ( StationKind )
    {
    default:
    case MAIN_CIRCLE_LINE:
        infoTable   = g_MainCircleLine;
        stationNum  = MAIN_CIRCLE_STATION_NUM;
        break;

    case FORWARD_SUBLINE:
        infoTable   = g_ForwardSubLine;
        stationNum  = FWD_SUBLINE_STATION_NUM;
        break;

    case REVERSE_SUBLINE:
        infoTable   = g_ReverseSubLine;
        stationNum  = RVS_SUBLINE_STATION_NUM;
        break;
    }

    for( int idx = 0; idx < stationNum; idx++ ) {
        if( head == NULL ) {
            head            = (stList *)malloc(sizeof(stList));
            curNode         = head;

            head->info      = infoTable[idx];
            head->prev      = NULL;
            head->next      = NULL;
        } 
        else {
            curNode->next   = (stList *)malloc(sizeof(stList));
            preNode         = curNode;
            curNode         = curNode->next;

            curNode->info   = infoTable[idx];
            curNode->prev   = preNode;
            curNode->next   = NULL;
        }
    }

    if( StationKind == MAIN_CIRCLE_LINE ) {
        curNode->next       = head;
        head->prev          = curNode;
    }

    return head;
}

static void SetBranchNode( char *branchStationName, stList *subLineHead )
{
    stList *branchStation = GetStationByName( branchStationName );

    if( branchStation == NULL ) {
        ERROR_TRACE("Cannot Find branchStation\n");
        return;        
    }

    branchStation->info.branchNode  = (void *)subLineHead;
    subLineHead->info.branchNode    = (void *)branchStation;
}

stList *CreateStationList( void )
{
    stHead              = CreatePartialStationList( MAIN_CIRCLE_LINE );
    stList* FWD_SubLine = CreatePartialStationList( FORWARD_SUBLINE  );
    stList* RVS_SubLine = CreatePartialStationList( REVERSE_SUBLINE  );

    SetBranchNode( "성수",   FWD_SubLine );
    SetBranchNode( "신도림", RVS_SubLine );

    return stHead;
}

void DestroyStationList( stList *targetHead )
{
    if( targetHead == NULL ) return;

    stList *curNode   = targetHead;
    stList *nextNode  = NULL;

    while( TRUE ) {
        if( curNode == NULL )
            break;

        nextNode = curNode->next;

        if( curNode->info.branchNode != NULL  && 
            curNode->info.isSubLine  == FALSE ){
            DestroyStationList( (stList *)(curNode->info.branchNode) );
        }

        free( curNode );
        curNode = NULL;

        if( nextNode == targetHead ) 
            break;

        curNode = nextNode;
    }
}

stList *GetStationByName( char *stName )
{
    if( stHead == NULL ) {
        ERROR_TRACE("StationList Head Node Not Exist\n");
        return NULL;
    }

    stList *Node      = stHead;
    stList *saveNode  = NULL;

    int     maxCount  = MAIN_CIRCLE_STATION_NUM;
    int     subCount  = FWD_SUBLINE_STATION_NUM;

    for( int i = 0; i < maxCount; i++ ) {
        // Search MainLine
        if( !( strcmp( stName, Node->info.name ) ) ) {
            return Node;
        }

        // Search SubLine
        if( Node->info.branchNode != NULL ) {
            saveNode = Node;
            Node     = (stList *)(Node->info.branchNode);

            for( int j = 0; j < subCount; j++ ) {
                if( !( strcmp( stName, Node->info.name ) ) ) {
                    return Node;
                }

                if( Node->next == NULL ) break;
                Node = Node->next;
            }
            Node = saveNode;
        }
        Node = Node->next;
    }
    return NULL;
}

stList *GetStartStation( BOOL bClockWise )
{
    if( bClockWise == TRUE ) {
        return GetStationByName( FWD_START_STATION_NAME );
    } else {
        return GetStationByName( RVS_START_STATION_NAME );
    }
}

BOOL isClockWiseSubLine( stList *target )
{
    if( target->info.isSubLine == FALSE ) return FALSE;

    for( int i = 0; i < FWD_SUBLINE_STATION_NUM; i++ ) {
        if( !strcmp( target->info.name, g_ForwardSubLine[i].name ) ) return TRUE;
    }

    return FALSE;
}

static BOOL isVaildSubwayEnterSubLine( swList *target )
{
    BOOL isValid = FALSE;

    if( ( target->info.bClockwise == TRUE  && target->info.subwayID % FWD_SUBWAY_IN_SUBLINE == 0 ) ||
        ( target->info.bClockwise == FALSE && target->info.subwayID % RVS_SUBWAY_IN_SUBLINE == 0 ) ){
        isValid = TRUE;
    }

    return isValid;
}

stList *GetNextStation( swList *target )
{
    swInfo curSubwayInfo  = target->info;
    stInfo curStationInfo = curSubwayInfo.curStation->info;

    stList *CurStation    = curSubwayInfo.curStation;
    stList *NextStation   = NULL;

/*  [ 경우의 수 ]

    1. 현재 역이 분기점 진입하는 역일 때
        1) 방향과 특정 배수가 맞는 지하철일 때      -> 분기점 진입
        2) 아닐 때                                  -> 다음 순환 방향 역

    2. 현재 역이 분기 내 서브라인 역일 때
        1) 현재 역이 순환 메인 역 진입하는 역일 때
            i)  아직 되돌아오지 않았을 때           -> 다음 서브라인 역
            ii) 이미 되돌아 온 상태일 때            -> 순환 메인 역으로 진입
        2) 현재 역의 다음 역이 없을 때 : 맨 끝 역   -> 이전역으로 회귀
        3) 위의 경우에 해당하지 않을 때
            i)  아직 되돌아오지 않았을 때           -> 다음 서브라인 역
            ii) 이미 되돌아 온 상태일 때            -> 이전 서브라인 역

    3. Else                                         -> 다음 순환 방향 역
*/

    // Case 1.
    if( curStationInfo.isBranched == TRUE && curStationInfo.isSubLine  == FALSE ) {
        // 1)
        if( isVaildSubwayEnterSubLine( target ) && curSubwayInfo.bGoSubLine ) {
            NextStation = (stList *)(curStationInfo.branchNode);
        } 
        // 2)
        else {
            NextStation = ( curSubwayInfo.bClockwise ) ? CurStation->next : CurStation->prev;
        }
    }
    // Case 2.
    else if( curStationInfo.isSubLine == TRUE ) {
        // 1)
        if( curStationInfo.isBranched == TRUE ) {
            // i)
            if( curSubwayInfo.bTurn == FALSE ) {
                NextStation = CurStation->next;
            }
            // ii) 
            else {
                NextStation = (stList *)(curStationInfo.branchNode);
            }
        }
        // 2)
        else if( CurStation->next == NULL ) {
            NextStation = CurStation->prev;
        }
        // 3)
        else {
            // i)
            if( curSubwayInfo.bTurn == FALSE ) {
                NextStation = CurStation->next;
            }
            // ii)
            else {
                NextStation = CurStation->prev;
            }
        }
    }
    // Case 3.
    else {
        NextStation = ( curSubwayInfo.bClockwise ) ? CurStation->next : CurStation->prev;
    }

    return NextStation;
}

static void ApplyStationChanged( swList *target, stNode *applyStation )
{
/*  [ 경우의 수 ]

    1. 왕십리 역( 시점/종점 기준 ) 도착했을 때      -> isVaildSubwayEnterSubLine 이라면 bGoSubLine TRUE

    2. 현재 역이 분기점 진입하는 역일 때
        1) 방향과 특정 배수가 맞는 지하철일 때      -> bGoSubLine FALSE / bTurn FALSE
        2) 아닐 때                                  -> None

    3. 현재 역이 분기 내 서브라인 역일 때
        1) 현재 역이 순환 메인 역 진입하는 역일 때
            i)  아직 되돌아오지 않았을 때           -> None
            ii) 이미 되돌아 온 상태일 때            -> bTurn False
        2) 현재 역의 다음 역이 없을 때 : 맨 끝 역   -> bTurn TRUE
        3) 위의 경우에 해당하지 않을 때
            i)  아직 되돌아오지 않았을 때           -> None
            ii) 이미 되돌아 온 상태일 때            -> None

    4. Else                                         -> None
*/

    swInfo curSubwayInfo  = target->info;
    stInfo curStationInfo = curSubwayInfo.curStation->info;
    stList *CurStation    = curSubwayInfo.curStation;

    // Apply
    target->info.curStation = applyStation;
    target->info.stationCount++;

    // Case 1.
    if( !strcmp( curStationInfo.name, FWD_START_STATION_NAME ) ) {
        if( isVaildSubwayEnterSubLine( target ) ) {
            target->info.bGoSubLine = TRUE;
        }
    }
    // Case 2.
    else if( curStationInfo.isBranched == TRUE && curStationInfo.isSubLine == FALSE ) {
        // 1)
        if( isVaildSubwayEnterSubLine( target ) && curSubwayInfo.bGoSubLine ) {
            target->info.bGoSubLine = FALSE;
            target->info.bTurn      = FALSE;
        }
    }
    // Case 3.
    else if( curStationInfo.isSubLine == TRUE ) {
        if( curStationInfo.isBranched == TRUE && curSubwayInfo.bTurn == TRUE ) {
            target->info.bTurn = FALSE;
        }
        else if( CurStation->next == NULL && curSubwayInfo.bTurn == FALSE ) {
            target->info.bTurn = TRUE;
        }
    }
}

static int AddSubLineDistance( stList *target )
{
    if( target->info.isSubLine == FALSE )
        return 0;

    stInfo *InfoTable   = NULL;
    int     distance    = 0;

    if( isClockWiseSubLine( target ) == TRUE ) {
        InfoTable = g_ForwardSubLine;
    } else {
        InfoTable = g_ReverseSubLine;
    }

    for( int i = 0; i < FWD_SUBLINE_STATION_NUM; i++ ) {
        distance++;
        if( !strcmp( target->info.name, InfoTable[i].name ) )
            break;
    }

    return distance;
}

static stList *GetBranchedStationFromSubLine( stList *target )
{
    if( target->info.isSubLine == FALSE )
        return NULL;

    stList *dst = target;

    while( 1 ) {
        if( dst->prev == NULL ) break;
        dst = dst->prev;
    }

    return (stList *)(dst->info.branchNode);
}

int GetDistance( stList *src, stList *dst, BOOL bClockWise )
{
    int totalDistance = 0;

    if( src == dst ) return 0;

    stList *startStation  = src;
    stList *finishStation = dst;

    if( startStation->info.isSubLine == TRUE ) {
        totalDistance += AddSubLineDistance( startStation );
        startStation  =  GetBranchedStationFromSubLine( startStation );
    }

    if( finishStation->info.isSubLine == TRUE ) {
        totalDistance += AddSubLineDistance( finishStation );
        finishStation =  GetBranchedStationFromSubLine( finishStation );
    }

    while ( startStation != finishStation )
    {
        totalDistance++;
        startStation = (bClockWise) ? startStation->next : startStation->prev;
    }

    return totalDistance;
}

static swList *CreatePartialSubwayList( BOOL bClockWise )
{
    swList *head        = NULL;
    swList *curNode     = NULL;
    int     startNum    = (bClockWise) ? 1 : 2;
    int     idx         = 0;
    int     waitMin     = 0;

    for( idx = startNum; idx <= MAX_SUBWAY_CNT; idx += 2 ) {
        if( head == NULL ) {
            head            = (swList *)malloc(sizeof(swList));
            curNode         = head;
            head->link      = NULL;
        } else {
            curNode->link   = (swList *)malloc(sizeof(swList));
            curNode         = curNode->link;
            curNode->link   = NULL;
        }
        
        curNode->info.subwayID      = idx;
        curNode->info.bClockwise    = bClockWise;

        curNode->info.remainTime    = waitMin;
        curNode->info.bWait         = TRUE;
        curNode->info.bOnRail       = FALSE;
        curNode->info.bGoSubLine    = isVaildSubwayEnterSubLine( curNode );
        curNode->info.bTurn         = FALSE;
        curNode->info.curStation    = GetStartStation( bClockWise );

        curNode->info.stationCount  = 0;
        curNode->info.runTime       -= waitMin;

        waitMin += WAIT_WHEN_FRONT_START;
    }

    TRACE("Create SubWayList [%d] Done\n", bClockWise);

    return head;
}

void CreateSubwayList( void )
{
    swHead_FWD = CreatePartialSubwayList( TRUE  );
    swHead_RVS = CreatePartialSubwayList( FALSE );
}

static void DestroyPartialSubwayList( swList *sHead )
{
    if( sHead == NULL ) return;

    swList *nextNode  = NULL;
    swList *curNode   = sHead;

    while( 1 ) {
        if( curNode == NULL ) break;
        nextNode = curNode->link;

        free( curNode );
        curNode = NULL;

        if( nextNode == sHead ) break;
        curNode = nextNode;
    }    
}

void DestroySubwayList( void )
{
    DestroyPartialSubwayList( swHead_FWD );
    DestroyPartialSubwayList( swHead_RVS );
}

static void Swap( swList *src, swList *dst )
{
    if( src == dst ) return;

    swInfo tmpInfo  = src->info;
    src->info       = dst->info;
    dst->info       = tmpInfo;
}

static void SortEachSubwayNode( swList *head )
{
    if( head == NULL ) return;

/*
Algorithm
=========
    3. 매 초마다 출발기준역을 0 거리값 기준으로 하여, SubwayList 거리 기준 내림차순 정렬
        1) 거리가 같을 시, 주요 순환 노선이 priority 높음
        2) 거리가 같고 지하철 역도 동일할 경우(아직 출발 안 했을 때), 지하철 번호가 낮을수록 priority 높음
*/


    BOOL bCLockWise = head->info.bClockwise;

    swList *checkNode   = NULL;
    swList *compNode    = NULL;

    stList *startPoint  = GetStartStation( bCLockWise );

    int     checkDist   = 0;
    int     compDist    = 0;
    int     sortIdx, i;
    
    for( sortIdx = 1; sortIdx < MAX_SUBWAY_CNT/2; sortIdx++ ) {

        checkNode = head;
        for( i = 0; i < sortIdx; i++ ) {
            checkNode = checkNode->link;
        }

        compNode = head;
        while( compNode != checkNode ) {
        // Sort Compare
            checkDist = GetDistance( startPoint, checkNode->info.curStation, bCLockWise );
            compDist  = GetDistance( startPoint, compNode->info.curStation,  bCLockWise );

            if( compDist  == MAIN_CIRCLE_STATION_NUM - 1 && checkDist == 0 ) {
                checkDist += MAIN_CIRCLE_STATION_NUM;
            }
            
            if( checkDist > compDist ) {
                Swap( checkNode, compNode );
                break;
            }
            else if( checkDist == compDist ) {
                if( checkNode->info.curStation->info.isSubLine == FALSE &&
                    compNode->info.curStation->info.isSubLine == TRUE  ){
                    Swap( checkNode, compNode );
                    break;
                }
                else {
                    if( checkNode->info.subwayID < compNode->info.subwayID ) {
                        Swap( checkNode, compNode );
                        break;
                    }
                }
            }
            compNode = compNode->link;
        }
    }
}

void SortSubwayNode()
{
    SortEachSubwayNode( swHead_FWD );
    SortEachSubwayNode( swHead_RVS );
}

static BOOL checkOccufyNextStation( swList *target )
{
    BOOL    bClockWise  = target->info.bClockwise;
    swList *checkPoint  = (bClockWise) ? swHead_FWD : swHead_RVS;
    stList *nextStation = GetNextStation( target );

    while( checkPoint != NULL ) {
        if( !strcmp( nextStation->info.name, checkPoint->info.curStation->info.name ) ) {
            if( nextStation->info.isSubLine == TRUE ) {
                if( target->info.curStation->next == NULL ) {
                    return FALSE;
                }
                else if( target->info.bTurn == checkPoint->info.bTurn ) {
                    return TRUE;
                }
            } else {
                if( target->info.bClockwise == checkPoint->info.bClockwise ) {
                    return TRUE;
                }
            }
        }
        checkPoint = checkPoint->link;
    }

    return FALSE;
}

void RunPartialOneTime( swList *swHead )
{
    swList  *curNode    = swHead;

    while( curNode != NULL ) {
        if( curNode->info.remainTime > 0 ) {
            curNode->info.remainTime--;
            curNode->info.runTime++;
        }

        if( curNode->info.remainTime == 0 && curNode->info.bWait == TRUE ) {
            curNode->info.bWait = FALSE;
        }

        if( checkOccufyNextStation( curNode ) == TRUE ) {
            if( curNode->info.bWait == FALSE && curNode->info.remainTime == 0 ) {
                curNode->info.bWait =  TRUE;
                curNode->info.remainTime += WAIT_WHEN_NEXT_OCCUPY;
            }
        }

        else if( curNode->link            != NULL                           &&
                 curNode->info.curStation == curNode->link->info.curStation &&
                 curNode->info.bOnRail    == FALSE                          &&
                 curNode->info.remainTime == 0                              ){
            curNode->info.bWait            = FALSE;
            curNode->info.bOnRail          = TRUE;
            curNode->info.remainTime       += TRAVEL_TIME_ON_RAIL;
            // curNode->link->info.remainTime += WAIT_WHEN_FRONT_START;
            ApplyStationChanged( curNode, GetNextStation( curNode ) );
        }

        else if( curNode->info.bOnRail == TRUE && curNode->info.remainTime == 0 ) {
            curNode->info.bOnRail   = FALSE;
            curNode->info.bWait     = TRUE;

            if( curNode->info.curStation->info.isTaeguek ) {
                curNode->info.remainTime += WAIT_WHEN_TAGUEK_STATION;
            } else {
                curNode->info.remainTime += WAIT_WHEN_WITHE_STATION;
            }
        }

        else if( curNode->info.bWait == FALSE && curNode->info.remainTime == 0 ) {
            curNode->info.bOnRail = TRUE;
            curNode->info.remainTime += TRAVEL_TIME_ON_RAIL;
            ApplyStationChanged( curNode, GetNextStation( curNode ) );
        }

        curNode = curNode->link;
    }
}

void RunOneTime( void )
{
    RunPartialOneTime( swHead_FWD );    
    RunPartialOneTime( swHead_RVS );    
}

swList *GetSubwayListHead_FWD( void )
{
    return swHead_FWD;
}

swList *GetSubwayListHead_RVS( void )
{
    return swHead_RVS;
}
