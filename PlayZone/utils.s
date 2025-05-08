	.extern drawWithDelay
	.extern screenWs
	.extern screenHs
	
	.data
count:
	.word 0
	
	.text
	.global close_window
close_window:
	mov #0,r5		;left
	mov &screenWs,r6	;right
	sub #1,r6
	mov #0,r7		;top
	mov &screenHs,r8	;bottom
	sub #1,r8		;bottom = screenH - 1
	mov #0,r9		;col
	mov #0,r10		;row
	mov #0,r11		;count
TOP:
	cmp r5,r6		;jmp if carry a>b on borrow
	jnc end
	cmp r7,r8
	jnc end
InTOP:
	cmp r10,r8
	jnc rightloop
	mov r9,r12
	mov r10,r13
	mov #count,r14
	call #drawWithDelay
	add #1,r10
	jmp InTOP
rightloop:
	sub #1,r10
	add #1,r5
	add #1,r9
InTOP2:
	cmp r9,r6		;jmp if carry a>b on borrow
	jnc uploop
	mov r9,r12
	mov r10,r13
	mov #count,r14
	call #drawWithDelay
	add #1,r9
	jmp InTOP2
uploop:
	sub #1,r9
	sub #1,r8
	sub #1,r10
InTOP3:
	cmp r7,r10
	jl leftloop
	mov r9,r12
	mov r10,r13
	mov #count,r14
	call #drawWithDelay
	sub #1,r10
	jmp InTOP3
leftloop:
	add #1,r10
	sub #1,r6
	sub #1,r9
InTOP4:
	cmp r5,r9
	jnc endloops
	mov r9,r12
	mov r10,r13
	mov #count,r14
	call #drawWithDelay
	sub #1,r9
	jmp InTOP4
endloops:
	add #1,r9
	add #1,r7
	add #1,r10
	jmp TOP
end:
	RETI
