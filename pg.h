#ifndef __pg_h
#define __pg_h

#define		RGB(r, g, b) 	((b<<10)+(g<<5)+(r<<0)+0x8000)

// primitive graphics for Hello World PSP
// originally by nem
// modified by Trevor Wilson

void pgInit();
void pgWaitV();
void pgWaitVn(unsigned long count);
void pgScreenFrame(long mode,long frame);
void pgScreenFlip();
void pgScreenFlipV();
void pgPrint(unsigned long x,unsigned long y,unsigned long color,const char *str);
void pgPrint2(unsigned long x,unsigned long y,unsigned long color,const char *str);
void pgPrint4(unsigned long x,unsigned long y,unsigned long color,const char *str);
void pgFillvram(unsigned long color);
void pgBitBlt(unsigned long x,unsigned long y,unsigned long w,unsigned long h,unsigned long mag,const unsigned short *d);
void pgSpriteBlt(unsigned long x,unsigned long y, unsigned long w, unsigned long mag, const unsigned short *d);
void pgPutChar(unsigned long x,unsigned long y,unsigned long color,unsigned long bgcolor,unsigned char ch,char drawfg,char drawbg,char mag);
void pgPutPoint(unsigned long x, unsigned long y, unsigned long color);
void pgPutLine(unsigned long x0, unsigned long y0, unsigned long x1, unsigned long y1, unsigned long color);
void pgDrawMapTile(unsigned long x, unsigned long y, const unsigned short *d);
void pgDrawLevelScreen();
void pgLevelScreenInit();



unsigned long pgRGB(unsigned long r, unsigned long g, unsigned long b);
int abs(int value);
void swap(int v1, int v2);


#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 272

#endif
