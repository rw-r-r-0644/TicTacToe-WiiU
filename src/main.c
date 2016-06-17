#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/fs_functions.h"
#include "dynamic_libs/gx2_functions.h"
#include "dynamic_libs/sys_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "dynamic_libs/padscore_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "dynamic_libs/ax_functions.h"
#include "fs/fs_utils.h"
#include "fs/sd_fat_devoptab.h"
#include "system/memory.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include "common/common.h"
#include "main.h"

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

bool exitCheck = false;
VPADData vpad_data;
int error = 0;

int _main() {
	InitOSFunctionPointers();
	InitVPadFunctionPointers();
	memoryInitialize();
	
	_osscreeninit();
	//Game Loop
	while(1) {
		welcomescreen(); //Start screen
		if(exitCheck) break;
		mainscreen_two(); //2 Players game
		if(exitCheck) break;
	}
	_osscreenexit();
	memoryRelease();
	return EXIT_SUCCESS;
}
//Basic Graphs
void _osscreeninit() {
	OSScreenInit();
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	//int buf1_size = OSScreenGetBufferSizeEx(1);
	//Set the buffer area.
	OSScreenSetBufferEx(0, (void *)0xF4000000);
	OSScreenSetBufferEx(1, (void *)0xF4000000 + buf0_size);
	OSScreenEnableEx(0, 1);
	OSScreenEnableEx(1, 1);
}
void _osscreenexit() {
	for(int ii=0;ii<2;ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
}
void ExitApplication() { //Thanks to Blackspoon (@GBATemp)
	_osscreenexit();
}
//Gamepad lines:
//18 lines: 0-17
void drawString(int x, int line, char * string) { //Just draw a string
	OSScreenPutFontEx(0, x, line, string);
	OSScreenPutFontEx(1, x, line, string);
}
void flipBuffers() {
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int buf1_size = OSScreenGetBufferSizeEx(1);
	//Flush the cache
	DCFlushRange((void *)0xF4000000 + buf0_size, buf1_size);
	DCFlushRange((void *)0xF4000000, buf0_size);
	//Flip the buffer
	OSScreenFlipBuffersEx(0);
	OSScreenFlipBuffersEx(1);
}
void fillScreen(char r,char g,char b,char a) {
	unsigned int num = (r << 24) | (g << 16) | (b << 8) | a;
	OSScreenClearBufferEx(0, num);
	OSScreenClearBufferEx(1, num);
}
void drawPixel(int x, int y, char r, char g, char b, char a) {
	uint32_t num = (r << 24) | (g << 16) | (b << 8) | a;
	OSScreenPutPixelEx(0,x,y,num);
	OSScreenPutPixelEx(1,x,y,num);
} 
void drawCircle(int x0, int y0, int radius) { //code for circles from Wikipedia
  int x = radius;
  int y = 0;
  int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0

  while( y <= x )
  {
    drawPixel( x + x0,  y + y0, 255, 255, 255, 1); // Octant 1
    drawPixel( y + x0,  x + y0, 255, 255, 255, 1); // Octant 2
    drawPixel(-x + x0,  y + y0, 255, 255, 255, 1); // Octant 4
    drawPixel(-y + x0,  x + y0, 255, 255, 255, 1); // Octant 3
    drawPixel(-x + x0, -y + y0, 255, 255, 255, 1); // Octant 5
    drawPixel(-y + x0, -x + y0, 255, 255, 255, 1); // Octant 6
    drawPixel( x + x0, -y + y0, 255, 255, 255, 1); // Octant 7
    drawPixel( y + x0, -x + y0, 255, 255, 255, 1); // Octant 8
    y++;
    if (decisionOver2<=0)
    {
      decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
    }
    else
    {
      x--;
      decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
    }
  }
}
void drawRectangle(int x0, int y0, int x1, int y1) { //draw a simple Shape by 4 lines
	drawLine(x0, y0, x0, y1);
	drawLine(x0, y1, x1, y1);
	drawLine(x1, y1, x1, y0);
	drawLine(x1, y0, x0, y0);
}
void drawLine(int x0, int y0, int x1, int y1) {
 
  int dx = absu(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = absu(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  for(;;){
    drawPixel(x0,y0, 255, 255, 255, 1);
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}
void drawCross(int x, int y) { //Draw a 40x40 px cross for
	drawLine(x, y, x+40, y+40);
	drawLine(x, y+40, x+40, y);
}
unsigned int absu(int num) { //abs() clone for wii u
	if (num < 0) {
		return -num;
	}
	return num;
}
//Game Graphs
void welcomescreen() {
	fillScreen(0,0,0,1);
	drawString(23,0,"TicTacToe v0.1");
	drawString(22,2,"Press A or Play");
	drawRectangle(350, 240, 450, 280);
	//drawRectangle(275, 300, 475, 340);
	drawString(27,9,"Play");
	//drawString(20,11,"Play with Computer");
	flipBuffers();
	while(1) {
		VPADRead(0, &vpad_data, 1, &error); //Read the vpaddata
		if (vpad_data.btns_h & VPAD_BUTTON_HOME) {
			exitCheck = true;
			return;
		}
		if (vpad_data.btns_h & VPAD_BUTTON_A) {
			break;
		}
		if (vpad_data.tpdata.touched == 1)
		{
			/*
			fillScreen(0,0,0,1);
			drawRectangle(275, 300, 475, 340);
			drawString(20,11,"Play with Computer");
			char tb[35];
			__os_snprintf(tb, 35, "%i,%i", vpad_data.tpdata.x, vpad_data.tpdata.y);
			drawString(10,15,tb);
			*/
			if (vpad_data.tpdata.x >= 1664 && vpad_data.tpdata.x <= 2152 && vpad_data.tpdata.y >= 1718 && vpad_data.tpdata.y <= 2014) {
				break;
			}
			//flipBuffers();
			//unsigned int t1 = 0x500000; //wait between checks
			//while(t1--) ;
		}
	}
}
void mainscreen_two() { //2 Players game
	//TODO: USE ARRAY INSTEAD OF INTs and solve array compilation problem
	//int board[] = {2,2,2,2,2,2,2,2,2} --> loader.c:(.text+0xde8): undefined reference to `memcpy'
	reset_game();
	turn = 1;
	update_board(turn);
	flipBuffers();
	int round = 0;
	while(1) {
		if (turn == 1) {
			player_turn(1);
			if(exitCheck) return;
			turn = 0;
		} else {
			player_turn(0);
			if(exitCheck) return;
			turn = 1;
		}
		unsigned int chkwin = check_win();
		if (chkwin != 2) {
			if (chkwin == 1) {
				update_board(2);
				drawString(40,9,"X Won");
				flipBuffers();
				break;
			} else if (chkwin == 0) {
				update_board(2);
				drawString(40,9,"O Won");
				flipBuffers();
				break;
			}
		}
		round++;
		if (round == 9){
			update_board(2);
			drawString(40,9,"Nobody Won");
			flipBuffers();
			break;
		}
	}
	sleep(5);
}
void player_turn(int player){ //require input from human player(s)
	while(1) {
		VPADRead(0, &vpad_data, 1, &error); //Read the vpaddata
		
		//check home VPAD_BUTTON
		if (vpad_data.btns_h & VPAD_BUTTON_HOME) { //check if we should exit from application
			exitCheck = true;
			return;
		}
		
		//touch code
		if (vpad_data.tpdata.touched == 1) {
			if (vpad_data.tpdata.x > 422 && vpad_data.tpdata.x < 800) { //first column
				if (vpad_data.tpdata.y > 2418 && vpad_data.tpdata.y < 3122) { //first row
					if (b1==2) {b1=player; break;}
				} else if (vpad_data.tpdata.y > 1754 && vpad_data.tpdata.y < 2418) { //second row
					if (b4==2) {b4=player; break;}
				} else if (vpad_data.tpdata.y > 1068 && vpad_data.tpdata.y < 1754) { //third row
					if (b7==2) {b7=player; break;}
				}
			} else if (vpad_data.tpdata.x > 800 && vpad_data.tpdata.x < 1186) { //second column
				if (vpad_data.tpdata.y > 2418 && vpad_data.tpdata.y < 3122) { //first row
					if (b2==2) {b2=player; break;}
				} else if (vpad_data.tpdata.y > 1754 && vpad_data.tpdata.y < 2418) { //second row
					if (b5==2) {b5=player; break;}
				} else if (vpad_data.tpdata.y > 1068 && vpad_data.tpdata.y < 1754) { //third row
					if (b8==2) {b8=player; break;}
				}
			} else if (vpad_data.tpdata.x > 1186 && vpad_data.tpdata.x <= 1578) { //third column
				if (vpad_data.tpdata.y > 2418 && vpad_data.tpdata.y < 3122) { //first row
					if (b3==2) {b3=player; break;}
				} else if (vpad_data.tpdata.y > 1754 && vpad_data.tpdata.y < 2418) { //second row
					if (b6==2) {b6=player; break;}
				} else if (vpad_data.tpdata.y > 1068 && vpad_data.tpdata.y < 1754) { //third row
					if (b9==2) {b9=player; break;}
				}
			}
			unsigned int t1 = 0x500000; //wait between checks
			while(t1--) ;
		}
	}
	update_board(absu(player-1)); //update the 
	flipBuffers(); //
	unsigned int t1 = 0x5000000;
	while(t1--) ;
}
void update_board(int player) { //Updates the current TicTac Toe game with player turn (it doesn't flip buffers)
	fillScreen(0,0,0,1);
	drawString(23,0,"TicTacToe v0.1");
	drawLine(156, 105, 156, 360);// |	line
	drawLine(241, 105, 241, 360);//   | line
	drawLine(71, 190, 326, 190); // -	line
	drawLine(71, 275, 326, 275); // _	line
	if (player==0) {
		drawString(40,7,"O turn");
	} else if (player==1) {
		drawString(40,7,"X turn");
	}
	
	//TEMP CODE; TO SWITCH TO ARRAY
	if (b1==0) {drawCircle(113, 147, 20);} else if (b1==1) {drawCross(93, 127);}
	if (b2==0) {drawCircle(198, 147, 20);} else if (b2==1) {drawCross(178, 127);}
	if (b3==0) {drawCircle(283, 147, 20);} else if (b3==1) {drawCross(263, 127);}
	if (b4==0) {drawCircle(113, 232, 20);} else if (b4==1) {drawCross(93, 212);}
	if (b5==0) {drawCircle(198, 232, 20);} else if (b5==1) {drawCross(178, 212);}
	if (b6==0) {drawCircle(283, 232, 20);} else if (b6==1) {drawCross(263, 212);}
	if (b7==0) {drawCircle(113, 317, 20);} else if (b7==1) {drawCross(93, 297);}
	if (b8==0) {drawCircle(198, 317, 20);} else if (b8==1) {drawCross(178, 297);}
	if (b9==0) {drawCircle(283, 317, 20);} else if (b9==1) {drawCross(263, 297);}
}
unsigned int check_win() {
	//Check for winning combinations
	if (b1!=2 && b1==b2 && b2==b3) {if (b1 == 0) {return 0;} else if (b1 == 1) {return 1;}}
	if (b4!=2 && b4==b5 && b5==b6) {if (b4 == 0) {return 0;} else if (b4 == 1) {return 1;}}
	if (b7!=2 && b7==b8 && b8==b9) {if (b7 == 0) {return 0;} else if (b7 == 1) {return 1;}}
	if (b1!=2 && b1==b4 && b4==b7) {if (b1 == 0) {return 0;} else if (b1 == 1) {return 1;}}
	if (b2!=2 && b2==b5 && b5==b8) {if (b2 == 0) {return 0;} else if (b2 == 1) {return 1;}}
	if (b3!=2 && b3==b6 && b6==b9) {if (b3 == 0) {return 0;} else if (b3 == 1) {return 1;}}
	if (b1!=2 && b1==b5 && b5==b9) {if (b1 == 0) {return 0;} else if (b1 == 1) {return 1;}}
	if (b3!=2 && b3==b5 && b5==b7) {if (b3 == 0) {return 0;} else if (b3 == 1) {return 1;}}
	//if none is found return 2
	return 2;
}
void reset_game() {
	//set all board boxes to 2 (=empty)
	b1 = 2;
	b2 = 2;
	b3 = 2;
	b4 = 2;
	b5 = 2;
	b6 = 2;
	b7 = 2;
	b8 = 2;
	b9 = 2;
}
