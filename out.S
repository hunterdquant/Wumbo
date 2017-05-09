	.file	"boo.p"
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
