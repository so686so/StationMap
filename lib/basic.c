#include "basic.h"

void jumpTo( int xPos, int yPos )
{
    printf("\033[%d;%df", yPos+1, xPos);
}