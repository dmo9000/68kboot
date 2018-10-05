#NO_APP
	.file	"main.c"
	.text
	.section	.rodata
.LC0:
	.string	"BOOT>\r\n"
	.data
	.align	2
	.type	msg, @object
	.size	msg, 4
msg:
	.long	.LC0
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	link.w %fp,#0
	move.l msg,%d0
	move.l %d0,-(%sp)
	jsr puts
	addq.l #4,%sp
.L2:
	jra .L2
	.size	main, .-main
	.align	2
	.globl	putchar
	.type	putchar, @function
putchar:
	link.w %fp,#-4
	move.l #16715778,-4(%fp)
	move.l 8(%fp),%d0
	move.l -4(%fp),%a0
	move.b %d0,(%a0)
	moveq #0,%d0
	unlk %fp
	rts
	.size	putchar, .-putchar
	.align	2
	.globl	puts
	.type	puts, @function
puts:
	link.w %fp,#0
	jra .L6
.L7:
	move.l 8(%fp),%a0
	move.b (%a0),%d0
	ext.w %d0
	move.w %d0,%a0
	move.l %a0,-(%sp)
	jsr putchar
	addq.l #4,%sp
	addq.l #1,8(%fp)
.L6:
	move.l 8(%fp),%a0
	move.b (%a0),%d0
	tst.b %d0
	jne .L7
	moveq #0,%d0
	unlk %fp
	rts
	.size	puts, .-puts
	.align	2
	.globl	strlen
	.type	strlen, @function
strlen:
	link.w %fp,#-4
	clr.l -4(%fp)
	jra .L10
.L11:
	addq.l #1,-4(%fp)
.L10:
	move.l 8(%fp),%d0
	move.l %d0,%d1
	addq.l #1,%d1
	move.l %d1,8(%fp)
	move.l %d0,%a0
	move.b (%a0),%d0
	tst.b %d0
	jne .L11
	move.l -4(%fp),%d0
	unlk %fp
	rts
	.size	strlen, .-strlen
	.ident	"GCC: (GNU) 8.2.0"
