/*
 *	cursor.c
 */


#include	<stdio.h>
#include    "_debug_.h"
#include	"oe.h"

extern int	ln;

void	cmove(int);

/*****

	カーソル制御

******/
void cmove(int d)
{
	switch( d ) {
		case CURUP:
			if( ln <= 13 ) {
				ln += 50;
				if( ln > 60 ) ln = 60;
				break;
			}
			if( ln < 26 ) {
				ln -= 14;
				break;
			}
			ln -= 12;               // 12:sizeof(OPMREG)
			if( ln < 0 ) ln = 0;
			break;

		case CURDOWN:
			if( ln >= 50 ) {
				ln -= 50;
				break;
			}
			if( ln <= 13 ) {
				ln += 14;
				if( ln > 24 ) ln = 24;
				break;
			}
			ln += 12;
			if( ln > 60 ) ln = 60;
			break;

		case CURRIGHT:
			if( ln <= 13 ) {
				ln++;
				if( ln > 12 ) ln = 0;
				break;
			}
			ln++;
			if( (ln+11)%12 == 0 ) ln -= 11;
			break;

		case CURLEFT:
			if( ln <= 13 ) {
				ln--;
				if( ln < 0 ) ln = 12;
				break;
			}
			ln--;
			if( (ln+11)%12 == 0 ) ln += 11;
			break;
	}
}
