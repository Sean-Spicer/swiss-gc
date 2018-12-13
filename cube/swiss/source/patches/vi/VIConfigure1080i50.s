#include "../asm.h"
#define _LANGUAGE_ASSEMBLY
#include "../../../../reservedarea.h"

.globl VIConfigure1080i50
VIConfigure1080i50:
	li			%r0, 20
	lwz			%r6, 0 (%r3)
	lis			%r4, VAR_AREA
	lwz			%r5, VAR_TVMODE (%r4)
	cmpwi		%r5, 0
	beq			1f
	srwi.		%r5, %r6, 2
	bne			2f
	stw			%r5, VAR_TVMODE (%r4)
1:	insrwi		%r0, %r5, 30, 0
2:	stw			%r3, VAR_RMODE (%r4)
	li			%r7, 1
	li			%r6, 0
	lhz			%r5, 8 (%r3)
	slwi		%r5, %r5, 1
	subic.		%r4, %r5, 480
	ble			4f
	xori		%r0, %r0, 16
	subic.		%r4, %r5, 576
	ble			4f
	xori		%r0, %r0, 18
	lhz			%r5, 8 (%r3)
	cmpwi		%r5, 480
	ble			3f
	xori		%r0, %r0, 16
3:	subic		%r4, %r5, 540
4:	srawi		%r4, %r4, 1
	addze		%r4, %r4
	neg			%r4, %r4
	sth			%r4, 12 (%r3)
	sth			%r5, 16 (%r3)
	stw			%r6, 20 (%r3)
	stb			%r7, 24 (%r3)
	stw			%r0, 0 (%r3)

.globl VIConfigure1080i50_length
VIConfigure1080i50_length:
.long (VIConfigure1080i50_length - VIConfigure1080i50)