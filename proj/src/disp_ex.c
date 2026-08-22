//
//	disp_ex.c
//

#include	<stdio.h>
#include	<iocslib.h>
#include    "_debug_.h"
#include	"disp.h"

void vwait(int);
void set_exdisp(int);
void set_console(int, int);
void ex_scroll(int);

//
// set exdisp
//
void set_exdisp(int ypos)
{
	volatile unsigned short *TEXTSCROLL_XPOS;
	volatile unsigned short *TEXTSCROLL_YPOS;

	TEXTSCROLL_YPOS = (void *)0xE80016;

	vwait(1);
	*TEXTSCROLL_YPOS = ypos;
}

//
// set console
//
void set_console(int xpos, int ypos)
{
	int XS, YS;
	int XL, YL;

	XS = (xpos/2)*16;
	YS = (ypos*4)*4;
	XL = -1;
	YL = -1;
	B_CONSOL( XS, YS, XL, YL );
}

#define EX_YPOS (SCREEN_VWIDTH*16)
//
// ex-scroll
//
void ex_scroll(int sw)
{
	int a = 100;
    int b = (a*a)/(2*EX_YPOS)+2;
    int y = 0;

    if(b < 1) b = 1;

    while(y < EX_YPOS) {
        y += a;
        a -= b;
        if(a <= 0) a = 1;
        if(y > EX_YPOS) y = EX_YPOS;
        set_exdisp(!sw ? y : EX_YPOS-y);
    }
}

//
// vwait
//
void vwait(int loop_count)
{
	volatile unsigned short *VSTAT;

	VSTAT = (void *)0xE88000;

	while(loop_count-- > 0) {
	  while( 0 == ((*VSTAT) & 0b00010000));
	  while( 0 != ((*VSTAT) & 0b00010000));
	}
}

