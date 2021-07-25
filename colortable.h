/* Color table definition file for robotfindskitten for PSP.
   Trevor Wilson (jiji@namakoteam.com)
*/

#ifndef __colortable_h
#define __colortable_h

#include "pg.h"

#define V1	11
#define V2	20
#define V3	31

#define BLACK 		0
#define BLUE		1
#define	GREEN		2
#define	CYAN		3
#define	RED			4
#define	MAGENTA		5
#define	BROWN		6
#define	LIGHTGRAY	7
#define DARKGRAY	8
#define	LIGHTBLUE	9
#define	LIGHTGREEN	10
#define	LIGHTCYAN	11
#define	LIGHTRED	12
#define	LTMAGENTA	13
#define	YELLOW		14
#define	WHITE		15


const unsigned short colorTable[] = {
	RGB( 0, 0, 0 ),    // 0 black
	RGB( 0, 0, V2 ),   // 1 blue
	RGB( 0, V2, 0 ),   // 2 green
	RGB( 0, V2, V2 ),  // 3 cyan
	RGB( V2, 0, 0 ),   // 4 red
	RGB( V2, 0, V2 ),  // 5 magenta
	RGB( V2, V1, 0 ),  // 6 brown (orange)
	RGB( V2, V2, V2 ), // 7 white (light gray) 
	RGB( V1, V1, V1 ), // 8 (dark) gray
	RGB( V1, V1, V3 ), // 9 bright blue
	RGB( V1, V3, V1 ), // 10 bright green
	RGB( V1, V3, V3 ), // 11 bright cyan
	RGB( V3, V1, V1 ), // 12 bright red
	RGB( V3, V1, V3 ), // 13 bright magenta
	RGB( V3, V3, V1 ), // 14 yellow
	RGB( V3, V3, V3 ), // 15 bright white 
};

#endif

