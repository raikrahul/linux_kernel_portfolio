
reloc_test.ko:     file format elf64-x86-64


Disassembly of section .init.text:

0000000000000000 <__pfx_init_module>:
   0:	90                   	nop
   1:	90                   	nop
   2:	90                   	nop
   3:	90                   	nop
   4:	90                   	nop
   5:	90                   	nop
   6:	90                   	nop
   7:	90                   	nop
   8:	90                   	nop
   9:	90                   	nop
   a:	90                   	nop
   b:	90                   	nop
   c:	90                   	nop
   d:	90                   	nop
   e:	90                   	nop
   f:	90                   	nop

0000000000000010 <init_module>:
  10:	e8 00 00 00 00       	call   15 <init_module+0x5>
  15:	55                   	push   %rbp
  16:	b8 ef be ad de       	mov    $0xdeadbeef,%eax
  1b:	48 c7 05 00 00 00 00 	movq   $0x11111111,0x0(%rip)        # 26 <init_module+0x16>
  22:	11 11 11 11 
  26:	48 c7 05 00 00 00 00 	movq   $0x22222222,0x0(%rip)        # 31 <init_module+0x21>
  2d:	22 22 22 22 
  31:	48 c7 05 00 00 00 00 	movq   $0x33333333,0x0(%rip)        # 3c <init_module+0x2c>
  38:	33 33 33 33 
  3c:	48 89 e5             	mov    %rsp,%rbp
  3f:	5d                   	pop    %rbp
  40:	48 c7 05 00 00 00 00 	movq   $0x44444444,0x0(%rip)        # 4b <init_module+0x3b>
  47:	44 44 44 44 
  4b:	48 c7 05 00 00 00 00 	movq   $0x55555555,0x0(%rip)        # 56 <init_module+0x46>
  52:	55 55 55 55 
  56:	48 89 05 00 00 00 00 	mov    %rax,0x0(%rip)        # 5d <init_module+0x4d>
  5d:	31 c0                	xor    %eax,%eax
  5f:	e9 00 00 00 00       	jmp    64 <__UNIQUE_ID_vermagic467+0x10>

Disassembly of section .exit.text:

0000000000000000 <__pfx_cleanup_module>:
   0:	90                   	nop
   1:	90                   	nop
   2:	90                   	nop
   3:	90                   	nop
   4:	90                   	nop
   5:	90                   	nop
   6:	90                   	nop
   7:	90                   	nop
   8:	90                   	nop
   9:	90                   	nop
   a:	90                   	nop
   b:	90                   	nop
   c:	90                   	nop
   d:	90                   	nop
   e:	90                   	nop
   f:	90                   	nop

0000000000000010 <cleanup_module>:
  10:	b8 aa aa aa aa       	mov    $0xaaaaaaaa,%eax
  15:	48 89 05 00 00 00 00 	mov    %rax,0x0(%rip)        # 1c <cleanup_module+0xc>
  1c:	31 c0                	xor    %eax,%eax
  1e:	e9 00 00 00 00       	jmp    23 <global_var_1+0x3>
