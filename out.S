	.file	"Testing/while.p"
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
	.globl	main1
	.type	main1,	@function
main1:
	pushq	%rbp
	movq	%rsp,	%rbp
	addq	$-8,	%rsp
	movq	$0,	%rcx
	movq	%rcx,	-8(%rbp)
.L0:
	movq	$1,	%rcx
	movq	$10,%rax
	cmp	%rax,	%rcx
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
	movq	%rbp,	%rsp
	popq	%rbp
	ret
	.size	main1,	.-main1
	.ident	"GCC: (Ubuntu 4.8.5-2ubuntu1-14.04.1) 4.8.5"
	.section	.note.GNU-stack,"",@progbits
