#include "key.h"

#define INPUT_MAX_LEN ( 3 + 1 )

/*  일반적인 아스키 값과 다르게 
    방향키는 아스키 값이 '27 91 65' 처럼 
    연속된 아스키 값으로 읽힘 */

#define DEFAULT_READ_IDX    0   // 방향키와 ESC 키를 제외한 다른 값의 idx
#define ARROW_CHECK_IDX     1   // 방향키인지 ESC 키인지 체크하는 값의 idx
#define ARROW_KEY_IDX       2   // 입력값이 방향키일 때 실제 구별값 idx

#define ARROW_CHECK_KEY     91

typedef struct termios Termios;
static Termios set_termios_non_canonical_opt( Termios *set_attr )
{
    Termios save_attr = *set_attr;

    // 즉시 입력 받아지게 새로운 stdin 옵션 설정
    set_attr->c_lflag       &= ~(ICANON | ECHO);
    set_attr->c_cc[VMIN]    = 2;
    set_attr->c_cc[VTIME]   = 1;

    // set
    tcsetattr(STDIN_FILENO, TCSANOW, set_attr);

    return save_attr;
}

static int verify_keyboard_input( char* read_data )
{
    if( read_data[DEFAULT_READ_IDX] != KEY_ESC )
    {
        return read_data[DEFAULT_READ_IDX];
    }
    else if( read_data[ARROW_CHECK_IDX] == ARROW_CHECK_KEY )
    {
        return read_data[ARROW_KEY_IDX];
    }
    else {
        return KEY_ESC;
    }
}

int key()
{
    int  res                        = KEY_NON_INPUT; // ASCII
    int  read_bytes                 = 0;
    char read_data[INPUT_MAX_LEN]   = { 0x00, };

    Termios stdin_attr;
    memset( &stdin_attr, 0x00, sizeof(Termios) );

    // 기존 stdin 옵션 get 및 새로운 옵션 할당을 위한 함수 실행
    tcgetattr( STDIN_FILENO, &stdin_attr );
    Termios save_attr = set_termios_non_canonical_opt( &stdin_attr );

    // read
    read_bytes = read( STDIN_FILENO, read_data, INPUT_MAX_LEN );

    if( read_bytes > 0 ) {
        res = verify_keyboard_input( read_data );
    }

    // 기존 stdin 옵션으로 롤백
    tcsetattr( STDIN_FILENO, TCSANOW, &save_attr );

    return res;
}