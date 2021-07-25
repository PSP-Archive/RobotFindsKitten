/*robotfindskitten for PSP v1.0 source code.
 *
 *Copyright (C) 1997,1999 Leonard Richardson leonardr@segfault.org
 *
 *PSP port (C) 2006 Trevor Wilson jiji@namakoteam.com
*/


#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdio.h>
#include "pg.h"
#include "colortable.h"
#include "messages.h"

PSP_MODULE_INFO("ROBOTFINDSKITTEN", 0, 1, 1);

PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#define EMPTY -1
#define ROBOT 0
#define KITTEN 1
#define MESSAGES 252
#define NUM_BOGUS 20
#define SCREEN_ROWS	31
#define SCREEN_COLS	60
#define INPUTFRAMES	4
#define SCREEN_W 480
#define SCREEN_H 272
#define VERT_OFFSET 1
#define TILE_W 8
#define TILE_H 8



void instructions();
void draw_robot();
void draw_kitten();
void draw_bogusobjects();
void draw_gamescreen();
void foundCinema();
void process_input();
void process_game();
void gameInit();
void objectsInit();
void waitForX();
void showMessage(char *message);
void gradientBox( int x0, int y0, int x1, int y1 );
void drawBigRobot(int x, int y);
void drawBigHeart(int x, int y);
void drawBigKitten(int x, int y);

int kittenFound = 0;
int done = 0;
int frameCount = 0;
char screenMessage[2][SCREEN_COLS];


// Exit callback 
int exit_callback(void)
{
	sceKernelExitGame();
	done = 1;

	return 0;
}

// Callback thread 
void CallbackThread(void *arg)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", (void*)exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();
}

// Sets up the callback thread and returns its thread id 
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", (void*)CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}


typedef struct tag_object
{
	int x;
	int y;
	int color;
	char character;
	int moveProgress;
} object;

object robot;
object kitten;
object empty;
object bogus[MESSAGES];
int bogus_messages[MESSAGES];

int used_messages[MESSAGES];
int screen[SCREEN_COLS][SCREEN_ROWS];



void gameInit()
{

	
	//Do general start-of-program stuff.
	srand(sceKernelLibcTime(NULL));
	pgInit();



}

void objectsInit()
{
	int counter, counter2, x, y;
	
	empty.x = -1;
	empty.y = -1;
	empty.color = 0;
	empty.character = ' ';

	//
	//Now we have the filling in of the various arrays.
	//

	//Create an array to represent the screen so that we can make sure all
	//the objects go on different squares.
	for (counter = 0; counter <= SCREEN_COLS; counter++)
	{
		for (counter2 = 0; counter2 <= SCREEN_ROWS; counter2++)
		{
			screen[counter][counter2] = EMPTY;
		}
	}
	//Create an array to ensure we don't get duplicate messages.
	for (counter = 0; counter < MESSAGES; counter++)
	{
		used_messages[counter] = 0;
		bogus_messages[counter] = 0;
		bogus[counter] = empty;
	}
	//
	//Now we initialize the various game objects.
	//
	//Assign a position to the player.
	robot.x = rand() % SCREEN_COLS;
	robot.y = rand() % SCREEN_ROWS;
	robot.character = '#';
	robot.color = WHITE;
	screen[robot.x][robot.y] = ROBOT;

	//Assign the kitten a unique position.
	do
	{
		kitten.x = rand() % SCREEN_COLS;
		kitten.y = rand() % SCREEN_ROWS;
	} while (screen[kitten.x][kitten.y] != EMPTY);

	//Assign the kitten a character and a color.
	do
	{
		kitten.character = (rand() % 63) + 33;
	} while ( (kitten.character == '#') || (kitten.character == ' ') || (kitten.character == 7 ));
	kitten.color = (rand() % 15) + 1;
	screen[kitten.x][kitten.y] = KITTEN;

	//Now, initialize non-kitten objects.
	for (counter = 0; counter < NUM_BOGUS; counter++)
	{
		//Assign a unique position.
		do
		{
			bogus[counter].x = rand() % SCREEN_COLS;
			bogus[counter].y = rand() % SCREEN_ROWS;
		} while ( (screen[bogus[counter].x][bogus[counter].y] != EMPTY) && ( ( bogus[counter].x != 0 ) && ( bogus[counter].y != 0 ) ) );
		screen[bogus[counter].x][bogus[counter].y] = counter+2;

		//Assign a character.
		do
		{
			bogus[counter].character = (rand() % 64) + 33;
		} while ( (bogus[counter].character == '#') || (bogus[counter].character == ' ') || (bogus[counter].character == 7));
		bogus[counter].color = (rand() % 15) + 1;

		//Assign a unique message.
		int index = 0;
		do
		{
			index = rand() % MESSAGES;
		} while (used_messages[index] != 0);
		bogus_messages[counter] = index;
		used_messages[index] = 1;
	}
	
	
	// Clear out the messages array.
	for ( y = 0; y < 2; y++ )
		for ( x = 0; x < SCREEN_COLS; x++ )
			screenMessage[y][x] = ' ';
}	

void process_game()
{
	frameCount++;
	if ( frameCount == INPUTFRAMES )
	{
		// Process input every few frames, so that the player doesn't race across the board instantly.
		process_input();
		frameCount = 0;
	}
}


//Given the keyboard input, interprets it.
void process_input()
{
	int check_x = robot.x;
	int check_y = robot.y;
	int idx;
	SceCtrlData ctrl;
	sceCtrlReadBufferPositive(&ctrl, 1); 
	if (ctrl.Buttons & PSP_CTRL_UP) { 
		check_y--;
	} else if (ctrl.Buttons & PSP_CTRL_DOWN) { 
		check_y++;
	} else if (ctrl.Buttons & PSP_CTRL_LEFT) { 
		check_x--;
	} else if (ctrl.Buttons & PSP_CTRL_RIGHT) { 
		check_x++;
	} 

	//Check for going off the edge of the screen.
	if (check_y < 0 || check_y >= SCREEN_ROWS || check_x < 0 || check_x >= SCREEN_COLS )
	{
		return;
	}

	//Check for collision
	if (screen[check_x][check_y] != EMPTY)
	{
		switch (screen[check_x][check_y])
		{
			case ROBOT:
				//We didn't move.
				break;
			
			case KITTEN: //Found it!
				kittenFound = 1;
				break;

			default:
				// TODO: write message to screen and wait for input
				idx = bogus_messages[screen[check_x][check_y]-2];
				showMessage(messages[idx]);
				break;
		}
		
		return;
	}
	//Otherwise, move the robot.
	robot.x = check_x;
	robot.y = check_y;
}

// Draw a big ASCII robot.
void drawBigRobot(int x, int y)
{
	int color;
	color = colorTable[BLUE];
	pgPrint(x, y, color, "[ ]");
	pgPrint(x, y+1, color, "( )");
	pgPrint(x, y+2, color, "| |");
	
	color = colorTable[CYAN];
	pgPrint(x+3, y+1, color, "=C");
	
	color = colorTable[LIGHTGRAY];
	pgPrint(x, y+3, color, "OOO");
	
	color = colorTable[GREEN];
	pgPrint(x+1, y, color, "-");
	
	color = colorTable[RED];
	pgPrint(x+1, y+1, color, "+");
}

// Draw a big ASCII heart.
void drawBigHeart(int x, int y)
{
	int color;
	color = colorTable[LIGHTRED];
	pgPrint(x, y, color,     ".::. . :.");
	pgPrint(x, y+1, color,   "::::::: :");
	pgPrint(x, y+2, color,   "`::::: :'");
	pgPrint(x+2, y+3, color,   "`:: '");
	
	color = colorTable[RED];
	pgPrint(x+6, y, color, ":");
	pgPrint(x+7, y+1, color, ":");
	pgPrint(x+6, y+2, color, ":");
	pgPrint(x+5, y+3, color, ":");
}
	
// Draw a big ASCII kitten.
void drawBigKitten(int x, int y)
{
	int color;
	color = colorTable[BROWN];
	pgPrint(x+1, y, color,   "|\\_/|");
	pgPrint(x+1, y+1, color, "|   |__");
	pgPrint(x+6, y+2, color,      "__\\");
	pgPrint(x, y+3, color,  "c_c__(___)");
	
	color = colorTable[LIGHTGREEN];
	pgPrint(x+2, y+1, color, "o o");
	
	color = colorTable[WHITE];
	pgPrint(x+1, y+2, color, "=- -=");
	
	color = colorTable[LIGHTRED];
	pgPrint(x+3, y+2, color, "*");
}

void instructions()
{
	int y = 1;
	
	pgPrint(21, y++, 0xFFFF, "robotfindskitten");
	y++;
	pgPrint(18, y++, 0xFFFF, "v72617319.4p (for PSP)");
	y+=2;
	pgPrint(6, y++, 0xFFFF, "By the illustrious Leonard Richardson (C) 1997");
	pgPrint(6, y++, 0xFFFF, "PSP port by the amazing Trevor Wilson (C) 2006");
	pgPrint(6, y++, 0xFFFF, "                         (jiji@namakoteam.com)");
	
	drawBigRobot(6, 11);
	drawBigHeart(25, 11);
	drawBigKitten(47, 11);
	
	y += 9;
	pgPrint(3, y++, 0xFFFF, "In this game, you are robot (#). Your job is to find");
	pgPrint(3, y++, 0xFFFF, "kitten. This task is complicated by the existence of");
	pgPrint(3, y++, 0xFFFF, "various things which are not kitten. Robot must touch");
	pgPrint(2, y++, 0xFFFF, "items to determine if they are kitten or not.  The game");
	pgPrint(0, y++, 0xFFFF, "ends when robotfindskitten. Alternatively, you may end the");
	pgPrint(13, y++, 0xFFFF, "game by pressing the Home button.");
	y+= 2;

	pgPrint(15, y++, 0xFFFF, "Press the X button to start.");
	pgPrint(17, y + 3, 0xFFFF, "www.robotfindskitten.org");
	
	pgScreenFlipV();
	
	waitForX();	

}

// Display "robot kinds kitten" cinematic.
void foundCinema()
{
	int robotX, kittenX, i;
	
	// Clear screen.	
	
	
	robotX = 1;
	kittenX = 57;
	robot.y = 13;
	kitten.y = 13;
	
	while ( robotX < 29 )
	{
		pgFillvram(0x0);
		robot.x = robotX;
		kitten.x = kittenX;
		draw_robot();
		draw_kitten();
		
		robotX += 2;
		kittenX -= 2;
		
		pgScreenFlipV();
		for ( i = 0; i < 5; i++ )
			pgWaitV();
	}
	
	pgFillvram(0x0);
	draw_robot();
	draw_kitten();
	
	drawBigHeart(25, 9);
	
	pgPrint(21, 6, 0xFFFF, "You found kitten!");
	pgPrint(21, 17, 0xFFFF, "Way to go, robot!");
	
	pgPrint(20, 22, 0xFFFF, "Press X to continue");	
	
	pgScreenFlipV();
	
	waitForX();

}

void draw_robot() //Draws robot at current position
{
	char out[2];
	sprintf(out, "%c", robot.character);
	pgPrint(robot.x, robot.y + VERT_OFFSET, colorTable[robot.color], out);
}

void draw_kitten() //Draws kitten at current position
{
	char out[2];
	sprintf(out, "%c", kitten.character);
	pgPrint(kitten.x, kitten.y + VERT_OFFSET, colorTable[kitten.color], out);
}

void draw_bogusobjects() // draws bogus objects
{
	int counter;
	char out[2];
	for (counter = 0; counter < NUM_BOGUS; counter++)
	{
		sprintf(out, "%c", bogus[counter].character);
		pgPrint(bogus[counter].x, bogus[counter].y + VERT_OFFSET, colorTable[bogus[counter].color], out);
	}
}

#define TITLEBAR_Y	1
#define	FONT_SIZE	16

void draw_gamescreen()
{
	// Clear screen.
	pgFillvram(0x0);
	
	// Make title bar.
	gradientBox( 0, TITLEBAR_Y - 1, SCREEN_W, TITLEBAR_Y - 1 + TILE_H );
	pgPrint(0, TITLEBAR_Y - 1, 0xFFFF, "robotfindskitten for PSP.");
	
	// Draw message box, and display anythign that might be in the message buffer.
	gradientBox( 0, (SCREEN_H - ( 2 * TILE_H ) ), SCREEN_W, SCREEN_H );
	pgPrint(0, 32, 0xFFFF, screenMessage[0]);
	pgPrint(0, 33, 0xFFFF, screenMessage[1]);
	
	// Draw objects.
	draw_robot();
	draw_kitten();	
	draw_bogusobjects();
}

// Draws a rectangular box from top to bottom, starting bright blue and fading as it goes.

void gradientBox( int x0, int y0, int x1, int y1 )
{
	int dl = 2,
		dcolor,
		color = 31,
		y;
	
	dcolor = 32 / (y1 - y0);
	dl = 2;
		
	for ( y = y0; y < y1; y++ )
	{
		pgPutLine(x0, y, x1, y, RGB(0, 0, color));
		dl--;
		if ( dl <= 0 )
		{
			color -= dcolor;
			dl = 2;
		}
	}
}

// Fills the message buffer with the given string.  Adds linebreaks where necessary.

void showMessage(char *message)
{
	int rows, 
		x, 
		y, 
		length,
		col,
		row,
		idx, 
		lookahead,
		columnCount,
		linebreakIdx;
	
	int linebreaks[4] = {0, 0, 0, 0};
	
	
	draw_gamescreen();
	length = strlen(message);
	
	linebreakIdx = 0;
	idx = 0;
	rows = 0;
	columnCount = 0;

	// Clear out the message buffer.
	for ( y = 0; y < 2; y++ )
		for ( x = 0; x < SCREEN_COLS; x++ )
			screenMessage[y][x] = ' ';
	
	// Iterate through the message string.
	while ( idx < length )
	{
		lookahead = idx;
		// Look ahead in the string to find the end of the next word (or the end of the string). 
		do
		{
			lookahead++;
			columnCount++;
		} while ( ( message[lookahead] != ' ' ) && ( lookahead < length ) );
		
		// If the next word will spill past the end of the line, define a linebreak.
		if ( columnCount > SCREEN_COLS - 1 )
		{
			linebreaks[linebreakIdx] = idx - 1;
			columnCount = (lookahead - idx);
			linebreakIdx++;
			rows++;
		}
		idx = lookahead;
	}
	
	row = 0;
	col = 0;
	linebreakIdx = 0;	
	idx = 0;
	
	// Iterate through the message string again.
	while ( idx < length )
	{
		// Copy a character from the message string into the message buffer.
		screenMessage[row][col] = message[idx];
		col += 1;

		// If a linebreak is located, move the cursor down a row and back to the beginning of the line.
		if ( ( rows > 0 ) && ( linebreakIdx < rows ) && ( idx == linebreaks[linebreakIdx] ) )
		{
			col = 0;
			row += 1;
			linebreakIdx++;
			idx++;
		}
		idx++;
	}	

}

// Wait for the user to press the X button.

void waitForX()
{
	int x_hit = 0;
	SceCtrlData ctrl;
	while ( !x_hit )
	{
		sceCtrlReadBufferPositive(&ctrl, 1); 
		if (ctrl.Buttons & PSP_CTRL_CROSS) { 
			x_hit = 1;
		}
	}
}


int main()
{
	SetupCallbacks();
	gameInit();
	pgScreenFrame(2,0);
	


	//Print instructions.
	instructions();
	
	
	int old_x = robot.x;
	int old_y = robot.y;

	//
	// main game loop
	//
	while ( !done )
	{
		objectsInit();
		while ( !kittenFound && !done )
		{
			process_game();
			
			if (!(old_x == robot.x && old_y == robot.y))
			{
				screen[old_x][old_y] = EMPTY;
				screen[robot.x][robot.y] = ROBOT;
				old_x = robot.x;
				old_y = robot.y;
			}
			draw_gamescreen();
			pgScreenFlipV();
		}
		foundCinema();
		kittenFound = 0;
	}
	sceKernelSleepThread();
	return 0;
}

