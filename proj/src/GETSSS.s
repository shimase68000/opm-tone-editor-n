;
; GETSSS.s
;

	.include doscall.mac
	.include iocscall.mac

_GETSSS::
	movem.l	d1/a0,-(sp)
	lea.l	L000610(pc),a0
	move.l	$000c(sp),$0000.w(a0)
	move.w	#__ERRABORT,-(sp)
	DOS	__INTVCG
	addq.l	#2,sp
	move.l	d0,$000c(a0)
	move.w	#__INPOUT,-(sp)
	DOS	__INTVCG
	addq.l	#2,sp
	move.l	d0,$0004(a0)
	move.w	#__INKEY,-(sp)
	DOS	__INTVCG
	addq.l	#2,sp
	move.l	d0,$0008(a0)
	pea.l	__ERRABORT_new(pc)
	move.w	#__ERRABORT,-(sp)
	DOS	__INTVCS				; __ERRABORT書き換え
	addq.l	#6,sp
	pea.l	__INPOUT_new(pc)
	move.w	#__INPOUT,-(sp)
	DOS	__INTVCS				; __INPOUT書き換え
	addq.l	#6,sp
	pea.l	__INKEY_new(pc)
	move.w	#__INKEY,-(sp)
	DOS	__INTVCS				; __INKEY書き換え
	addq.l	#6,sp
	move.l	$0010(sp),-(sp)
	DOS	__GETSS					; _GETSS
	addq.l	#4,sp
	move.l	d0,d1
	move.l	$0004(a0),-(sp)
	move.w	#__INPOUT,-(sp)
	DOS	__INTVCS
	addq.l	#6,sp
	move.l	$0008(a0),-(sp)
	move.w	#__INKEY,-(sp)
	DOS	__INTVCS
	addq.l	#6,sp
	move.l	$000c(a0),-(sp)
	move.w	#__ERRABORT,-(sp)
	DOS	__INTVCS
	addq.l	#6,sp
	move.l	d1,d0
	movem.l	(sp)+,d1/a0
	rts

__ERRABORT_new:
	move.l	L000614(pc),-(sp)
	move.w	#__INPOUT,-(sp)
	DOS	__INTVCS
	addq.l	#6,sp
	move.l	L000618(pc),-(sp)
	move.w	#__INKEY,-(sp)
	DOS	__INTVCS
	addq.l	#6,sp
	move.l	L00061c(pc),-(sp)
	rts

__INPOUT_new:
	move.l	a5,-(sp)
	movea.l	L000610(pc),a5
	moveq.l	#$00,d0
	move.b	(a5),d0
	beq	L0005e4
	cmpi.b	#$fe,$0001(a6)
	beq	L0005ea
L0005e4:
	movea.l	L000614(pc),a5
	jsr	(a5)
L0005ea:
	movea.l	(sp)+,a5
	rts

__INKEY_new:
	movem.l	a5,-(sp)
	movea.l	L000610(pc),a5
	moveq.l	#$00,d0
	move.b	(a5),d0
	beq	L000604
	lea.l	L000610(pc),a5
	addq.l	#1,(a5)
	bra	L00060a
L000604:
	movea.l	L000618(pc),a5
	jsr	(a5)
L00060a:
	movem.l	(sp)+,a5
	rts

L000610:
	.dc.l	$00000000
L000614:
	.dc.l	$00000000	; &(__INPOUT)
L000618:
	.dc.l	$00000000	; &(__INKEY)
L00061c:
	.dc.l	$00000000	; &(__ERRABORT)
