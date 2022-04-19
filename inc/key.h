#ifndef _KEY_H_
#define _KEY_H_

/*
    Code for immediate keystrokes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>

/* 특수 키 Define */
#define KEY_TAP         9
#define KEY_UP          65
#define KEY_DOWN        66
#define KEY_RIGHT       67
#define KEY_LEFT        68
#define KEY_ENTER       10
#define KEY_ESC         27
#define KEY_SPACEBAR    32
#define KEY_NON_INPUT   -2

int key();

#endif