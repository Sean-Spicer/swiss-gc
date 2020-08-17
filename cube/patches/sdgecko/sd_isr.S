/* 
 * Copyright (c) 2020, Extrems <extrems@extremscorner.org>
 * 
 * This file is part of Swiss.
 * 
 * Swiss is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Swiss is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * with Swiss.  If not, see <https://www.gnu.org/licenses/>.
 */

#define _LANGUAGE_ASSEMBLY
#include "../../reservedarea.h"

.section .text.external_interrupt_vector,"ax",@progbits
	.balign 32
	.globl isr_registers, isr_transferred
isr_registers:
	.long	0x0C006800
isr_transferred:
	.long	512

	.globl external_interrupt_vector
external_interrupt_vector:
	mtsprg	0, r4
	mtsprg	1, r5
	mtsprg	2, r6
	mfcr	r6
	mtsprg	3, r6
	lwz		r4, isr_registers - 0x80000000 (r0)
	eciwx	r5, r0, r4
	rlwinm	r6, r5, 1, 28, 28
	and.	r6, r5, r6
	beq		6f
	andi.	r5, r5, (0x3FFF & ~0x80A) | (1 << 3)
	lwz		r6, isr_transferred - 0x80000000 (r0)
	cmpwi	cr0, r6, 0
	cmpwi	cr1, r6, 512 - 4
	bgt		cr1, 6f
	beq		cr1, 1f
	ecowx	r5, r0, r4
1:	li		r5, 4*4
	eciwx	r5, r5, r4
	blt		cr0, 2f
	stw		r5, VAR_SECTOR_BUF (r6)
	addi	r6, r6, 4
	stw		r6, isr_transferred - 0x80000000 (r0)
	beq		cr1, 6f
2:	cmpw	cr1, r5, r6
	li		r5, 4*4
	li		r6, -1
	ecowx	r6, r5, r4
	bnl		cr0, 4f
	beq		cr1, 3f
	li		r5, 3*4
	li		r6, ((1 - 1) << 4) | 0b01
	b		5f
3:	li		r6, 0
	stw		r6, isr_transferred - 0x80000000 (r0)
4:	li		r5, 3*4
	li		r6, ((4 - 1) << 4) | 0b01
5:	ecowx	r6, r5, r4
	lis		r4, 0x0C00
	li		r5, 0x3000
	eciwx	r5, r5, r4
	rlwinm.	r6, r5, 0, 16, 14
	bne		6f
	mfsprg	r6, 3
	mtcr	r6
	mfsprg	r6, 2
	mfsprg	r5, 1
	mfsprg	r4, 0
	rfi
6:	mfsprg	r6, 3
	mtcr	r6
	mfsprg	r6, 2
	mfsprg	r5, 1
	ba		0x00000504