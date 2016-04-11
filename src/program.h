#ifndef PROGRAM_H
#define PROGRAM_H

#include "../../../libwiiu/src/coreinit.h"


void _entryPoint();
void _osscreeninit();
void _osscreenexit();
void ExitApplication();
void flipBuffers();
void fillScreen(char r,char g,char b,char a);
void drawPixel(int x, int y, char r, char g, char b, char a);
void drawCircle(int x0, int y0, int radius);
void drawString(int x, int line, char * string);
void welcomescreen();
void drawLine(int x0, int y0, int x1, int y1);
void drawCross(int x, int y);
unsigned int absu(int num);
void drawRectangle(int x0, int y0, int x1, int y1);
void mainscreen_two(); //human vs human
void update_board(); //update screen
unsigned int check_win(); //check if amd what player win
void reset_game(); //reset the game
void player_turn(int player);

//types.h

typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;
typedef char int8_t;
typedef uint32_t size_t;
typedef _Bool bool;
#define true 1
#define false 0
#define null 0
#define NULL (void*)0

//handles
unsigned int vpadHandle;

//VPAD.H

#define BUTTON_A        0x8000
#define BUTTON_B        0x4000
#define BUTTON_X        0x2000
#define BUTTON_Y        0x1000
#define BUTTON_LEFT     0x0800
#define BUTTON_RIGHT    0x0400
#define BUTTON_UP       0x0200
#define BUTTON_DOWN     0x0100
#define BUTTON_ZL       0x0080
#define BUTTON_ZR       0x0040
#define BUTTON_L        0x0020
#define BUTTON_R        0x0010
#define BUTTON_PLUS     0x0008
#define BUTTON_MINUS    0x0004
#define BUTTON_HOME     0x0002
#define BUTTON_SYNC     0x0001
#define BUTTON_STICK_R  0x00020000
#define BUTTON_STICK_L  0x00040000
#define BUTTON_TV       0x00010000

typedef struct
{
    float x,y;
} Vec2D;

typedef struct
{
    uint16_t x, y;               /* Touch coordinates */
    uint16_t touched;            /* 1 = Touched, 0 = Not touched */
    uint16_t validity;           /* 0 = All valid, 1 = X invalid, 2 = Y invalid, 3 = Both invalid? */
} VPADTPData;
 
typedef struct
{
    uint32_t btn_hold;           /* Held buttons */
    uint32_t btn_trigger;        /* Buttons that are pressed at that instant */
    uint32_t btn_release;        /* Released buttons */
    Vec2D lstick, rstick;        /* Each contains 4-byte X and Y components */
    char unknown1c[0x52 - 0x1c]; /* Contains accelerometer and gyroscope data somewhere */
    VPADTPData tpdata;           /* Normal touchscreen data */
    VPADTPData tpdata1;          /* Modified touchscreen data 1 */
    VPADTPData tpdata2;          /* Modified touchscreen data 2 */
    char unknown6a[0xa0 - 0x6a];
    uint8_t volume;
    uint8_t battery;             /* 0 to 6 */
    uint8_t unk_volume;          /* One less than volume */
    char unknowna4[0xac - 0xa4];
} VPADData;

VPADData vpad_data; //store VPADData fromvpad library in structure vpad_data
int error; //error for vpad data

//WiiU functions
void(*DCFlushRange)(void *buffer, unsigned int length);
void*(*OSAllocFromSystem)(uint32_t size, int align);
void(*OSFreeToSystem)(void *ptr);
unsigned int(*OSScreenFlipBuffersEx)(unsigned int bufferNum);
unsigned int(*OSScreenGetBufferSizeEx)(unsigned int bufferNum);
unsigned int(*OSScreenClearBufferEx)(unsigned int bufferNum, unsigned int temp);
unsigned int(*OSScreenPutPixelEx)(unsigned int bufferNum, unsigned int posX, unsigned int posY, uint32_t color);
unsigned int(*OSScreenPutFontEx)(unsigned int bufferNum, unsigned int posX, unsigned int line, void * buffer);
int(*VPADRead)(int controller, VPADData *buffer, unsigned int num, int *error);

//board
int b1 = 2;
int b2 = 2;
int b3 = 2;
int b4 = 2;
int b5 = 2;
int b6 = 2;
int b7 = 2;
int b8 = 2;
int b9 = 2;

int turn;

#endif /* PROGRAM_H */