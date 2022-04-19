#include "linked_list.h"

stInfo g_MainCircleLine[MAIN_CIRCLE_STATION_NUM] =
{
//    Name                  Taguek      SubLine     isBranch    BranchNode
    { "왕십리",             TRUE,       FALSE,      FALSE,      NULL },    // 0
    { "한양대",             FALSE,      FALSE,      FALSE,      NULL },
    { "뚝섬",               FALSE,      FALSE,      FALSE,      NULL },
    { "성수",               TRUE,       FALSE,      TRUE,       NULL },
    { "건대입구",           TRUE,       FALSE,      FALSE,      NULL },
    { "구의",               FALSE,      FALSE,      FALSE,      NULL },    // 5
    { "강변",               FALSE,      FALSE,      FALSE,      NULL },
    { "잠실나루",           FALSE,      FALSE,      FALSE,      NULL },
    { "잠실",               TRUE,       FALSE,      FALSE,      NULL },
    { "신천",               FALSE,      FALSE,      FALSE,      NULL },
    { "종합운동장",         TRUE,       FALSE,      FALSE,      NULL },    // 10
    { "삼성",               FALSE,      FALSE,      FALSE,      NULL },
    { "선릉",               TRUE,       FALSE,      FALSE,      NULL },
    { "역삼",               FALSE,      FALSE,      FALSE,      NULL },
    { "강남",               TRUE,       FALSE,      FALSE,      NULL },
    { "교대",               TRUE,       FALSE,      FALSE,      NULL },    // 15
    { "서초",               FALSE,      FALSE,      FALSE,      NULL },
    { "방배",               FALSE,      FALSE,      FALSE,      NULL },
    { "사당",               TRUE,       FALSE,      FALSE,      NULL },
    { "낙성대",             FALSE,      FALSE,      FALSE,      NULL },
    { "서울대입구",         FALSE,      FALSE,      FALSE,      NULL },    // 20
    { "봉천",               FALSE,      FALSE,      FALSE,      NULL },
    { "신림",               FALSE,      FALSE,      FALSE,      NULL },
    { "신대방",             FALSE,      FALSE,      FALSE,      NULL },
    { "구로디지털단지",     FALSE,      FALSE,      FALSE,      NULL },
    { "대림",               TRUE,       FALSE,      FALSE,      NULL },    // 25
    { "신도림",             TRUE,       FALSE,      TRUE,       NULL },
    { "문래",               FALSE,      FALSE,      FALSE,      NULL },
    { "영등포구청",         TRUE,       FALSE,      FALSE,      NULL },
    { "당산",               TRUE,       FALSE,      FALSE,      NULL },
    { "합정",               TRUE,       FALSE,      FALSE,      NULL },    // 30
    { "홍대입구",           TRUE,       FALSE,      FALSE,      NULL }, 
    { "신촌",               FALSE,      FALSE,      FALSE,      NULL }, 
    { "이대",               FALSE,      FALSE,      FALSE,      NULL }, 
    { "아현",               FALSE,      FALSE,      FALSE,      NULL }, 
    { "충정로",             TRUE,       FALSE,      FALSE,      NULL },    // 35
    { "시청",               TRUE,       FALSE,      FALSE,      NULL }, 
    { "을지로입구",         FALSE,      FALSE,      FALSE,      NULL }, 
    { "을지로3가",          TRUE,       FALSE,      FALSE,      NULL }, 
    { "을지로4가",          TRUE,       FALSE,      FALSE,      NULL }, 
    { "동대문역사문화공원", TRUE,       FALSE,      FALSE,      NULL },    // 40
    { "신당",               TRUE,       FALSE,      FALSE,      NULL }, 
    { "상왕십리",           FALSE,      FALSE,      FALSE,      NULL }, 
};

stInfo g_ForwardSubLine[FWD_SUBLINE_STATION_NUM] =
{
//    Name                  Taguek      SubLine     isBranch    BranchNode
    { "용답",               FALSE,      TRUE,       TRUE,       NULL }, 
    { "신답",               FALSE,      TRUE,       FALSE,      NULL }, 
    { "용두",               FALSE,      TRUE,       FALSE,      NULL }, 
    { "신설동",             TRUE,       TRUE,       FALSE,      NULL },
};

stInfo g_ReverseSubLine[RVS_SUBLINE_STATION_NUM] =
{
//    Name                  Taguek      SubLine     isBranch    BranchNode
    { "도림천",             FALSE,      TRUE,       TRUE,       NULL }, 
    { "양천구청",           FALSE,      TRUE,       FALSE,      NULL }, 
    { "신정네거리",         FALSE,      TRUE,       FALSE,      NULL }, 
    { "까치산",             TRUE,       TRUE,       FALSE,      NULL },
};