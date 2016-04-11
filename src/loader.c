#include "loader.h"

void _start()
{
	/****************************>            Fix Stack            <****************************/
	//Load a good stack
	asm(
		"lis %r1, 0x1ab5 ;"
		"ori %r1, %r1, 0xd138 ;"
		);
	/****************************>           Get Handles           <****************************/
	//Get a handle to coreinit.rpl
	unsigned int coreinit_handle;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
	/****************************>       External Prototypes       <****************************/
	//OSScreen functions
	void(*OSScreenInit)();
	unsigned int(*OSScreenGetBufferSizeEx)(unsigned int bufferNum);
	unsigned int(*OSScreenSetBufferEx)(unsigned int bufferNum, void * addr);
	//OS Memory functions
	void*(*memset)(void * dest, uint32_t value, uint32_t bytes);
	void*(*OSAllocFromSystem)(uint32_t size, int align);
	void(*OSFreeToSystem)(void *ptr);
	//IM functions
	int(*IM_Open)();
	int(*IM_Close)(int fd);
	int(*IM_SetDeviceState)(int fd, void *mem, int state, int a, int b);
	/****************************>             Exports             <****************************/
	//OSScreen functions
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenInit", &OSScreenInit);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenGetBufferSizeEx", &OSScreenGetBufferSizeEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenSetBufferEx", &OSScreenSetBufferEx);
	//OS Memory functions
	OSDynLoad_FindExport(coreinit_handle, 0, "memset", &memset);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSAllocFromSystem", &OSAllocFromSystem);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSFreeToSystem", &OSFreeToSystem);
	//IM functions
	OSDynLoad_FindExport(coreinit_handle, 0, "IM_Open", &IM_Open);
	OSDynLoad_FindExport(coreinit_handle, 0, "IM_Close", &IM_Close);
	OSDynLoad_FindExport(coreinit_handle, 0, "IM_SetDeviceState", &IM_SetDeviceState);
	/****************************>          Initial Setup          <****************************/
	//Restart system to get lib access
	int fd = IM_Open();
	void *mem = OSAllocFromSystem(0x100, 64);
	memset(mem, 0, 0x100);
	//set restart flag to force quit browser
	IM_SetDeviceState(fd, mem, 3, 0, 0); 
	IM_Close(fd);
	OSFreeToSystem(mem);
	//wait a bit for browser end
	unsigned int t1 = 0x1FFFFFFF;
	while(t1--) ;
	//Call the Screen initilzation function.
	OSScreenInit();
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int buf1_size = OSScreenGetBufferSizeEx(1);
	//Set the buffer area.
	OSScreenSetBufferEx(0, (void *)0xF4000000);
	OSScreenSetBufferEx(1, (void *)0xF4000000 + buf0_size);
	//Clear both framebuffers.
	int ii = 0;
	for (ii; ii < 2; ii++)
	{
		_fillScreen(0,0,0,0);
		_flipBuffers();
	}
	//Jump to entry point.
	_entryPoint();
}
void _entryPoint()
{
	//Get a handle to coreinit.rpl
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
	_osscreeninit();
	//Export functions in main() so you won't have to export them every time you use
	OSDynLoad_Acquire("vpad.rpl", &vpadHandle);
	OSDynLoad_FindExport(vpadHandle, 0, "VPADRead", &VPADRead);
	//OSScreen Rendering Stuff (for speedy loads)
	OSDynLoad_FindExport(coreinit_handle, 0, "DCFlushRange", &DCFlushRange);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenFlipBuffersEx", &OSScreenFlipBuffersEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenGetBufferSizeEx", &OSScreenGetBufferSizeEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenClearBufferEx", &OSScreenClearBufferEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenPutPixelEx", &OSScreenPutPixelEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSAllocFromSystem", &OSAllocFromSystem);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSFreeToSystem", &OSFreeToSystem);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenPutFontEx", &OSScreenPutFontEx);
	//Game Loop
	while(1) {
		welcomescreen(); //Start screen
		mainscreen_two(); //2 Players game
	}
}
//Basic Graphs
void _osscreeninit() {
	void(*OSScreenInit)();
	unsigned int(*OSScreenGetBufferSizeEx)(unsigned int bufferNum);
	unsigned int(*OSScreenSetBufferEx)(unsigned int bufferNum, void * addr);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenInit", &OSScreenInit);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenGetBufferSizeEx", &OSScreenGetBufferSizeEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenSetBufferEx", &OSScreenSetBufferEx);
	//Call the Screen initilzation function.
	OSScreenInit();
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int buf1_size = OSScreenGetBufferSizeEx(1);
	//Set the buffer area.
	OSScreenSetBufferEx(0, (void *)0xF4000000);
	OSScreenSetBufferEx(1, (void *)0xF4000000 + buf0_size);
}
void _osscreenexit() {
	int ii=0;
	for(ii;ii<2;ii++)
	{
		_fillScreen(0,0,0,0);
		_flipBuffers();
	}
}
void ExitApplication() { //Thanks to Blackspoon (@GBATemp)
	_osscreenexit(); //clear buffers
   unsigned int coreinit;
   OSDynLoad_Acquire("coreinit.rpl", &coreinit);
   void(*_Exit)();
   OSDynLoad_FindExport(coreinit, 0, "_Exit", &_Exit);

   _Exit();
}
//Gamepad lines:
//18 lines: 0-17
void _drawString(int x, int line, char * string) { //Just draw a string
	OSScreenPutFontEx(0, x, line, string);
	OSScreenPutFontEx(1, x, line, string);
}
void _flipBuffers() {
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
void _fillScreen(char r,char g,char b,char a) {
	unsigned int num = (r << 24) | (g << 16) | (b << 8) | a;
	OSScreenClearBufferEx(0, num);
	OSScreenClearBufferEx(1, num);
}
void _drawPixel(int x, int y, char r, char g, char b, char a) {
	uint32_t num = (r << 24) | (g << 16) | (b << 8) | a;
	OSScreenPutPixelEx(0,x,y,num);
	OSScreenPutPixelEx(1,x,y,num);
} 
void _drawCircle(int x0, int y0, int radius) { //code for circles from Wikipedia
  int x = radius;
  int y = 0;
  int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0

  while( y <= x )
  {
    _drawPixel( x + x0,  y + y0, 255, 255, 255, 1); // Octant 1
    _drawPixel( y + x0,  x + y0, 255, 255, 255, 1); // Octant 2
    _drawPixel(-x + x0,  y + y0, 255, 255, 255, 1); // Octant 4
    _drawPixel(-y + x0,  x + y0, 255, 255, 255, 1); // Octant 3
    _drawPixel(-x + x0, -y + y0, 255, 255, 255, 1); // Octant 5
    _drawPixel(-y + x0, -x + y0, 255, 255, 255, 1); // Octant 6
    _drawPixel( x + x0, -y + y0, 255, 255, 255, 1); // Octant 7
    _drawPixel( y + x0, -x + y0, 255, 255, 255, 1); // Octant 8
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
void _drawRectangle(int x0, int y0, int x1, int y1) { //draw a simple Shape by 4 lines
	_drawLine(x0, y0, x0, y1);
	_drawLine(x0, y1, x1, y1);
	_drawLine(x1, y1, x1, y0);
	_drawLine(x1, y0, x0, y0);
}
void _drawLine(int x0, int y0, int x1, int y1) {
 
  int dx = absu(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = absu(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  for(;;){
    _drawPixel(x0,y0, 255, 255, 255, 1);
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}
void _drawCross(int x, int y) { //Draw a 40x40 px cross for
	_drawLine(x, y, x+40, y+40);
	_drawLine(x, y+40, x+40, y);
}
unsigned int absu(int num) { //abs() clone for wii u
	if (num < 0) {
		return -num;
	}
}
//Game Graphs
void welcomescreen() {
	_fillScreen(0,0,0,1);
	_drawString(23,0,"TicTacToe v0.1");
	_drawString(22,2,"Press A or Play");
	_drawRectangle(350, 240, 450, 280);
	//_drawRectangle(275, 300, 475, 340);
	_drawString(27,9,"Play");
	//_drawString(20,11,"Play with Computer");
	_flipBuffers();
	while(1) {
		VPADRead(0, &vpad_data, 1, &error); //Read the vpaddata
		if (vpad_data.btn_hold & BUTTON_HOME) {
			ExitApplication();
		}
		if (vpad_data.btn_hold & BUTTON_A) {
			break;
		}
		if (vpad_data.tpdata.touched == 1)
		{
			/*
			_fillScreen(0,0,0,1);
			_drawRectangle(275, 300, 475, 340);
			_drawString(20,11,"Play with Computer");
			char tb[35];
			__os_snprintf(tb, 35, "%i,%i", vpad_data.tpdata.x, vpad_data.tpdata.y);
			_drawString(10,15,tb);
			*/
			if (vpad_data.tpdata.x >= 1664 && vpad_data.tpdata.x <= 2152 && vpad_data.tpdata.y >= 1718 && vpad_data.tpdata.y <= 2014) {
				break;
			}
			//_flipBuffers();
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
	_flipBuffers();
	int round = 0;
	while(1) {
		if (turn == 1) {
			player_turn(1);
			turn = 0;
		} else {
			player_turn(0);
			turn = 1;
		}
		unsigned int chkwin = check_win();
		if (chkwin != 2) {
			if (chkwin == 1) {
				update_board(2);
				_drawString(40,9,"X Won");
				_flipBuffers();
				break;
			} else if (chkwin == 0) {
				update_board(2);
				_drawString(40,9,"O Won");
				_flipBuffers();
				break;
			}
		}
		round++;
		if (round == 9){
			update_board(2);
			_drawString(40,9,"Nobody Won");
			_flipBuffers();
			break;
		}
	}
	unsigned int t1 = 0x40000000;
	while(t1--) ;
}
void player_turn(int player){ //require input from human player(s)
	while(1) {
		VPADRead(0, &vpad_data, 1, &error); //Read the vpaddata
		
		//check home button
		if (vpad_data.btn_hold & BUTTON_HOME) { //check if we should exit from application
			ExitApplication();
		}
		
		//touch code
		if (vpad_data.tpdata.touched == 1) {
			if (vpad_data.tpdata.x > 422 && vpad_data.tpdata.x < 800) { //first column
				if (vpad_data.tpdata.y > 2418 && vpad_data.tpdata.y < 3122) { //first row
					if (board1==2) {board1=player; break;}
				} else if (vpad_data.tpdata.y > 1754 && vpad_data.tpdata.y < 2418) { //second row
					if (board4==2) {board4=player; break;}
				} else if (vpad_data.tpdata.y > 1068 && vpad_data.tpdata.y < 1754) { //third row
					if (board7==2) {board7=player; break;}
				}
			} else if (vpad_data.tpdata.x > 800 && vpad_data.tpdata.x < 1186) { //second column
				if (vpad_data.tpdata.y > 2418 && vpad_data.tpdata.y < 3122) { //first row
					if (board2==2) {board2=player; break;}
				} else if (vpad_data.tpdata.y > 1754 && vpad_data.tpdata.y < 2418) { //second row
					if (board5==2) {board5=player; break;}
				} else if (vpad_data.tpdata.y > 1068 && vpad_data.tpdata.y < 1754) { //third row
					if (board8==2) {board8=player; break;}
				}
			} else if (vpad_data.tpdata.x > 1186 && vpad_data.tpdata.x <= 1578) { //third column
				if (vpad_data.tpdata.y > 2418 && vpad_data.tpdata.y < 3122) { //first row
					if (board3==2) {board3=player; break;}
				} else if (vpad_data.tpdata.y > 1754 && vpad_data.tpdata.y < 2418) { //second row
					if (board6==2) {board6=player; break;}
				} else if (vpad_data.tpdata.y > 1068 && vpad_data.tpdata.y < 1754) { //third row
					if (board9==2) {board9=player; break;}
				}
			}
			unsigned int t1 = 0x500000; //wait between checks
			while(t1--) ;
		}
	}
	update_board(absu(player-1)); //update the 
	_flipBuffers(); //
	unsigned int t1 = 0x5000000;
	while(t1--) ;
}
void update_board(int player) { //Updates the current TicTac Toe game with player turn (it doesn't flip buffers)
	_fillScreen(0,0,0,1);
	_drawString(23,0,"TicTacToe v0.1");
	_drawLine(156, 105, 156, 360);// |	line
	_drawLine(241, 105, 241, 360);//   | line
	_drawLine(71, 190, 326, 190); // -	line
	_drawLine(71, 275, 326, 275); // _	line
	if (player==0) {
		_drawString(40,7,"O turn");
	} else if (player==1) {
		_drawString(40,7,"X turn");
	}
	
	//TEMP CODE; TO SWITCH TO ARRAY
	if (board1==0) {_drawCircle(113, 147, 20);} else if (board1==1) {_drawCross(93, 127);}
	if (board2==0) {_drawCircle(198, 147, 20);} else if (board2==1) {_drawCross(178, 127);}
	if (board3==0) {_drawCircle(283, 147, 20);} else if (board3==1) {_drawCross(263, 127);}
	if (board4==0) {_drawCircle(113, 232, 20);} else if (board4==1) {_drawCross(93, 212);}
	if (board5==0) {_drawCircle(198, 232, 20);} else if (board5==1) {_drawCross(178, 212);}
	if (board6==0) {_drawCircle(283, 232, 20);} else if (board6==1) {_drawCross(263, 212);}
	if (board7==0) {_drawCircle(113, 317, 20);} else if (board7==1) {_drawCross(93, 297);}
	if (board8==0) {_drawCircle(198, 317, 20);} else if (board8==1) {_drawCross(178, 297);}
	if (board9==0) {_drawCircle(283, 317, 20);} else if (board9==1) {_drawCross(263, 297);}
}
unsigned int check_win() {
	//Check for winning combinations
	if (board1!=2 && board1==board2 && board2==board3) {if (board1 == 0) {return 0;} else if (board1 == 1) {return 1;}}
	if (board4!=2 && board4==board5 && board5==board6) {if (board4 == 0) {return 0;} else if (board4 == 1) {return 1;}}
	if (board7!=2 && board7==board8 && board8==board9) {if (board7 == 0) {return 0;} else if (board7 == 1) {return 1;}}
	if (board1!=2 && board1==board4 && board4==board7) {if (board1 == 0) {return 0;} else if (board1 == 1) {return 1;}}
	if (board2!=2 && board2==board5 && board5==board8) {if (board2 == 0) {return 0;} else if (board2 == 1) {return 1;}}
	if (board3!=2 && board3==board6 && board6==board9) {if (board3 == 0) {return 0;} else if (board3 == 1) {return 1;}}
	if (board1!=2 && board1==board5 && board5==board9) {if (board1 == 0) {return 0;} else if (board1 == 1) {return 1;}}
	if (board3!=2 && board3==board5 && board5==board7) {if (board3 == 0) {return 0;} else if (board3 == 1) {return 1;}}
	//if none is found return 2
	return 2;
}
void reset_game() {
	//set all board boxes to 2 (=empty)
	board1 = 2;
	board2 = 2;
	board3 = 2;
	board4 = 2;
	board5 = 2;
	board6 = 2;
	board7 = 2;
	board8 = 2;
	board9 = 2;
}
