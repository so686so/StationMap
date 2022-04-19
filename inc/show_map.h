#ifndef __SHOW_MAP_H__
#define __SHOW_MAP_H__

#include "linked_list.h"

#define MSG_LEN   100

enum _eMenu {
    MENU_SUBWAY_CUR_POS = 0,
    MENU_STATION_HISTORY,
};

typedef struct _tShowMapArg {
    int  nMenu;
    int  nTimeCount;
    BOOL bClockwise;
} SM_ARG;

#define MAIN_SHOW_MAP_START_COL 5
#define MAIN_SHOW_MAP_START_ROW 3
#define MAIN_SHOW_MAP_ARROW_POS 35

#define SLCT_SHOW_MAP_START_COL 80
#define SLCT_SHOW_MAP_START_ROW 1
#define SLCT_SHOW_MAP_ARROW_POS -3

#define COUNT_TO_HOUR(x)    ( ( x / 60 ) % 24 )
#define COUNT_TO_MIN(x)     ( x % 60 )

void showBorderLine();
void showMainMap( stList *stHead, swList *swHead, SM_ARG *arg );
void showSelectMap( int nMenu );

#endif