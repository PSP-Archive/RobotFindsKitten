// primitive graphics for Hello World sce
// originally by nem
// modified by Trevor Wilson


#include "pg.h"


#include <pspdisplay.h>
#include <pspdebug.h>

extern const unsigned char font[];

//constants
#define		PIXELSIZE	1				//in short
#define		LINESIZE	512				//in short
#define		FRAMESIZE	0x44000			//in byte

//480*272 = 60*38
#define CMAX_X 60
#define CMAX_Y 38
#define CMAX2_X 30
#define CMAX2_Y 19
#define CMAX4_X 15
#define CMAX4_Y 9

#define TILE_W	16
#define TILE_H	16
#define	SPRITE_W	16
#define SPRITE_H	16
#define	TRANSCOLOR	0x4200


//variables
char *pg_vramtop=(char *)0x04000000;
long pg_screenmode;
long pg_showframe;
long pg_drawframe;



void pgWaitVn(unsigned long count)
{
	for (; count>0; --count) {
		sceDisplayWaitVblankStart();
	}
}


void pgWaitV()
{
	sceDisplayWaitVblankStart();
}


unsigned char *pgGetVramAddr(unsigned long x,unsigned long y)
{
	return (unsigned char* )pg_vramtop+(pg_drawframe?FRAMESIZE:0)+x*PIXELSIZE*2+y*LINESIZE*2+0x40000000;
}


void pgInit()
{
	sceDisplaySetMode(0,SCREEN_WIDTH,SCREEN_HEIGHT);
	pgScreenFrame(0,0);
}

void pgPutPoint(unsigned long x, unsigned long y, unsigned long color)
{
	unsigned char *vptr;		//pointer to vram
	vptr=pgGetVramAddr(x,y);
	*(unsigned short *)vptr=color;
}

int abs(int value)
{
	return value > 0 ? value : -1 * value;
}
		
void swap(int v1, int v2)
{
	int t = v1;
	v1 = v2;
	v2 = t;
}

void pgPutLine(unsigned long x0, unsigned long y0, unsigned long x1, unsigned long y1, unsigned long color)
{
	int steep = abs(y1 - y0) > abs(x1 - x0);
	if ( steep )
	{
		swap(x0, y0);
		swap(x1, y1);
	}
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int err = 0;
	int deltaerr = dy;
	int x = x0;
	int y = y0;
	int xstep;
	int ystep;
	if ( x0 < x1 )
		xstep = 1;
	else
		xstep = -1;
	if ( y0 < y1 )
		ystep = 1;
	else
		ystep = -1;
		
	if ( steep )
		pgPutPoint(y, x, color);
	else
		pgPutPoint(x, y, color);
		
	while ( x != x1 )
	{
		x += xstep;
		err += deltaerr;
		if ( ( 2 * err ) >= dx )
		{
			y += ystep;
			err -= dx;
		}
		if ( steep )
			pgPutPoint(y, x, color);
		else
			pgPutPoint(x, y, color);
	}
}		
		
unsigned long pgRGB(unsigned long r, unsigned long g, unsigned long b)
{
	 return (b<<10)+(g<<5)+(r<<0)+0x8000;
}




void pgPrint(unsigned long x,unsigned long y,unsigned long color,const char *str)
{
	while (*str!=0 && x<CMAX_X && y<CMAX_Y) {
		pgPutChar(x*8,y*8,color,0,*str,1,0,1);
		str++;
		x++;
		if (x>=CMAX_X) {
			x=0;
			y++;
		}
	}
}


void pgPrint2(unsigned long x,unsigned long y,unsigned long color,const char *str)
{
	while (*str!=0 && x<CMAX2_X && y<CMAX2_Y) {
		pgPutChar(x*16,y*16,color,0,*str,1,0,2);
		str++;
		x++;
		if (x>=CMAX2_X) {
			x=0;
			y++;
		}
	}
}


void pgPrint4(unsigned long x,unsigned long y,unsigned long color,const char *str)
{
	while (*str!=0 && x<CMAX4_X && y<CMAX4_Y) {
		pgPutChar(x*32,y*32,color,0,*str,1,0,4);
		str++;
		x++;
		if (x>=CMAX4_X) {
			x=0;
			y++;
		}
	}
}


void pgFillvram(unsigned long color)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned long i;

	vptr0=pgGetVramAddr(0,0);
	for (i=0; i<FRAMESIZE/2; i++) {
		*(unsigned short *)vptr0=color;
		vptr0+=PIXELSIZE*2;
	}
}

void pgBitBlt(unsigned long x,unsigned long y,unsigned long w,unsigned long h,unsigned long mag,const unsigned short *d)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned char *vptr;		//pointer to vram
	unsigned long xx,yy,mx,my;
	const unsigned short *dd;
	
	vptr0=pgGetVramAddr(x,y);
	for (yy=0; yy<h; yy++) {
		for (my=0; my<mag; my++) {
			vptr=vptr0;
			dd=d;
			for (xx=0; xx<w; xx++) {
				for (mx=0; mx<mag; mx++) {
					*(unsigned short *)vptr=*dd;
					vptr+=PIXELSIZE*2;
				}
				dd++;
			}
			vptr0+=LINESIZE*2;
		}
		d+=w;
	}
	
}

void pgSpriteBlt(unsigned long x,unsigned long y, unsigned long w, unsigned long mag, const unsigned short *d)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned char *vptr;		//pointer to vram
	unsigned long xx,yy,mx,my;
	const unsigned short *dd;
	
	vptr0=pgGetVramAddr(x,y);
	for (yy=0; yy<SPRITE_H; yy++) {
		for (my=0; my<mag; my++) {
			vptr=vptr0;
			dd=d;
			for (xx=0; xx < w; xx++) {
				for (mx=0; mx<mag; mx++) {
					if ( *dd != TRANSCOLOR )
						*(unsigned short *)vptr=*dd;
					vptr+=PIXELSIZE*2;
				}
				dd++;
			}
			vptr0+=LINESIZE*2;
		}
		d+=SPRITE_W;
	}
	
}

void pgPutChar(unsigned long x,unsigned long y,unsigned long color,unsigned long bgcolor,unsigned char ch,char drawfg,char drawbg,char mag)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned char *vptr;		//pointer to vram
	const unsigned char *cfont;		//pointer to font
	unsigned long cx,cy;
	unsigned long b;
	char mx,my;

	//if (ch>255) return;
	cfont=font+ch*8;
	vptr0=pgGetVramAddr(x,y);
	for (cy=0; cy<8; cy++) {
		for (my=0; my<mag; my++) {
			vptr=vptr0;
			b=0x80;
			for (cx=0; cx<8; cx++) {
				for (mx=0; mx<mag; mx++) {
					if ((*cfont&b)!=0) {
						if (drawfg) *(unsigned short *)vptr=color;
					} else {
						if (drawbg) *(unsigned short *)vptr=bgcolor;
					}
					vptr+=PIXELSIZE*2;
				}
				b=b>>1;
			}
			vptr0+=LINESIZE*2;
		}
		cfont++;
	}
}


void pgScreenFrame(long mode,long frame)
{
	pg_screenmode=mode;
	frame=(frame?1:0);
	pg_showframe=frame;
	if (mode==0) {
		//screen off
		pg_drawframe=frame;
		sceDisplaySetFrameBuf(0,0,0,1);
	} else if (mode==1) {
		//show/draw same
		pg_drawframe=frame;
		sceDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,1);
	} else if (mode==2) {
		//show/draw different
		pg_drawframe=(frame?0:1);
		sceDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,1);
	}
}


void pgScreenFlip()
{
	pg_showframe=(pg_showframe?0:1);
	pg_drawframe=(pg_drawframe?0:1);
	sceDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,0);
}


void pgScreenFlipV()
{
	pgWaitV();
	pgScreenFlip();
}
