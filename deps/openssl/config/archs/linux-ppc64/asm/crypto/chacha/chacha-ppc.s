.machine	"any"
.text

.globl	ChaCha20_ctr32_int
.type	ChaCha20_ctr32_int,@function
.section	".opd","aw"
.align	3
ChaCha20_ctr32_int:
.quad	.ChaCha20_ctr32_int,.TOC.@tocbase,0
.previous

.align	5
.ChaCha20_ctr32_int:
__ChaCha20_ctr32_int:
	cmpldi	5,0
	.long	0x4DC20020

	stdu	1,-256(1)
	mflr	0

	std	14,112(1)
	std	15,120(1)
	std	16,128(1)
	std	17,136(1)
	std	18,144(1)
	std	19,152(1)
	std	20,160(1)
	std	21,168(1)
	std	22,176(1)
	std	23,184(1)
	std	24,192(1)
	std	25,200(1)
	std	26,208(1)
	std	27,216(1)
	std	28,224(1)
	std	29,232(1)
	std	30,240(1)
	std	31,248(1)
	std	0,272(1)

	lwz	11,0(7)
	lwz	12,4(7)
	lwz	14,8(7)
	lwz	15,12(7)

	bl	__ChaCha20_1x

	ld	0,272(1)
	ld	14,112(1)
	ld	15,120(1)
	ld	16,128(1)
	ld	17,136(1)
	ld	18,144(1)
	ld	19,152(1)
	ld	20,160(1)
	ld	21,168(1)
	ld	22,176(1)
	ld	23,184(1)
	ld	24,192(1)
	ld	25,200(1)
	ld	26,208(1)
	ld	27,216(1)
	ld	28,224(1)
	ld	29,232(1)
	ld	30,240(1)
	ld	31,248(1)
	mtlr	0
	addi	1,1,256
	blr
.long	0
.byte	0,12,4,1,0x80,18,5,0
.long	0
.size	ChaCha20_ctr32_int,.-.ChaCha20_ctr32_int
.size	.ChaCha20_ctr32_int,.-.ChaCha20_ctr32_int

.align	5
__ChaCha20_1x:
.Loop_outer:
	lis	16,0x6170
	lis	17,0x3320
	lis	18,0x7962
	lis	19,0x6b20
	ori	16,16,0x7865
	ori	17,17,0x646e
	ori	18,18,0x2d32
	ori	19,19,0x6574

	li	0,10
	lwz	20,0(6)
	lwz	21,4(6)
	lwz	22,8(6)
	lwz	23,12(6)
	lwz	24,16(6)
	mr	28,11
	lwz	25,20(6)
	mr	29,12
	lwz	26,24(6)
	mr	30,14
	lwz	27,28(6)
	mr	31,15

	mr	7,20
	mr	8,21
	mr	9,22
	mr	10,23

	mtctr	0
.Loop:
	add	16,16,20
	add	17,17,21
	add	18,18,22
	add	19,19,23
	xor	28,28,16
	xor	29,29,17
	xor	30,30,18
	xor	31,31,19
	rotlwi	28,28,16
	rotlwi	29,29,16
	rotlwi	30,30,16
	rotlwi	31,31,16
	add	24,24,28
	add	25,25,29
	add	26,26,30
	add	27,27,31
	xor	20,20,24
	xor	21,21,25
	xor	22,22,26
	xor	23,23,27
	rotlwi	20,20,12
	rotlwi	21,21,12
	rotlwi	22,22,12
	rotlwi	23,23,12
	add	16,16,20
	add	17,17,21
	add	18,18,22
	add	19,19,23
	xor	28,28,16
	xor	29,29,17
	xor	30,30,18
	xor	31,31,19
	rotlwi	28,28,8
	rotlwi	29,29,8
	rotlwi	30,30,8
	rotlwi	31,31,8
	add	24,24,28
	add	25,25,29
	add	26,26,30
	add	27,27,31
	xor	20,20,24
	xor	21,21,25
	xor	22,22,26
	xor	23,23,27
	rotlwi	20,20,7
	rotlwi	21,21,7
	rotlwi	22,22,7
	rotlwi	23,23,7
	add	16,16,21
	add	17,17,22
	add	18,18,23
	add	19,19,20
	xor	31,31,16
	xor	28,28,17
	xor	29,29,18
	xor	30,30,19
	rotlwi	31,31,16
	rotlwi	28,28,16
	rotlwi	29,29,16
	rotlwi	30,30,16
	add	26,26,31
	add	27,27,28
	add	24,24,29
	add	25,25,30
	xor	21,21,26
	xor	22,22,27
	xor	23,23,24
	xor	20,20,25
	rotlwi	21,21,12
	rotlwi	22,22,12
	rotlwi	23,23,12
	rotlwi	20,20,12
	add	16,16,21
	add	17,17,22
	add	18,18,23
	add	19,19,20
	xor	31,31,16
	xor	28,28,17
	xor	29,29,18
	xor	30,30,19
	rotlwi	31,31,8
	rotlwi	28,28,8
	rotlwi	29,29,8
	rotlwi	30,30,8
	add	26,26,31
	add	27,27,28
	add	24,24,29
	add	25,25,30
	xor	21,21,26
	xor	22,22,27
	xor	23,23,24
	xor	20,20,25
	rotlwi	21,21,7
	rotlwi	22,22,7
	rotlwi	23,23,7
	rotlwi	20,20,7
	bdnz	.Loop

	subic	5,5,64
	addi	16,16,0x7865
	addi	17,17,0x646e
	addi	18,18,0x2d32
	addi	19,19,0x6574
	addis	16,16,0x6170
	addis	17,17,0x3320
	addis	18,18,0x7962
	addis	19,19,0x6b20

	subfe.	0,0,0
	add	20,20,7
	lwz	7,16(6)
	add	21,21,8
	lwz	8,20(6)
	add	22,22,9
	lwz	9,24(6)
	add	23,23,10
	lwz	10,28(6)
	add	24,24,7
	add	25,25,8
	add	26,26,9
	add	27,27,10

	add	28,28,11
	add	29,29,12
	add	30,30,14
	add	31,31,15
	addi	11,11,1
	mr	7,16
	rotlwi	16,16,8
	rlwimi	16,7,24,0,7
	rlwimi	16,7,24,16,23
	mr	8,17
	rotlwi	17,17,8
	rlwimi	17,8,24,0,7
	rlwimi	17,8,24,16,23
	mr	9,18
	rotlwi	18,18,8
	rlwimi	18,9,24,0,7
	rlwimi	18,9,24,16,23
	mr	10,19
	rotlwi	19,19,8
	rlwimi	19,10,24,0,7
	rlwimi	19,10,24,16,23
	mr	7,20
	rotlwi	20,20,8
	rlwimi	20,7,24,0,7
	rlwimi	20,7,24,16,23
	mr	8,21
	rotlwi	21,21,8
	rlwimi	21,8,24,0,7
	rlwimi	21,8,24,16,23
	mr	9,22
	rotlwi	22,22,8
	rlwimi	22,9,24,0,7
	rlwimi	22,9,24,16,23
	mr	10,23
	rotlwi	23,23,8
	rlwimi	23,10,24,0,7
	rlwimi	23,10,24,16,23
	mr	7,24
	rotlwi	24,24,8
	rlwimi	24,7,24,0,7
	rlwimi	24,7,24,16,23
	mr	8,25
	rotlwi	25,25,8
	rlwimi	25,8,24,0,7
	rlwimi	25,8,24,16,23
	mr	9,26
	rotlwi	26,26,8
	rlwimi	26,9,24,0,7
	rlwimi	26,9,24,16,23
	mr	10,27
	rotlwi	27,27,8
	rlwimi	27,10,24,0,7
	rlwimi	27,10,24,16,23
	mr	7,28
	rotlwi	28,28,8
	rlwimi	28,7,24,0,7
	rlwimi	28,7,24,16,23
	mr	8,29
	rotlwi	29,29,8
	rlwimi	29,8,24,0,7
	rlwimi	29,8,24,16,23
	mr	9,30
	rotlwi	30,30,8
	rlwimi	30,9,24,0,7
	rlwimi	30,9,24,16,23
	mr	10,31
	rotlwi	31,31,8
	rlwimi	31,10,24,0,7
	rlwimi	31,10,24,16,23
	bne	.Ltail

	lwz	7,0(4)
	lwz	8,4(4)
	cmpldi	5,0
	lwz	9,8(4)
	lwz	10,12(4)
	xor	16,16,7
	lwz	7,16(4)
	xor	17,17,8
	lwz	8,20(4)
	xor	18,18,9
	lwz	9,24(4)
	xor	19,19,10
	lwz	10,28(4)
	xor	20,20,7
	lwz	7,32(4)
	xor	21,21,8
	lwz	8,36(4)
	xor	22,22,9
	lwz	9,40(4)
	xor	23,23,10
	lwz	10,44(4)
	xor	24,24,7
	lwz	7,48(4)
	xor	25,25,8
	lwz	8,52(4)
	xor	26,26,9
	lwz	9,56(4)
	xor	27,27,10
	lwz	10,60(4)
	xor	28,28,7
	stw	16,0(3)
	xor	29,29,8
	stw	17,4(3)
	xor	30,30,9
	stw	18,8(3)
	xor	31,31,10
	stw	19,12(3)
	stw	20,16(3)
	stw	21,20(3)
	stw	22,24(3)
	stw	23,28(3)
	stw	24,32(3)
	stw	25,36(3)
	stw	26,40(3)
	stw	27,44(3)
	stw	28,48(3)
	stw	29,52(3)
	stw	30,56(3)
	addi	4,4,64
	stw	31,60(3)
	addi	3,3,64

	bne	.Loop_outer

	blr

.align	4
.Ltail:
	addi	5,5,64
	subi	4,4,1
	subi	3,3,1
	addi	7,1,48-1
	mtctr	5

	stw	16,48(1)
	stw	17,52(1)
	stw	18,56(1)
	stw	19,60(1)
	stw	20,64(1)
	stw	21,68(1)
	stw	22,72(1)
	stw	23,76(1)
	stw	24,80(1)
	stw	25,84(1)
	stw	26,88(1)
	stw	27,92(1)
	stw	28,96(1)
	stw	29,100(1)
	stw	30,104(1)
	stw	31,108(1)

.Loop_tail:
	lbzu	11,1(4)
	lbzu	16,1(7)
	xor	12,11,16
	stbu	12,1(3)
	bdnz	.Loop_tail

	stw	1,48(1)
	stw	1,52(1)
	stw	1,56(1)
	stw	1,60(1)
	stw	1,64(1)
	stw	1,68(1)
	stw	1,72(1)
	stw	1,76(1)
	stw	1,80(1)
	stw	1,84(1)
	stw	1,88(1)
	stw	1,92(1)
	stw	1,96(1)
	stw	1,100(1)
	stw	1,104(1)
	stw	1,108(1)

	blr
.long	0
.byte	0,12,0x14,0,0,0,0,0

.globl	ChaCha20_ctr32_vmx
.type	ChaCha20_ctr32_vmx,@function
.section	".opd","aw"
.align	3
ChaCha20_ctr32_vmx:
.quad	.ChaCha20_ctr32_vmx,.TOC.@tocbase,0
.previous

.align	5
.ChaCha20_ctr32_vmx:
	cmpldi	5,256
	blt	__ChaCha20_ctr32_int

	stdu	1,-464(1)
	mflr	0
	li	10,127
	li	11,143
	mfspr	12,256
	stvx	20,10,1
	addi	10,10,32
	stvx	21,11,1
	addi	11,11,32
	stvx	22,10,1
	addi	10,10,32
	stvx	23,11,1
	addi	11,11,32
	stvx	24,10,1
	addi	10,10,32
	stvx	25,11,1
	addi	11,11,32
	stvx	26,10,1
	addi	10,10,32
	stvx	27,11,1
	addi	11,11,32
	stvx	28,10,1
	addi	10,10,32
	stvx	29,11,1
	addi	11,11,32
	stvx	30,10,1
	stvx	31,11,1
	stw	12,316(1)
	std	14,320(1)
	std	15,328(1)
	std	16,336(1)
	std	17,344(1)
	std	18,352(1)
	std	19,360(1)
	std	20,368(1)
	std	21,376(1)
	std	22,384(1)
	std	23,392(1)
	std	24,400(1)
	std	25,408(1)
	std	26,416(1)
	std	27,424(1)
	std	28,432(1)
	std	29,440(1)
	std	30,448(1)
	std	31,456(1)
	li	12,-1
	std	0, 480(1)
	mtspr	256,12

	bl	.Lconsts
	li	16,16
	li	17,32
	li	18,48
	li	19,64
	li	20,31
	li	21,15

	lvx	16,0,6
	lvsl	12,0,6
	lvx	17,16,6
	lvx	31,20,6

	lvx	18,0,7
	lvsl	13,0,7
	lvx	27,21,7

	lvx	15,0,12
	lvx	20,16,12
	lvx	21,17,12
	lvx	22,18,12
	lvx	23,19,12

	vperm	16,16,17,12
	vperm	17,17,31,12
	vperm	18,18,27,13

	lwz	11,0(7)
	lwz	12,4(7)
	vadduwm	18,18,20
	lwz	14,8(7)
	vadduwm	19,18,20
	lwz	15,12(7)
	vadduwm	20,19,20

	vspltisw	24,-12
	vspltisw	25,12
	vspltisw	26,-7


	vxor	12,12,12
	vspltisw	30,-1
	lvsl	28,0,4
	lvsr	29,0,3
	vperm	30,12,30,29

	lvsl	12,0,16
	vspltisb	13,3
	vxor	12,12,13
	vxor	29,29,13
	vperm	28,28,28,12

	b	.Loop_outer_vmx

.align	4
.Loop_outer_vmx:
	lis	16,0x6170
	lis	17,0x3320
	vor	0,15,15
	lis	18,0x7962
	lis	19,0x6b20
	vor	4,15,15
	ori	16,16,0x7865
	ori	17,17,0x646e
	vor	8,15,15
	ori	18,18,0x2d32
	ori	19,19,0x6574
	vor	1,16,16

	li	0,10
	lwz	20,0(6)
	vor	5,16,16
	lwz	21,4(6)
	vor	9,16,16
	lwz	22,8(6)
	vor	2,17,17
	lwz	23,12(6)
	vor	6,17,17
	lwz	24,16(6)
	vor	10,17,17
	mr	28,11
	lwz	25,20(6)
	vor	3,18,18
	mr	29,12
	lwz	26,24(6)
	vor	7,19,19
	mr	30,14
	lwz	27,28(6)
	vor	11,20,20
	mr	31,15

	mr	7,20
	mr	8,21
	mr	9,22
	mr	10,23
	vspltisw	27,7

	mtctr	0
	nop
.Loop_vmx:
	vadduwm	0,0,1
	add	16,16,20
	vadduwm	4,4,5
	add	17,17,21
	vadduwm	8,8,9
	add	18,18,22
	vxor	3,3,0
	add	19,19,23
	vxor	7,7,4
	xor	28,28,16
	vxor	11,11,8
	xor	29,29,17
	vperm	3,3,3,22
	xor	30,30,18
	vperm	7,7,7,22
	xor	31,31,19
	vperm	11,11,11,22
	rotlwi	28,28,16
	vadduwm	2,2,3
	rotlwi	29,29,16
	vadduwm	6,6,7
	rotlwi	30,30,16
	vadduwm	10,10,11
	rotlwi	31,31,16
	vxor	12,1,2
	add	24,24,28
	vxor	13,5,6
	add	25,25,29
	vxor	14,9,10
	add	26,26,30
	vsrw	1,12,24
	add	27,27,31
	vsrw	5,13,24
	xor	20,20,24
	vsrw	9,14,24
	xor	21,21,25
	vslw	12,12,25
	xor	22,22,26
	vslw	13,13,25
	xor	23,23,27
	vslw	14,14,25
	rotlwi	20,20,12
	vor	1,1,12
	rotlwi	21,21,12
	vor	5,5,13
	rotlwi	22,22,12
	vor	9,9,14
	rotlwi	23,23,12
	vadduwm	0,0,1
	add	16,16,20
	vadduwm	4,4,5
	add	17,17,21
	vadduwm	8,8,9
	add	18,18,22
	vxor	3,3,0
	add	19,19,23
	vxor	7,7,4
	xor	28,28,16
	vxor	11,11,8
	xor	29,29,17
	vperm	3,3,3,23
	xor	30,30,18
	vperm	7,7,7,23
	xor	31,31,19
	vperm	11,11,11,23
	rotlwi	28,28,8
	vadduwm	2,2,3
	rotlwi	29,29,8
	vadduwm	6,6,7
	rotlwi	30,30,8
	vadduwm	10,10,11
	rotlwi	31,31,8
	vxor	12,1,2
	add	24,24,28
	vxor	13,5,6
	add	25,25,29
	vxor	14,9,10
	add	26,26,30
	vsrw	1,12,26
	add	27,27,31
	vsrw	5,13,26
	xor	20,20,24
	vsrw	9,14,26
	xor	21,21,25
	vslw	12,12,27
	xor	22,22,26
	vslw	13,13,27
	xor	23,23,27
	vslw	14,14,27
	rotlwi	20,20,7
	vor	1,1,12
	rotlwi	21,21,7
	vor	5,5,13
	rotlwi	22,22,7
	vor	9,9,14
	rotlwi	23,23,7
	vsldoi	2,2,2, 16-8
	vsldoi	6,6,6, 16-8
	vsldoi	10,10,10, 16-8
	vsldoi	1,1,1, 16-12
	vsldoi	5,5,5, 16-12
	vsldoi	9,9,9, 16-12
	vsldoi	3,3,3, 16-4
	vsldoi	7,7,7, 16-4
	vsldoi	11,11,11, 16-4
	vadduwm	0,0,1
	add	16,16,21
	vadduwm	4,4,5
	add	17,17,22
	vadduwm	8,8,9
	add	18,18,23
	vxor	3,3,0
	add	19,19,20
	vxor	7,7,4
	xor	31,31,16
	vxor	11,11,8
	xor	28,28,17
	vperm	3,3,3,22
	xor	29,29,18
	vperm	7,7,7,22
	xor	30,30,19
	vperm	11,11,11,22
	rotlwi	31,31,16
	vadduwm	2,2,3
	rotlwi	28,28,16
	vadduwm	6,6,7
	rotlwi	29,29,16
	vadduwm	10,10,11
	rotlwi	30,30,16
	vxor	12,1,2
	add	26,26,31
	vxor	13,5,6
	add	27,27,28
	vxor	14,9,10
	add	24,24,29
	vsrw	1,12,24
	add	25,25,30
	vsrw	5,13,24
	xor	21,21,26
	vsrw	9,14,24
	xor	22,22,27
	vslw	12,12,25
	xor	23,23,24
	vslw	13,13,25
	xor	20,20,25
	vslw	14,14,25
	rotlwi	21,21,12
	vor	1,1,12
	rotlwi	22,22,12
	vor	5,5,13
	rotlwi	23,23,12
	vor	9,9,14
	rotlwi	20,20,12
	vadduwm	0,0,1
	add	16,16,21
	vadduwm	4,4,5
	add	17,17,22
	vadduwm	8,8,9
	add	18,18,23
	vxor	3,3,0
	add	19,19,20
	vxor	7,7,4
	xor	31,31,16
	vxor	11,11,8
	xor	28,28,17
	vperm	3,3,3,23
	xor	29,29,18
	vperm	7,7,7,23
	xor	30,30,19
	vperm	11,11,11,23
	rotlwi	31,31,8
	vadduwm	2,2,3
	rotlwi	28,28,8
	vadduwm	6,6,7
	rotlwi	29,29,8
	vadduwm	10,10,11
	rotlwi	30,30,8
	vxor	12,1,2
	add	26,26,31
	vxor	13,5,6
	add	27,27,28
	vxor	14,9,10
	add	24,24,29
	vsrw	1,12,26
	add	25,25,30
	vsrw	5,13,26
	xor	21,21,26
	vsrw	9,14,26
	xor	22,22,27
	vslw	12,12,27
	xor	23,23,24
	vslw	13,13,27
	xor	20,20,25
	vslw	14,14,27
	rotlwi	21,21,7
	vor	1,1,12
	rotlwi	22,22,7
	vor	5,5,13
	rotlwi	23,23,7
	vor	9,9,14
	rotlwi	20,20,7
	vsldoi	2,2,2, 16-8
	vsldoi	6,6,6, 16-8
	vsldoi	10,10,10, 16-8
	vsldoi	1,1,1, 16-4
	vsldoi	5,5,5, 16-4
	vsldoi	9,9,9, 16-4
	vsldoi	3,3,3, 16-12
	vsldoi	7,7,7, 16-12
	vsldoi	11,11,11, 16-12
	bdnz	.Loop_vmx

	subi	5,5,256
	addi	16,16,0x7865
	addi	17,17,0x646e
	addi	18,18,0x2d32
	addi	19,19,0x6574
	addis	16,16,0x6170
	addis	17,17,0x3320
	addis	18,18,0x7962
	addis	19,19,0x6b20
	add	20,20,7
	lwz	7,16(6)
	add	21,21,8
	lwz	8,20(6)
	add	22,22,9
	lwz	9,24(6)
	add	23,23,10
	lwz	10,28(6)
	add	24,24,7
	add	25,25,8
	add	26,26,9
	add	27,27,10
	add	28,28,11
	add	29,29,12
	add	30,30,14
	add	31,31,15

	vadduwm	0,0,15
	vadduwm	4,4,15
	vadduwm	8,8,15
	vadduwm	1,1,16
	vadduwm	5,5,16
	vadduwm	9,9,16
	vadduwm	2,2,17
	vadduwm	6,6,17
	vadduwm	10,10,17
	vadduwm	3,3,18
	vadduwm	7,7,19
	vadduwm	11,11,20

	addi	11,11,4
	vadduwm	18,18,21
	vadduwm	19,19,21
	vadduwm	20,20,21

	mr	7,16
	rotlwi	16,16,8
	rlwimi	16,7,24,0,7
	rlwimi	16,7,24,16,23
	mr	8,17
	rotlwi	17,17,8
	rlwimi	17,8,24,0,7
	rlwimi	17,8,24,16,23
	mr	9,18
	rotlwi	18,18,8
	rlwimi	18,9,24,0,7
	rlwimi	18,9,24,16,23
	mr	10,19
	rotlwi	19,19,8
	rlwimi	19,10,24,0,7
	rlwimi	19,10,24,16,23
	mr	7,20
	rotlwi	20,20,8
	rlwimi	20,7,24,0,7
	rlwimi	20,7,24,16,23
	mr	8,21
	rotlwi	21,21,8
	rlwimi	21,8,24,0,7
	rlwimi	21,8,24,16,23
	mr	9,22
	rotlwi	22,22,8
	rlwimi	22,9,24,0,7
	rlwimi	22,9,24,16,23
	mr	10,23
	rotlwi	23,23,8
	rlwimi	23,10,24,0,7
	rlwimi	23,10,24,16,23
	mr	7,24
	rotlwi	24,24,8
	rlwimi	24,7,24,0,7
	rlwimi	24,7,24,16,23
	mr	8,25
	rotlwi	25,25,8
	rlwimi	25,8,24,0,7
	rlwimi	25,8,24,16,23
	mr	9,26
	rotlwi	26,26,8
	rlwimi	26,9,24,0,7
	rlwimi	26,9,24,16,23
	mr	10,27
	rotlwi	27,27,8
	rlwimi	27,10,24,0,7
	rlwimi	27,10,24,16,23
	mr	7,28
	rotlwi	28,28,8
	rlwimi	28,7,24,0,7
	rlwimi	28,7,24,16,23
	mr	8,29
	rotlwi	29,29,8
	rlwimi	29,8,24,0,7
	rlwimi	29,8,24,16,23
	mr	9,30
	rotlwi	30,30,8
	rlwimi	30,9,24,0,7
	rlwimi	30,9,24,16,23
	mr	10,31
	rotlwi	31,31,8
	rlwimi	31,10,24,0,7
	rlwimi	31,10,24,16,23
	lwz	7,0(4)
	lwz	8,4(4)
	lwz	9,8(4)
	lwz	10,12(4)
	xor	16,16,7
	lwz	7,16(4)
	xor	17,17,8
	lwz	8,20(4)
	xor	18,18,9
	lwz	9,24(4)
	xor	19,19,10
	lwz	10,28(4)
	xor	20,20,7
	lwz	7,32(4)
	xor	21,21,8
	lwz	8,36(4)
	xor	22,22,9
	lwz	9,40(4)
	xor	23,23,10
	lwz	10,44(4)
	xor	24,24,7
	lwz	7,48(4)
	xor	25,25,8
	lwz	8,52(4)
	xor	26,26,9
	lwz	9,56(4)
	xor	27,27,10
	lwz	10,60(4)
	xor	28,28,7
	stw	16,0(3)
	xor	29,29,8
	stw	17,4(3)
	xor	30,30,9
	stw	18,8(3)
	xor	31,31,10
	stw	19,12(3)
	addi	4,4,64
	stw	20,16(3)
	li	7,16
	stw	21,20(3)
	li	8,32
	stw	22,24(3)
	li	9,48
	stw	23,28(3)
	li	10,64
	stw	24,32(3)
	stw	25,36(3)
	stw	26,40(3)
	stw	27,44(3)
	stw	28,48(3)
	stw	29,52(3)
	stw	30,56(3)
	stw	31,60(3)
	addi	3,3,64

	lvx	31,0,4
	lvx	27,7,4
	lvx	12,8,4
	lvx	13,9,4
	lvx	14,10,4
	addi	4,4,64

	vperm	31,31,27,28
	vperm	27,27,12,28
	vperm	12,12,13,28
	vperm	13,13,14,28
	vxor	0,0,31
	vxor	1,1,27
	lvx	27,7,4
	vxor	2,2,12
	lvx	12,8,4
	vxor	3,3,13
	lvx	13,9,4
	lvx	31,10,4
	addi	4,4,64
	li	10,63
	vperm	0,0,0,29
	vperm	1,1,1,29
	vperm	2,2,2,29
	vperm	3,3,3,29

	vperm	14,14,27,28
	vperm	27,27,12,28
	vperm	12,12,13,28
	vperm	13,13,31,28
	vxor	4,4,14
	vxor	5,5,27
	lvx	27,7,4
	vxor	6,6,12
	lvx	12,8,4
	vxor	7,7,13
	lvx	13,9,4
	lvx	14,10,4
	addi	4,4,64
	vperm	4,4,4,29
	vperm	5,5,5,29
	vperm	6,6,6,29
	vperm	7,7,7,29

	vperm	31,31,27,28
	vperm	27,27,12,28
	vperm	12,12,13,28
	vperm	13,13,14,28
	vxor	8,8,31
	vxor	9,9,27
	vxor	10,10,12
	vxor	11,11,13
	vperm	8,8,8,29
	vperm	9,9,9,29
	vperm	10,10,10,29
	vperm	11,11,11,29

	andi.	17,3,15
	mr	16,3

	vsel	31,0,1,30
	vsel	27,1,2,30
	vsel	12,2,3,30
	vsel	13,3,4,30
	vsel	1,4,5,30
	vsel	2,5,6,30
	vsel	3,6,7,30
	vsel	4,7,8,30
	vsel	5,8,9,30
	vsel	6,9,10,30
	vsel	7,10,11,30


	stvx	31,7,3
	stvx	27,8,3
	stvx	12,9,3
	addi	3,3,64
	stvx	13,0,3
	stvx	1,7,3
	stvx	2,8,3
	stvx	3,9,3
	addi	3,3,64
	stvx	4,0,3
	stvx	5,7,3
	stvx	6,8,3
	stvx	7,9,3
	addi	3,3,64

	beq	.Laligned_vmx

	sub	18,3,17
	li	19,0
.Lunaligned_tail_vmx:
	stvebx	11,19,18
	addi	19,19,1
	cmpw	19,17
	bne	.Lunaligned_tail_vmx

	sub	18,16,17
.Lunaligned_head_vmx:
	stvebx	0,17,18
	cmpwi	17,15
	addi	17,17,1
	bne	.Lunaligned_head_vmx

	cmpldi	5,255
	bgt	.Loop_outer_vmx

	b	.Ldone_vmx

.align	4
.Laligned_vmx:
	stvx	0,0,16

	cmpldi	5,255
	bgt	.Loop_outer_vmx
	nop

.Ldone_vmx:
	cmpldi	5,0
	bnel	__ChaCha20_1x

	lwz	12,316(1)
	li	10,127
	li	11,143
	mtspr	256,12
	lvx	20,10,1
	addi	10,10,32
	lvx	21,11,1
	addi	11,11,32
	lvx	22,10,1
	addi	10,10,32
	lvx	23,11,1
	addi	11,11,32
	lvx	24,10,1
	addi	10,10,32
	lvx	25,11,1
	addi	11,11,32
	lvx	26,10,1
	addi	10,10,32
	lvx	27,11,1
	addi	11,11,32
	lvx	28,10,1
	addi	10,10,32
	lvx	29,11,1
	addi	11,11,32
	lvx	30,10,1
	lvx	31,11,1
	ld	0, 480(1)
	ld	14,320(1)
	ld	15,328(1)
	ld	16,336(1)
	ld	17,344(1)
	ld	18,352(1)
	ld	19,360(1)
	ld	20,368(1)
	ld	21,376(1)
	ld	22,384(1)
	ld	23,392(1)
	ld	24,400(1)
	ld	25,408(1)
	ld	26,416(1)
	ld	27,424(1)
	ld	28,432(1)
	ld	29,440(1)
	ld	30,448(1)
	ld	31,456(1)
	mtlr	0
	addi	1,1,464
	blr
.long	0
.byte	0,12,0x04,1,0x80,18,5,0
.long	0
.size	ChaCha20_ctr32_vmx,.-.ChaCha20_ctr32_vmx
.size	.ChaCha20_ctr32_vmx,.-.ChaCha20_ctr32_vmx

.align	5
.Lconsts:
	mflr	0
	bcl	20,31,$+4
	mflr	12
	addi	12,12,56
	mtlr	0
	blr
.long	0
.byte	0,12,0x14,0,0,0,0,0
.space	28
.Lsigma:
.long	0x61707865,0x3320646e,0x79622d32,0x6b206574
.long	1,0,0,0
.long	4,0,0,0
.long	0x02030001,0x06070405,0x0a0b0809,0x0e0f0c0d
.long	0x01020300,0x05060704,0x090a0b08,0x0d0e0f0c
.byte	67,104,97,67,104,97,50,48,32,102,111,114,32,80,111,119,101,114,80,67,47,65,108,116,105,86,101,99,44,32,67,82,89,80,84,79,71,65,77,83,32,98,121,32,60,97,112,112,114,111,64,111,112,101,110,115,115,108,46,111,114,103,62,0
.align	2
.align	2
