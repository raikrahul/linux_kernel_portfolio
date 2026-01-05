
pagewalk_driver.ko:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <__pfx_proc_write>:
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

0000000000000010 <proc_write>:
  10:	e8 00 00 00 00       	call   15 <proc_write+0x5>
  15:	55                   	push   %rbp
  16:	b9 08 00 00 00       	mov    $0x8,%ecx
  1b:	48 89 e5             	mov    %rsp,%rbp
  1e:	41 55                	push   %r13
  20:	49 89 f5             	mov    %rsi,%r13
  23:	41 54                	push   %r12
  25:	4c 8d 65 a0          	lea    -0x60(%rbp),%r12
  29:	53                   	push   %rbx
  2a:	4c 89 e7             	mov    %r12,%rdi
  2d:	48 83 ec 50          	sub    $0x50,%rsp
  31:	65 48 8b 04 25 28 00 	mov    %gs:0x28,%rax
  38:	00 00 
  3a:	48 89 45 e0          	mov    %rax,-0x20(%rbp)
  3e:	31 c0                	xor    %eax,%eax
  40:	f3 48 ab             	rep stos %rax,%es:(%rdi)
  43:	b8 3f 00 00 00       	mov    $0x3f,%eax
  48:	4c 89 e7             	mov    %r12,%rdi
  4b:	48 39 c2             	cmp    %rax,%rdx
  4e:	48 0f 46 c2          	cmovbe %rdx,%rax
  52:	31 d2                	xor    %edx,%edx
  54:	48 89 c3             	mov    %rax,%rbx
  57:	48 89 c6             	mov    %rax,%rsi
  5a:	e8 00 00 00 00       	call   5f <proc_write+0x4f>
  5f:	48 89 da             	mov    %rbx,%rdx
  62:	4c 89 ee             	mov    %r13,%rsi
  65:	4c 89 e7             	mov    %r12,%rdi
  68:	e8 00 00 00 00       	call   6d <proc_write+0x5d>
  6d:	48 85 c0             	test   %rax,%rax
  70:	0f 85 8f 00 00 00    	jne    105 <proc_write+0xf5>
  76:	48 83 fb 40          	cmp    $0x40,%rbx
  7a:	0f 83 a3 00 00 00    	jae    123 <proc_write+0x113>
  80:	c6 44 1d a0 00       	movb   $0x0,-0x60(%rbp,%rbx,1)
  85:	81 7d a0 73 74 61 63 	cmpl   $0x63617473,-0x60(%rbp)
  8c:	74 51                	je     df <proc_write+0xcf>
  8e:	31 f6                	xor    %esi,%esi
  90:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
  97:	4c 89 e7             	mov    %r12,%rdi
  9a:	e8 00 00 00 00       	call   9f <proc_write+0x8f>
  9f:	85 c0                	test   %eax,%eax
  a1:	78 6b                	js     10e <proc_write+0xfe>
  a3:	48 8b 35 00 00 00 00 	mov    0x0(%rip),%rsi        # aa <proc_write+0x9a>
  aa:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
  b1:	e8 00 00 00 00       	call   b6 <proc_write+0xa6>
  b6:	48 89 d8             	mov    %rbx,%rax
  b9:	48 8b 55 e0          	mov    -0x20(%rbp),%rdx
  bd:	65 48 2b 14 25 28 00 	sub    %gs:0x28,%rdx
  c4:	00 00 
  c6:	75 6f                	jne    137 <proc_write+0x127>
  c8:	48 83 c4 50          	add    $0x50,%rsp
  cc:	5b                   	pop    %rbx
  cd:	41 5c                	pop    %r12
  cf:	41 5d                	pop    %r13
  d1:	5d                   	pop    %rbp
  d2:	31 d2                	xor    %edx,%edx
  d4:	31 c9                	xor    %ecx,%ecx
  d6:	31 f6                	xor    %esi,%esi
  d8:	31 ff                	xor    %edi,%edi
  da:	e9 00 00 00 00       	jmp    df <proc_write+0xcf>
  df:	80 7d a4 6b          	cmpb   $0x6b,-0x5c(%rbp)
  e3:	75 a9                	jne    8e <proc_write+0x7e>
  e5:	48 8d 75 9c          	lea    -0x64(%rbp),%rsi
  e9:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
  f0:	c7 45 9c 2a 00 00 00 	movl   $0x2a,-0x64(%rbp)
  f7:	48 89 35 00 00 00 00 	mov    %rsi,0x0(%rip)        # fe <proc_write+0xee>
  fe:	e8 00 00 00 00       	call   103 <proc_write+0xf3>
 103:	eb 9e                	jmp    a3 <proc_write+0x93>
 105:	48 c7 c0 f2 ff ff ff 	mov    $0xfffffffffffffff2,%rax
 10c:	eb ab                	jmp    b9 <proc_write+0xa9>
 10e:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
 115:	e8 00 00 00 00       	call   11a <proc_write+0x10a>
 11a:	48 c7 c0 ea ff ff ff 	mov    $0xffffffffffffffea,%rax
 121:	eb 96                	jmp    b9 <proc_write+0xa9>
 123:	48 89 de             	mov    %rbx,%rsi
 126:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
 12d:	e8 00 00 00 00       	call   132 <proc_write+0x122>
 132:	e9 49 ff ff ff       	jmp    80 <proc_write+0x70>
 137:	e8 00 00 00 00       	call   13c <proc_write+0x12c>
 13c:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000000140 <__pfx_proc_read>:
 140:	90                   	nop
 141:	90                   	nop
 142:	90                   	nop
 143:	90                   	nop
 144:	90                   	nop
 145:	90                   	nop
 146:	90                   	nop
 147:	90                   	nop
 148:	90                   	nop
 149:	90                   	nop
 14a:	90                   	nop
 14b:	90                   	nop
 14c:	90                   	nop
 14d:	90                   	nop
 14e:	90                   	nop
 14f:	90                   	nop

0000000000000150 <proc_read>:
 150:	e8 00 00 00 00       	call   155 <proc_read+0x5>
 155:	55                   	push   %rbp
 156:	31 c0                	xor    %eax,%eax
 158:	48 89 e5             	mov    %rsp,%rbp
 15b:	41 57                	push   %r15
 15d:	41 56                	push   %r14
 15f:	41 55                	push   %r13
 161:	41 54                	push   %r12
 163:	53                   	push   %rbx
 164:	48 83 ec 40          	sub    $0x40,%rsp
 168:	48 83 39 00          	cmpq   $0x0,(%rcx)
 16c:	48 89 75 d0          	mov    %rsi,-0x30(%rbp)
 170:	0f 8f 80 00 00 00    	jg     1f6 <proc_read+0xa6>
 176:	4c 8b 25 00 00 00 00 	mov    0x0(%rip),%r12        # 17d <proc_read+0x2d>
 17d:	49 89 ce             	mov    %rcx,%r14
 180:	4d 85 e4             	test   %r12,%r12
 183:	0f 85 94 00 00 00    	jne    21d <proc_read+0xcd>
 189:	b9 10 00 00 00       	mov    $0x10,%ecx
 18e:	48 c7 c0 00 00 00 00 	mov    $0x0,%rax
 195:	bb 82 00 00 00       	mov    $0x82,%ebx
 19a:	48 c7 c6 00 00 00 00 	mov    $0x0,%rsi
 1a1:	48 89 c7             	mov    %rax,%rdi
 1a4:	41 bf 82 00 00 00    	mov    $0x82,%r15d
 1aa:	f3 48 a5             	rep movsq %ds:(%rsi),%es:(%rdi)
 1ad:	0f b7 06             	movzwl (%rsi),%eax
 1b0:	66 89 07             	mov    %ax,(%rdi)
 1b3:	0f b6 05 00 00 00 00 	movzbl 0x0(%rip),%eax        # 1ba <proc_read+0x6a>
 1ba:	88 05 00 00 00 00    	mov    %al,0x0(%rip)        # 1c0 <proc_read+0x70>
 1c0:	ba 01 00 00 00       	mov    $0x1,%edx
 1c5:	48 89 de             	mov    %rbx,%rsi
 1c8:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
 1cf:	e8 00 00 00 00       	call   1d4 <proc_read+0x84>
 1d4:	48 8b 7d d0          	mov    -0x30(%rbp),%rdi
 1d8:	48 89 da             	mov    %rbx,%rdx
 1db:	48 c7 c6 00 00 00 00 	mov    $0x0,%rsi
 1e2:	e8 00 00 00 00       	call   1e7 <proc_read+0x97>
 1e7:	48 85 c0             	test   %rax,%rax
 1ea:	0f 85 c0 08 00 00    	jne    ab0 <proc_read+0x960>
 1f0:	49 63 c7             	movslq %r15d,%rax
 1f3:	49 89 06             	mov    %rax,(%r14)
 1f6:	48 8d 65 d8          	lea    -0x28(%rbp),%rsp
 1fa:	5b                   	pop    %rbx
 1fb:	41 5c                	pop    %r12
 1fd:	41 5d                	pop    %r13
 1ff:	41 5e                	pop    %r14
 201:	41 5f                	pop    %r15
 203:	5d                   	pop    %rbp
 204:	31 d2                	xor    %edx,%edx
 206:	31 c9                	xor    %ecx,%ecx
 208:	31 f6                	xor    %esi,%esi
 20a:	31 ff                	xor    %edi,%edi
 20c:	45 31 c0             	xor    %r8d,%r8d
 20f:	45 31 c9             	xor    %r9d,%r9d
 212:	45 31 d2             	xor    %r10d,%r10d
 215:	45 31 db             	xor    %r11d,%r11d
 218:	e9 00 00 00 00       	jmp    21d <proc_read+0xcd>
 21d:	65 48 8b 05 00 00 00 	mov    %gs:0x0(%rip),%rax        # 225 <proc_read+0xd5>
 224:	00 
 225:	48 c7 c6 00 00 00 00 	mov    $0x0,%rsi
 22c:	b9 39 00 00 00       	mov    $0x39,%ecx
 231:	4c 89 e2             	mov    %r12,%rdx
 234:	81 e2 ff 0f 00 00    	and    $0xfff,%edx
 23a:	4c 8b a8 50 0a 00 00 	mov    0xa50(%rax),%r13
 241:	4c 89 e0             	mov    %r12,%rax
 244:	48 89 55 b0          	mov    %rdx,-0x50(%rbp)
 248:	48 c1 e8 27          	shr    $0x27,%rax
 24c:	48 89 45 a8          	mov    %rax,-0x58(%rbp)
 250:	49 89 c7             	mov    %rax,%r15
 253:	4c 89 e0             	mov    %r12,%rax
 256:	48 c1 e8 1e          	shr    $0x1e,%rax
 25a:	41 81 e7 ff 01 00 00 	and    $0x1ff,%r15d
 261:	25 ff 01 00 00       	and    $0x1ff,%eax
 266:	48 89 45 c8          	mov    %rax,-0x38(%rbp)
 26a:	4c 89 e0             	mov    %r12,%rax
 26d:	48 c1 e8 15          	shr    $0x15,%rax
 271:	25 ff 01 00 00       	and    $0x1ff,%eax
 276:	48 89 45 c0          	mov    %rax,-0x40(%rbp)
 27a:	4c 89 e0             	mov    %r12,%rax
 27d:	48 c1 e8 0c          	shr    $0xc,%rax
 281:	48 89 45 a0          	mov    %rax,-0x60(%rbp)
 285:	25 ff 01 00 00       	and    $0x1ff,%eax
 28a:	48 89 45 b8          	mov    %rax,-0x48(%rbp)
 28e:	48 c7 c0 00 00 00 00 	mov    $0x0,%rax
 295:	48 89 c7             	mov    %rax,%rdi
 298:	f3 48 a5             	rep movsq %ds:(%rsi),%es:(%rdi)
 29b:	0f b7 06             	movzwl (%rsi),%eax
 29e:	66 89 07             	mov    %ax,(%rdi)
 2a1:	0f b6 05 00 00 00 00 	movzbl 0x0(%rip),%eax        # 2a8 <proc_read+0x158>
 2a8:	88 05 00 00 00 00    	mov    %al,0x0(%rip)        # 2ae <proc_read+0x15e>
 2ae:	0f 20 d9             	mov    %cr3,%rcx
 2b1:	49 89 c8             	mov    %rcx,%r8
 2b4:	48 89 cb             	mov    %rcx,%rbx
 2b7:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 2be:	be 36 1e 00 00       	mov    $0x1e36,%esi
 2c3:	49 81 e0 00 f0 ff ff 	and    $0xfffffffffffff000,%r8
 2ca:	81 e3 ff 0f 00 00    	and    $0xfff,%ebx
 2d0:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
 2d7:	41 50                	push   %r8
 2d9:	49 89 d9             	mov    %rbx,%r9
 2dc:	e8 00 00 00 00       	call   2e1 <proc_read+0x191>
 2e1:	48 8b 55 b0          	mov    -0x50(%rbp),%rdx
 2e5:	4d 89 f8             	mov    %r15,%r8
 2e8:	4c 89 e1             	mov    %r12,%rcx
 2eb:	8d 98 ca 01 00 00    	lea    0x1ca(%rax),%ebx
 2f1:	be 00 20 00 00       	mov    $0x2000,%esi
 2f6:	52                   	push   %rdx
 2f7:	48 63 fb             	movslq %ebx,%rdi
 2fa:	52                   	push   %rdx
 2fb:	48 29 fe             	sub    %rdi,%rsi
 2fe:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 305:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
 30c:	ff 75 b8             	push   -0x48(%rbp)
 30f:	ff 75 c0             	push   -0x40(%rbp)
 312:	4c 8b 4d c8          	mov    -0x38(%rbp),%r9
 316:	e8 00 00 00 00       	call   31b <proc_read+0x1cb>
 31b:	be 00 20 00 00       	mov    $0x2000,%esi
 320:	01 c3                	add    %eax,%ebx
 322:	48 83 c4 28          	add    $0x28,%rsp
 326:	4d 85 ed             	test   %r13,%r13
 329:	48 63 fb             	movslq %ebx,%rdi
 32c:	0f 84 41 04 00 00    	je     773 <proc_read+0x623>
 332:	48 29 fe             	sub    %rdi,%rsi
 335:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 33c:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
 343:	81 c3 0d 01 00 00    	add    $0x10d,%ebx
 349:	e8 00 00 00 00       	call   34e <proc_read+0x1fe>
 34e:	8b 0d 00 00 00 00    	mov    0x0(%rip),%ecx        # 354 <proc_read+0x204>
 354:	4d 8b 6d 78          	mov    0x78(%r13),%r13
 358:	83 f9 3f             	cmp    $0x3f,%ecx
 35b:	0f 87 00 00 00 00    	ja     361 <proc_read+0x211>
 361:	4c 89 e0             	mov    %r12,%rax
 364:	48 d3 e8             	shr    %cl,%rax
 367:	25 ff 01 00 00       	and    $0x1ff,%eax
 36c:	4d 8d 6c c5 00       	lea    0x0(%r13,%rax,8),%r13
 371:	49 8b 7d 00          	mov    0x0(%r13),%rdi
 375:	e9 00 00 00 00       	jmp    37a <proc_read+0x22a>
 37a:	40 f6 c7 01          	test   $0x1,%dil
 37e:	49 c7 c0 00 00 00 00 	mov    $0x0,%r8
 385:	48 c7 c0 00 00 00 00 	mov    $0x0,%rax
 38c:	4c 0f 44 c0          	cmove  %rax,%r8
 390:	eb 07                	jmp    399 <proc_read+0x249>
 392:	49 c7 c0 00 00 00 00 	mov    $0x0,%r8
 399:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 39f <proc_read+0x24f>
 39f:	41 50                	push   %r8
 3a1:	48 63 fb             	movslq %ebx,%rdi
 3a4:	be 00 20 00 00       	mov    $0x2000,%esi
 3a9:	49 89 c1             	mov    %rax,%r9
 3ac:	48 29 fe             	sub    %rdi,%rsi
 3af:	4d 89 f8             	mov    %r15,%r8
 3b2:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
 3b9:	4c 89 e1             	mov    %r12,%rcx
 3bc:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 3c3:	e8 00 00 00 00       	call   3c8 <proc_read+0x278>
 3c8:	49 8b 7d 00          	mov    0x0(%r13),%rdi
 3cc:	5e                   	pop    %rsi
 3cd:	01 c3                	add    %eax,%ebx
 3cf:	e9 00 00 00 00       	jmp    3d4 <proc_read+0x284>
 3d4:	40 f6 c7 01          	test   $0x1,%dil
 3d8:	75 2a                	jne    404 <proc_read+0x2b4>
 3da:	48 63 fb             	movslq %ebx,%rdi
 3dd:	be 00 20 00 00       	mov    $0x2000,%esi
 3e2:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 3e9:	48 29 fe             	sub    %rdi,%rsi
 3ec:	44 8d 7b 25          	lea    0x25(%rbx),%r15d
 3f0:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
 3f7:	e8 00 00 00 00       	call   3fc <proc_read+0x2ac>
 3fc:	49 63 df             	movslq %r15d,%rbx
 3ff:	e9 bc fd ff ff       	jmp    1c0 <proc_read+0x70>
 404:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 40a <proc_read+0x2ba>
 40a:	48 23 05 00 00 00 00 	and    0x0(%rip),%rax        # 411 <proc_read+0x2c1>
 411:	48 63 fb             	movslq %ebx,%rdi
 414:	be 00 20 00 00       	mov    $0x2000,%esi
 419:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 420:	48 25 00 f0 ff ff    	and    $0xfffffffffffff000,%rax
 426:	48 29 fe             	sub    %rdi,%rsi
 429:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
 430:	48 89 c1             	mov    %rax,%rcx
 433:	e8 00 00 00 00       	call   438 <proc_read+0x2e8>
 438:	01 c3                	add    %eax,%ebx
 43a:	e9 00 00 00 00       	jmp    43f <proc_read+0x2ef>
 43f:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # 445 <proc_read+0x2f5>
 445:	49 8b 7d 00          	mov    0x0(%r13),%rdi
 449:	44 8d 40 ff          	lea    -0x1(%rax),%r8d
 44d:	4c 23 45 a8          	and    -0x58(%rbp),%r8
 451:	49 c1 e0 03          	shl    $0x3,%r8
 455:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 45b <proc_read+0x30b>
 45b:	48 23 05 00 00 00 00 	and    0x0(%rip),%rax        # 462 <proc_read+0x312>
 462:	4c 03 05 00 00 00 00 	add    0x0(%rip),%r8        # 469 <proc_read+0x319>
 469:	48 25 00 f0 ff ff    	and    $0xfffffffffffff000,%rax
 46f:	4d 8d 2c 00          	lea    (%r8,%rax,1),%r13
 473:	49 8b 7d 00          	mov    0x0(%r13),%rdi
 477:	48 c7 c0 00 00 00 00 	mov    $0x0,%rax
 47e:	49 c7 c0 00 00 00 00 	mov    $0x0,%r8
 485:	40 f6 c7 01          	test   $0x1,%dil
 489:	4c 0f 44 c0          	cmove  %rax,%r8
 48d:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 493 <proc_read+0x343>
 493:	41 bf 00 20 00 00    	mov    $0x2000,%r15d
 499:	48 63 fb             	movslq %ebx,%rdi
 49c:	48 89 c1             	mov    %rax,%rcx
 49f:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 4a6:	4c 89 fe             	mov    %r15,%rsi
 4a9:	48 29 fe             	sub    %rdi,%rsi
 4ac:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
 4b3:	e8 00 00 00 00       	call   4b8 <proc_read+0x368>
 4b8:	4c 89 fe             	mov    %r15,%rsi
 4bb:	01 c3                	add    %eax,%ebx
 4bd:	48 63 fb             	movslq %ebx,%rdi
 4c0:	48 29 fe             	sub    %rdi,%rsi
 4c3:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
 4ca:	41 f6 45 00 01       	testb  $0x1,0x0(%r13)
 4cf:	0f 84 86 02 00 00    	je     75b <proc_read+0x60b>
 4d5:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 4dc:	83 c3 01             	add    $0x1,%ebx
 4df:	e8 00 00 00 00       	call   4e4 <proc_read+0x394>
 4e4:	49 8b 7d 00          	mov    0x0(%r13),%rdi
 4e8:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 4ee <proc_read+0x39e>
 4ee:	4c 8b 5d c8          	mov    -0x38(%rbp),%r11
 4f2:	48 23 05 00 00 00 00 	and    0x0(%rip),%rax        # 4f9 <proc_read+0x3a9>
 4f9:	48 25 00 f0 ff ff    	and    $0xfffffffffffff000,%rax
 4ff:	48 03 05 00 00 00 00 	add    0x0(%rip),%rax        # 506 <proc_read+0x3b6>
 506:	4e 8d 2c d8          	lea    (%rax,%r11,8),%r13
 50a:	49 8b 7d 00          	mov    0x0(%r13),%rdi
 50e:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 514 <proc_read+0x3c4>
 514:	a8 80                	test   $0x80,%al
 516:	48 8b 05 00 00 00 00 	mov    0x0(%rip),%rax        # 51d <proc_read+0x3cd>
 51d:	49 8b 7d 00          	mov    0x0(%r13),%rdi
 521:	48 c7 c1 00 00 00 00 	mov    $0x0,%rcx
 528:	49 c7 c2 00 00 00 00 	mov    $0x0,%r10
 52f:	49 c7 c1 00 00 00 00 	mov    $0x0,%r9
 536:	48 89 c2             	mov    %rax,%rdx
 539:	4c 0f 44 d1          	cmove  %rcx,%r10
 53d:	48 25 00 f0 ff ff    	and    $0xfffffffffffff000,%rax
 543:	48 81 e2 00 00 00 c0 	and    $0xffffffffc0000000,%rdx
 54a:	40 f6 c7 80          	test   $0x80,%dil
 54e:	48 0f 45 c2          	cmovne %rdx,%rax
 552:	48 f7 d0             	not    %rax
 555:	48 21 f8             	and    %rdi,%rax
 558:	a8 01                	test   $0x1,%al
 55a:	4c 0f 44 c9          	cmove  %rcx,%r9
 55e:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 564 <proc_read+0x414>
 564:	41 52                	push   %r10
 566:	48 63 fb             	movslq %ebx,%rdi
 569:	4c 89 fe             	mov    %r15,%rsi
 56c:	4c 89 d9             	mov    %r11,%rcx
 56f:	48 29 fe             	sub    %rdi,%rsi
 572:	49 89 c0             	mov    %rax,%r8
 575:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
 57c:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 583:	e8 00 00 00 00       	call   588 <proc_read+0x438>
 588:	49 8b 7d 00          	mov    0x0(%r13),%rdi
 58c:	59                   	pop    %rcx
 58d:	01 c3                	add    %eax,%ebx
 58f:	48 8b 05 00 00 00 00 	mov    0x0(%rip),%rax        # 596 <proc_read+0x446>
 596:	40 f6 c7 80          	test   $0x80,%dil
 59a:	0f 84 f5 01 00 00    	je     795 <proc_read+0x645>
 5a0:	48 25 00 00 00 c0    	and    $0xffffffffc0000000,%rax
 5a6:	48 f7 d0             	not    %rax
 5a9:	4c 63 c3             	movslq %ebx,%r8
 5ac:	41 b9 00 20 00 00    	mov    $0x2000,%r9d
 5b2:	48 21 f8             	and    %rdi,%rax
 5b5:	4d 29 c1             	sub    %r8,%r9
 5b8:	49 81 c0 00 00 00 00 	add    $0x0,%r8
 5bf:	a8 01                	test   $0x1,%al
 5c1:	0f 84 10 02 00 00    	je     7d7 <proc_read+0x687>
 5c7:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 5cd <proc_read+0x47d>
 5cd:	49 8b 7d 00          	mov    0x0(%r13),%rdi
 5d1:	a8 80                	test   $0x80,%al
 5d3:	0f 85 c7 01 00 00    	jne    7a0 <proc_read+0x650>
 5d9:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 5df <proc_read+0x48f>
 5df:	48 23 05 00 00 00 00 	and    0x0(%rip),%rax        # 5e6 <proc_read+0x496>
 5e6:	4c 89 c7             	mov    %r8,%rdi
 5e9:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 5f0:	4c 89 ce             	mov    %r9,%rsi
 5f3:	48 25 00 f0 ff ff    	and    $0xfffffffffffff000,%rax
 5f9:	48 89 c1             	mov    %rax,%rcx
 5fc:	e8 00 00 00 00       	call   601 <proc_read+0x4b1>
 601:	4d 8b 45 00          	mov    0x0(%r13),%r8
 605:	44 8d 3c 03          	lea    (%rbx,%rax,1),%r15d
 609:	4c 89 c7             	mov    %r8,%rdi
 60c:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 612 <proc_read+0x4c2>
 612:	48 8b 15 00 00 00 00 	mov    0x0(%rip),%rdx        # 619 <proc_read+0x4c9>
 619:	41 81 e0 80 00 00 00 	and    $0x80,%r8d
 620:	48 89 d1             	mov    %rdx,%rcx
 623:	0f 84 08 04 00 00    	je     a31 <proc_read+0x8e1>
 629:	48 81 e1 00 00 00 c0 	and    $0xffffffffc0000000,%rcx
 630:	48 21 c1             	and    %rax,%rcx
 633:	48 8b 45 c0          	mov    -0x40(%rbp),%rax
 637:	48 03 0d 00 00 00 00 	add    0x0(%rip),%rcx        # 63e <proc_read+0x4ee>
 63e:	48 8d 1c c1          	lea    (%rcx,%rax,8),%rbx
 642:	48 8b 3b             	mov    (%rbx),%rdi
 645:	40 f6 c7 80          	test   $0x80,%dil
 649:	0f 84 c9 03 00 00    	je     a18 <proc_read+0x8c8>
 64f:	48 81 e2 00 00 e0 ff 	and    $0xffffffffffe00000,%rdx
 656:	49 c7 c2 00 00 00 00 	mov    $0x0,%r10
 65d:	48 c7 c0 00 00 00 00 	mov    $0x0,%rax
 664:	48 f7 d2             	not    %rdx
 667:	48 21 fa             	and    %rdi,%rdx
 66a:	f6 c2 80             	test   $0x80,%dl
 66d:	4c 0f 44 d0          	cmove  %rax,%r10
 671:	f7 c2 81 01 00 00    	test   $0x181,%edx
 677:	48 c7 c0 00 00 00 00 	mov    $0x0,%rax
 67e:	49 c7 c1 00 00 00 00 	mov    $0x0,%r9
 685:	4c 0f 45 c8          	cmovne %rax,%r9
 689:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 68f <proc_read+0x53f>
 68f:	41 bd 00 20 00 00    	mov    $0x2000,%r13d
 695:	41 52                	push   %r10
 697:	49 63 ff             	movslq %r15d,%rdi
 69a:	48 8b 4d c0          	mov    -0x40(%rbp),%rcx
 69e:	4c 89 ee             	mov    %r13,%rsi
 6a1:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 6a8:	49 89 c0             	mov    %rax,%r8
 6ab:	48 29 fe             	sub    %rdi,%rsi
 6ae:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
 6b5:	e8 00 00 00 00       	call   6ba <proc_read+0x56a>
 6ba:	48 8b 3b             	mov    (%rbx),%rdi
 6bd:	5a                   	pop    %rdx
 6be:	41 01 c7             	add    %eax,%r15d
 6c1:	40 f6 c7 80          	test   $0x80,%dil
 6c5:	0f 84 17 03 00 00    	je     9e2 <proc_read+0x892>
 6cb:	48 8b 05 00 00 00 00 	mov    0x0(%rip),%rax        # 6d2 <proc_read+0x582>
 6d2:	48 25 00 00 e0 ff    	and    $0xffffffffffe00000,%rax
 6d8:	48 f7 d0             	not    %rax
 6db:	48 21 f8             	and    %rdi,%rax
 6de:	a9 81 01 00 00       	test   $0x181,%eax
 6e3:	0f 84 05 03 00 00    	je     9ee <proc_read+0x89e>
 6e9:	a8 80                	test   $0x80,%al
 6eb:	0f 84 04 01 00 00    	je     7f5 <proc_read+0x6a5>
 6f1:	4d 63 cf             	movslq %r15d,%r9
 6f4:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 6fa <proc_read+0x5aa>
 6fa:	4c 89 e2             	mov    %r12,%rdx
 6fd:	48 25 00 00 e0 ff    	and    $0xffffffffffe00000,%rax
 703:	4c 89 ee             	mov    %r13,%rsi
 706:	81 e2 ff ff 1f 00    	and    $0x1fffff,%edx
 70c:	4c 29 ce             	sub    %r9,%rsi
 70f:	49 8d b9 00 00 00 00 	lea    0x0(%r9),%rdi
 716:	48 09 d0             	or     %rdx,%rax
 719:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 720:	48 89 c1             	mov    %rax,%rcx
 723:	48 89 c3             	mov    %rax,%rbx
 726:	e8 00 00 00 00       	call   72b <proc_read+0x5db>
 72b:	4c 89 ee             	mov    %r13,%rsi
 72e:	49 89 d8             	mov    %rbx,%r8
 731:	4c 89 e1             	mov    %r12,%rcx
 734:	41 01 c7             	add    %eax,%r15d
 737:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 73e:	49 63 c7             	movslq %r15d,%rax
 741:	48 29 c6             	sub    %rax,%rsi
 744:	48 8d b8 00 00 00 00 	lea    0x0(%rax),%rdi
 74b:	e8 00 00 00 00       	call   750 <proc_read+0x600>
 750:	41 01 c7             	add    %eax,%r15d
 753:	49 63 df             	movslq %r15d,%rbx
 756:	e9 65 fa ff ff       	jmp    1c0 <proc_read+0x70>
 75b:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 762:	44 8d 7b 25          	lea    0x25(%rbx),%r15d
 766:	e8 00 00 00 00       	call   76b <proc_read+0x61b>
 76b:	49 63 df             	movslq %r15d,%rbx
 76e:	e9 4d fa ff ff       	jmp    1c0 <proc_read+0x70>
 773:	48 29 fe             	sub    %rdi,%rsi
 776:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 77d:	44 8d 7b 28          	lea    0x28(%rbx),%r15d
 781:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
 788:	e8 00 00 00 00       	call   78d <proc_read+0x63d>
 78d:	49 63 df             	movslq %r15d,%rbx
 790:	e9 2b fa ff ff       	jmp    1c0 <proc_read+0x70>
 795:	48 25 00 f0 ff ff    	and    $0xfffffffffffff000,%rax
 79b:	e9 06 fe ff ff       	jmp    5a6 <proc_read+0x456>
 7a0:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 7a6 <proc_read+0x656>
 7a6:	48 25 00 00 00 c0    	and    $0xffffffffc0000000,%rax
 7ac:	41 81 e4 ff ff ff 3f 	and    $0x3fffffff,%r12d
 7b3:	4c 89 ce             	mov    %r9,%rsi
 7b6:	4c 89 c7             	mov    %r8,%rdi
 7b9:	4c 09 e0             	or     %r12,%rax
 7bc:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 7c3:	48 89 c1             	mov    %rax,%rcx
 7c6:	e8 00 00 00 00       	call   7cb <proc_read+0x67b>
 7cb:	44 8d 3c 03          	lea    (%rbx,%rax,1),%r15d
 7cf:	49 63 df             	movslq %r15d,%rbx
 7d2:	e9 e9 f9 ff ff       	jmp    1c0 <proc_read+0x70>
 7d7:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 7de:	4c 89 ce             	mov    %r9,%rsi
 7e1:	44 8d 7b 25          	lea    0x25(%rbx),%r15d
 7e5:	4c 89 c7             	mov    %r8,%rdi
 7e8:	e8 00 00 00 00       	call   7ed <proc_read+0x69d>
 7ed:	49 63 df             	movslq %r15d,%rbx
 7f0:	e9 cb f9 ff ff       	jmp    1c0 <proc_read+0x70>
 7f5:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 7fb <proc_read+0x6ab>
 7fb:	48 23 05 00 00 00 00 	and    0x0(%rip),%rax        # 802 <proc_read+0x6b2>
 802:	49 63 ff             	movslq %r15d,%rdi
 805:	be 00 20 00 00       	mov    $0x2000,%esi
 80a:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 811:	48 25 00 f0 ff ff    	and    $0xfffffffffffff000,%rax
 817:	48 29 fe             	sub    %rdi,%rsi
 81a:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
 821:	48 89 c1             	mov    %rax,%rcx
 824:	e8 00 00 00 00       	call   829 <proc_read+0x6d9>
 829:	4c 8b 03             	mov    (%rbx),%r8
 82c:	41 01 c7             	add    %eax,%r15d
 82f:	48 8b 45 b8          	mov    -0x48(%rbp),%rax
 833:	4c 89 c7             	mov    %r8,%rdi
 836:	4c 8d 0c c5 00 00 00 	lea    0x0(,%rax,8),%r9
 83d:	00 
 83e:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 844 <proc_read+0x6f4>
 844:	41 81 e0 80 00 00 00 	and    $0x80,%r8d
 84b:	0f 84 ec 01 00 00    	je     a3d <proc_read+0x8ed>
 851:	48 8b 15 00 00 00 00 	mov    0x0(%rip),%rdx        # 858 <proc_read+0x708>
 858:	48 89 d1             	mov    %rdx,%rcx
 85b:	48 81 e2 00 f0 ff ff 	and    $0xfffffffffffff000,%rdx
 862:	48 81 e1 00 00 e0 ff 	and    $0xffffffffffe00000,%rcx
 869:	4c 03 0d 00 00 00 00 	add    0x0(%rip),%r9        # 870 <proc_read+0x720>
 870:	48 21 c1             	and    %rax,%rcx
 873:	48 89 d0             	mov    %rdx,%rax
 876:	49 c7 c0 00 00 00 00 	mov    $0x0,%r8
 87d:	49 8d 1c 09          	lea    (%r9,%rcx,1),%rbx
 881:	48 f7 d0             	not    %rax
 884:	4d 89 c2             	mov    %r8,%r10
 887:	4d 89 c3             	mov    %r8,%r11
 88a:	48 8b 3b             	mov    (%rbx),%rdi
 88d:	49 c7 c1 00 00 00 00 	mov    $0x0,%r9
 894:	48 ba 40 00 00 00 00 	movabs $0x400000000000040,%rdx
 89b:	00 00 04 
 89e:	48 21 f8             	and    %rdi,%rax
 8a1:	48 85 d0             	test   %rdx,%rax
 8a4:	4d 0f 44 d1          	cmove  %r9,%r10
 8a8:	a8 20                	test   $0x20,%al
 8aa:	4d 0f 44 d9          	cmove  %r9,%r11
 8ae:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 8b4 <proc_read+0x764>
 8b4:	a8 04                	test   $0x4,%al
 8b6:	48 8b 05 00 00 00 00 	mov    0x0(%rip),%rax        # 8bd <proc_read+0x76d>
 8bd:	48 8b 3b             	mov    (%rbx),%rdi
 8c0:	4d 0f 44 c1          	cmove  %r9,%r8
 8c4:	48 25 00 f0 ff ff    	and    $0xfffffffffffff000,%rax
 8ca:	48 f7 d0             	not    %rax
 8cd:	4c 89 45 c8          	mov    %r8,-0x38(%rbp)
 8d1:	49 c7 c0 00 00 00 00 	mov    $0x0,%r8
 8d8:	48 21 f8             	and    %rdi,%rax
 8db:	a8 02                	test   $0x2,%al
 8dd:	0f 84 98 01 00 00    	je     a7b <proc_read+0x92b>
 8e3:	a9 01 01 00 00       	test   $0x101,%eax
 8e8:	49 c7 c1 00 00 00 00 	mov    $0x0,%r9
 8ef:	48 c7 c0 00 00 00 00 	mov    $0x0,%rax
 8f6:	4c 0f 45 c8          	cmovne %rax,%r9
 8fa:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 900 <proc_read+0x7b0>
 900:	41 bd 00 20 00 00    	mov    $0x2000,%r13d
 906:	41 52                	push   %r10
 908:	49 63 ff             	movslq %r15d,%rdi
 90b:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 912:	41 53                	push   %r11
 914:	4c 89 ee             	mov    %r13,%rsi
 917:	ff 75 c8             	push   -0x38(%rbp)
 91a:	48 29 fe             	sub    %rdi,%rsi
 91d:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
 924:	41 50                	push   %r8
 926:	49 89 c0             	mov    %rax,%r8
 929:	48 8b 4d b8          	mov    -0x48(%rbp),%rcx
 92d:	e8 00 00 00 00       	call   932 <proc_read+0x7e2>
 932:	48 8b 3b             	mov    (%rbx),%rdi
 935:	41 01 c7             	add    %eax,%r15d
 938:	48 83 c4 20          	add    $0x20,%rsp
 93c:	f7 c7 01 01 00 00    	test   $0x101,%edi
 942:	0f 84 0b 01 00 00    	je     a53 <proc_read+0x903>
 948:	ff 15 00 00 00 00    	call   *0x0(%rip)        # 94e <proc_read+0x7fe>
 94e:	48 85 c0             	test   %rax,%rax
 951:	74 0d                	je     960 <proc_read+0x810>
 953:	48 89 c2             	mov    %rax,%rdx
 956:	83 e2 01             	and    $0x1,%edx
 959:	48 83 ea 01          	sub    $0x1,%rdx
 95d:	48 31 d0             	xor    %rdx,%rax
 960:	48 23 05 00 00 00 00 	and    0x0(%rip),%rax        # 967 <proc_read+0x817>
 967:	41 bd 00 20 00 00    	mov    $0x2000,%r13d
 96d:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 974:	49 89 c0             	mov    %rax,%r8
 977:	48 25 00 f0 ff ff    	and    $0xfffffffffffff000,%rax
 97d:	4c 89 ee             	mov    %r13,%rsi
 980:	49 c1 e8 0c          	shr    $0xc,%r8
 984:	48 89 c3             	mov    %rax,%rbx
 987:	49 63 c7             	movslq %r15d,%rax
 98a:	49 89 d9             	mov    %rbx,%r9
 98d:	4c 89 c1             	mov    %r8,%rcx
 990:	48 29 c6             	sub    %rax,%rsi
 993:	4c 89 45 c8          	mov    %r8,-0x38(%rbp)
 997:	48 8d b8 00 00 00 00 	lea    0x0(%rax),%rdi
 99e:	e8 00 00 00 00       	call   9a3 <proc_read+0x853>
 9a3:	4c 8b 45 c8          	mov    -0x38(%rbp),%r8
 9a7:	4c 89 ee             	mov    %r13,%rsi
 9aa:	4c 89 e1             	mov    %r12,%rcx
 9ad:	41 01 c7             	add    %eax,%r15d
 9b0:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 9b7:	41 50                	push   %r8
 9b9:	4c 8b 45 b0          	mov    -0x50(%rbp),%r8
 9bd:	49 63 c7             	movslq %r15d,%rax
 9c0:	4c 8b 4d a0          	mov    -0x60(%rbp),%r9
 9c4:	48 29 c6             	sub    %rax,%rsi
 9c7:	48 8d b8 00 00 00 00 	lea    0x0(%rax),%rdi
 9ce:	49 09 d8             	or     %rbx,%r8
 9d1:	e8 00 00 00 00       	call   9d6 <proc_read+0x886>
 9d6:	41 01 c7             	add    %eax,%r15d
 9d9:	58                   	pop    %rax
 9da:	49 63 df             	movslq %r15d,%rbx
 9dd:	e9 de f7 ff ff       	jmp    1c0 <proc_read+0x70>
 9e2:	f7 c7 81 01 00 00    	test   $0x181,%edi
 9e8:	0f 85 07 fe ff ff    	jne    7f5 <proc_read+0x6a5>
 9ee:	49 63 c7             	movslq %r15d,%rax
 9f1:	be 00 20 00 00       	mov    $0x2000,%esi
 9f6:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 9fd:	41 83 c7 25          	add    $0x25,%r15d
 a01:	48 29 c6             	sub    %rax,%rsi
 a04:	48 8d b8 00 00 00 00 	lea    0x0(%rax),%rdi
 a0b:	49 63 df             	movslq %r15d,%rbx
 a0e:	e8 00 00 00 00       	call   a13 <proc_read+0x8c3>
 a13:	e9 a8 f7 ff ff       	jmp    1c0 <proc_read+0x70>
 a18:	48 81 e2 00 f0 ff ff 	and    $0xfffffffffffff000,%rdx
 a1f:	49 c7 c2 00 00 00 00 	mov    $0x0,%r10
 a26:	48 f7 d2             	not    %rdx
 a29:	48 21 fa             	and    %rdi,%rdx
 a2c:	e9 40 fc ff ff       	jmp    671 <proc_read+0x521>
 a31:	48 81 e1 00 f0 ff ff 	and    $0xfffffffffffff000,%rcx
 a38:	e9 f3 fb ff ff       	jmp    630 <proc_read+0x4e0>
 a3d:	48 8b 0d 00 00 00 00 	mov    0x0(%rip),%rcx        # a44 <proc_read+0x8f4>
 a44:	48 81 e1 00 f0 ff ff 	and    $0xfffffffffffff000,%rcx
 a4b:	48 89 ca             	mov    %rcx,%rdx
 a4e:	e9 16 fe ff ff       	jmp    869 <proc_read+0x719>
 a53:	49 63 c7             	movslq %r15d,%rax
 a56:	4c 89 ee             	mov    %r13,%rsi
 a59:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 a60:	41 83 c7 25          	add    $0x25,%r15d
 a64:	48 29 c6             	sub    %rax,%rsi
 a67:	48 8d b8 00 00 00 00 	lea    0x0(%rax),%rdi
 a6e:	49 63 df             	movslq %r15d,%rbx
 a71:	e8 00 00 00 00       	call   a76 <proc_read+0x926>
 a76:	e9 45 f7 ff ff       	jmp    1c0 <proc_read+0x70>
 a7b:	e9 00 00 00 00       	jmp    a80 <proc_read+0x930>
 a80:	48 89 c2             	mov    %rax,%rdx
 a83:	48 c7 c1 00 00 00 00 	mov    $0x0,%rcx
 a8a:	83 e2 42             	and    $0x42,%edx
 a8d:	48 83 fa 40          	cmp    $0x40,%rdx
 a91:	48 c7 c2 00 00 00 00 	mov    $0x0,%rdx
 a98:	48 0f 44 d1          	cmove  %rcx,%rdx
 a9c:	49 89 d0             	mov    %rdx,%r8
 a9f:	e9 3f fe ff ff       	jmp    8e3 <proc_read+0x793>
 aa4:	49 c7 c0 00 00 00 00 	mov    $0x0,%r8
 aab:	e9 33 fe ff ff       	jmp    8e3 <proc_read+0x793>
 ab0:	48 c7 c0 f2 ff ff ff 	mov    $0xfffffffffffffff2,%rax
 ab7:	e9 3a f7 ff ff       	jmp    1f6 <proc_read+0xa6>

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
  16:	48 c7 c1 00 00 00 00 	mov    $0x0,%rcx
  1d:	31 d2                	xor    %edx,%edx
  1f:	be b6 01 00 00       	mov    $0x1b6,%esi
  24:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
  2b:	48 89 e5             	mov    %rsp,%rbp
  2e:	e8 00 00 00 00       	call   33 <init_module+0x23>
  33:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
  3a:	e8 00 00 00 00       	call   3f <init_module+0x2f>
  3f:	0f 20 de             	mov    %cr3,%rsi
  42:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
  49:	e8 00 00 00 00       	call   4e <init_module+0x3e>
  4e:	31 c0                	xor    %eax,%eax
  50:	5d                   	pop    %rbp
  51:	31 d2                	xor    %edx,%edx
  53:	31 c9                	xor    %ecx,%ecx
  55:	31 f6                	xor    %esi,%esi
  57:	31 ff                	xor    %edi,%edi
  59:	e9 00 00 00 00       	jmp    5e <__UNIQUE_ID_srcversion469>

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
  10:	55                   	push   %rbp
  11:	31 f6                	xor    %esi,%esi
  13:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
  1a:	48 89 e5             	mov    %rsp,%rbp
  1d:	e8 00 00 00 00       	call   22 <cleanup_module+0x12>
  22:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
  29:	e8 00 00 00 00       	call   2e <cleanup_module+0x1e>
  2e:	5d                   	pop    %rbp
  2f:	31 c0                	xor    %eax,%eax
  31:	31 f6                	xor    %esi,%esi
  33:	31 ff                	xor    %edi,%edi
  35:	e9 00 00 00 00       	jmp    3a <result_buffer+0x1a>

Disassembly of section .text.unlikely:

0000000000000000 <proc_read.cold>:
   0:	89 ca                	mov    %ecx,%edx
   2:	4c 89 e6             	mov    %r12,%rsi
   5:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
   c:	89 4d 9c             	mov    %ecx,-0x64(%rbp)
   f:	e8 00 00 00 00       	call   14 <proc_read.cold+0x14>
  14:	8b 4d 9c             	mov    -0x64(%rbp),%ecx
  17:	e9 00 00 00 00       	jmp    1c <_note_18+0x4>

Disassembly of section .altinstr_replacement:

0000000000000000 <.altinstr_replacement>:
   0:	e9 00 00 00 00       	jmp    5 <.altinstr_replacement+0x5>
   5:	e8 00 00 00 00       	call   a <.altinstr_replacement+0xa>
   a:	48 89 f8             	mov    %rdi,%rax
   d:	e9 00 00 00 00       	jmp    12 <.altinstr_replacement+0x12>
  12:	e8 00 00 00 00       	call   17 <.altinstr_replacement+0x17>
  17:	48 89 f8             	mov    %rdi,%rax
  1a:	e9 00 00 00 00       	jmp    1f <.altinstr_replacement+0x1f>
  1f:	e8 00 00 00 00       	call   24 <.altinstr_replacement+0x24>
  24:	48 89 f8             	mov    %rdi,%rax
  27:	e8 00 00 00 00       	call   2c <.altinstr_replacement+0x2c>
  2c:	48 89 f8             	mov    %rdi,%rax
  2f:	e8 00 00 00 00       	call   34 <.altinstr_replacement+0x34>
  34:	48 89 f8             	mov    %rdi,%rax
  37:	e8 00 00 00 00       	call   3c <.altinstr_replacement+0x3c>
  3c:	48 89 f8             	mov    %rdi,%rax
  3f:	e8 00 00 00 00       	call   44 <.altinstr_replacement+0x44>
  44:	48 89 f8             	mov    %rdi,%rax
  47:	e8 00 00 00 00       	call   4c <.altinstr_replacement+0x4c>
  4c:	48 89 f8             	mov    %rdi,%rax
  4f:	e8 00 00 00 00       	call   54 <.altinstr_replacement+0x54>
  54:	48 89 f8             	mov    %rdi,%rax
  57:	e8 00 00 00 00       	call   5c <.altinstr_replacement+0x5c>
  5c:	48 89 f8             	mov    %rdi,%rax
  5f:	e8 00 00 00 00       	call   64 <.altinstr_replacement+0x64>
  64:	48 89 f8             	mov    %rdi,%rax
  67:	e8 00 00 00 00       	call   6c <.altinstr_replacement+0x6c>
  6c:	48 89 f8             	mov    %rdi,%rax
  6f:	e8 00 00 00 00       	call   74 <.altinstr_replacement+0x74>
  74:	48 89 f8             	mov    %rdi,%rax
  77:	e8 00 00 00 00       	call   7c <.altinstr_replacement+0x7c>
  7c:	48 89 f8             	mov    %rdi,%rax
  7f:	e8 00 00 00 00       	call   84 <.altinstr_replacement+0x84>
  84:	48 89 f8             	mov    %rdi,%rax
  87:	e8 00 00 00 00       	call   8c <.altinstr_replacement+0x8c>
  8c:	48 89 f8             	mov    %rdi,%rax
  8f:	e8 00 00 00 00       	call   94 <.altinstr_replacement+0x94>
  94:	48 89 f8             	mov    %rdi,%rax
  97:	e8 00 00 00 00       	call   9c <.altinstr_replacement+0x9c>
  9c:	48 89 f8             	mov    %rdi,%rax
  9f:	e9 00 00 00 00       	jmp    a4 <.LC10+0x4>

Disassembly of section .altinstr_aux:

0000000000000000 <.altinstr_aux>:
   0:	f6 05 00 00 00 00 01 	testb  $0x1,0x0(%rip)        # 7 <.altinstr_aux+0x7>
   7:	0f 85 00 00 00 00    	jne    d <.altinstr_aux+0xd>
   d:	e9 00 00 00 00       	jmp    12 <.altinstr_aux+0x12>
  12:	f6 05 00 00 00 00 01 	testb  $0x1,0x0(%rip)        # 19 <.altinstr_aux+0x19>
  19:	0f 85 00 00 00 00    	jne    1f <.altinstr_aux+0x1f>
  1f:	e9 00 00 00 00       	jmp    24 <.altinstr_aux+0x24>
  24:	f6 05 00 00 00 00 01 	testb  $0x1,0x0(%rip)        # 2b <.altinstr_aux+0x2b>
  2b:	0f 85 00 00 00 00    	jne    31 <.altinstr_aux+0x31>
  31:	e9 00 00 00 00       	jmp    36 <.altinstr_aux+0x36>
  36:	f6 05 00 00 00 00 80 	testb  $0x80,0x0(%rip)        # 3d <.altinstr_aux+0x3d>
  3d:	0f 85 00 00 00 00    	jne    43 <.altinstr_aux+0x43>
  43:	e9 00 00 00 00       	jmp    48 <__UNIQUE_ID_author536+0xa>
