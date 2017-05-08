	.file	"Testing/reflex-action.p"
	.text
	.globl	main
	.type	main,	@function
main:
	pushq	%rbp
	movq	%rsp,	%rbp
	movl	$0,	%eax
	call	main1
	popq	%rbp
	ret
	.size	main,	.-main
.LC1:
	.string	"%d\n"
.LC0:
	.string	"%d"
	.globl	gcd
	.type	gcd,	@function
gcd:
	pushq	%rbp
	movq	%rsp,	%rbp
	addq	$0,	%rsp
	movq	16(%rbp),	%rcx
	movq	%rcx,	%rsi
	movq	$.LC1,	%rdi
	movq	$0,	%rax
	call	printf
	movq	24(%rbp),	%rcx
	movq	%rcx,	%rsi
	movq	$.LC1,	%rdi
	movq	$0,	%rax
	call	printf
	movq	24(%rbp),	%rcx
	movq	$1,%rax
	cmp	%rax,	%rcx
	jl	.L0
	movq	16(%rbp),	%rcx
	movq	%rcx,	%rax
	movq	$0,	%rdx
	idivq	24(%rbp)
	movq	%rax,	%rcx
	movq	%rcx,	-8(%rbp)
	movq	16(%rbp),	%rcx
	movq	24(%rbp),	%r8
	imulq	-8(%rbp),	%r8
	subq	%r8,	%rcx
	movq	%rcx,	-16(%rbp)
	movq	-16(%rbp),	%rcx
	pushq	%rcx
	movq	24(%rbp),	%rcx
	pushq	%rcx
	call	gcd
	movq	%rcx,	%rax
	jmp	.L1
.L0:
	movq	16(%rbp),	%rcx
	movq	%rcx,	%rax
.L1:
	movq	%rbp,	%rsp
	popq	%rbp
	ret
	.size	gcd,	.-gcd
	.globl	main1
	.type	main1,	@function
main1:
	pushq	%rbp
	movq	%rsp,	%rbp
	addq	$-16,	%rsp
	leaq	-8(%rbp),	%rax
	movq	%rax,	%rsi
	movq	$.LC0,	%rdi
	movq	$0,	%rax
	call	__isoc99_scanf
	leaq	-16(%rbp),	%rax
	movq	%rax,	%rsi
	movq	$.LC0,	%rdi
	movq	$0,	%rax
	call	__isoc99_scanf
	movq	-16(%rbp),	%rcx
	pushq	%rcx
	movq	-8(%rbp),	%rcx
	pushq	%rcx
	call	gcd
	movq	%rcx,	%rsi
	movq	$.LC1,	%rdi
	movq	$0,	%rax
	call	printf
	movq	%rbp,	%rsp
	popq	%rbp
	ret
	.size	main1,	.-main1
	.ident	"GCC: (Ubuntu 4.8.5-2ubuntu1-14.04.1) 4.8.5"
	.section	.note.GNU-stack,"",@progbits
