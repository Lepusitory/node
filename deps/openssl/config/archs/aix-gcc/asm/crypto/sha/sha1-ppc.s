.machine	"any"
.csect	.text[PR],7

.globl	.sha1_block_data_order
.align	4
.sha1_block_data_order:
	stwu	1,-160(1)
	mflr	0
	stw	15,92(1)
	stw	16,96(1)
	stw	17,100(1)
	stw	18,104(1)
	stw	19,108(1)
	stw	20,112(1)
	stw	21,116(1)
	stw	22,120(1)
	stw	23,124(1)
	stw	24,128(1)
	stw	25,132(1)
	stw	26,136(1)
	stw	27,140(1)
	stw	28,144(1)
	stw	29,148(1)
	stw	30,152(1)
	stw	31,156(1)
	stw	0,164(1)
	lwz	7,0(3)
	lwz	8,4(3)
	lwz	9,8(3)
	lwz	10,12(3)
	lwz	11,16(3)
	andi.	0,4,3
	bne	Lunaligned
Laligned:
	mtctr	5
	bl	Lsha1_block_private
	b	Ldone







.align	4
Lunaligned:
	subfic	6,4,4096
	andi.	6,6,4095
	srwi.	6,6,6
	beq	Lcross_page
	cmplw	0,5,6
	ble	Laligned
	mtctr	6
	subfc	5,6,5
	bl	Lsha1_block_private
Lcross_page:
	li	6,16
	mtctr	6
	addi	20,1,24
Lmemcpy:
	lbz	16,0(4)
	lbz	17,1(4)
	lbz	18,2(4)
	lbz	19,3(4)
	addi	4,4,4
	stb	16,0(20)
	stb	17,1(20)
	stb	18,2(20)
	stb	19,3(20)
	addi	20,20,4
	bc	16,0,Lmemcpy

	stw	4,88(1)
	li	6,1
	addi	4,1,24
	mtctr	6
	bl	Lsha1_block_private
	lwz	4,88(1)
	addic.	5,5,-1
	bne	Lunaligned

Ldone:
	lwz	0,164(1)
	lwz	15,92(1)
	lwz	16,96(1)
	lwz	17,100(1)
	lwz	18,104(1)
	lwz	19,108(1)
	lwz	20,112(1)
	lwz	21,116(1)
	lwz	22,120(1)
	lwz	23,124(1)
	lwz	24,128(1)
	lwz	25,132(1)
	lwz	26,136(1)
	lwz	27,140(1)
	lwz	28,144(1)
	lwz	29,148(1)
	lwz	30,152(1)
	lwz	31,156(1)
	mtlr	0
	addi	1,1,160
	blr
.long	0
.byte	0,12,4,1,0x80,18,3,0
.long	0
.align	4
Lsha1_block_private:
	lis	0,0x5a82
	ori	0,0,0x7999
	lwz	16,0(4)
	lwz	17,4(4)
	add	12,0,11
	rotlwi	11,7,5
	add	12,12,16
	and	15,9,8
	add	12,12,11
	andc	6,10,8
	rotlwi	8,8,30
	or	15,15,6
	add	12,12,15
	lwz	18,8(4)
	add	11,0,10
	rotlwi	10,12,5
	add	11,11,17
	and	15,8,7
	add	11,11,10
	andc	6,9,7
	rotlwi	7,7,30
	or	15,15,6
	add	11,11,15
	lwz	19,12(4)
	add	10,0,9
	rotlwi	9,11,5
	add	10,10,18
	and	15,7,12
	add	10,10,9
	andc	6,8,12
	rotlwi	12,12,30
	or	15,15,6
	add	10,10,15
	lwz	20,16(4)
	add	9,0,8
	rotlwi	8,10,5
	add	9,9,19
	and	15,12,11
	add	9,9,8
	andc	6,7,11
	rotlwi	11,11,30
	or	15,15,6
	add	9,9,15
	lwz	21,20(4)
	add	8,0,7
	rotlwi	7,9,5
	add	8,8,20
	and	15,11,10
	add	8,8,7
	andc	6,12,10
	rotlwi	10,10,30
	or	15,15,6
	add	8,8,15
	lwz	22,24(4)
	add	7,0,12
	rotlwi	12,8,5
	add	7,7,21
	and	15,10,9
	add	7,7,12
	andc	6,11,9
	rotlwi	9,9,30
	or	15,15,6
	add	7,7,15
	lwz	23,28(4)
	add	12,0,11
	rotlwi	11,7,5
	add	12,12,22
	and	15,9,8
	add	12,12,11
	andc	6,10,8
	rotlwi	8,8,30
	or	15,15,6
	add	12,12,15
	lwz	24,32(4)
	add	11,0,10
	rotlwi	10,12,5
	add	11,11,23
	and	15,8,7
	add	11,11,10
	andc	6,9,7
	rotlwi	7,7,30
	or	15,15,6
	add	11,11,15
	lwz	25,36(4)
	add	10,0,9
	rotlwi	9,11,5
	add	10,10,24
	and	15,7,12
	add	10,10,9
	andc	6,8,12
	rotlwi	12,12,30
	or	15,15,6
	add	10,10,15
	lwz	26,40(4)
	add	9,0,8
	rotlwi	8,10,5
	add	9,9,25
	and	15,12,11
	add	9,9,8
	andc	6,7,11
	rotlwi	11,11,30
	or	15,15,6
	add	9,9,15
	lwz	27,44(4)
	add	8,0,7
	rotlwi	7,9,5
	add	8,8,26
	and	15,11,10
	add	8,8,7
	andc	6,12,10
	rotlwi	10,10,30
	or	15,15,6
	add	8,8,15
	lwz	28,48(4)
	add	7,0,12
	rotlwi	12,8,5
	add	7,7,27
	and	15,10,9
	add	7,7,12
	andc	6,11,9
	rotlwi	9,9,30
	or	15,15,6
	add	7,7,15
	lwz	29,52(4)
	add	12,0,11
	rotlwi	11,7,5
	add	12,12,28
	and	15,9,8
	add	12,12,11
	andc	6,10,8
	rotlwi	8,8,30
	or	15,15,6
	add	12,12,15
	lwz	30,56(4)
	add	11,0,10
	rotlwi	10,12,5
	add	11,11,29
	and	15,8,7
	add	11,11,10
	andc	6,9,7
	rotlwi	7,7,30
	or	15,15,6
	add	11,11,15
	lwz	31,60(4)
	add	10,0,9
	rotlwi	9,11,5
	add	10,10,30
	and	15,7,12
	add	10,10,9
	andc	6,8,12
	rotlwi	12,12,30
	or	15,15,6
	add	10,10,15
	add	9,0,8
	rotlwi	8,10,5
	xor	16,16,18
	add	9,9,31
	and	15,12,11
	xor	16,16,24
	add	9,9,8
	andc	6,7,11
	rotlwi	11,11,30
	or	15,15,6
	xor	16,16,29
	add	9,9,15
	rotlwi	16,16,1
	add	8,0,7
	rotlwi	7,9,5
	xor	17,17,19
	add	8,8,16
	and	15,11,10
	xor	17,17,25
	add	8,8,7
	andc	6,12,10
	rotlwi	10,10,30
	or	15,15,6
	xor	17,17,30
	add	8,8,15
	rotlwi	17,17,1
	add	7,0,12
	rotlwi	12,8,5
	xor	18,18,20
	add	7,7,17
	and	15,10,9
	xor	18,18,26
	add	7,7,12
	andc	6,11,9
	rotlwi	9,9,30
	or	15,15,6
	xor	18,18,31
	add	7,7,15
	rotlwi	18,18,1
	add	12,0,11
	rotlwi	11,7,5
	xor	19,19,21
	add	12,12,18
	and	15,9,8
	xor	19,19,27
	add	12,12,11
	andc	6,10,8
	rotlwi	8,8,30
	or	15,15,6
	xor	19,19,16
	add	12,12,15
	rotlwi	19,19,1
	add	11,0,10
	rotlwi	10,12,5
	xor	20,20,22
	add	11,11,19
	and	15,8,7
	xor	20,20,28
	add	11,11,10
	andc	6,9,7
	rotlwi	7,7,30
	or	15,15,6
	xor	20,20,17
	add	11,11,15
	rotlwi	20,20,1
	lis	0,0x6ed9
	ori	0,0,0xeba1
	add	10,0,9
	xor	15,12,8
	rotlwi	9,11,5
	xor	21,21,23
	add	10,10,20
	xor	15,15,7
	xor	21,21,29
	add	10,10,15
	rotlwi	12,12,30
	xor	21,21,18
	add	10,10,9
	rotlwi	21,21,1
	add	9,0,8
	xor	15,11,7
	rotlwi	8,10,5
	xor	22,22,24
	add	9,9,21
	xor	15,15,12
	xor	22,22,30
	add	9,9,15
	rotlwi	11,11,30
	xor	22,22,19
	add	9,9,8
	rotlwi	22,22,1
	add	8,0,7
	xor	15,10,12
	rotlwi	7,9,5
	xor	23,23,25
	add	8,8,22
	xor	15,15,11
	xor	23,23,31
	add	8,8,15
	rotlwi	10,10,30
	xor	23,23,20
	add	8,8,7
	rotlwi	23,23,1
	add	7,0,12
	xor	15,9,11
	rotlwi	12,8,5
	xor	24,24,26
	add	7,7,23
	xor	15,15,10
	xor	24,24,16
	add	7,7,15
	rotlwi	9,9,30
	xor	24,24,21
	add	7,7,12
	rotlwi	24,24,1
	add	12,0,11
	xor	15,8,10
	rotlwi	11,7,5
	xor	25,25,27
	add	12,12,24
	xor	15,15,9
	xor	25,25,17
	add	12,12,15
	rotlwi	8,8,30
	xor	25,25,22
	add	12,12,11
	rotlwi	25,25,1
	add	11,0,10
	xor	15,7,9
	rotlwi	10,12,5
	xor	26,26,28
	add	11,11,25
	xor	15,15,8
	xor	26,26,18
	add	11,11,15
	rotlwi	7,7,30
	xor	26,26,23
	add	11,11,10
	rotlwi	26,26,1
	add	10,0,9
	xor	15,12,8
	rotlwi	9,11,5
	xor	27,27,29
	add	10,10,26
	xor	15,15,7
	xor	27,27,19
	add	10,10,15
	rotlwi	12,12,30
	xor	27,27,24
	add	10,10,9
	rotlwi	27,27,1
	add	9,0,8
	xor	15,11,7
	rotlwi	8,10,5
	xor	28,28,30
	add	9,9,27
	xor	15,15,12
	xor	28,28,20
	add	9,9,15
	rotlwi	11,11,30
	xor	28,28,25
	add	9,9,8
	rotlwi	28,28,1
	add	8,0,7
	xor	15,10,12
	rotlwi	7,9,5
	xor	29,29,31
	add	8,8,28
	xor	15,15,11
	xor	29,29,21
	add	8,8,15
	rotlwi	10,10,30
	xor	29,29,26
	add	8,8,7
	rotlwi	29,29,1
	add	7,0,12
	xor	15,9,11
	rotlwi	12,8,5
	xor	30,30,16
	add	7,7,29
	xor	15,15,10
	xor	30,30,22
	add	7,7,15
	rotlwi	9,9,30
	xor	30,30,27
	add	7,7,12
	rotlwi	30,30,1
	add	12,0,11
	xor	15,8,10
	rotlwi	11,7,5
	xor	31,31,17
	add	12,12,30
	xor	15,15,9
	xor	31,31,23
	add	12,12,15
	rotlwi	8,8,30
	xor	31,31,28
	add	12,12,11
	rotlwi	31,31,1
	add	11,0,10
	xor	15,7,9
	rotlwi	10,12,5
	xor	16,16,18
	add	11,11,31
	xor	15,15,8
	xor	16,16,24
	add	11,11,15
	rotlwi	7,7,30
	xor	16,16,29
	add	11,11,10
	rotlwi	16,16,1
	add	10,0,9
	xor	15,12,8
	rotlwi	9,11,5
	xor	17,17,19
	add	10,10,16
	xor	15,15,7
	xor	17,17,25
	add	10,10,15
	rotlwi	12,12,30
	xor	17,17,30
	add	10,10,9
	rotlwi	17,17,1
	add	9,0,8
	xor	15,11,7
	rotlwi	8,10,5
	xor	18,18,20
	add	9,9,17
	xor	15,15,12
	xor	18,18,26
	add	9,9,15
	rotlwi	11,11,30
	xor	18,18,31
	add	9,9,8
	rotlwi	18,18,1
	add	8,0,7
	xor	15,10,12
	rotlwi	7,9,5
	xor	19,19,21
	add	8,8,18
	xor	15,15,11
	xor	19,19,27
	add	8,8,15
	rotlwi	10,10,30
	xor	19,19,16
	add	8,8,7
	rotlwi	19,19,1
	add	7,0,12
	xor	15,9,11
	rotlwi	12,8,5
	xor	20,20,22
	add	7,7,19
	xor	15,15,10
	xor	20,20,28
	add	7,7,15
	rotlwi	9,9,30
	xor	20,20,17
	add	7,7,12
	rotlwi	20,20,1
	add	12,0,11
	xor	15,8,10
	rotlwi	11,7,5
	xor	21,21,23
	add	12,12,20
	xor	15,15,9
	xor	21,21,29
	add	12,12,15
	rotlwi	8,8,30
	xor	21,21,18
	add	12,12,11
	rotlwi	21,21,1
	add	11,0,10
	xor	15,7,9
	rotlwi	10,12,5
	xor	22,22,24
	add	11,11,21
	xor	15,15,8
	xor	22,22,30
	add	11,11,15
	rotlwi	7,7,30
	xor	22,22,19
	add	11,11,10
	rotlwi	22,22,1
	add	10,0,9
	xor	15,12,8
	rotlwi	9,11,5
	xor	23,23,25
	add	10,10,22
	xor	15,15,7
	xor	23,23,31
	add	10,10,15
	rotlwi	12,12,30
	xor	23,23,20
	add	10,10,9
	rotlwi	23,23,1
	add	9,0,8
	xor	15,11,7
	rotlwi	8,10,5
	xor	24,24,26
	add	9,9,23
	xor	15,15,12
	xor	24,24,16
	add	9,9,15
	rotlwi	11,11,30
	xor	24,24,21
	add	9,9,8
	rotlwi	24,24,1
	lis	0,0x8f1b
	ori	0,0,0xbcdc
	add	8,0,7
	rotlwi	7,9,5
	xor	25,25,27
	add	8,8,24
	and	15,10,11
	xor	25,25,17
	add	8,8,7
	or	6,10,11
	rotlwi	10,10,30
	xor	25,25,22
	and	6,6,12
	or	15,15,6
	rotlwi	25,25,1
	add	8,8,15
	add	7,0,12
	rotlwi	12,8,5
	xor	26,26,28
	add	7,7,25
	and	15,9,10
	xor	26,26,18
	add	7,7,12
	or	6,9,10
	rotlwi	9,9,30
	xor	26,26,23
	and	6,6,11
	or	15,15,6
	rotlwi	26,26,1
	add	7,7,15
	add	12,0,11
	rotlwi	11,7,5
	xor	27,27,29
	add	12,12,26
	and	15,8,9
	xor	27,27,19
	add	12,12,11
	or	6,8,9
	rotlwi	8,8,30
	xor	27,27,24
	and	6,6,10
	or	15,15,6
	rotlwi	27,27,1
	add	12,12,15
	add	11,0,10
	rotlwi	10,12,5
	xor	28,28,30
	add	11,11,27
	and	15,7,8
	xor	28,28,20
	add	11,11,10
	or	6,7,8
	rotlwi	7,7,30
	xor	28,28,25
	and	6,6,9
	or	15,15,6
	rotlwi	28,28,1
	add	11,11,15
	add	10,0,9
	rotlwi	9,11,5
	xor	29,29,31
	add	10,10,28
	and	15,12,7
	xor	29,29,21
	add	10,10,9
	or	6,12,7
	rotlwi	12,12,30
	xor	29,29,26
	and	6,6,8
	or	15,15,6
	rotlwi	29,29,1
	add	10,10,15
	add	9,0,8
	rotlwi	8,10,5
	xor	30,30,16
	add	9,9,29
	and	15,11,12
	xor	30,30,22
	add	9,9,8
	or	6,11,12
	rotlwi	11,11,30
	xor	30,30,27
	and	6,6,7
	or	15,15,6
	rotlwi	30,30,1
	add	9,9,15
	add	8,0,7
	rotlwi	7,9,5
	xor	31,31,17
	add	8,8,30
	and	15,10,11
	xor	31,31,23
	add	8,8,7
	or	6,10,11
	rotlwi	10,10,30
	xor	31,31,28
	and	6,6,12
	or	15,15,6
	rotlwi	31,31,1
	add	8,8,15
	add	7,0,12
	rotlwi	12,8,5
	xor	16,16,18
	add	7,7,31
	and	15,9,10
	xor	16,16,24
	add	7,7,12
	or	6,9,10
	rotlwi	9,9,30
	xor	16,16,29
	and	6,6,11
	or	15,15,6
	rotlwi	16,16,1
	add	7,7,15
	add	12,0,11
	rotlwi	11,7,5
	xor	17,17,19
	add	12,12,16
	and	15,8,9
	xor	17,17,25
	add	12,12,11
	or	6,8,9
	rotlwi	8,8,30
	xor	17,17,30
	and	6,6,10
	or	15,15,6
	rotlwi	17,17,1
	add	12,12,15
	add	11,0,10
	rotlwi	10,12,5
	xor	18,18,20
	add	11,11,17
	and	15,7,8
	xor	18,18,26
	add	11,11,10
	or	6,7,8
	rotlwi	7,7,30
	xor	18,18,31
	and	6,6,9
	or	15,15,6
	rotlwi	18,18,1
	add	11,11,15
	add	10,0,9
	rotlwi	9,11,5
	xor	19,19,21
	add	10,10,18
	and	15,12,7
	xor	19,19,27
	add	10,10,9
	or	6,12,7
	rotlwi	12,12,30
	xor	19,19,16
	and	6,6,8
	or	15,15,6
	rotlwi	19,19,1
	add	10,10,15
	add	9,0,8
	rotlwi	8,10,5
	xor	20,20,22
	add	9,9,19
	and	15,11,12
	xor	20,20,28
	add	9,9,8
	or	6,11,12
	rotlwi	11,11,30
	xor	20,20,17
	and	6,6,7
	or	15,15,6
	rotlwi	20,20,1
	add	9,9,15
	add	8,0,7
	rotlwi	7,9,5
	xor	21,21,23
	add	8,8,20
	and	15,10,11
	xor	21,21,29
	add	8,8,7
	or	6,10,11
	rotlwi	10,10,30
	xor	21,21,18
	and	6,6,12
	or	15,15,6
	rotlwi	21,21,1
	add	8,8,15
	add	7,0,12
	rotlwi	12,8,5
	xor	22,22,24
	add	7,7,21
	and	15,9,10
	xor	22,22,30
	add	7,7,12
	or	6,9,10
	rotlwi	9,9,30
	xor	22,22,19
	and	6,6,11
	or	15,15,6
	rotlwi	22,22,1
	add	7,7,15
	add	12,0,11
	rotlwi	11,7,5
	xor	23,23,25
	add	12,12,22
	and	15,8,9
	xor	23,23,31
	add	12,12,11
	or	6,8,9
	rotlwi	8,8,30
	xor	23,23,20
	and	6,6,10
	or	15,15,6
	rotlwi	23,23,1
	add	12,12,15
	add	11,0,10
	rotlwi	10,12,5
	xor	24,24,26
	add	11,11,23
	and	15,7,8
	xor	24,24,16
	add	11,11,10
	or	6,7,8
	rotlwi	7,7,30
	xor	24,24,21
	and	6,6,9
	or	15,15,6
	rotlwi	24,24,1
	add	11,11,15
	add	10,0,9
	rotlwi	9,11,5
	xor	25,25,27
	add	10,10,24
	and	15,12,7
	xor	25,25,17
	add	10,10,9
	or	6,12,7
	rotlwi	12,12,30
	xor	25,25,22
	and	6,6,8
	or	15,15,6
	rotlwi	25,25,1
	add	10,10,15
	add	9,0,8
	rotlwi	8,10,5
	xor	26,26,28
	add	9,9,25
	and	15,11,12
	xor	26,26,18
	add	9,9,8
	or	6,11,12
	rotlwi	11,11,30
	xor	26,26,23
	and	6,6,7
	or	15,15,6
	rotlwi	26,26,1
	add	9,9,15
	add	8,0,7
	rotlwi	7,9,5
	xor	27,27,29
	add	8,8,26
	and	15,10,11
	xor	27,27,19
	add	8,8,7
	or	6,10,11
	rotlwi	10,10,30
	xor	27,27,24
	and	6,6,12
	or	15,15,6
	rotlwi	27,27,1
	add	8,8,15
	add	7,0,12
	rotlwi	12,8,5
	xor	28,28,30
	add	7,7,27
	and	15,9,10
	xor	28,28,20
	add	7,7,12
	or	6,9,10
	rotlwi	9,9,30
	xor	28,28,25
	and	6,6,11
	or	15,15,6
	rotlwi	28,28,1
	add	7,7,15
	lis	0,0xca62
	ori	0,0,0xc1d6
	add	12,0,11
	xor	15,8,10
	rotlwi	11,7,5
	xor	29,29,31
	add	12,12,28
	xor	15,15,9
	xor	29,29,21
	add	12,12,15
	rotlwi	8,8,30
	xor	29,29,26
	add	12,12,11
	rotlwi	29,29,1
	add	11,0,10
	xor	15,7,9
	rotlwi	10,12,5
	xor	30,30,16
	add	11,11,29
	xor	15,15,8
	xor	30,30,22
	add	11,11,15
	rotlwi	7,7,30
	xor	30,30,27
	add	11,11,10
	rotlwi	30,30,1
	add	10,0,9
	xor	15,12,8
	rotlwi	9,11,5
	xor	31,31,17
	add	10,10,30
	xor	15,15,7
	xor	31,31,23
	add	10,10,15
	rotlwi	12,12,30
	xor	31,31,28
	add	10,10,9
	rotlwi	31,31,1
	add	9,0,8
	xor	15,11,7
	rotlwi	8,10,5
	xor	16,16,18
	add	9,9,31
	xor	15,15,12
	xor	16,16,24
	add	9,9,15
	rotlwi	11,11,30
	xor	16,16,29
	add	9,9,8
	rotlwi	16,16,1
	add	8,0,7
	xor	15,10,12
	rotlwi	7,9,5
	xor	17,17,19
	add	8,8,16
	xor	15,15,11
	xor	17,17,25
	add	8,8,15
	rotlwi	10,10,30
	xor	17,17,30
	add	8,8,7
	rotlwi	17,17,1
	add	7,0,12
	xor	15,9,11
	rotlwi	12,8,5
	xor	18,18,20
	add	7,7,17
	xor	15,15,10
	xor	18,18,26
	add	7,7,15
	rotlwi	9,9,30
	xor	18,18,31
	add	7,7,12
	rotlwi	18,18,1
	add	12,0,11
	xor	15,8,10
	rotlwi	11,7,5
	xor	19,19,21
	add	12,12,18
	xor	15,15,9
	xor	19,19,27
	add	12,12,15
	rotlwi	8,8,30
	xor	19,19,16
	add	12,12,11
	rotlwi	19,19,1
	add	11,0,10
	xor	15,7,9
	rotlwi	10,12,5
	xor	20,20,22
	add	11,11,19
	xor	15,15,8
	xor	20,20,28
	add	11,11,15
	rotlwi	7,7,30
	xor	20,20,17
	add	11,11,10
	rotlwi	20,20,1
	add	10,0,9
	xor	15,12,8
	rotlwi	9,11,5
	xor	21,21,23
	add	10,10,20
	xor	15,15,7
	xor	21,21,29
	add	10,10,15
	rotlwi	12,12,30
	xor	21,21,18
	add	10,10,9
	rotlwi	21,21,1
	add	9,0,8
	xor	15,11,7
	rotlwi	8,10,5
	xor	22,22,24
	add	9,9,21
	xor	15,15,12
	xor	22,22,30
	add	9,9,15
	rotlwi	11,11,30
	xor	22,22,19
	add	9,9,8
	rotlwi	22,22,1
	add	8,0,7
	xor	15,10,12
	rotlwi	7,9,5
	xor	23,23,25
	add	8,8,22
	xor	15,15,11
	xor	23,23,31
	add	8,8,15
	rotlwi	10,10,30
	xor	23,23,20
	add	8,8,7
	rotlwi	23,23,1
	add	7,0,12
	xor	15,9,11
	rotlwi	12,8,5
	xor	24,24,26
	add	7,7,23
	xor	15,15,10
	xor	24,24,16
	add	7,7,15
	rotlwi	9,9,30
	xor	24,24,21
	add	7,7,12
	rotlwi	24,24,1
	add	12,0,11
	xor	15,8,10
	rotlwi	11,7,5
	xor	25,25,27
	add	12,12,24
	xor	15,15,9
	xor	25,25,17
	add	12,12,15
	rotlwi	8,8,30
	xor	25,25,22
	add	12,12,11
	rotlwi	25,25,1
	add	11,0,10
	xor	15,7,9
	rotlwi	10,12,5
	xor	26,26,28
	add	11,11,25
	xor	15,15,8
	xor	26,26,18
	add	11,11,15
	rotlwi	7,7,30
	xor	26,26,23
	add	11,11,10
	rotlwi	26,26,1
	add	10,0,9
	xor	15,12,8
	rotlwi	9,11,5
	xor	27,27,29
	add	10,10,26
	xor	15,15,7
	xor	27,27,19
	add	10,10,15
	rotlwi	12,12,30
	xor	27,27,24
	add	10,10,9
	rotlwi	27,27,1
	add	9,0,8
	xor	15,11,7
	rotlwi	8,10,5
	xor	28,28,30
	add	9,9,27
	xor	15,15,12
	xor	28,28,20
	add	9,9,15
	rotlwi	11,11,30
	xor	28,28,25
	add	9,9,8
	rotlwi	28,28,1
	add	8,0,7
	xor	15,10,12
	rotlwi	7,9,5
	xor	29,29,31
	add	8,8,28
	xor	15,15,11
	xor	29,29,21
	add	8,8,15
	rotlwi	10,10,30
	xor	29,29,26
	add	8,8,7
	rotlwi	29,29,1
	add	7,0,12
	xor	15,9,11
	rotlwi	12,8,5
	xor	30,30,16
	add	7,7,29
	xor	15,15,10
	xor	30,30,22
	add	7,7,15
	rotlwi	9,9,30
	xor	30,30,27
	add	7,7,12
	rotlwi	30,30,1
	add	12,0,11
	xor	15,8,10
	rotlwi	11,7,5
	xor	31,31,17
	add	12,12,30
	xor	15,15,9
	xor	31,31,23
	add	12,12,15
	rotlwi	8,8,30
	xor	31,31,28
	add	12,12,11
	rotlwi	31,31,1
	add	11,0,10
	xor	15,7,9
	rotlwi	10,12,5
	lwz	16,0(3)
	add	11,11,31
	xor	15,15,8
	lwz	17,4(3)
	add	11,11,15
	rotlwi	7,7,30
	lwz	18,8(3)
	lwz	19,12(3)
	add	11,11,10
	lwz	20,16(3)
	add	16,16,11
	add	17,17,12
	add	18,18,7
	add	19,19,8
	add	20,20,9
	stw	16,0(3)
	mr	7,16
	stw	17,4(3)
	mr	8,17
	stw	18,8(3)
	mr	9,18
	stw	19,12(3)
	mr	10,19
	stw	20,16(3)
	mr	11,20
	addi	4,4,64
	bc	16,0,Lsha1_block_private
	blr
.long	0
.byte	0,12,0x14,0,0,0,0,0

.byte	83,72,65,49,32,98,108,111,99,107,32,116,114,97,110,115,102,111,114,109,32,102,111,114,32,80,80,67,44,32,67,82,89,80,84,79,71,65,77,83,32,98,121,32,60,97,112,112,114,111,64,102,121,46,99,104,97,108,109,101,114,115,46,115,101,62,0
.align	2
