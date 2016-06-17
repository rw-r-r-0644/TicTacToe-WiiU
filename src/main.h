#ifndef MAIN_H
#define MAIN_H

//Prototypes

int _main();
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

#endif /* LOADER_H */