	.file	"Testing/while.p"
	.text
	.globl	main
	.type	main,	@function
main:
	pushq	%rbp
	pushq	%rbp
	movq	%rsp,	%rbp
	movl	$0,	%eax
	call	main1
	popq	%rbp
	popq	%rbp
	ret
	.size	main,	.-main
.LC1:
	.string	"%ld\n"
.LC0:
	.string	"%ld"
	.globl	main1
	.type	main1,	@function
main1:
	pushq	%rbp
	movq	%rsp,	%rbp
	addq	$-8,	%rsp
	pushq	%rcx
	pushq	%r8
	pushq	%r9
	pushq	%r10
	pushq	%r11
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
	movq	$2,	%rcx
	movq	%rcx,	-8(%rbp)
.L0:
	movq	$10,	%rcx
	movq	$10,	%r8
	addq	$1,	%r8
	imul	%r8,	%rcx
	imul	$10,	%rcx
	imul	$10,	%rcx
	movq	$10,	%r9
	imul	$10,	%r9
	movq	$10,	%r8
	addq	$10,	%r8
	addq	$2,	%r8
	imul	%r8,	%r9
	addq	$10,	%r9
	movq	$10,	%r8
	imul	$10,	%r8
	imul	%r9,	%r8
	addq	%rcx,	%r8
	movq	$10,	%rcx
	imul	-8(%rbp),	%rcx
	cmp	%r8,	%rcx
	jge	.L1
	movq	-8(%rbp),	%rcx
	addq	$1,	%rcx
	movq	%rcx,	-8(%rbp)
	movq	-8(%rbp),	%rcx
	movq	%rcx,	%rsi
	movq	$.LC1,	%rdi
	movq	$0,	%rax
	call	printf
	jmp	.L0
.L1:
	movq	-8(%rbp),	%rcx
	movq	%rcx,	%rsi
	movq	$.LC1,	%rdi
	movq	$0,	%rax
	call	printf
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%r11
	popq	%r10
	popq	%r9
	popq	%r8
	popq	%rcx
	movq	%rbp,	%rsp
	popq	%rbp
	ret
	.size	main1,	.-main1
	.ident	"GCC: (Ubuntu 4.8.5-2ubuntu1-14.04.1) 4.8.5"
	.section	.note.GNU-stack,"",@progbits
