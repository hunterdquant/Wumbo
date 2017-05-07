	.file	"Testing/proc_func.p"
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
	.globl	fact
	.type	fact,	@function
fact:
	pushq	%rbp
	movq	%rsp,	%rbp
	addq	$-8,	%rsp
	movq	%rbp,	%rsp
	popq	%rbp
	ret
	.size	fact,	.-fact
	.globl	main1
	.type	main1,	@function
main1:
	pushq	%rbp
	movq	%rsp,	%rbp
	addq	$-8,	%rsp
	leaq	-8(%rbp),	%rax
	movq	%rax,	%rsi
	movq	$.LC0,	%rdi
	movq	$0,	%rax
	call	__isoc99_scanf
	movq	-8(%rbp),	%rcx
	imulq	-8(%rbp),	%rcx
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
