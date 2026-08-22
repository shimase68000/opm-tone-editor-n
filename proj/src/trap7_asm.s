;
; trap7_asm.s
;

	.xdef	_trap7

_trap7:
	link	a6,#0
	move.l	d2,-(sp)
	move.l	8(a6),d0
	move.l	12(a6),d1
	move.l	16(a6),d2
	trap	#7
	move.l	(sp)+,d2
	unlk	a6
	rts
