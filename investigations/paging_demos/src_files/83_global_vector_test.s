	.file	"83_global_vector_test.cpp"
# GNU C++17 (Ubuntu 13.3.0-6ubuntu2~24.04) version 13.3.0 (x86_64-linux-gnu)
#	compiled by GNU C version 13.3.0, GMP version 6.3.0, MPFR version 4.2.1, MPC version 1.3.1, isl version isl-0.26-GMP

# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -mtune=generic -march=x86-64 -O0 -fasynchronous-unwind-tables -fstack-protector-strong -fstack-clash-protection -fcf-protection
	.text
	.section	.text._ZSt23__is_constant_evaluatedv,"axG",@progbits,_ZSt23__is_constant_evaluatedv,comdat
	.weak	_ZSt23__is_constant_evaluatedv
	.type	_ZSt23__is_constant_evaluatedv, @function
_ZSt23__is_constant_evaluatedv:
.LFB1:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
# /usr/include/x86_64-linux-gnu/c++/13/bits/c++config.h:547:     return __builtin_is_constant_evaluated();
	movl	$0, %eax	#, _1
# /usr/include/x86_64-linux-gnu/c++/13/bits/c++config.h:551:   }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE1:
	.size	_ZSt23__is_constant_evaluatedv, .-_ZSt23__is_constant_evaluatedv
	.section	.text._ZNSt11char_traitsIcE6lengthEPKc,"axG",@progbits,_ZNSt11char_traitsIcE6lengthEPKc,comdat
	.weak	_ZNSt11char_traitsIcE6lengthEPKc
	.type	_ZNSt11char_traitsIcE6lengthEPKc, @function
_ZNSt11char_traitsIcE6lengthEPKc:
.LFB42:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$16, %rsp	#,
	movq	%rdi, -8(%rbp)	# __s, __s
# /usr/include/c++/13/bits/char_traits.h:396: 	if (std::__is_constant_evaluated())
	call	_ZSt23__is_constant_evaluatedv	#
# /usr/include/c++/13/bits/char_traits.h:396: 	if (std::__is_constant_evaluated())
	testb	%al, %al	# retval.4_5
	je	.L4	#,
# /usr/include/c++/13/bits/char_traits.h:397: 	  return __gnu_cxx::char_traits<char_type>::length(__s);
	movq	-8(%rbp), %rax	# __s, tmp85
	movq	%rax, %rdi	# tmp85,
	call	_ZN9__gnu_cxx11char_traitsIcE6lengthEPKc	#
# /usr/include/c++/13/bits/char_traits.h:397: 	  return __gnu_cxx::char_traits<char_type>::length(__s);
	jmp	.L5	#
.L4:
# /usr/include/c++/13/bits/char_traits.h:399: 	return __builtin_strlen(__s);
	movq	-8(%rbp), %rax	# __s, tmp87
	movq	%rax, %rdi	# tmp87,
	call	strlen@PLT	#
# /usr/include/c++/13/bits/char_traits.h:399: 	return __builtin_strlen(__s);
	nop	
.L5:
# /usr/include/c++/13/bits/char_traits.h:400:       }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE42:
	.size	_ZNSt11char_traitsIcE6lengthEPKc, .-_ZNSt11char_traitsIcE6lengthEPKc
	.section	.text._ZnwmPv,"axG",@progbits,_ZnwmPv,comdat
	.weak	_ZnwmPv
	.type	_ZnwmPv, @function
_ZnwmPv:
.LFB103:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# D.8499, D.8499
	movq	%rsi, -16(%rbp)	# __p, __p
# /usr/include/c++/13/new:175: { return __p; }
	movq	-16(%rbp), %rax	# __p, _2
# /usr/include/c++/13/new:175: { return __p; }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE103:
	.size	_ZnwmPv, .-_ZnwmPv
	.section	.text._ZSt17__size_to_integerm,"axG",@progbits,_ZSt17__size_to_integerm,comdat
	.weak	_ZSt17__size_to_integerm
	.type	_ZSt17__size_to_integerm, @function
_ZSt17__size_to_integerm:
.LFB563:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# __n, __n
# /usr/include/c++/13/bits/stl_algobase.h:1018:   __size_to_integer(unsigned long __n) { return __n; }
	movq	-8(%rbp), %rax	# __n, _2
# /usr/include/c++/13/bits/stl_algobase.h:1018:   __size_to_integer(unsigned long __n) { return __n; }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE563:
	.size	_ZSt17__size_to_integerm, .-_ZSt17__size_to_integerm
	.globl	x1
	.data
	.align 4
	.type	x1, @object
	.size	x1, 4
x1:
	.long	3
	.globl	v1
	.bss
	.align 16
	.type	v1, @object
	.size	v1, 24
v1:
	.zero	24
	.section	.text._ZNSt12_Vector_baseIiSaIiEE12_Vector_implD2Ev,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD5Ev,comdat
	.align 2
	.weak	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD2Ev
	.type	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD2Ev, @function
_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD2Ev:
.LFB1743:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -24(%rbp)	# this, this
	movq	-24(%rbp), %rax	# this, tmp82
	movq	%rax, -8(%rbp)	# tmp82, this
# /usr/include/c++/13/bits/allocator.h:184:       ~allocator() _GLIBCXX_NOTHROW { }
	movq	-8(%rbp), %rax	# this, tmp83
	movq	%rax, %rdi	# tmp83,
	call	_ZNSt15__new_allocatorIiED2Ev	#
	nop	
# /usr/include/c++/13/bits/stl_vector.h:133:       struct _Vector_impl
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE1743:
	.size	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD2Ev, .-_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD2Ev
	.weak	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD1Ev
	.set	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD1Ev,_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD2Ev
	.text
	.globl	_Z11doSomethingNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
	.type	_Z11doSomethingNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE, @function
_Z11doSomethingNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE:
.LFB1739:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA1739
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	pushq	%rbx	#
	subq	$56, %rsp	#,
	.cfi_offset 3, -24
	movq	%rdi, -56(%rbp)	# .result_ptr, .result_ptr
	movq	%rsi, -64(%rbp)	# name, name
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:8: vector<int> doSomething(string name) {
	movq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp89
	movq	%rax, -24(%rbp)	# tmp89, D.45700
	xorl	%eax, %eax	# tmp89
	leaq	-37(%rbp), %rax	#, tmp83
	movq	%rax, -32(%rbp)	# tmp83, this
# /usr/include/c++/13/bits/new_allocator.h:88:       __new_allocator() _GLIBCXX_USE_NOEXCEPT { }
	nop	
# /usr/include/c++/13/bits/allocator.h:163:       allocator() _GLIBCXX_NOTHROW { }
	nop	
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:9:     auto v2 = vector<int>(3);
	leaq	-37(%rbp), %rdx	#, tmp84
	movq	-56(%rbp), %rax	# <retval>, tmp85
	movl	$3, %esi	#,
	movq	%rax, %rdi	# tmp85,
.LEHB0:
	call	_ZNSt6vectorIiSaIiEEC1EmRKS0_	#
.LEHE0:
# /usr/include/c++/13/bits/allocator.h:184:       ~allocator() _GLIBCXX_NOTHROW { }
	leaq	-37(%rbp), %rax	#, tmp86
	movq	%rax, %rdi	# tmp86,
	call	_ZNSt15__new_allocatorIiED2Ev	#
	nop	
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:10:     auto x2 = 7;
	movl	$7, -36(%rbp)	#, x2
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:12:     return v2;
	nop	
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:13: }
	movq	-24(%rbp), %rax	# D.45700, tmp90
	subq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp90
	je	.L15	#,
	jmp	.L17	#
.L16:
	endbr64	
# /usr/include/c++/13/bits/allocator.h:184:       ~allocator() _GLIBCXX_NOTHROW { }
	movq	%rax, %rbx	#, tmp88
	leaq	-37(%rbp), %rax	#, tmp87
	movq	%rax, %rdi	# tmp87,
	call	_ZNSt15__new_allocatorIiED2Ev	#
	nop	
	movq	%rbx, %rax	# tmp88, D.45699
	movq	-24(%rbp), %rdx	# D.45700, tmp91
	subq	%fs:40, %rdx	# MEM[(<address-space-1> long unsigned int *)40B], tmp91
	je	.L14	#,
	call	__stack_chk_fail@PLT	#
.L14:
	movq	%rax, %rdi	# D.45699,
.LEHB1:
	call	_Unwind_Resume@PLT	#
.LEHE1:
.L17:
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:13: }
	call	__stack_chk_fail@PLT	#
.L15:
	movq	-56(%rbp), %rax	# <retval>,
	movq	-8(%rbp), %rbx	#,
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE1739:
	.globl	__gxx_personality_v0
	.section	.gcc_except_table,"a",@progbits
.LLSDA1739:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE1739-.LLSDACSB1739
.LLSDACSB1739:
	.uleb128 .LEHB0-.LFB1739
	.uleb128 .LEHE0-.LEHB0
	.uleb128 .L16-.LFB1739
	.uleb128 0
	.uleb128 .LEHB1-.LFB1739
	.uleb128 .LEHE1-.LEHB1
	.uleb128 0
	.uleb128 0
.LLSDACSE1739:
	.text
	.size	_Z11doSomethingNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE, .-_Z11doSomethingNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
	.section	.rodata
.LC0:
	.string	"hi"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1749:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA1749
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	pushq	%rbx	#
	subq	$104, %rsp	#,
	.cfi_offset 3, -24
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	movq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp101
	movq	%rax, -24(%rbp)	# tmp101, D.45704
	xorl	%eax, %eax	# tmp101
	leaq	-105(%rbp), %rax	#, tmp85
	movq	%rax, -104(%rbp)	# tmp85, this
# /usr/include/c++/13/bits/new_allocator.h:88:       __new_allocator() _GLIBCXX_USE_NOEXCEPT { }
	nop	
# /usr/include/c++/13/bits/allocator.h:163:       allocator() _GLIBCXX_NOTHROW { }
	nop	
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	leaq	-105(%rbp), %rdx	#, tmp86
	leaq	-64(%rbp), %rax	#, tmp87
	leaq	.LC0(%rip), %rcx	#, tmp88
	movq	%rcx, %rsi	# tmp88,
	movq	%rax, %rdi	# tmp87,
.LEHB2:
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1IS3_EEPKcRKS3_	#
.LEHE2:
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	leaq	-96(%rbp), %rax	#, tmp89
	leaq	-64(%rbp), %rdx	#, tmp90
	movq	%rdx, %rsi	# tmp90,
	movq	%rax, %rdi	# tmp89,
.LEHB3:
	call	_Z11doSomethingNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE	#
.LEHE3:
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	leaq	-64(%rbp), %rax	#, tmp91
	movq	%rax, %rdi	# tmp91,
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev@PLT	#
# /usr/include/c++/13/bits/allocator.h:184:       ~allocator() _GLIBCXX_NOTHROW { }
	leaq	-105(%rbp), %rax	#, tmp92
	movq	%rax, %rdi	# tmp92,
	call	_ZNSt15__new_allocatorIcED2Ev	#
	nop	
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	leaq	-96(%rbp), %rax	#, tmp93
	movq	%rax, %rdi	# tmp93,
	call	_ZNSt6vectorIiSaIiEED1Ev	#
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	movl	$0, %eax	#, _10
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	movq	-24(%rbp), %rdx	# D.45704, tmp102
	subq	%fs:40, %rdx	# MEM[(<address-space-1> long unsigned int *)40B], tmp102
	je	.L23	#,
	jmp	.L26	#
.L25:
	endbr64	
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	movq	%rax, %rbx	#, tmp97
	leaq	-64(%rbp), %rax	#, tmp95
	movq	%rax, %rdi	# tmp95,
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev@PLT	#
	jmp	.L21	#
.L24:
	endbr64	
# /usr/include/c++/13/bits/allocator.h:184:       ~allocator() _GLIBCXX_NOTHROW { }
	movq	%rax, %rbx	#, tmp96
.L21:
	leaq	-105(%rbp), %rax	#, tmp100
	movq	%rax, %rdi	# tmp100,
	call	_ZNSt15__new_allocatorIcED2Ev	#
	nop	
	movq	%rbx, %rax	# tmp96, D.45703
	movq	-24(%rbp), %rdx	# D.45704, tmp103
	subq	%fs:40, %rdx	# MEM[(<address-space-1> long unsigned int *)40B], tmp103
	je	.L22	#,
	call	__stack_chk_fail@PLT	#
.L22:
	movq	%rax, %rdi	# D.45703,
.LEHB4:
	call	_Unwind_Resume@PLT	#
.LEHE4:
.L26:
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	call	__stack_chk_fail@PLT	#
.L23:
	movq	-8(%rbp), %rbx	#,
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE1749:
	.section	.gcc_except_table
.LLSDA1749:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE1749-.LLSDACSB1749
.LLSDACSB1749:
	.uleb128 .LEHB2-.LFB1749
	.uleb128 .LEHE2-.LEHB2
	.uleb128 .L24-.LFB1749
	.uleb128 0
	.uleb128 .LEHB3-.LFB1749
	.uleb128 .LEHE3-.LEHB3
	.uleb128 .L25-.LFB1749
	.uleb128 0
	.uleb128 .LEHB4-.LFB1749
	.uleb128 .LEHE4-.LEHB4
	.uleb128 0
	.uleb128 0
.LLSDACSE1749:
	.text
	.size	main, .-main
	.section	.text._ZN9__gnu_cxx11char_traitsIcE6lengthEPKc,"axG",@progbits,_ZN9__gnu_cxx11char_traitsIcE6lengthEPKc,comdat
	.align 2
	.weak	_ZN9__gnu_cxx11char_traitsIcE6lengthEPKc
	.type	_ZN9__gnu_cxx11char_traitsIcE6lengthEPKc, @function
_ZN9__gnu_cxx11char_traitsIcE6lengthEPKc:
.LFB1750:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$48, %rsp	#,
	movq	%rdi, -40(%rbp)	# __p, __p
# /usr/include/c++/13/bits/char_traits.h:199:     char_traits<_CharT>::
	movq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp91
	movq	%rax, -8(%rbp)	# tmp91, D.45707
	xorl	%eax, %eax	# tmp91
# /usr/include/c++/13/bits/char_traits.h:202:       std::size_t __i = 0;
	movq	$0, -16(%rbp)	#, __i
# /usr/include/c++/13/bits/char_traits.h:203:       while (!eq(__p[__i], char_type()))
	jmp	.L28	#
.L29:
# /usr/include/c++/13/bits/char_traits.h:204:         ++__i;
	addq	$1, -16(%rbp)	#, __i
.L28:
# /usr/include/c++/13/bits/char_traits.h:203:       while (!eq(__p[__i], char_type()))
	movb	$0, -17(%rbp)	#, D.40448
# /usr/include/c++/13/bits/char_traits.h:203:       while (!eq(__p[__i], char_type()))
	movq	-40(%rbp), %rdx	# __p, tmp87
	movq	-16(%rbp), %rax	# __i, tmp88
	addq	%rax, %rdx	# tmp88, _1
# /usr/include/c++/13/bits/char_traits.h:203:       while (!eq(__p[__i], char_type()))
	leaq	-17(%rbp), %rax	#, tmp89
	movq	%rax, %rsi	# tmp89,
	movq	%rdx, %rdi	# _1,
	call	_ZN9__gnu_cxx11char_traitsIcE2eqERKcS3_	#
# /usr/include/c++/13/bits/char_traits.h:203:       while (!eq(__p[__i], char_type()))
	xorl	$1, %eax	#, retval.5_10
	testb	%al, %al	# retval.5_10
	jne	.L29	#,
# /usr/include/c++/13/bits/char_traits.h:205:       return __i;
	movq	-16(%rbp), %rax	# __i, _12
# /usr/include/c++/13/bits/char_traits.h:206:     }
	movq	-8(%rbp), %rdx	# D.45707, tmp92
	subq	%fs:40, %rdx	# MEM[(<address-space-1> long unsigned int *)40B], tmp92
	je	.L31	#,
	call	__stack_chk_fail@PLT	#
.L31:
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE1750:
	.size	_ZN9__gnu_cxx11char_traitsIcE6lengthEPKc, .-_ZN9__gnu_cxx11char_traitsIcE6lengthEPKc
	.section	.text._ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderD2Ev,"axG",@progbits,_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderD5Ev,comdat
	.align 2
	.weak	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderD2Ev
	.type	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderD2Ev, @function
_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderD2Ev:
.LFB1859:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -24(%rbp)	# this, this
	movq	-24(%rbp), %rax	# this, tmp82
	movq	%rax, -8(%rbp)	# tmp82, this
# /usr/include/c++/13/bits/allocator.h:184:       ~allocator() _GLIBCXX_NOTHROW { }
	movq	-8(%rbp), %rax	# this, tmp83
	movq	%rax, %rdi	# tmp83,
	call	_ZNSt15__new_allocatorIcED2Ev	#
	nop	
# /usr/include/c++/13/bits/basic_string.h:181:       struct _Alloc_hider : allocator_type // TODO check __is_final
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE1859:
	.size	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderD2Ev, .-_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderD2Ev
	.weak	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderD1Ev
	.set	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderD1Ev,_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderD2Ev
	.section	.text._ZNSt6vectorIiSaIiEEC2ESt16initializer_listIiERKS0_,"axG",@progbits,_ZNSt6vectorIiSaIiEEC5ESt16initializer_listIiERKS0_,comdat
	.align 2
	.weak	_ZNSt6vectorIiSaIiEEC2ESt16initializer_listIiERKS0_
	.type	_ZNSt6vectorIiSaIiEEC2ESt16initializer_listIiERKS0_, @function
_ZNSt6vectorIiSaIiEEC2ESt16initializer_listIiERKS0_:
.LFB2006:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA2006
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	pushq	%rbx	#
	subq	$56, %rsp	#,
	.cfi_offset 3, -24
	movq	%rdi, -40(%rbp)	# this, this
	movq	%rsi, %rax	# __l, tmp90
	movq	%rdx, %rsi	# __l, tmp91
	movq	%rsi, %rdx	# tmp91,
	movq	%rax, -64(%rbp)	# tmp89, __l
	movq	%rdx, -56(%rbp)	#, __l
	movq	%rcx, -48(%rbp)	# __a, __a
# /usr/include/c++/13/bits/stl_vector.h:678:       vector(initializer_list<value_type> __l,
	movq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp97
	movq	%rax, -24(%rbp)	# tmp97, D.45709
	xorl	%eax, %eax	# tmp97
# /usr/include/c++/13/bits/stl_vector.h:680:       : _Base(__a)
	movq	-40(%rbp), %rax	# this, _1
	movq	-48(%rbp), %rdx	# __a, tmp92
	movq	%rdx, %rsi	# tmp92,
	movq	%rax, %rdi	# _1,
	call	_ZNSt12_Vector_baseIiSaIiEEC2ERKS0_	#
# /usr/include/c++/13/bits/stl_vector.h:682: 	_M_range_initialize(__l.begin(), __l.end(),
	leaq	-64(%rbp), %rax	#, tmp93
	movq	%rax, %rdi	# tmp93,
	call	_ZNKSt16initializer_listIiE3endEv	#
	movq	%rax, %rbx	#, _3
# /usr/include/c++/13/bits/stl_vector.h:682: 	_M_range_initialize(__l.begin(), __l.end(),
	leaq	-64(%rbp), %rax	#, tmp94
	movq	%rax, %rdi	# tmp94,
	call	_ZNKSt16initializer_listIiE5beginEv	#
	movq	%rax, %rcx	#, _4
# /usr/include/c++/13/bits/stl_vector.h:682: 	_M_range_initialize(__l.begin(), __l.end(),
	movq	-40(%rbp), %rax	# this, tmp95
	movq	%rbx, %rdx	# _3,
	movq	%rcx, %rsi	# _4,
	movq	%rax, %rdi	# tmp95,
.LEHB5:
	call	_ZNSt6vectorIiSaIiEE19_M_range_initializeIPKiEEvT_S5_St20forward_iterator_tag	#
.LEHE5:
# /usr/include/c++/13/bits/stl_vector.h:684:       }
	jmp	.L38	#
.L37:
	endbr64	
	movq	%rax, %rbx	#, tmp96
	movq	-40(%rbp), %rax	# this, _5
	movq	%rax, %rdi	# _5,
	call	_ZNSt12_Vector_baseIiSaIiEED2Ev	#
	movq	%rbx, %rax	# tmp96, D.45708
	movq	-24(%rbp), %rdx	# D.45709, tmp98
	subq	%fs:40, %rdx	# MEM[(<address-space-1> long unsigned int *)40B], tmp98
	je	.L35	#,
	call	__stack_chk_fail@PLT	#
.L35:
	movq	%rax, %rdi	# D.45708,
.LEHB6:
	call	_Unwind_Resume@PLT	#
.LEHE6:
.L38:
	movq	-24(%rbp), %rax	# D.45709, tmp99
	subq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp99
	je	.L36	#,
	call	__stack_chk_fail@PLT	#
.L36:
	movq	-8(%rbp), %rbx	#,
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2006:
	.section	.gcc_except_table
.LLSDA2006:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE2006-.LLSDACSB2006
.LLSDACSB2006:
	.uleb128 .LEHB5-.LFB2006
	.uleb128 .LEHE5-.LEHB5
	.uleb128 .L37-.LFB2006
	.uleb128 0
	.uleb128 .LEHB6-.LFB2006
	.uleb128 .LEHE6-.LEHB6
	.uleb128 0
	.uleb128 0
.LLSDACSE2006:
	.section	.text._ZNSt6vectorIiSaIiEEC2ESt16initializer_listIiERKS0_,"axG",@progbits,_ZNSt6vectorIiSaIiEEC5ESt16initializer_listIiERKS0_,comdat
	.size	_ZNSt6vectorIiSaIiEEC2ESt16initializer_listIiERKS0_, .-_ZNSt6vectorIiSaIiEEC2ESt16initializer_listIiERKS0_
	.weak	_ZNSt6vectorIiSaIiEEC1ESt16initializer_listIiERKS0_
	.set	_ZNSt6vectorIiSaIiEEC1ESt16initializer_listIiERKS0_,_ZNSt6vectorIiSaIiEEC2ESt16initializer_listIiERKS0_
	.section	.text._ZNSt6vectorIiSaIiEED2Ev,"axG",@progbits,_ZNSt6vectorIiSaIiEED5Ev,comdat
	.align 2
	.weak	_ZNSt6vectorIiSaIiEED2Ev
	.type	_ZNSt6vectorIiSaIiEED2Ev, @function
_ZNSt6vectorIiSaIiEED2Ev:
.LFB2009:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA2009
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$48, %rsp	#,
	movq	%rdi, -40(%rbp)	# this, this
# /usr/include/c++/13/bits/stl_vector.h:736: 		      _M_get_Tp_allocator());
	movq	-40(%rbp), %rax	# this, _1
	movq	%rax, %rdi	# _1,
	call	_ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv	#
# /usr/include/c++/13/bits/stl_vector.h:735: 	std::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish,
	movq	-40(%rbp), %rdx	# this, tmp87
	movq	8(%rdx), %rdx	# this_6(D)->D.40346._M_impl.D.39653._M_finish, _3
	movq	-40(%rbp), %rcx	# this, tmp88
	movq	(%rcx), %rcx	# this_6(D)->D.40346._M_impl.D.39653._M_start, _4
	movq	%rcx, -24(%rbp)	# _4, __first
	movq	%rdx, -16(%rbp)	# _3, __last
	movq	%rax, -8(%rbp)	# _2, D.45590
# /usr/include/c++/13/bits/alloc_traits.h:948:       std::_Destroy(__first, __last);
	movq	-16(%rbp), %rdx	# __last, tmp89
	movq	-24(%rbp), %rax	# __first, tmp90
	movq	%rdx, %rsi	# tmp89,
	movq	%rax, %rdi	# tmp90,
	call	_ZSt8_DestroyIPiEvT_S1_	#
# /usr/include/c++/13/bits/alloc_traits.h:949:     }
	nop	
# /usr/include/c++/13/bits/stl_vector.h:738:       }
	movq	-40(%rbp), %rax	# this, _5
	movq	%rax, %rdi	# _5,
	call	_ZNSt12_Vector_baseIiSaIiEED2Ev	#
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2009:
	.section	.gcc_except_table
.LLSDA2009:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE2009-.LLSDACSB2009
.LLSDACSB2009:
.LLSDACSE2009:
	.section	.text._ZNSt6vectorIiSaIiEED2Ev,"axG",@progbits,_ZNSt6vectorIiSaIiEED5Ev,comdat
	.size	_ZNSt6vectorIiSaIiEED2Ev, .-_ZNSt6vectorIiSaIiEED2Ev
	.weak	_ZNSt6vectorIiSaIiEED1Ev
	.set	_ZNSt6vectorIiSaIiEED1Ev,_ZNSt6vectorIiSaIiEED2Ev
	.section	.text._ZNSt6vectorIiSaIiEEC2EmRKS0_,"axG",@progbits,_ZNSt6vectorIiSaIiEEC5EmRKS0_,comdat
	.align 2
	.weak	_ZNSt6vectorIiSaIiEEC2EmRKS0_
	.type	_ZNSt6vectorIiSaIiEEC2EmRKS0_, @function
_ZNSt6vectorIiSaIiEEC2EmRKS0_:
.LFB2012:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA2012
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	pushq	%rbx	#
	subq	$40, %rsp	#,
	.cfi_offset 3, -24
	movq	%rdi, -24(%rbp)	# this, this
	movq	%rsi, -32(%rbp)	# __n, __n
	movq	%rdx, -40(%rbp)	# __a, __a
# /usr/include/c++/13/bits/stl_vector.h:557:       : _Base(_S_check_init_len(__n, __a), __a)
	movq	-24(%rbp), %rbx	# this, _1
	movq	-40(%rbp), %rdx	# __a, tmp86
	movq	-32(%rbp), %rax	# __n, tmp87
	movq	%rdx, %rsi	# tmp86,
	movq	%rax, %rdi	# tmp87,
.LEHB7:
	call	_ZNSt6vectorIiSaIiEE17_S_check_init_lenEmRKS0_	#
	movq	%rax, %rcx	#, _2
# /usr/include/c++/13/bits/stl_vector.h:557:       : _Base(_S_check_init_len(__n, __a), __a)
	movq	-40(%rbp), %rax	# __a, tmp88
	movq	%rax, %rdx	# tmp88,
	movq	%rcx, %rsi	# _2,
	movq	%rbx, %rdi	# _1,
	call	_ZNSt12_Vector_baseIiSaIiEEC2EmRKS0_	#
.LEHE7:
# /usr/include/c++/13/bits/stl_vector.h:558:       { _M_default_initialize(__n); }
	movq	-32(%rbp), %rdx	# __n, tmp89
	movq	-24(%rbp), %rax	# this, tmp90
	movq	%rdx, %rsi	# tmp89,
	movq	%rax, %rdi	# tmp90,
.LEHB8:
	call	_ZNSt6vectorIiSaIiEE21_M_default_initializeEm	#
.LEHE8:
# /usr/include/c++/13/bits/stl_vector.h:558:       { _M_default_initialize(__n); }
	jmp	.L43	#
.L42:
	endbr64	
# /usr/include/c++/13/bits/stl_vector.h:558:       { _M_default_initialize(__n); }
	movq	%rax, %rbx	#, tmp91
	movq	-24(%rbp), %rax	# this, _3
	movq	%rax, %rdi	# _3,
	call	_ZNSt12_Vector_baseIiSaIiEED2Ev	#
	movq	%rbx, %rax	# tmp91, D.45710
	movq	%rax, %rdi	# D.45710,
.LEHB9:
	call	_Unwind_Resume@PLT	#
.LEHE9:
.L43:
# /usr/include/c++/13/bits/stl_vector.h:558:       { _M_default_initialize(__n); }
	movq	-8(%rbp), %rbx	#,
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2012:
	.section	.gcc_except_table
.LLSDA2012:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE2012-.LLSDACSB2012
.LLSDACSB2012:
	.uleb128 .LEHB7-.LFB2012
	.uleb128 .LEHE7-.LEHB7
	.uleb128 0
	.uleb128 0
	.uleb128 .LEHB8-.LFB2012
	.uleb128 .LEHE8-.LEHB8
	.uleb128 .L42-.LFB2012
	.uleb128 0
	.uleb128 .LEHB9-.LFB2012
	.uleb128 .LEHE9-.LEHB9
	.uleb128 0
	.uleb128 0
.LLSDACSE2012:
	.section	.text._ZNSt6vectorIiSaIiEEC2EmRKS0_,"axG",@progbits,_ZNSt6vectorIiSaIiEEC5EmRKS0_,comdat
	.size	_ZNSt6vectorIiSaIiEEC2EmRKS0_, .-_ZNSt6vectorIiSaIiEEC2EmRKS0_
	.weak	_ZNSt6vectorIiSaIiEEC1EmRKS0_
	.set	_ZNSt6vectorIiSaIiEEC1EmRKS0_,_ZNSt6vectorIiSaIiEEC2EmRKS0_
	.section	.text._ZNSt12_Vector_baseIiSaIiEED2Ev,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEED5Ev,comdat
	.align 2
	.weak	_ZNSt12_Vector_baseIiSaIiEED2Ev
	.type	_ZNSt12_Vector_baseIiSaIiEED2Ev, @function
_ZNSt12_Vector_baseIiSaIiEED2Ev:
.LFB2018:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA2018
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$16, %rsp	#,
	movq	%rdi, -8(%rbp)	# this, this
# /usr/include/c++/13/bits/stl_vector.h:370: 		      _M_impl._M_end_of_storage - _M_impl._M_start);
	movq	-8(%rbp), %rax	# this, tmp89
	movq	16(%rax), %rdx	# this_9(D)->_M_impl.D.39653._M_end_of_storage, _1
# /usr/include/c++/13/bits/stl_vector.h:370: 		      _M_impl._M_end_of_storage - _M_impl._M_start);
	movq	-8(%rbp), %rax	# this, tmp90
	movq	(%rax), %rax	# this_9(D)->_M_impl.D.39653._M_start, _2
# /usr/include/c++/13/bits/stl_vector.h:370: 		      _M_impl._M_end_of_storage - _M_impl._M_start);
	subq	%rax, %rdx	# _2, _3
	movq	%rdx, %rax	# _3, _3
	sarq	$2, %rax	#, _3
# /usr/include/c++/13/bits/stl_vector.h:369: 	_M_deallocate(_M_impl._M_start,
	movq	%rax, %rdx	# _4, _5
	movq	-8(%rbp), %rax	# this, tmp92
	movq	(%rax), %rcx	# this_9(D)->_M_impl.D.39653._M_start, _6
	movq	-8(%rbp), %rax	# this, tmp93
	movq	%rcx, %rsi	# _6,
	movq	%rax, %rdi	# tmp93,
	call	_ZNSt12_Vector_baseIiSaIiEE13_M_deallocateEPim	#
# /usr/include/c++/13/bits/stl_vector.h:371:       }
	movq	-8(%rbp), %rax	# this, _7
	movq	%rax, %rdi	# _7,
	call	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD1Ev	#
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2018:
	.section	.gcc_except_table
.LLSDA2018:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE2018-.LLSDACSB2018
.LLSDACSB2018:
.LLSDACSE2018:
	.section	.text._ZNSt12_Vector_baseIiSaIiEED2Ev,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEED5Ev,comdat
	.size	_ZNSt12_Vector_baseIiSaIiEED2Ev, .-_ZNSt12_Vector_baseIiSaIiEED2Ev
	.weak	_ZNSt12_Vector_baseIiSaIiEED1Ev
	.set	_ZNSt12_Vector_baseIiSaIiEED1Ev,_ZNSt12_Vector_baseIiSaIiEED2Ev
	.section	.rodata
	.align 8
.LC1:
	.string	"basic_string: construction from null is not valid"
	.section	.text._ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2IS3_EEPKcRKS3_,"axG",@progbits,_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC5IS3_EEPKcRKS3_,comdat
	.align 2
	.weak	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2IS3_EEPKcRKS3_
	.type	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2IS3_EEPKcRKS3_, @function
_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2IS3_EEPKcRKS3_:
.LFB2021:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA2021
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	pushq	%rbx	#
	subq	$56, %rsp	#,
	.cfi_offset 3, -24
	movq	%rdi, -40(%rbp)	# this, this
	movq	%rsi, -48(%rbp)	# __s, __s
	movq	%rdx, -56(%rbp)	# __a, __a
# /usr/include/c++/13/bits/basic_string.h:642:       : _M_dataplus(_M_local_data(), __a)
	movq	-40(%rbp), %rbx	# this, _1
	movq	-40(%rbp), %rax	# this, tmp88
	movq	%rax, %rdi	# tmp88,
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE13_M_local_dataEv@PLT	#
	movq	%rax, %rcx	#, _2
# /usr/include/c++/13/bits/basic_string.h:642:       : _M_dataplus(_M_local_data(), __a)
	movq	-56(%rbp), %rax	# __a, tmp89
	movq	%rax, %rdx	# tmp89,
	movq	%rcx, %rsi	# _2,
	movq	%rbx, %rdi	# _1,
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderC1EPcRKS3_@PLT	#
# /usr/include/c++/13/bits/basic_string.h:645: 	if (__s == 0)
	cmpq	$0, -48(%rbp)	#, __s
	jne	.L46	#,
# /usr/include/c++/13/bits/basic_string.h:646: 	  std::__throw_logic_error(__N("basic_string: "
	leaq	.LC1(%rip), %rax	#, tmp90
	movq	%rax, %rdi	# tmp90,
.LEHB10:
	call	_ZSt19__throw_logic_errorPKc@PLT	#
.L46:
# /usr/include/c++/13/bits/basic_string.h:648: 	const _CharT* __end = __s + traits_type::length(__s);
	movq	-48(%rbp), %rax	# __s, tmp91
	movq	%rax, %rdi	# tmp91,
	call	_ZNSt11char_traitsIcE6lengthEPKc	#
# /usr/include/c++/13/bits/basic_string.h:648: 	const _CharT* __end = __s + traits_type::length(__s);
	movq	-48(%rbp), %rdx	# __s, tmp95
	addq	%rdx, %rax	# tmp95, tmp94
	movq	%rax, -24(%rbp)	# tmp94, __end
# /usr/include/c++/13/bits/basic_string.h:649: 	_M_construct(__s, __end, forward_iterator_tag());
	movq	-24(%rbp), %rdx	# __end, tmp96
	movq	-48(%rbp), %rcx	# __s, tmp97
	movq	-40(%rbp), %rax	# this, tmp98
	movq	%rcx, %rsi	# tmp97,
	movq	%rax, %rdi	# tmp98,
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag	#
.LEHE10:
# /usr/include/c++/13/bits/basic_string.h:650:       }
	jmp	.L49	#
.L48:
	endbr64	
	movq	%rax, %rbx	#, tmp99
	movq	-40(%rbp), %rax	# this, _4
	movq	%rax, %rdi	# _4,
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderD1Ev	#
	movq	%rbx, %rax	# tmp99, D.45711
	movq	%rax, %rdi	# D.45711,
.LEHB11:
	call	_Unwind_Resume@PLT	#
.LEHE11:
.L49:
	movq	-8(%rbp), %rbx	#,
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2021:
	.section	.gcc_except_table
.LLSDA2021:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE2021-.LLSDACSB2021
.LLSDACSB2021:
	.uleb128 .LEHB10-.LFB2021
	.uleb128 .LEHE10-.LEHB10
	.uleb128 .L48-.LFB2021
	.uleb128 0
	.uleb128 .LEHB11-.LFB2021
	.uleb128 .LEHE11-.LEHB11
	.uleb128 0
	.uleb128 0
.LLSDACSE2021:
	.section	.text._ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2IS3_EEPKcRKS3_,"axG",@progbits,_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC5IS3_EEPKcRKS3_,comdat
	.size	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2IS3_EEPKcRKS3_, .-_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2IS3_EEPKcRKS3_
	.weak	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1IS3_EEPKcRKS3_
	.set	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1IS3_EEPKcRKS3_,_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2IS3_EEPKcRKS3_
	.section	.text._ZN9__gnu_cxx11char_traitsIcE2eqERKcS3_,"axG",@progbits,_ZN9__gnu_cxx11char_traitsIcE2eqERKcS3_,comdat
	.weak	_ZN9__gnu_cxx11char_traitsIcE2eqERKcS3_
	.type	_ZN9__gnu_cxx11char_traitsIcE2eqERKcS3_, @function
_ZN9__gnu_cxx11char_traitsIcE2eqERKcS3_:
.LFB2023:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# __c1, __c1
	movq	%rsi, -16(%rbp)	# __c2, __c2
# /usr/include/c++/13/bits/char_traits.h:137:       { return __c1 == __c2; }
	movq	-8(%rbp), %rax	# __c1, tmp86
	movzbl	(%rax), %edx	# *__c1_4(D), _1
	movq	-16(%rbp), %rax	# __c2, tmp87
	movzbl	(%rax), %eax	# *__c2_5(D), _2
# /usr/include/c++/13/bits/char_traits.h:137:       { return __c1 == __c2; }
	cmpb	%al, %dl	# _2, _1
	sete	%al	#, _6
# /usr/include/c++/13/bits/char_traits.h:137:       { return __c1 == __c2; }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2023:
	.size	_ZN9__gnu_cxx11char_traitsIcE2eqERKcS3_, .-_ZN9__gnu_cxx11char_traitsIcE2eqERKcS3_
	.section	.text._ZNSt15__new_allocatorIcED2Ev,"axG",@progbits,_ZNSt15__new_allocatorIcED5Ev,comdat
	.align 2
	.weak	_ZNSt15__new_allocatorIcED2Ev
	.type	_ZNSt15__new_allocatorIcED2Ev, @function
_ZNSt15__new_allocatorIcED2Ev:
.LFB2031:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# this, this
# /usr/include/c++/13/bits/new_allocator.h:104:       ~__new_allocator() _GLIBCXX_USE_NOEXCEPT { }
	nop	
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2031:
	.size	_ZNSt15__new_allocatorIcED2Ev, .-_ZNSt15__new_allocatorIcED2Ev
	.weak	_ZNSt15__new_allocatorIcED1Ev
	.set	_ZNSt15__new_allocatorIcED1Ev,_ZNSt15__new_allocatorIcED2Ev
	.section	.text._ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardC2EPS4_,"axG",@progbits,_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardC5EPS4_,comdat
	.align 2
	.weak	_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardC2EPS4_
	.type	_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardC2EPS4_, @function
_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardC2EPS4_:
.LFB2061:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# this, this
	movq	%rsi, -16(%rbp)	# __s, __s
# /usr/include/c++/13/bits/basic_string.tcc:239: 	  explicit _Guard(basic_string* __s) : _M_guarded(__s) { }
	movq	-8(%rbp), %rax	# this, tmp82
	movq	-16(%rbp), %rdx	# __s, tmp83
	movq	%rdx, (%rax)	# tmp83, this_2(D)->_M_guarded
# /usr/include/c++/13/bits/basic_string.tcc:239: 	  explicit _Guard(basic_string* __s) : _M_guarded(__s) { }
	nop	
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2061:
	.size	_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardC2EPS4_, .-_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardC2EPS4_
	.weak	_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardC1EPS4_
	.set	_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardC1EPS4_,_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardC2EPS4_
	.section	.text._ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD2Ev,"axG",@progbits,_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD5Ev,comdat
	.align 2
	.weak	_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD2Ev
	.type	_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD2Ev, @function
_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD2Ev:
.LFB2064:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA2064
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$16, %rsp	#,
	movq	%rdi, -8(%rbp)	# this, this
# /usr/include/c++/13/bits/basic_string.tcc:242: 	  ~_Guard() { if (_M_guarded) _M_guarded->_M_dispose(); }
	movq	-8(%rbp), %rax	# this, tmp84
	movq	(%rax), %rax	# this_5(D)->_M_guarded, _1
# /usr/include/c++/13/bits/basic_string.tcc:242: 	  ~_Guard() { if (_M_guarded) _M_guarded->_M_dispose(); }
	testq	%rax, %rax	# _1
	je	.L56	#,
# /usr/include/c++/13/bits/basic_string.tcc:242: 	  ~_Guard() { if (_M_guarded) _M_guarded->_M_dispose(); }
	movq	-8(%rbp), %rax	# this, tmp85
	movq	(%rax), %rax	# this_5(D)->_M_guarded, _2
# /usr/include/c++/13/bits/basic_string.tcc:242: 	  ~_Guard() { if (_M_guarded) _M_guarded->_M_dispose(); }
	movq	%rax, %rdi	# _2,
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv@PLT	#
.L56:
# /usr/include/c++/13/bits/basic_string.tcc:242: 	  ~_Guard() { if (_M_guarded) _M_guarded->_M_dispose(); }
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2064:
	.section	.gcc_except_table
.LLSDA2064:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE2064-.LLSDACSB2064
.LLSDACSB2064:
.LLSDACSE2064:
	.section	.text._ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD2Ev,"axG",@progbits,_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD5Ev,comdat
	.size	_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD2Ev, .-_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD2Ev
	.weak	_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD1Ev
	.set	_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD1Ev,_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD2Ev
	.section	.text._ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag,"axG",@progbits,_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag,comdat
	.align 2
	.weak	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag
	.type	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag, @function
_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag:
.LFB2059:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$96, %rsp	#,
	movq	%rdi, -72(%rbp)	# this, this
	movq	%rsi, -80(%rbp)	# __beg, __beg
	movq	%rdx, -88(%rbp)	# __end, __end
# /usr/include/c++/13/bits/basic_string.tcc:221:       basic_string<_CharT, _Traits, _Alloc>::
	movq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp109
	movq	%rax, -8(%rbp)	# tmp109, D.45714
	xorl	%eax, %eax	# tmp109
	movq	-80(%rbp), %rax	# __beg, tmp93
	movq	%rax, -48(%rbp)	# tmp93, MEM[(const char * *)_9]
	movq	-88(%rbp), %rax	# __end, tmp94
	movq	%rax, -40(%rbp)	# tmp94, __last
# /usr/include/c++/13/bits/stl_iterator_base_types.h:240:     { return typename iterator_traits<_Iter>::iterator_category(); }
	nop	
# /usr/include/c++/13/bits/stl_iterator_base_funcs.h:151:       return std::__distance(__first, __last,
	movq	-48(%rbp), %rax	# MEM[(const char * *)_9], __first.9_31
	movq	%rax, -32(%rbp)	# __first.9_31, __first
	movq	-40(%rbp), %rax	# __last, tmp95
	movq	%rax, -24(%rbp)	# tmp95, __last
# /usr/include/c++/13/bits/stl_iterator_base_funcs.h:106:       return __last - __first;
	movq	-24(%rbp), %rax	# __last, tmp96
	subq	-32(%rbp), %rax	# __first, D.45615
# /usr/include/c++/13/bits/stl_iterator_base_funcs.h:152: 			     std::__iterator_category(__first));
	nop	
# /usr/include/c++/13/bits/basic_string.tcc:225: 	size_type __dnew = static_cast<size_type>(std::distance(__beg, __end));
	movq	%rax, -56(%rbp)	# _2, __dnew
# /usr/include/c++/13/bits/basic_string.tcc:227: 	if (__dnew > size_type(_S_local_capacity))
	movq	-56(%rbp), %rax	# __dnew, __dnew.6_3
# /usr/include/c++/13/bits/basic_string.tcc:227: 	if (__dnew > size_type(_S_local_capacity))
	cmpq	$15, %rax	#, __dnew.6_3
	jbe	.L61	#,
# /usr/include/c++/13/bits/basic_string.tcc:229: 	    _M_data(_M_create(__dnew, size_type(0)));
	leaq	-56(%rbp), %rcx	#, tmp97
	movq	-72(%rbp), %rax	# this, tmp98
	movl	$0, %edx	#,
	movq	%rcx, %rsi	# tmp97,
	movq	%rax, %rdi	# tmp98,
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERmm@PLT	#
	movq	%rax, %rdx	#, _4
# /usr/include/c++/13/bits/basic_string.tcc:229: 	    _M_data(_M_create(__dnew, size_type(0)));
	movq	-72(%rbp), %rax	# this, tmp99
	movq	%rdx, %rsi	# _4,
	movq	%rax, %rdi	# tmp99,
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7_M_dataEPc@PLT	#
# /usr/include/c++/13/bits/basic_string.tcc:230: 	    _M_capacity(__dnew);
	movq	-56(%rbp), %rdx	# __dnew, __dnew.7_5
	movq	-72(%rbp), %rax	# this, tmp100
	movq	%rdx, %rsi	# __dnew.7_5,
	movq	%rax, %rdi	# tmp100,
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE11_M_capacityEm@PLT	#
	jmp	.L62	#
.L61:
	movq	-72(%rbp), %rax	# this, tmp101
	movq	%rax, -16(%rbp)	# tmp101, this
# /usr/include/c++/13/bits/basic_string.h:355:       }
	nop	
.L62:
# /usr/include/c++/13/bits/basic_string.tcc:245: 	} __guard(this);
	movq	-72(%rbp), %rdx	# this, tmp102
	leaq	-48(%rbp), %rax	#, tmp103
	movq	%rdx, %rsi	# tmp102,
	movq	%rax, %rdi	# tmp103,
	call	_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardC1EPS4_	#
# /usr/include/c++/13/bits/basic_string.tcc:247: 	this->_S_copy_chars(_M_data(), __beg, __end);
	movq	-72(%rbp), %rax	# this, tmp104
	movq	%rax, %rdi	# tmp104,
	call	_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7_M_dataEv@PLT	#
	movq	%rax, %rcx	#, _6
# /usr/include/c++/13/bits/basic_string.tcc:247: 	this->_S_copy_chars(_M_data(), __beg, __end);
	movq	-88(%rbp), %rdx	# __end, tmp105
	movq	-80(%rbp), %rax	# __beg, tmp106
	movq	%rax, %rsi	# tmp106,
	movq	%rcx, %rdi	# _6,
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE13_S_copy_charsEPcPKcS7_@PLT	#
# /usr/include/c++/13/bits/basic_string.tcc:249: 	__guard._M_guarded = 0;
	movq	$0, -48(%rbp)	#, MEM[(struct _Guard *)_9]._M_guarded
# /usr/include/c++/13/bits/basic_string.tcc:251: 	_M_set_length(__dnew);
	movq	-56(%rbp), %rdx	# __dnew, __dnew.8_7
	movq	-72(%rbp), %rax	# this, tmp107
	movq	%rdx, %rsi	# __dnew.8_7,
	movq	%rax, %rdi	# tmp107,
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE13_M_set_lengthEm@PLT	#
# /usr/include/c++/13/bits/basic_string.tcc:252:       }
	leaq	-48(%rbp), %rax	#, tmp108
	movq	%rax, %rdi	# tmp108,
	call	_ZZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tagEN6_GuardD1Ev	#
	movq	-8(%rbp), %rax	# D.45714, tmp110
	subq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp110
	je	.L63	#,
	call	__stack_chk_fail@PLT	#
.L63:
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2059:
	.size	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag, .-_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag
	.section	.text._ZNSt15__new_allocatorIiED2Ev,"axG",@progbits,_ZNSt15__new_allocatorIiED5Ev,comdat
	.align 2
	.weak	_ZNSt15__new_allocatorIiED2Ev
	.type	_ZNSt15__new_allocatorIiED2Ev, @function
_ZNSt15__new_allocatorIiED2Ev:
.LFB2123:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# this, this
# /usr/include/c++/13/bits/new_allocator.h:104:       ~__new_allocator() _GLIBCXX_USE_NOEXCEPT { }
	nop	
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2123:
	.size	_ZNSt15__new_allocatorIiED2Ev, .-_ZNSt15__new_allocatorIiED2Ev
	.weak	_ZNSt15__new_allocatorIiED1Ev
	.set	_ZNSt15__new_allocatorIiED1Ev,_ZNSt15__new_allocatorIiED2Ev
	.section	.text._ZNSt12_Vector_baseIiSaIiEEC2ERKS0_,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEEC5ERKS0_,comdat
	.align 2
	.weak	_ZNSt12_Vector_baseIiSaIiEEC2ERKS0_
	.type	_ZNSt12_Vector_baseIiSaIiEEC2ERKS0_, @function
_ZNSt12_Vector_baseIiSaIiEEC2ERKS0_:
.LFB2126:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$16, %rsp	#,
	movq	%rdi, -8(%rbp)	# this, this
	movq	%rsi, -16(%rbp)	# __a, __a
# /usr/include/c++/13/bits/stl_vector.h:322:       : _M_impl(__a) { }
	movq	-8(%rbp), %rax	# this, _1
	movq	-16(%rbp), %rdx	# __a, tmp84
	movq	%rdx, %rsi	# tmp84,
	movq	%rax, %rdi	# _1,
	call	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implC1ERKS0_	#
# /usr/include/c++/13/bits/stl_vector.h:322:       : _M_impl(__a) { }
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2126:
	.size	_ZNSt12_Vector_baseIiSaIiEEC2ERKS0_, .-_ZNSt12_Vector_baseIiSaIiEEC2ERKS0_
	.weak	_ZNSt12_Vector_baseIiSaIiEEC1ERKS0_
	.set	_ZNSt12_Vector_baseIiSaIiEEC1ERKS0_,_ZNSt12_Vector_baseIiSaIiEEC2ERKS0_
	.section	.text._ZNKSt16initializer_listIiE5beginEv,"axG",@progbits,_ZNKSt16initializer_listIiE5beginEv,comdat
	.align 2
	.weak	_ZNKSt16initializer_listIiE5beginEv
	.type	_ZNKSt16initializer_listIiE5beginEv, @function
_ZNKSt16initializer_listIiE5beginEv:
.LFB2128:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# this, this
# /usr/include/c++/13/initializer_list:73:       begin() const noexcept { return _M_array; }
	movq	-8(%rbp), %rax	# this, tmp84
	movq	(%rax), %rax	# this_2(D)->_M_array, _3
# /usr/include/c++/13/initializer_list:73:       begin() const noexcept { return _M_array; }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2128:
	.size	_ZNKSt16initializer_listIiE5beginEv, .-_ZNKSt16initializer_listIiE5beginEv
	.section	.text._ZNKSt16initializer_listIiE3endEv,"axG",@progbits,_ZNKSt16initializer_listIiE3endEv,comdat
	.align 2
	.weak	_ZNKSt16initializer_listIiE3endEv
	.type	_ZNKSt16initializer_listIiE3endEv, @function
_ZNKSt16initializer_listIiE3endEv:
.LFB2129:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	pushq	%rbx	#
	subq	$24, %rsp	#,
	.cfi_offset 3, -24
	movq	%rdi, -24(%rbp)	# this, this
# /usr/include/c++/13/initializer_list:77:       end() const noexcept { return begin() + size(); }
	movq	-24(%rbp), %rax	# this, tmp87
	movq	%rax, %rdi	# tmp87,
	call	_ZNKSt16initializer_listIiE5beginEv	#
	movq	%rax, %rbx	#, _1
# /usr/include/c++/13/initializer_list:77:       end() const noexcept { return begin() + size(); }
	movq	-24(%rbp), %rax	# this, tmp88
	movq	%rax, %rdi	# tmp88,
	call	_ZNKSt16initializer_listIiE4sizeEv	#
# /usr/include/c++/13/initializer_list:77:       end() const noexcept { return begin() + size(); }
	salq	$2, %rax	#, _3
# /usr/include/c++/13/initializer_list:77:       end() const noexcept { return begin() + size(); }
	addq	%rbx, %rax	# _1, _8
# /usr/include/c++/13/initializer_list:77:       end() const noexcept { return begin() + size(); }
	movq	-8(%rbp), %rbx	#,
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2129:
	.size	_ZNKSt16initializer_listIiE3endEv, .-_ZNKSt16initializer_listIiE3endEv
	.section	.text._ZNSt6vectorIiSaIiEE19_M_range_initializeIPKiEEvT_S5_St20forward_iterator_tag,"axG",@progbits,_ZNSt6vectorIiSaIiEE19_M_range_initializeIPKiEEvT_S5_St20forward_iterator_tag,comdat
	.align 2
	.weak	_ZNSt6vectorIiSaIiEE19_M_range_initializeIPKiEEvT_S5_St20forward_iterator_tag
	.type	_ZNSt6vectorIiSaIiEE19_M_range_initializeIPKiEEvT_S5_St20forward_iterator_tag, @function
_ZNSt6vectorIiSaIiEE19_M_range_initializeIPKiEEvT_S5_St20forward_iterator_tag:
.LFB2130:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	pushq	%rbx	#
	subq	$88, %rsp	#,
	.cfi_offset 3, -24
	movq	%rdi, -72(%rbp)	# this, this
	movq	%rsi, -80(%rbp)	# __first, __first
	movq	%rdx, -88(%rbp)	# __last, __last
# /usr/include/c++/13/bits/stl_vector.h:1687: 	_M_range_initialize(_ForwardIterator __first, _ForwardIterator __last,
	movq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp114
	movq	%rax, -24(%rbp)	# tmp114, D.45716
	xorl	%eax, %eax	# tmp114
	movq	-80(%rbp), %rax	# __first, tmp100
	movq	%rax, -64(%rbp)	# tmp100, MEM[(const int * *)_44]
	movq	-88(%rbp), %rax	# __last, tmp101
	movq	%rax, -48(%rbp)	# tmp101, __last
# /usr/include/c++/13/bits/stl_iterator_base_types.h:240:     { return typename iterator_traits<_Iter>::iterator_category(); }
	nop	
# /usr/include/c++/13/bits/stl_iterator_base_funcs.h:151:       return std::__distance(__first, __last,
	movq	-64(%rbp), %rax	# MEM[(const int * *)_44], __first.10_32
	movq	%rax, -40(%rbp)	# __first.10_32, __first
	movq	-48(%rbp), %rax	# __last, tmp102
	movq	%rax, -32(%rbp)	# tmp102, __last
# /usr/include/c++/13/bits/stl_iterator_base_funcs.h:106:       return __last - __first;
	movq	-32(%rbp), %rax	# __last, tmp103
	subq	-40(%rbp), %rax	# __first, _35
# /usr/include/c++/13/bits/stl_iterator_base_funcs.h:106:       return __last - __first;
	sarq	$2, %rax	#, tmp104
# /usr/include/c++/13/bits/stl_iterator_base_funcs.h:152: 			     std::__iterator_category(__first));
	nop	
# /usr/include/c++/13/bits/stl_vector.h:1690: 	  const size_type __n = std::distance(__first, __last);
	movq	%rax, -56(%rbp)	# D.45687, __n
# /usr/include/c++/13/bits/stl_vector.h:1692: 	    = this->_M_allocate(_S_check_init_len(__n, _M_get_Tp_allocator()));
	movq	-72(%rbp), %rbx	# this, _2
# /usr/include/c++/13/bits/stl_vector.h:1692: 	    = this->_M_allocate(_S_check_init_len(__n, _M_get_Tp_allocator()));
	movq	-72(%rbp), %rax	# this, _3
	movq	%rax, %rdi	# _3,
	call	_ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv	#
	movq	%rax, %rdx	#, _4
# /usr/include/c++/13/bits/stl_vector.h:1692: 	    = this->_M_allocate(_S_check_init_len(__n, _M_get_Tp_allocator()));
	movq	-56(%rbp), %rax	# __n, tmp105
	movq	%rdx, %rsi	# _4,
	movq	%rax, %rdi	# tmp105,
	call	_ZNSt6vectorIiSaIiEE17_S_check_init_lenEmRKS0_	#
# /usr/include/c++/13/bits/stl_vector.h:1692: 	    = this->_M_allocate(_S_check_init_len(__n, _M_get_Tp_allocator()));
	movq	%rax, %rsi	# _5,
	movq	%rbx, %rdi	# _2,
	call	_ZNSt12_Vector_baseIiSaIiEE11_M_allocateEm	#
# /usr/include/c++/13/bits/stl_vector.h:1692: 	    = this->_M_allocate(_S_check_init_len(__n, _M_get_Tp_allocator()));
	movq	-72(%rbp), %rdx	# this, tmp106
	movq	%rax, (%rdx)	# _6, this_19(D)->D.40346._M_impl.D.39653._M_start
# /usr/include/c++/13/bits/stl_vector.h:1693: 	  this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
	movq	-72(%rbp), %rax	# this, tmp107
	movq	(%rax), %rax	# this_19(D)->D.40346._M_impl.D.39653._M_start, _7
# /usr/include/c++/13/bits/stl_vector.h:1693: 	  this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
	movq	-56(%rbp), %rdx	# __n, tmp108
	salq	$2, %rdx	#, _8
	addq	%rax, %rdx	# _7, _9
# /usr/include/c++/13/bits/stl_vector.h:1693: 	  this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
	movq	-72(%rbp), %rax	# this, tmp109
	movq	%rdx, 16(%rax)	# _9, this_19(D)->D.40346._M_impl.D.39653._M_end_of_storage
# /usr/include/c++/13/bits/stl_vector.h:1697: 					_M_get_Tp_allocator());
	movq	-72(%rbp), %rax	# this, _10
	movq	%rax, %rdi	# _10,
	call	_ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv	#
	movq	%rax, %rcx	#, _11
# /usr/include/c++/13/bits/stl_vector.h:1695: 	    std::__uninitialized_copy_a(__first, __last,
	movq	-72(%rbp), %rax	# this, tmp110
	movq	(%rax), %rdx	# this_19(D)->D.40346._M_impl.D.39653._M_start, _12
	movq	-88(%rbp), %rsi	# __last, tmp111
	movq	-80(%rbp), %rax	# __first, tmp112
	movq	%rax, %rdi	# tmp112,
	call	_ZSt22__uninitialized_copy_aIPKiPiiET0_T_S4_S3_RSaIT1_E	#
# /usr/include/c++/13/bits/stl_vector.h:1694: 	  this->_M_impl._M_finish =
	movq	-72(%rbp), %rdx	# this, tmp113
	movq	%rax, 8(%rdx)	# _13, this_19(D)->D.40346._M_impl.D.39653._M_finish
# /usr/include/c++/13/bits/stl_vector.h:1698: 	}
	nop	
	movq	-24(%rbp), %rax	# D.45716, tmp115
	subq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp115
	je	.L74	#,
	call	__stack_chk_fail@PLT	#
.L74:
	movq	-8(%rbp), %rbx	#,
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2130:
	.size	_ZNSt6vectorIiSaIiEE19_M_range_initializeIPKiEEvT_S5_St20forward_iterator_tag, .-_ZNSt6vectorIiSaIiEE19_M_range_initializeIPKiEEvT_S5_St20forward_iterator_tag
	.section	.text._ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv,comdat
	.align 2
	.weak	_ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv
	.type	_ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv, @function
_ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv:
.LFB2132:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# this, this
# /usr/include/c++/13/bits/stl_vector.h:302:       { return this->_M_impl; }
	movq	-8(%rbp), %rax	# this, _2
# /usr/include/c++/13/bits/stl_vector.h:302:       { return this->_M_impl; }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2132:
	.size	_ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv, .-_ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv
	.section	.rodata
	.align 8
.LC2:
	.string	"cannot create std::vector larger than max_size()"
	.section	.text._ZNSt6vectorIiSaIiEE17_S_check_init_lenEmRKS0_,"axG",@progbits,_ZNSt6vectorIiSaIiEE17_S_check_init_lenEmRKS0_,comdat
	.weak	_ZNSt6vectorIiSaIiEE17_S_check_init_lenEmRKS0_
	.type	_ZNSt6vectorIiSaIiEE17_S_check_init_lenEmRKS0_, @function
_ZNSt6vectorIiSaIiEE17_S_check_init_lenEmRKS0_:
.LFB2134:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	pushq	%rbx	#
	subq	$72, %rsp	#,
	.cfi_offset 3, -24
	movq	%rdi, -72(%rbp)	# __n, __n
	movq	%rsi, -80(%rbp)	# __a, __a
# /usr/include/c++/13/bits/stl_vector.h:1907:       _S_check_init_len(size_type __n, const allocator_type& __a)
	movq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp93
	movq	%rax, -24(%rbp)	# tmp93, D.45717
	xorl	%eax, %eax	# tmp93
	movq	-80(%rbp), %rax	# __a, tmp86
	movq	%rax, -48(%rbp)	# tmp86, __a
	leaq	-49(%rbp), %rax	#, tmp87
	movq	%rax, -40(%rbp)	# tmp87, this
	movq	-48(%rbp), %rax	# __a, tmp88
	movq	%rax, -32(%rbp)	# tmp88, D.45661
# /usr/include/c++/13/bits/new_allocator.h:92:       __new_allocator(const __new_allocator&) _GLIBCXX_USE_NOEXCEPT { }
	nop	
# /usr/include/c++/13/bits/allocator.h:168:       : __allocator_base<_Tp>(__a) { }
	nop	
# /usr/include/c++/13/bits/stl_vector.h:1909: 	if (__n > _S_max_size(_Tp_alloc_type(__a)))
	leaq	-49(%rbp), %rax	#, tmp89
	movq	%rax, %rdi	# tmp89,
	call	_ZNSt6vectorIiSaIiEE11_S_max_sizeERKS0_	#
# /usr/include/c++/13/bits/stl_vector.h:1909: 	if (__n > _S_max_size(_Tp_alloc_type(__a)))
	cmpq	-72(%rbp), %rax	# __n, _1
	setb	%bl	#, retval.0_6
# /usr/include/c++/13/bits/allocator.h:184:       ~allocator() _GLIBCXX_NOTHROW { }
	leaq	-49(%rbp), %rax	#, tmp90
	movq	%rax, %rdi	# tmp90,
	call	_ZNSt15__new_allocatorIiED2Ev	#
	nop	
# /usr/include/c++/13/bits/stl_vector.h:1909: 	if (__n > _S_max_size(_Tp_alloc_type(__a)))
	testb	%bl, %bl	# retval.0_6
	je	.L78	#,
# /usr/include/c++/13/bits/stl_vector.h:1910: 	  __throw_length_error(
	movq	-24(%rbp), %rax	# D.45717, tmp94
	subq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp94
	je	.L79	#,
	call	__stack_chk_fail@PLT	#
.L79:
	leaq	.LC2(%rip), %rax	#, tmp91
	movq	%rax, %rdi	# tmp91,
	call	_ZSt20__throw_length_errorPKc@PLT	#
.L78:
# /usr/include/c++/13/bits/stl_vector.h:1912: 	return __n;
	movq	-72(%rbp), %rax	# __n, _8
# /usr/include/c++/13/bits/stl_vector.h:1913:       }
	movq	-24(%rbp), %rdx	# D.45717, tmp95
	subq	%fs:40, %rdx	# MEM[(<address-space-1> long unsigned int *)40B], tmp95
	je	.L81	#,
	call	__stack_chk_fail@PLT	#
.L81:
	movq	-8(%rbp), %rbx	#,
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2134:
	.size	_ZNSt6vectorIiSaIiEE17_S_check_init_lenEmRKS0_, .-_ZNSt6vectorIiSaIiEE17_S_check_init_lenEmRKS0_
	.section	.text._ZNSt12_Vector_baseIiSaIiEEC2EmRKS0_,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEEC5EmRKS0_,comdat
	.align 2
	.weak	_ZNSt12_Vector_baseIiSaIiEEC2EmRKS0_
	.type	_ZNSt12_Vector_baseIiSaIiEEC2EmRKS0_, @function
_ZNSt12_Vector_baseIiSaIiEEC2EmRKS0_:
.LFB2136:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA2136
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	pushq	%rbx	#
	subq	$40, %rsp	#,
	.cfi_offset 3, -24
	movq	%rdi, -24(%rbp)	# this, this
	movq	%rsi, -32(%rbp)	# __n, __n
	movq	%rdx, -40(%rbp)	# __a, __a
# /usr/include/c++/13/bits/stl_vector.h:334:       : _M_impl(__a)
	movq	-24(%rbp), %rax	# this, _1
	movq	-40(%rbp), %rdx	# __a, tmp85
	movq	%rdx, %rsi	# tmp85,
	movq	%rax, %rdi	# _1,
	call	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implC1ERKS0_	#
# /usr/include/c++/13/bits/stl_vector.h:335:       { _M_create_storage(__n); }
	movq	-32(%rbp), %rdx	# __n, tmp86
	movq	-24(%rbp), %rax	# this, tmp87
	movq	%rdx, %rsi	# tmp86,
	movq	%rax, %rdi	# tmp87,
.LEHB12:
	call	_ZNSt12_Vector_baseIiSaIiEE17_M_create_storageEm	#
.LEHE12:
# /usr/include/c++/13/bits/stl_vector.h:335:       { _M_create_storage(__n); }
	jmp	.L85	#
.L84:
	endbr64	
# /usr/include/c++/13/bits/stl_vector.h:335:       { _M_create_storage(__n); }
	movq	%rax, %rbx	#, tmp88
	movq	-24(%rbp), %rax	# this, _2
	movq	%rax, %rdi	# _2,
	call	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD1Ev	#
	movq	%rbx, %rax	# tmp88, D.45719
	movq	%rax, %rdi	# D.45719,
.LEHB13:
	call	_Unwind_Resume@PLT	#
.LEHE13:
.L85:
# /usr/include/c++/13/bits/stl_vector.h:335:       { _M_create_storage(__n); }
	movq	-8(%rbp), %rbx	#,
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2136:
	.section	.gcc_except_table
.LLSDA2136:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE2136-.LLSDACSB2136
.LLSDACSB2136:
	.uleb128 .LEHB12-.LFB2136
	.uleb128 .LEHE12-.LEHB12
	.uleb128 .L84-.LFB2136
	.uleb128 0
	.uleb128 .LEHB13-.LFB2136
	.uleb128 .LEHE13-.LEHB13
	.uleb128 0
	.uleb128 0
.LLSDACSE2136:
	.section	.text._ZNSt12_Vector_baseIiSaIiEEC2EmRKS0_,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEEC5EmRKS0_,comdat
	.size	_ZNSt12_Vector_baseIiSaIiEEC2EmRKS0_, .-_ZNSt12_Vector_baseIiSaIiEEC2EmRKS0_
	.weak	_ZNSt12_Vector_baseIiSaIiEEC1EmRKS0_
	.set	_ZNSt12_Vector_baseIiSaIiEEC1EmRKS0_,_ZNSt12_Vector_baseIiSaIiEEC2EmRKS0_
	.section	.text._ZNSt6vectorIiSaIiEE21_M_default_initializeEm,"axG",@progbits,_ZNSt6vectorIiSaIiEE21_M_default_initializeEm,comdat
	.align 2
	.weak	_ZNSt6vectorIiSaIiEE21_M_default_initializeEm
	.type	_ZNSt6vectorIiSaIiEE21_M_default_initializeEm, @function
_ZNSt6vectorIiSaIiEE21_M_default_initializeEm:
.LFB2138:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$16, %rsp	#,
	movq	%rdi, -8(%rbp)	# this, this
	movq	%rsi, -16(%rbp)	# __n, __n
# /usr/include/c++/13/bits/stl_vector.h:1719: 					   _M_get_Tp_allocator());
	movq	-8(%rbp), %rax	# this, _1
	movq	%rax, %rdi	# _1,
	call	_ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv	#
	movq	%rax, %rdx	#, _2
# /usr/include/c++/13/bits/stl_vector.h:1718: 	  std::__uninitialized_default_n_a(this->_M_impl._M_start, __n,
	movq	-8(%rbp), %rax	# this, tmp86
	movq	(%rax), %rax	# this_5(D)->D.40346._M_impl.D.39653._M_start, _3
	movq	-16(%rbp), %rcx	# __n, tmp87
	movq	%rcx, %rsi	# tmp87,
	movq	%rax, %rdi	# _3,
	call	_ZSt27__uninitialized_default_n_aIPimiET_S1_T0_RSaIT1_E	#
# /usr/include/c++/13/bits/stl_vector.h:1717: 	this->_M_impl._M_finish =
	movq	-8(%rbp), %rdx	# this, tmp88
	movq	%rax, 8(%rdx)	# _4, this_5(D)->D.40346._M_impl.D.39653._M_finish
# /usr/include/c++/13/bits/stl_vector.h:1720:       }
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2138:
	.size	_ZNSt6vectorIiSaIiEE21_M_default_initializeEm, .-_ZNSt6vectorIiSaIiEE21_M_default_initializeEm
	.section	.text._ZNSt12_Vector_baseIiSaIiEE13_M_deallocateEPim,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEE13_M_deallocateEPim,comdat
	.align 2
	.weak	_ZNSt12_Vector_baseIiSaIiEE13_M_deallocateEPim
	.type	_ZNSt12_Vector_baseIiSaIiEE13_M_deallocateEPim, @function
_ZNSt12_Vector_baseIiSaIiEE13_M_deallocateEPim:
.LFB2146:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$64, %rsp	#,
	movq	%rdi, -40(%rbp)	# this, this
	movq	%rsi, -48(%rbp)	# __p, __p
	movq	%rdx, -56(%rbp)	# __n, __n
# /usr/include/c++/13/bits/stl_vector.h:389: 	if (__p)
	cmpq	$0, -48(%rbp)	#, __p
	je	.L89	#,
# /usr/include/c++/13/bits/stl_vector.h:390: 	  _Tr::deallocate(_M_impl, __p, __n);
	movq	-40(%rbp), %rax	# this, _1
	movq	%rax, -24(%rbp)	# _1, __a
	movq	-48(%rbp), %rax	# __p, tmp83
	movq	%rax, -16(%rbp)	# tmp83, __p
	movq	-56(%rbp), %rax	# __n, tmp84
	movq	%rax, -8(%rbp)	# tmp84, __n
# /usr/include/c++/13/bits/alloc_traits.h:517:       { __a.deallocate(__p, __n); }
	movq	-8(%rbp), %rdx	# __n, tmp85
	movq	-16(%rbp), %rcx	# __p, tmp86
	movq	-24(%rbp), %rax	# __a, tmp87
	movq	%rcx, %rsi	# tmp86,
	movq	%rax, %rdi	# tmp87,
	call	_ZNSt15__new_allocatorIiE10deallocateEPim	#
# /usr/include/c++/13/bits/alloc_traits.h:517:       { __a.deallocate(__p, __n); }
	nop	
.L89:
# /usr/include/c++/13/bits/stl_vector.h:391:       }
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2146:
	.size	_ZNSt12_Vector_baseIiSaIiEE13_M_deallocateEPim, .-_ZNSt12_Vector_baseIiSaIiEE13_M_deallocateEPim
	.section	.text._ZNSt12_Vector_baseIiSaIiEE12_Vector_implC2ERKS0_,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEE12_Vector_implC5ERKS0_,comdat
	.align 2
	.weak	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implC2ERKS0_
	.type	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implC2ERKS0_, @function
_ZNSt12_Vector_baseIiSaIiEE12_Vector_implC2ERKS0_:
.LFB2212:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$48, %rsp	#,
	movq	%rdi, -40(%rbp)	# this, this
	movq	%rsi, -48(%rbp)	# __a, __a
	movq	-40(%rbp), %rax	# this, tmp83
	movq	%rax, -32(%rbp)	# tmp83, this
	movq	-48(%rbp), %rax	# __a, tmp84
	movq	%rax, -24(%rbp)	# tmp84, __a
	movq	-32(%rbp), %rax	# this, tmp85
	movq	%rax, -16(%rbp)	# tmp85, this
	movq	-24(%rbp), %rax	# __a, tmp86
	movq	%rax, -8(%rbp)	# tmp86, D.45626
# /usr/include/c++/13/bits/new_allocator.h:92:       __new_allocator(const __new_allocator&) _GLIBCXX_USE_NOEXCEPT { }
	nop	
# /usr/include/c++/13/bits/allocator.h:168:       : __allocator_base<_Tp>(__a) { }
	nop	
# /usr/include/c++/13/bits/stl_vector.h:147: 	: _Tp_alloc_type(__a)
	movq	-40(%rbp), %rax	# this, _1
	movq	%rax, %rdi	# _1,
	call	_ZNSt12_Vector_baseIiSaIiEE17_Vector_impl_dataC2Ev	#
# /usr/include/c++/13/bits/stl_vector.h:148: 	{ }
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2212:
	.size	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implC2ERKS0_, .-_ZNSt12_Vector_baseIiSaIiEE12_Vector_implC2ERKS0_
	.weak	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implC1ERKS0_
	.set	_ZNSt12_Vector_baseIiSaIiEE12_Vector_implC1ERKS0_,_ZNSt12_Vector_baseIiSaIiEE12_Vector_implC2ERKS0_
	.section	.text._ZNKSt16initializer_listIiE4sizeEv,"axG",@progbits,_ZNKSt16initializer_listIiE4sizeEv,comdat
	.align 2
	.weak	_ZNKSt16initializer_listIiE4sizeEv
	.type	_ZNKSt16initializer_listIiE4sizeEv, @function
_ZNKSt16initializer_listIiE4sizeEv:
.LFB2214:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# this, this
# /usr/include/c++/13/initializer_list:69:       size() const noexcept { return _M_len; }
	movq	-8(%rbp), %rax	# this, tmp84
	movq	8(%rax), %rax	# this_2(D)->_M_len, _3
# /usr/include/c++/13/initializer_list:69:       size() const noexcept { return _M_len; }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2214:
	.size	_ZNKSt16initializer_listIiE4sizeEv, .-_ZNKSt16initializer_listIiE4sizeEv
	.section	.text._ZNSt12_Vector_baseIiSaIiEE11_M_allocateEm,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEE11_M_allocateEm,comdat
	.align 2
	.weak	_ZNSt12_Vector_baseIiSaIiEE11_M_allocateEm
	.type	_ZNSt12_Vector_baseIiSaIiEE11_M_allocateEm, @function
_ZNSt12_Vector_baseIiSaIiEE11_M_allocateEm:
.LFB2217:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -24(%rbp)	# this, this
	movq	%rsi, -32(%rbp)	# __n, __n
# /usr/include/c++/13/bits/stl_vector.h:381: 	return __n != 0 ? _Tr::allocate(_M_impl, __n) : pointer();
	cmpq	$0, -32(%rbp)	#, __n
	je	.L94	#,
# /usr/include/c++/13/bits/stl_vector.h:381: 	return __n != 0 ? _Tr::allocate(_M_impl, __n) : pointer();
	movq	-24(%rbp), %rax	# this, _1
	movq	%rax, -16(%rbp)	# _1, __a
	movq	-32(%rbp), %rax	# __n, tmp85
	movq	%rax, -8(%rbp)	# tmp85, __n
# /usr/include/c++/13/bits/alloc_traits.h:482:       { return __a.allocate(__n); }
	movq	-8(%rbp), %rcx	# __n, tmp86
	movq	-16(%rbp), %rax	# __a, tmp87
	movl	$0, %edx	#,
	movq	%rcx, %rsi	# tmp86,
	movq	%rax, %rdi	# tmp87,
	call	_ZNSt15__new_allocatorIiE8allocateEmPKv	#
# /usr/include/c++/13/bits/alloc_traits.h:482:       { return __a.allocate(__n); }
	nop	
	jmp	.L96	#
.L94:
# /usr/include/c++/13/bits/stl_vector.h:381: 	return __n != 0 ? _Tr::allocate(_M_impl, __n) : pointer();
	movl	$0, %eax	#, D.45630
.L96:
# /usr/include/c++/13/bits/stl_vector.h:382:       }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2217:
	.size	_ZNSt12_Vector_baseIiSaIiEE11_M_allocateEm, .-_ZNSt12_Vector_baseIiSaIiEE11_M_allocateEm
	.section	.text._ZSt22__uninitialized_copy_aIPKiPiiET0_T_S4_S3_RSaIT1_E,"axG",@progbits,_ZSt22__uninitialized_copy_aIPKiPiiET0_T_S4_S3_RSaIT1_E,comdat
	.weak	_ZSt22__uninitialized_copy_aIPKiPiiET0_T_S4_S3_RSaIT1_E
	.type	_ZSt22__uninitialized_copy_aIPKiPiiET0_T_S4_S3_RSaIT1_E, @function
_ZSt22__uninitialized_copy_aIPKiPiiET0_T_S4_S3_RSaIT1_E:
.LFB2218:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -8(%rbp)	# __first, __first
	movq	%rsi, -16(%rbp)	# __last, __last
	movq	%rdx, -24(%rbp)	# __result, __result
	movq	%rcx, -32(%rbp)	# D.44252, D.44252
# /usr/include/c++/13/bits/stl_uninitialized.h:373:       return std::uninitialized_copy(__first, __last, __result);
	movq	-24(%rbp), %rdx	# __result, tmp84
	movq	-16(%rbp), %rcx	# __last, tmp85
	movq	-8(%rbp), %rax	# __first, tmp86
	movq	%rcx, %rsi	# tmp85,
	movq	%rax, %rdi	# tmp86,
	call	_ZSt18uninitialized_copyIPKiPiET0_T_S4_S3_	#
# /usr/include/c++/13/bits/stl_uninitialized.h:374:     }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2218:
	.size	_ZSt22__uninitialized_copy_aIPKiPiiET0_T_S4_S3_RSaIT1_E, .-_ZSt22__uninitialized_copy_aIPKiPiiET0_T_S4_S3_RSaIT1_E
	.section	.text._ZSt8_DestroyIPiEvT_S1_,"axG",@progbits,_ZSt8_DestroyIPiEvT_S1_,comdat
	.weak	_ZSt8_DestroyIPiEvT_S1_
	.type	_ZSt8_DestroyIPiEvT_S1_, @function
_ZSt8_DestroyIPiEvT_S1_:
.LFB2219:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$16, %rsp	#,
	movq	%rdi, -8(%rbp)	# __first, __first
	movq	%rsi, -16(%rbp)	# __last, __last
# /usr/include/c++/13/bits/stl_construct.h:196: 	__destroy(__first, __last);
	movq	-16(%rbp), %rdx	# __last, tmp82
	movq	-8(%rbp), %rax	# __first, tmp83
	movq	%rdx, %rsi	# tmp82,
	movq	%rax, %rdi	# tmp83,
	call	_ZNSt12_Destroy_auxILb1EE9__destroyIPiEEvT_S3_	#
# /usr/include/c++/13/bits/stl_construct.h:197:     }
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2219:
	.size	_ZSt8_DestroyIPiEvT_S1_, .-_ZSt8_DestroyIPiEvT_S1_
	.section	.text._ZNSt6vectorIiSaIiEE11_S_max_sizeERKS0_,"axG",@progbits,_ZNSt6vectorIiSaIiEE11_S_max_sizeERKS0_,comdat
	.weak	_ZNSt6vectorIiSaIiEE11_S_max_sizeERKS0_
	.type	_ZNSt6vectorIiSaIiEE11_S_max_sizeERKS0_, @function
_ZNSt6vectorIiSaIiEE11_S_max_sizeERKS0_:
.LFB2222:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$64, %rsp	#,
	movq	%rdi, -56(%rbp)	# __a, __a
# /usr/include/c++/13/bits/stl_vector.h:1916:       _S_max_size(const _Tp_alloc_type& __a) _GLIBCXX_NOEXCEPT
	movq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp92
	movq	%rax, -8(%rbp)	# tmp92, D.45720
	xorl	%eax, %eax	# tmp92
# /usr/include/c++/13/bits/stl_vector.h:1921: 	const size_t __diffmax
	movabsq	$2305843009213693951, %rax	#, tmp95
	movq	%rax, -48(%rbp)	# tmp95, __diffmax
	movq	-56(%rbp), %rax	# __a, tmp86
	movq	%rax, -32(%rbp)	# tmp86, __a
	movq	-32(%rbp), %rax	# __a, tmp87
	movq	%rax, -24(%rbp)	# tmp87, this
	movq	-24(%rbp), %rax	# this, tmp88
	movq	%rax, -16(%rbp)	# tmp88, this
# /usr/include/c++/13/bits/new_allocator.h:233: 	return std::size_t(__PTRDIFF_MAX__) / sizeof(_Tp);
	movabsq	$2305843009213693951, %rax	#, D.45651
# /usr/include/c++/13/bits/new_allocator.h:183:       { return _M_max_size(); }
	nop	
# /usr/include/c++/13/bits/alloc_traits.h:574: 	return __a.max_size();
	nop	
# /usr/include/c++/13/bits/stl_vector.h:1923: 	const size_t __allocmax = _Alloc_traits::max_size(__a);
	movq	%rax, -40(%rbp)	# D.45651, __allocmax
# /usr/include/c++/13/bits/stl_vector.h:1924: 	return (std::min)(__diffmax, __allocmax);
	leaq	-40(%rbp), %rdx	#, tmp89
	leaq	-48(%rbp), %rax	#, tmp90
	movq	%rdx, %rsi	# tmp89,
	movq	%rax, %rdi	# tmp90,
	call	_ZSt3minImERKT_S2_S2_	#
# /usr/include/c++/13/bits/stl_vector.h:1924: 	return (std::min)(__diffmax, __allocmax);
	movq	(%rax), %rax	# *_2, _8
# /usr/include/c++/13/bits/stl_vector.h:1925:       }
	movq	-8(%rbp), %rdx	# D.45720, tmp93
	subq	%fs:40, %rdx	# MEM[(<address-space-1> long unsigned int *)40B], tmp93
	je	.L106	#,
	call	__stack_chk_fail@PLT	#
.L106:
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2222:
	.size	_ZNSt6vectorIiSaIiEE11_S_max_sizeERKS0_, .-_ZNSt6vectorIiSaIiEE11_S_max_sizeERKS0_
	.section	.text._ZNSt12_Vector_baseIiSaIiEE17_M_create_storageEm,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEE17_M_create_storageEm,comdat
	.align 2
	.weak	_ZNSt12_Vector_baseIiSaIiEE17_M_create_storageEm
	.type	_ZNSt12_Vector_baseIiSaIiEE17_M_create_storageEm, @function
_ZNSt12_Vector_baseIiSaIiEE17_M_create_storageEm:
.LFB2223:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$16, %rsp	#,
	movq	%rdi, -8(%rbp)	# this, this
	movq	%rsi, -16(%rbp)	# __n, __n
# /usr/include/c++/13/bits/stl_vector.h:398: 	this->_M_impl._M_start = this->_M_allocate(__n);
	movq	-16(%rbp), %rdx	# __n, tmp87
	movq	-8(%rbp), %rax	# this, tmp88
	movq	%rdx, %rsi	# tmp87,
	movq	%rax, %rdi	# tmp88,
	call	_ZNSt12_Vector_baseIiSaIiEE11_M_allocateEm	#
# /usr/include/c++/13/bits/stl_vector.h:398: 	this->_M_impl._M_start = this->_M_allocate(__n);
	movq	-8(%rbp), %rdx	# this, tmp89
	movq	%rax, (%rdx)	# _1, this_7(D)->_M_impl.D.39653._M_start
# /usr/include/c++/13/bits/stl_vector.h:399: 	this->_M_impl._M_finish = this->_M_impl._M_start;
	movq	-8(%rbp), %rax	# this, tmp90
	movq	(%rax), %rdx	# this_7(D)->_M_impl.D.39653._M_start, _2
# /usr/include/c++/13/bits/stl_vector.h:399: 	this->_M_impl._M_finish = this->_M_impl._M_start;
	movq	-8(%rbp), %rax	# this, tmp91
	movq	%rdx, 8(%rax)	# _2, this_7(D)->_M_impl.D.39653._M_finish
# /usr/include/c++/13/bits/stl_vector.h:400: 	this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
	movq	-8(%rbp), %rax	# this, tmp92
	movq	(%rax), %rax	# this_7(D)->_M_impl.D.39653._M_start, _3
# /usr/include/c++/13/bits/stl_vector.h:400: 	this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
	movq	-16(%rbp), %rdx	# __n, tmp93
	salq	$2, %rdx	#, _4
	addq	%rax, %rdx	# _3, _5
# /usr/include/c++/13/bits/stl_vector.h:400: 	this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
	movq	-8(%rbp), %rax	# this, tmp94
	movq	%rdx, 16(%rax)	# _5, this_7(D)->_M_impl.D.39653._M_end_of_storage
# /usr/include/c++/13/bits/stl_vector.h:401:       }
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2223:
	.size	_ZNSt12_Vector_baseIiSaIiEE17_M_create_storageEm, .-_ZNSt12_Vector_baseIiSaIiEE17_M_create_storageEm
	.section	.text._ZSt27__uninitialized_default_n_aIPimiET_S1_T0_RSaIT1_E,"axG",@progbits,_ZSt27__uninitialized_default_n_aIPimiET_S1_T0_RSaIT1_E,comdat
	.weak	_ZSt27__uninitialized_default_n_aIPimiET_S1_T0_RSaIT1_E
	.type	_ZSt27__uninitialized_default_n_aIPimiET_S1_T0_RSaIT1_E, @function
_ZSt27__uninitialized_default_n_aIPimiET_S1_T0_RSaIT1_E:
.LFB2224:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -8(%rbp)	# __first, __first
	movq	%rsi, -16(%rbp)	# __n, __n
	movq	%rdx, -24(%rbp)	# D.44402, D.44402
# /usr/include/c++/13/bits/stl_uninitialized.h:779:     { return std::__uninitialized_default_n(__first, __n); }
	movq	-16(%rbp), %rdx	# __n, tmp84
	movq	-8(%rbp), %rax	# __first, tmp85
	movq	%rdx, %rsi	# tmp84,
	movq	%rax, %rdi	# tmp85,
	call	_ZSt25__uninitialized_default_nIPimET_S1_T0_	#
# /usr/include/c++/13/bits/stl_uninitialized.h:779:     { return std::__uninitialized_default_n(__first, __n); }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2224:
	.size	_ZSt27__uninitialized_default_n_aIPimiET_S1_T0_RSaIT1_E, .-_ZSt27__uninitialized_default_n_aIPimiET_S1_T0_RSaIT1_E
	.section	.text._ZNSt12_Vector_baseIiSaIiEE17_Vector_impl_dataC2Ev,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEE17_Vector_impl_dataC5Ev,comdat
	.align 2
	.weak	_ZNSt12_Vector_baseIiSaIiEE17_Vector_impl_dataC2Ev
	.type	_ZNSt12_Vector_baseIiSaIiEE17_Vector_impl_dataC2Ev, @function
_ZNSt12_Vector_baseIiSaIiEE17_Vector_impl_dataC2Ev:
.LFB2280:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# this, this
# /usr/include/c++/13/bits/stl_vector.h:100: 	: _M_start(), _M_finish(), _M_end_of_storage()
	movq	-8(%rbp), %rax	# this, tmp82
	movq	$0, (%rax)	#, this_2(D)->_M_start
# /usr/include/c++/13/bits/stl_vector.h:100: 	: _M_start(), _M_finish(), _M_end_of_storage()
	movq	-8(%rbp), %rax	# this, tmp83
	movq	$0, 8(%rax)	#, this_2(D)->_M_finish
# /usr/include/c++/13/bits/stl_vector.h:100: 	: _M_start(), _M_finish(), _M_end_of_storage()
	movq	-8(%rbp), %rax	# this, tmp84
	movq	$0, 16(%rax)	#, this_2(D)->_M_end_of_storage
# /usr/include/c++/13/bits/stl_vector.h:101: 	{ }
	nop	
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2280:
	.size	_ZNSt12_Vector_baseIiSaIiEE17_Vector_impl_dataC2Ev, .-_ZNSt12_Vector_baseIiSaIiEE17_Vector_impl_dataC2Ev
	.weak	_ZNSt12_Vector_baseIiSaIiEE17_Vector_impl_dataC1Ev
	.set	_ZNSt12_Vector_baseIiSaIiEE17_Vector_impl_dataC1Ev,_ZNSt12_Vector_baseIiSaIiEE17_Vector_impl_dataC2Ev
	.section	.text._ZSt18uninitialized_copyIPKiPiET0_T_S4_S3_,"axG",@progbits,_ZSt18uninitialized_copyIPKiPiET0_T_S4_S3_,comdat
	.weak	_ZSt18uninitialized_copyIPKiPiET0_T_S4_S3_
	.type	_ZSt18uninitialized_copyIPKiPiET0_T_S4_S3_, @function
_ZSt18uninitialized_copyIPKiPiET0_T_S4_S3_:
.LFB2283:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$48, %rsp	#,
	movq	%rdi, -24(%rbp)	# __first, __first
	movq	%rsi, -32(%rbp)	# __last, __last
	movq	%rdx, -40(%rbp)	# __result, __result
# /usr/include/c++/13/bits/stl_uninitialized.h:174:       const bool __can_memmove = __is_trivial(_ValueType1);
	movb	$1, -2(%rbp)	#, __can_memmove
# /usr/include/c++/13/bits/stl_uninitialized.h:181:       const bool __assignable
	movb	$1, -1(%rbp)	#, __assignable
# /usr/include/c++/13/bits/stl_uninitialized.h:185: 	__uninit_copy(__first, __last, __result);
	movq	-40(%rbp), %rdx	# __result, tmp84
	movq	-32(%rbp), %rcx	# __last, tmp85
	movq	-24(%rbp), %rax	# __first, tmp86
	movq	%rcx, %rsi	# tmp85,
	movq	%rax, %rdi	# tmp86,
	call	_ZNSt20__uninitialized_copyILb1EE13__uninit_copyIPKiPiEET0_T_S6_S5_	#
# /usr/include/c++/13/bits/stl_uninitialized.h:186:     }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2283:
	.size	_ZSt18uninitialized_copyIPKiPiET0_T_S4_S3_, .-_ZSt18uninitialized_copyIPKiPiET0_T_S4_S3_
	.section	.text._ZNSt12_Destroy_auxILb1EE9__destroyIPiEEvT_S3_,"axG",@progbits,_ZNSt12_Destroy_auxILb1EE9__destroyIPiEEvT_S3_,comdat
	.weak	_ZNSt12_Destroy_auxILb1EE9__destroyIPiEEvT_S3_
	.type	_ZNSt12_Destroy_auxILb1EE9__destroyIPiEEvT_S3_, @function
_ZNSt12_Destroy_auxILb1EE9__destroyIPiEEvT_S3_:
.LFB2285:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# D.44712, D.44712
	movq	%rsi, -16(%rbp)	# D.44713, D.44713
# /usr/include/c++/13/bits/stl_construct.h:172:         __destroy(_ForwardIterator, _ForwardIterator) { }
	nop	
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2285:
	.size	_ZNSt12_Destroy_auxILb1EE9__destroyIPiEEvT_S3_, .-_ZNSt12_Destroy_auxILb1EE9__destroyIPiEEvT_S3_
	.section	.text._ZSt3minImERKT_S2_S2_,"axG",@progbits,_ZSt3minImERKT_S2_S2_,comdat
	.weak	_ZSt3minImERKT_S2_S2_
	.type	_ZSt3minImERKT_S2_S2_, @function
_ZSt3minImERKT_S2_S2_:
.LFB2287:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# __a, __a
	movq	%rsi, -16(%rbp)	# __b, __b
# /usr/include/c++/13/bits/stl_algobase.h:238:       if (__b < __a)
	movq	-16(%rbp), %rax	# __b, tmp86
	movq	(%rax), %rdx	# *__b_5(D), _1
	movq	-8(%rbp), %rax	# __a, tmp87
	movq	(%rax), %rax	# *__a_6(D), _2
# /usr/include/c++/13/bits/stl_algobase.h:238:       if (__b < __a)
	cmpq	%rax, %rdx	# _2, _1
	jnb	.L115	#,
# /usr/include/c++/13/bits/stl_algobase.h:239: 	return __b;
	movq	-16(%rbp), %rax	# __b, _3
	jmp	.L116	#
.L115:
# /usr/include/c++/13/bits/stl_algobase.h:240:       return __a;
	movq	-8(%rbp), %rax	# __a, _3
.L116:
# /usr/include/c++/13/bits/stl_algobase.h:241:     }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2287:
	.size	_ZSt3minImERKT_S2_S2_, .-_ZSt3minImERKT_S2_S2_
	.section	.text._ZSt25__uninitialized_default_nIPimET_S1_T0_,"axG",@progbits,_ZSt25__uninitialized_default_nIPimET_S1_T0_,comdat
	.weak	_ZSt25__uninitialized_default_nIPimET_S1_T0_
	.type	_ZSt25__uninitialized_default_nIPimET_S1_T0_, @function
_ZSt25__uninitialized_default_nIPimET_S1_T0_:
.LFB2288:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -24(%rbp)	# __first, __first
	movq	%rsi, -32(%rbp)	# __n, __n
# /usr/include/c++/13/bits/stl_uninitialized.h:707:       constexpr bool __can_fill
	movb	$1, -1(%rbp)	#, __can_fill
# /usr/include/c++/13/bits/stl_uninitialized.h:712: 	__uninit_default_n(__first, __n);
	movq	-32(%rbp), %rdx	# __n, tmp84
	movq	-24(%rbp), %rax	# __first, tmp85
	movq	%rdx, %rsi	# tmp84,
	movq	%rax, %rdi	# tmp85,
	call	_ZNSt27__uninitialized_default_n_1ILb1EE18__uninit_default_nIPimEET_S3_T0_	#
# /usr/include/c++/13/bits/stl_uninitialized.h:713:     }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2288:
	.size	_ZSt25__uninitialized_default_nIPimET_S1_T0_, .-_ZSt25__uninitialized_default_nIPimET_S1_T0_
	.section	.text._ZNSt15__new_allocatorIiE10deallocateEPim,"axG",@progbits,_ZNSt15__new_allocatorIiE10deallocateEPim,comdat
	.align 2
	.weak	_ZNSt15__new_allocatorIiE10deallocateEPim
	.type	_ZNSt15__new_allocatorIiE10deallocateEPim, @function
_ZNSt15__new_allocatorIiE10deallocateEPim:
.LFB2289:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -8(%rbp)	# this, this
	movq	%rsi, -16(%rbp)	# __p, __p
	movq	%rdx, -24(%rbp)	# __n, __n
# /usr/include/c++/13/bits/new_allocator.h:172: 	_GLIBCXX_OPERATOR_DELETE(_GLIBCXX_SIZED_DEALLOC(__p, __n));
	movq	-24(%rbp), %rax	# __n, tmp83
	leaq	0(,%rax,4), %rdx	#, _2
	movq	-16(%rbp), %rax	# __p, tmp84
	movq	%rdx, %rsi	# _2,
	movq	%rax, %rdi	# tmp84,
	call	_ZdlPvm@PLT	#
# /usr/include/c++/13/bits/new_allocator.h:173:       }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2289:
	.size	_ZNSt15__new_allocatorIiE10deallocateEPim, .-_ZNSt15__new_allocatorIiE10deallocateEPim
	.section	.text._ZNSt15__new_allocatorIiE8allocateEmPKv,"axG",@progbits,_ZNSt15__new_allocatorIiE8allocateEmPKv,comdat
	.align 2
	.weak	_ZNSt15__new_allocatorIiE8allocateEmPKv
	.type	_ZNSt15__new_allocatorIiE8allocateEmPKv, @function
_ZNSt15__new_allocatorIiE8allocateEmPKv:
.LFB2310:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$48, %rsp	#,
	movq	%rdi, -24(%rbp)	# this, this
	movq	%rsi, -32(%rbp)	# __n, __n
	movq	%rdx, -40(%rbp)	# D.44848, D.44848
	movq	-24(%rbp), %rax	# this, tmp90
	movq	%rax, -8(%rbp)	# tmp90, this
# /usr/include/c++/13/bits/new_allocator.h:233: 	return std::size_t(__PTRDIFF_MAX__) / sizeof(_Tp);
	movabsq	$2305843009213693951, %rax	#, D.45664
# /usr/include/c++/13/bits/new_allocator.h:134: 	if (__builtin_expect(__n > this->_M_max_size(), false))
	cmpq	-32(%rbp), %rax	# __n, D.45664
	setb	%al	#, _2
# /usr/include/c++/13/bits/new_allocator.h:134: 	if (__builtin_expect(__n > this->_M_max_size(), false))
	movzbl	%al, %eax	# _2, _3
# /usr/include/c++/13/bits/new_allocator.h:134: 	if (__builtin_expect(__n > this->_M_max_size(), false))
	testq	%rax, %rax	# _4
	setne	%al	#, retval.2_9
# /usr/include/c++/13/bits/new_allocator.h:134: 	if (__builtin_expect(__n > this->_M_max_size(), false))
	testb	%al, %al	# retval.2_9
	je	.L123	#,
# /usr/include/c++/13/bits/new_allocator.h:138: 	    if (__n > (std::size_t(-1) / sizeof(_Tp)))
	movabsq	$4611686018427387903, %rax	#, tmp91
	cmpq	-32(%rbp), %rax	# __n, tmp91
	jnb	.L124	#,
# /usr/include/c++/13/bits/new_allocator.h:139: 	      std::__throw_bad_array_new_length();
	call	_ZSt28__throw_bad_array_new_lengthv@PLT	#
.L124:
# /usr/include/c++/13/bits/new_allocator.h:140: 	    std::__throw_bad_alloc();
	call	_ZSt17__throw_bad_allocv@PLT	#
.L123:
# /usr/include/c++/13/bits/new_allocator.h:151: 	return static_cast<_Tp*>(_GLIBCXX_OPERATOR_NEW(__n * sizeof(_Tp)));
	movq	-32(%rbp), %rax	# __n, tmp92
	salq	$2, %rax	#, _6
	movq	%rax, %rdi	# _6,
	call	_Znwm@PLT	#
# /usr/include/c++/13/bits/new_allocator.h:151: 	return static_cast<_Tp*>(_GLIBCXX_OPERATOR_NEW(__n * sizeof(_Tp)));
	nop	
# /usr/include/c++/13/bits/new_allocator.h:152:       }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2310:
	.size	_ZNSt15__new_allocatorIiE8allocateEmPKv, .-_ZNSt15__new_allocatorIiE8allocateEmPKv
	.section	.text._ZNSt20__uninitialized_copyILb1EE13__uninit_copyIPKiPiEET0_T_S6_S5_,"axG",@progbits,_ZNSt20__uninitialized_copyILb1EE13__uninit_copyIPKiPiEET0_T_S6_S5_,comdat
	.weak	_ZNSt20__uninitialized_copyILb1EE13__uninit_copyIPKiPiEET0_T_S6_S5_
	.type	_ZNSt20__uninitialized_copyILb1EE13__uninit_copyIPKiPiEET0_T_S6_S5_, @function
_ZNSt20__uninitialized_copyILb1EE13__uninit_copyIPKiPiEET0_T_S6_S5_:
.LFB2311:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -8(%rbp)	# __first, __first
	movq	%rsi, -16(%rbp)	# __last, __last
	movq	%rdx, -24(%rbp)	# __result, __result
# /usr/include/c++/13/bits/stl_uninitialized.h:147:         { return std::copy(__first, __last, __result); }
	movq	-24(%rbp), %rdx	# __result, tmp84
	movq	-16(%rbp), %rcx	# __last, tmp85
	movq	-8(%rbp), %rax	# __first, tmp86
	movq	%rcx, %rsi	# tmp85,
	movq	%rax, %rdi	# tmp86,
	call	_ZSt4copyIPKiPiET0_T_S4_S3_	#
# /usr/include/c++/13/bits/stl_uninitialized.h:147:         { return std::copy(__first, __last, __result); }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2311:
	.size	_ZNSt20__uninitialized_copyILb1EE13__uninit_copyIPKiPiEET0_T_S6_S5_, .-_ZNSt20__uninitialized_copyILb1EE13__uninit_copyIPKiPiEET0_T_S6_S5_
	.section	.text._ZNSt27__uninitialized_default_n_1ILb1EE18__uninit_default_nIPimEET_S3_T0_,"axG",@progbits,_ZNSt27__uninitialized_default_n_1ILb1EE18__uninit_default_nIPimEET_S3_T0_,comdat
	.weak	_ZNSt27__uninitialized_default_n_1ILb1EE18__uninit_default_nIPimEET_S3_T0_
	.type	_ZNSt27__uninitialized_default_n_1ILb1EE18__uninit_default_nIPimEET_S3_T0_, @function
_ZNSt27__uninitialized_default_n_1ILb1EE18__uninit_default_nIPimEET_S3_T0_:
.LFB2313:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -24(%rbp)	# __first, __first
	movq	%rsi, -32(%rbp)	# __n, __n
# /usr/include/c++/13/bits/stl_uninitialized.h:662: 	  if (__n > 0)
	cmpq	$0, -32(%rbp)	#, __n
	je	.L129	#,
# /usr/include/c++/13/bits/stl_uninitialized.h:665: 		= std::__addressof(*__first);
	movq	-24(%rbp), %rax	# __first, tmp86
	movq	%rax, %rdi	# tmp86,
	call	_ZSt11__addressofIiEPT_RS0_	#
	movq	%rax, -8(%rbp)	# tmp87, __val
# /usr/include/c++/13/bits/stl_uninitialized.h:666: 	      std::_Construct(__val);
	movq	-8(%rbp), %rax	# __val, tmp88
	movq	%rax, %rdi	# tmp88,
	call	_ZSt10_ConstructIiJEEvPT_DpOT0_	#
# /usr/include/c++/13/bits/stl_uninitialized.h:667: 	      ++__first;
	addq	$4, -24(%rbp)	#, __first
# /usr/include/c++/13/bits/stl_uninitialized.h:668: 	      __first = std::fill_n(__first, __n - 1, *__val);
	movq	-32(%rbp), %rax	# __n, tmp89
	leaq	-1(%rax), %rcx	#, _1
	movq	-8(%rbp), %rdx	# __val, tmp90
	movq	-24(%rbp), %rax	# __first, tmp91
	movq	%rcx, %rsi	# _1,
	movq	%rax, %rdi	# tmp91,
	call	_ZSt6fill_nIPimiET_S1_T0_RKT1_	#
# /usr/include/c++/13/bits/stl_uninitialized.h:668: 	      __first = std::fill_n(__first, __n - 1, *__val);
	movq	%rax, -24(%rbp)	# _12, __first
.L129:
# /usr/include/c++/13/bits/stl_uninitialized.h:670: 	  return __first;
	movq	-24(%rbp), %rax	# __first, _14
# /usr/include/c++/13/bits/stl_uninitialized.h:671: 	}
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2313:
	.size	_ZNSt27__uninitialized_default_n_1ILb1EE18__uninit_default_nIPimEET_S3_T0_, .-_ZNSt27__uninitialized_default_n_1ILb1EE18__uninit_default_nIPimEET_S3_T0_
	.section	.text._ZSt4copyIPKiPiET0_T_S4_S3_,"axG",@progbits,_ZSt4copyIPKiPiET0_T_S4_S3_,comdat
	.weak	_ZSt4copyIPKiPiET0_T_S4_S3_
	.type	_ZSt4copyIPKiPiET0_T_S4_S3_, @function
_ZSt4copyIPKiPiET0_T_S4_S3_:
.LFB2323:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	pushq	%rbx	#
	subq	$40, %rsp	#,
	.cfi_offset 3, -24
	movq	%rdi, -24(%rbp)	# __first, __first
	movq	%rsi, -32(%rbp)	# __last, __last
	movq	%rdx, -40(%rbp)	# __result, __result
# /usr/include/c++/13/bits/stl_algobase.h:633: 	     (std::__miter_base(__first), std::__miter_base(__last), __result);
	movq	-32(%rbp), %rax	# __last, tmp86
	movq	%rax, %rdi	# tmp86,
	call	_ZSt12__miter_baseIPKiET_S2_	#
	movq	%rax, %rbx	#, _1
# /usr/include/c++/13/bits/stl_algobase.h:633: 	     (std::__miter_base(__first), std::__miter_base(__last), __result);
	movq	-24(%rbp), %rax	# __first, tmp87
	movq	%rax, %rdi	# tmp87,
	call	_ZSt12__miter_baseIPKiET_S2_	#
	movq	%rax, %rcx	#, _2
# /usr/include/c++/13/bits/stl_algobase.h:633: 	     (std::__miter_base(__first), std::__miter_base(__last), __result);
	movq	-40(%rbp), %rax	# __result, tmp88
	movq	%rax, %rdx	# tmp88,
	movq	%rbx, %rsi	# _1,
	movq	%rcx, %rdi	# _2,
	call	_ZSt13__copy_move_aILb0EPKiPiET1_T0_S4_S3_	#
# /usr/include/c++/13/bits/stl_algobase.h:634:     }
	movq	-8(%rbp), %rbx	#,
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2323:
	.size	_ZSt4copyIPKiPiET0_T_S4_S3_, .-_ZSt4copyIPKiPiET0_T_S4_S3_
	.section	.text._ZSt11__addressofIiEPT_RS0_,"axG",@progbits,_ZSt11__addressofIiEPT_RS0_,comdat
	.weak	_ZSt11__addressofIiEPT_RS0_
	.type	_ZSt11__addressofIiEPT_RS0_, @function
_ZSt11__addressofIiEPT_RS0_:
.LFB2324:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# __r, __r
# /usr/include/c++/13/bits/move.h:52:     { return __builtin_addressof(__r); }
	movq	-8(%rbp), %rax	# __r, _2
# /usr/include/c++/13/bits/move.h:52:     { return __builtin_addressof(__r); }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2324:
	.size	_ZSt11__addressofIiEPT_RS0_, .-_ZSt11__addressofIiEPT_RS0_
	.section	.text._ZSt10_ConstructIiJEEvPT_DpOT0_,"axG",@progbits,_ZSt10_ConstructIiJEEvPT_DpOT0_,comdat
	.weak	_ZSt10_ConstructIiJEEvPT_DpOT0_
	.type	_ZSt10_ConstructIiJEEvPT_DpOT0_, @function
_ZSt10_ConstructIiJEEvPT_DpOT0_:
.LFB2325:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$16, %rsp	#,
	movq	%rdi, -8(%rbp)	# __p, __p
# /usr/include/c++/13/bits/stl_construct.h:119:       ::new((void*)__p) _Tp(std::forward<_Args>(__args)...);
	movq	-8(%rbp), %rax	# __p, _2
# /usr/include/c++/13/bits/stl_construct.h:119:       ::new((void*)__p) _Tp(std::forward<_Args>(__args)...);
	movq	%rax, %rsi	# _2,
	movl	$4, %edi	#,
	call	_ZnwmPv	#
# /usr/include/c++/13/bits/stl_construct.h:119:       ::new((void*)__p) _Tp(std::forward<_Args>(__args)...);
	movl	$0, (%rax)	#, MEM[(int *)_5]
# /usr/include/c++/13/bits/stl_construct.h:120:     }
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2325:
	.size	_ZSt10_ConstructIiJEEvPT_DpOT0_, .-_ZSt10_ConstructIiJEEvPT_DpOT0_
	.section	.text._ZSt6fill_nIPimiET_S1_T0_RKT1_,"axG",@progbits,_ZSt6fill_nIPimiET_S1_T0_RKT1_,comdat
	.weak	_ZSt6fill_nIPimiET_S1_T0_RKT1_
	.type	_ZSt6fill_nIPimiET_S1_T0_RKT1_, @function
_ZSt6fill_nIPimiET_S1_T0_RKT1_:
.LFB2326:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -8(%rbp)	# __first, __first
	movq	%rsi, -16(%rbp)	# __n, __n
	movq	%rdx, -24(%rbp)	# __value, __value
# /usr/include/c++/13/bits/stl_iterator_base_types.h:240:     { return typename iterator_traits<_Iter>::iterator_category(); }
	nop	
# /usr/include/c++/13/bits/stl_algobase.h:1157:       return std::__fill_n_a(__first, std::__size_to_integer(__n), __value,
	movq	-16(%rbp), %rax	# __n, tmp89
	movq	%rax, %rdi	# tmp89,
	call	_ZSt17__size_to_integerm	#
	movq	%rax, %rcx	#, _1
# /usr/include/c++/13/bits/stl_algobase.h:1157:       return std::__fill_n_a(__first, std::__size_to_integer(__n), __value,
	movq	-8(%rbp), %rax	# __first, __first.3_2
	movq	-24(%rbp), %rdx	# __value, tmp90
	movq	%rcx, %rsi	# _1,
	movq	%rax, %rdi	# __first.3_2,
	call	_ZSt10__fill_n_aIPimiET_S1_T0_RKT1_St26random_access_iterator_tag	#
# /usr/include/c++/13/bits/stl_algobase.h:1159:     }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2326:
	.size	_ZSt6fill_nIPimiET_S1_T0_RKT1_, .-_ZSt6fill_nIPimiET_S1_T0_RKT1_
	.section	.text._ZSt12__miter_baseIPKiET_S2_,"axG",@progbits,_ZSt12__miter_baseIPKiET_S2_,comdat
	.weak	_ZSt12__miter_baseIPKiET_S2_
	.type	_ZSt12__miter_baseIPKiET_S2_, @function
_ZSt12__miter_baseIPKiET_S2_:
.LFB2332:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# __it, __it
# /usr/include/c++/13/bits/cpp_type_traits.h:608:     { return __it; }
	movq	-8(%rbp), %rax	# __it, _2
# /usr/include/c++/13/bits/cpp_type_traits.h:608:     { return __it; }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2332:
	.size	_ZSt12__miter_baseIPKiET_S2_, .-_ZSt12__miter_baseIPKiET_S2_
	.section	.text._ZSt13__copy_move_aILb0EPKiPiET1_T0_S4_S3_,"axG",@progbits,_ZSt13__copy_move_aILb0EPKiPiET1_T0_S4_S3_,comdat
	.weak	_ZSt13__copy_move_aILb0EPKiPiET1_T0_S4_S3_
	.type	_ZSt13__copy_move_aILb0EPKiPiET1_T0_S4_S3_, @function
_ZSt13__copy_move_aILb0EPKiPiET1_T0_S4_S3_:
.LFB2333:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	pushq	%r12	#
	pushq	%rbx	#
	subq	$32, %rsp	#,
	.cfi_offset 12, -24
	.cfi_offset 3, -32
	movq	%rdi, -24(%rbp)	# __first, __first
	movq	%rsi, -32(%rbp)	# __last, __last
	movq	%rdx, -40(%rbp)	# __result, __result
# /usr/include/c++/13/bits/stl_algobase.h:540:       return std::__niter_wrap(__result,
	movq	-40(%rbp), %rax	# __result, __result.11_1
	movq	%rax, %rdi	# __result.11_1,
	call	_ZSt12__niter_baseIPiET_S1_	#
	movq	%rax, %r12	#, _2
# /usr/include/c++/13/bits/stl_algobase.h:540:       return std::__niter_wrap(__result,
	movq	-32(%rbp), %rax	# __last, tmp89
	movq	%rax, %rdi	# tmp89,
	call	_ZSt12__niter_baseIPKiET_S2_	#
	movq	%rax, %rbx	#, _3
# /usr/include/c++/13/bits/stl_algobase.h:540:       return std::__niter_wrap(__result,
	movq	-24(%rbp), %rax	# __first, tmp90
	movq	%rax, %rdi	# tmp90,
	call	_ZSt12__niter_baseIPKiET_S2_	#
# /usr/include/c++/13/bits/stl_algobase.h:540:       return std::__niter_wrap(__result,
	movq	%r12, %rdx	# _2,
	movq	%rbx, %rsi	# _3,
	movq	%rax, %rdi	# _4,
	call	_ZSt14__copy_move_a1ILb0EPKiPiET1_T0_S4_S3_	#
	movq	%rax, %rdx	#, _5
# /usr/include/c++/13/bits/stl_algobase.h:540:       return std::__niter_wrap(__result,
	leaq	-40(%rbp), %rax	#, tmp91
	movq	%rdx, %rsi	# _5,
	movq	%rax, %rdi	# tmp91,
	call	_ZSt12__niter_wrapIPiET_RKS1_S1_	#
# /usr/include/c++/13/bits/stl_algobase.h:544:     }
	addq	$32, %rsp	#,
	popq	%rbx	#
	popq	%r12	#
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2333:
	.size	_ZSt13__copy_move_aILb0EPKiPiET1_T0_S4_S3_, .-_ZSt13__copy_move_aILb0EPKiPiET1_T0_S4_S3_
	.section	.text._ZSt10__fill_n_aIPimiET_S1_T0_RKT1_St26random_access_iterator_tag,"axG",@progbits,_ZSt10__fill_n_aIPimiET_S1_T0_RKT1_St26random_access_iterator_tag,comdat
	.weak	_ZSt10__fill_n_aIPimiET_S1_T0_RKT1_St26random_access_iterator_tag
	.type	_ZSt10__fill_n_aIPimiET_S1_T0_RKT1_St26random_access_iterator_tag, @function
_ZSt10__fill_n_aIPimiET_S1_T0_RKT1_St26random_access_iterator_tag:
.LFB2337:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -8(%rbp)	# __first, __first
	movq	%rsi, -16(%rbp)	# __n, __n
	movq	%rdx, -24(%rbp)	# __value, __value
# /usr/include/c++/13/bits/stl_algobase.h:1123:       if (__n <= 0)
	cmpq	$0, -16(%rbp)	#, __n
	jne	.L144	#,
# /usr/include/c++/13/bits/stl_algobase.h:1124: 	return __first;
	movq	-8(%rbp), %rax	# __first, _4
	jmp	.L145	#
.L144:
# /usr/include/c++/13/bits/stl_algobase.h:1128:       std::__fill_a(__first, __first + __n, __value);
	movq	-16(%rbp), %rax	# __n, tmp87
	leaq	0(,%rax,4), %rdx	#, _1
# /usr/include/c++/13/bits/stl_algobase.h:1128:       std::__fill_a(__first, __first + __n, __value);
	movq	-8(%rbp), %rax	# __first, tmp88
	leaq	(%rdx,%rax), %rcx	#, _2
	movq	-24(%rbp), %rdx	# __value, tmp89
	movq	-8(%rbp), %rax	# __first, tmp90
	movq	%rcx, %rsi	# _2,
	movq	%rax, %rdi	# tmp90,
	call	_ZSt8__fill_aIPiiEvT_S1_RKT0_	#
# /usr/include/c++/13/bits/stl_algobase.h:1129:       return __first + __n;
	movq	-16(%rbp), %rax	# __n, tmp91
	leaq	0(,%rax,4), %rdx	#, _3
# /usr/include/c++/13/bits/stl_algobase.h:1129:       return __first + __n;
	movq	-8(%rbp), %rax	# __first, tmp92
	addq	%rdx, %rax	# _3, _4
.L145:
# /usr/include/c++/13/bits/stl_algobase.h:1130:     }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2337:
	.size	_ZSt10__fill_n_aIPimiET_S1_T0_RKT1_St26random_access_iterator_tag, .-_ZSt10__fill_n_aIPimiET_S1_T0_RKT1_St26random_access_iterator_tag
	.section	.text._ZSt12__niter_baseIPKiET_S2_,"axG",@progbits,_ZSt12__niter_baseIPKiET_S2_,comdat
	.weak	_ZSt12__niter_baseIPKiET_S2_
	.type	_ZSt12__niter_baseIPKiET_S2_, @function
_ZSt12__niter_baseIPKiET_S2_:
.LFB2338:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# __it, __it
# /usr/include/c++/13/bits/stl_algobase.h:318:     { return __it; }
	movq	-8(%rbp), %rax	# __it, _2
# /usr/include/c++/13/bits/stl_algobase.h:318:     { return __it; }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2338:
	.size	_ZSt12__niter_baseIPKiET_S2_, .-_ZSt12__niter_baseIPKiET_S2_
	.section	.text._ZSt12__niter_baseIPiET_S1_,"axG",@progbits,_ZSt12__niter_baseIPiET_S1_,comdat
	.weak	_ZSt12__niter_baseIPiET_S1_
	.type	_ZSt12__niter_baseIPiET_S1_, @function
_ZSt12__niter_baseIPiET_S1_:
.LFB2339:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# __it, __it
# /usr/include/c++/13/bits/stl_algobase.h:318:     { return __it; }
	movq	-8(%rbp), %rax	# __it, _2
# /usr/include/c++/13/bits/stl_algobase.h:318:     { return __it; }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2339:
	.size	_ZSt12__niter_baseIPiET_S1_, .-_ZSt12__niter_baseIPiET_S1_
	.section	.text._ZSt14__copy_move_a1ILb0EPKiPiET1_T0_S4_S3_,"axG",@progbits,_ZSt14__copy_move_a1ILb0EPKiPiET1_T0_S4_S3_,comdat
	.weak	_ZSt14__copy_move_a1ILb0EPKiPiET1_T0_S4_S3_
	.type	_ZSt14__copy_move_a1ILb0EPKiPiET1_T0_S4_S3_, @function
_ZSt14__copy_move_a1ILb0EPKiPiET1_T0_S4_S3_:
.LFB2340:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -8(%rbp)	# __first, __first
	movq	%rsi, -16(%rbp)	# __last, __last
	movq	%rdx, -24(%rbp)	# __result, __result
# /usr/include/c++/13/bits/stl_algobase.h:533:     { return std::__copy_move_a2<_IsMove>(__first, __last, __result); }
	movq	-24(%rbp), %rdx	# __result, tmp84
	movq	-16(%rbp), %rcx	# __last, tmp85
	movq	-8(%rbp), %rax	# __first, tmp86
	movq	%rcx, %rsi	# tmp85,
	movq	%rax, %rdi	# tmp86,
	call	_ZSt14__copy_move_a2ILb0EPKiPiET1_T0_S4_S3_	#
# /usr/include/c++/13/bits/stl_algobase.h:533:     { return std::__copy_move_a2<_IsMove>(__first, __last, __result); }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2340:
	.size	_ZSt14__copy_move_a1ILb0EPKiPiET1_T0_S4_S3_, .-_ZSt14__copy_move_a1ILb0EPKiPiET1_T0_S4_S3_
	.section	.text._ZSt12__niter_wrapIPiET_RKS1_S1_,"axG",@progbits,_ZSt12__niter_wrapIPiET_RKS1_S1_,comdat
	.weak	_ZSt12__niter_wrapIPiET_RKS1_S1_
	.type	_ZSt12__niter_wrapIPiET_RKS1_S1_, @function
_ZSt12__niter_wrapIPiET_RKS1_S1_:
.LFB2341:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# D.45168, D.45168
	movq	%rsi, -16(%rbp)	# __res, __res
# /usr/include/c++/13/bits/stl_algobase.h:339:     { return __res; }
	movq	-16(%rbp), %rax	# __res, _2
# /usr/include/c++/13/bits/stl_algobase.h:339:     { return __res; }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2341:
	.size	_ZSt12__niter_wrapIPiET_RKS1_S1_, .-_ZSt12__niter_wrapIPiET_RKS1_S1_
	.section	.text._ZSt8__fill_aIPiiEvT_S1_RKT0_,"axG",@progbits,_ZSt8__fill_aIPiiEvT_S1_RKT0_,comdat
	.weak	_ZSt8__fill_aIPiiEvT_S1_RKT0_
	.type	_ZSt8__fill_aIPiiEvT_S1_RKT0_, @function
_ZSt8__fill_aIPiiEvT_S1_RKT0_:
.LFB2342:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -8(%rbp)	# __first, __first
	movq	%rsi, -16(%rbp)	# __last, __last
	movq	%rdx, -24(%rbp)	# __value, __value
# /usr/include/c++/13/bits/stl_algobase.h:977:     { std::__fill_a1(__first, __last, __value); }
	movq	-24(%rbp), %rdx	# __value, tmp82
	movq	-16(%rbp), %rcx	# __last, tmp83
	movq	-8(%rbp), %rax	# __first, tmp84
	movq	%rcx, %rsi	# tmp83,
	movq	%rax, %rdi	# tmp84,
	call	_ZSt9__fill_a1IPiiEN9__gnu_cxx11__enable_ifIXsrSt11__is_scalarIT0_E7__valueEvE6__typeET_S8_RKS4_	#
# /usr/include/c++/13/bits/stl_algobase.h:977:     { std::__fill_a1(__first, __last, __value); }
	nop	
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2342:
	.size	_ZSt8__fill_aIPiiEvT_S1_RKT0_, .-_ZSt8__fill_aIPiiEvT_S1_RKT0_
	.section	.text._ZSt14__copy_move_a2ILb0EPKiPiET1_T0_S4_S3_,"axG",@progbits,_ZSt14__copy_move_a2ILb0EPKiPiET1_T0_S4_S3_,comdat
	.weak	_ZSt14__copy_move_a2ILb0EPKiPiET1_T0_S4_S3_
	.type	_ZSt14__copy_move_a2ILb0EPKiPiET1_T0_S4_S3_, @function
_ZSt14__copy_move_a2ILb0EPKiPiET1_T0_S4_S3_:
.LFB2343:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
	movq	%rdi, -8(%rbp)	# __first, __first
	movq	%rsi, -16(%rbp)	# __last, __last
	movq	%rdx, -24(%rbp)	# __result, __result
# /usr/include/c++/13/bits/stl_algobase.h:506: 			      _Category>::__copy_m(__first, __last, __result);
	movq	-24(%rbp), %rdx	# __result, tmp84
	movq	-16(%rbp), %rcx	# __last, tmp85
	movq	-8(%rbp), %rax	# __first, tmp86
	movq	%rcx, %rsi	# tmp85,
	movq	%rax, %rdi	# tmp86,
	call	_ZNSt11__copy_moveILb0ELb1ESt26random_access_iterator_tagE8__copy_mIKiiEEPT0_PT_S7_S5_	#
# /usr/include/c++/13/bits/stl_algobase.h:507:     }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2343:
	.size	_ZSt14__copy_move_a2ILb0EPKiPiET1_T0_S4_S3_, .-_ZSt14__copy_move_a2ILb0EPKiPiET1_T0_S4_S3_
	.section	.text._ZSt9__fill_a1IPiiEN9__gnu_cxx11__enable_ifIXsrSt11__is_scalarIT0_E7__valueEvE6__typeET_S8_RKS4_,"axG",@progbits,_ZSt9__fill_a1IPiiEN9__gnu_cxx11__enable_ifIXsrSt11__is_scalarIT0_E7__valueEvE6__typeET_S8_RKS4_,comdat
	.weak	_ZSt9__fill_a1IPiiEN9__gnu_cxx11__enable_ifIXsrSt11__is_scalarIT0_E7__valueEvE6__typeET_S8_RKS4_
	.type	_ZSt9__fill_a1IPiiEN9__gnu_cxx11__enable_ifIXsrSt11__is_scalarIT0_E7__valueEvE6__typeET_S8_RKS4_, @function
_ZSt9__fill_a1IPiiEN9__gnu_cxx11__enable_ifIXsrSt11__is_scalarIT0_E7__valueEvE6__typeET_S8_RKS4_:
.LFB2344:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)	# __first, __first
	movq	%rsi, -32(%rbp)	# __last, __last
	movq	%rdx, -40(%rbp)	# __value, __value
# /usr/include/c++/13/bits/stl_algobase.h:929:       const _Tp __tmp = __value;
	movq	-40(%rbp), %rax	# __value, tmp82
	movl	(%rax), %eax	# *__value_4(D), tmp83
	movl	%eax, -4(%rbp)	# tmp83, __tmp
# /usr/include/c++/13/bits/stl_algobase.h:930:       for (; __first != __last; ++__first)
	jmp	.L158	#
.L159:
# /usr/include/c++/13/bits/stl_algobase.h:931: 	*__first = __tmp;
	movq	-24(%rbp), %rax	# __first, tmp84
	movl	-4(%rbp), %edx	# __tmp, tmp85
	movl	%edx, (%rax)	# tmp85, *__first_1
# /usr/include/c++/13/bits/stl_algobase.h:930:       for (; __first != __last; ++__first)
	addq	$4, -24(%rbp)	#, __first
.L158:
# /usr/include/c++/13/bits/stl_algobase.h:930:       for (; __first != __last; ++__first)
	movq	-24(%rbp), %rax	# __first, tmp86
	cmpq	-32(%rbp), %rax	# __last, tmp86
	jne	.L159	#,
# /usr/include/c++/13/bits/stl_algobase.h:932:     }
	nop	
	nop	
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2344:
	.size	_ZSt9__fill_a1IPiiEN9__gnu_cxx11__enable_ifIXsrSt11__is_scalarIT0_E7__valueEvE6__typeET_S8_RKS4_, .-_ZSt9__fill_a1IPiiEN9__gnu_cxx11__enable_ifIXsrSt11__is_scalarIT0_E7__valueEvE6__typeET_S8_RKS4_
	.section	.text._ZNSt11__copy_moveILb0ELb1ESt26random_access_iterator_tagE8__copy_mIKiiEEPT0_PT_S7_S5_,"axG",@progbits,_ZNSt11__copy_moveILb0ELb1ESt26random_access_iterator_tagE8__copy_mIKiiEEPT0_PT_S7_S5_,comdat
	.weak	_ZNSt11__copy_moveILb0ELb1ESt26random_access_iterator_tagE8__copy_mIKiiEEPT0_PT_S7_S5_
	.type	_ZNSt11__copy_moveILb0ELb1ESt26random_access_iterator_tagE8__copy_mIKiiEEPT0_PT_S7_S5_, @function
_ZNSt11__copy_moveILb0ELb1ESt26random_access_iterator_tagE8__copy_mIKiiEEPT0_PT_S7_S5_:
.LFB2345:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$48, %rsp	#,
	movq	%rdi, -24(%rbp)	# __first, __first
	movq	%rsi, -32(%rbp)	# __last, __last
	movq	%rdx, -40(%rbp)	# __result, __result
# /usr/include/c++/13/bits/stl_algobase.h:435: 	  const ptrdiff_t _Num = __last - __first;
	movq	-32(%rbp), %rax	# __last, tmp92
	subq	-24(%rbp), %rax	# __first, _1
# /usr/include/c++/13/bits/stl_algobase.h:435: 	  const ptrdiff_t _Num = __last - __first;
	sarq	$2, %rax	#, tmp93
	movq	%rax, -8(%rbp)	# tmp93, _Num
# /usr/include/c++/13/bits/stl_algobase.h:436: 	  if (__builtin_expect(_Num > 1, true))
	cmpq	$1, -8(%rbp)	#, _Num
	setg	%al	#, _2
# /usr/include/c++/13/bits/stl_algobase.h:436: 	  if (__builtin_expect(_Num > 1, true))
	movzbl	%al, %eax	# _2, _3
# /usr/include/c++/13/bits/stl_algobase.h:436: 	  if (__builtin_expect(_Num > 1, true))
	testq	%rax, %rax	# _4
	je	.L161	#,
# /usr/include/c++/13/bits/stl_algobase.h:437: 	    __builtin_memmove(__result, __first, sizeof(_Tp) * _Num);
	movq	-8(%rbp), %rax	# _Num, _Num.12_5
# /usr/include/c++/13/bits/stl_algobase.h:437: 	    __builtin_memmove(__result, __first, sizeof(_Tp) * _Num);
	leaq	0(,%rax,4), %rdx	#, _6
	movq	-24(%rbp), %rcx	# __first, tmp96
	movq	-40(%rbp), %rax	# __result, tmp97
	movq	%rcx, %rsi	# tmp96,
	movq	%rax, %rdi	# tmp97,
	call	memmove@PLT	#
	jmp	.L162	#
.L161:
# /usr/include/c++/13/bits/stl_algobase.h:438: 	  else if (_Num == 1)
	cmpq	$1, -8(%rbp)	#, _Num
	jne	.L162	#,
# /usr/include/c++/13/bits/stl_algobase.h:440: 	      __assign_one(__result, __first);
	movq	-24(%rbp), %rdx	# __first, tmp98
	movq	-40(%rbp), %rax	# __result, tmp99
	movq	%rdx, %rsi	# tmp98,
	movq	%rax, %rdi	# tmp99,
	call	_ZNSt11__copy_moveILb0ELb0ESt26random_access_iterator_tagE12__assign_oneIiKiEEvPT_PT0_	#
.L162:
# /usr/include/c++/13/bits/stl_algobase.h:441: 	  return __result + _Num;
	movq	-8(%rbp), %rax	# _Num, _Num.13_7
# /usr/include/c++/13/bits/stl_algobase.h:441: 	  return __result + _Num;
	leaq	0(,%rax,4), %rdx	#, _8
# /usr/include/c++/13/bits/stl_algobase.h:441: 	  return __result + _Num;
	movq	-40(%rbp), %rax	# __result, tmp100
	addq	%rdx, %rax	# _8, _17
# /usr/include/c++/13/bits/stl_algobase.h:442: 	}
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2345:
	.size	_ZNSt11__copy_moveILb0ELb1ESt26random_access_iterator_tagE8__copy_mIKiiEEPT0_PT_S7_S5_, .-_ZNSt11__copy_moveILb0ELb1ESt26random_access_iterator_tagE8__copy_mIKiiEEPT0_PT_S7_S5_
	.section	.text._ZNSt11__copy_moveILb0ELb0ESt26random_access_iterator_tagE12__assign_oneIiKiEEvPT_PT0_,"axG",@progbits,_ZNSt11__copy_moveILb0ELb0ESt26random_access_iterator_tagE12__assign_oneIiKiEEvPT_PT0_,comdat
	.weak	_ZNSt11__copy_moveILb0ELb0ESt26random_access_iterator_tagE12__assign_oneIiKiEEvPT_PT0_
	.type	_ZNSt11__copy_moveILb0ELb0ESt26random_access_iterator_tagE12__assign_oneIiKiEEvPT_PT0_, @function
_ZNSt11__copy_moveILb0ELb0ESt26random_access_iterator_tagE12__assign_oneIiKiEEvPT_PT0_:
.LFB2346:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)	# __to, __to
	movq	%rsi, -16(%rbp)	# __from, __from
# /usr/include/c++/13/bits/stl_algobase.h:398: 	{ *__to = *__from; }
	movq	-16(%rbp), %rax	# __from, tmp83
	movl	(%rax), %edx	# *__from_3(D), _1
	movq	-8(%rbp), %rax	# __to, tmp84
	movl	%edx, (%rax)	# _1, *__to_4(D)
# /usr/include/c++/13/bits/stl_algobase.h:398: 	{ *__to = *__from; }
	nop	
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2346:
	.size	_ZNSt11__copy_moveILb0ELb0ESt26random_access_iterator_tagE12__assign_oneIiKiEEvPT_PT0_, .-_ZNSt11__copy_moveILb0ELb0ESt26random_access_iterator_tagE12__assign_oneIiKiEEvPT_PT0_
	.text
	.type	_Z41__static_initialization_and_destruction_0v, @function
_Z41__static_initialization_and_destruction_0v:
.LFB2347:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA2347
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	pushq	%rbx	#
	subq	$56, %rsp	#,
	.cfi_offset 3, -24
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	movq	%fs:40, %rcx	# MEM[(<address-space-1> long unsigned int *)40B], tmp98
	movq	%rcx, -24(%rbp)	# tmp98, D.45722
	xorl	%ecx, %ecx	# tmp98
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:6: auto v1 = vector<int>{1, 2, 3};
	movl	$1, -36(%rbp)	#, D.40385[0]
	movl	$2, -32(%rbp)	#, D.40385[1]
	movl	$3, -28(%rbp)	#, D.40385[2]
	leaq	-36(%rbp), %rcx	#, tmp84
	movq	%rcx, %rax	# tmp84, D.40386
	movl	$3, %edx	#, D.40386
	leaq	-49(%rbp), %rcx	#, tmp85
	movq	%rcx, -48(%rbp)	# tmp85, this
# /usr/include/c++/13/bits/new_allocator.h:88:       __new_allocator() _GLIBCXX_USE_NOEXCEPT { }
	nop	
# /usr/include/c++/13/bits/allocator.h:163:       allocator() _GLIBCXX_NOTHROW { }
	nop	
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:6: auto v1 = vector<int>{1, 2, 3};
	leaq	-49(%rbp), %rcx	#, tmp86
	movq	%rax, %r8	# D.40386, tmp87
	movq	%rdx, %r9	# D.40386,
	movq	%rax, %rsi	# D.40386, tmp88
	movq	%rdx, %rdi	# D.40386,
	movq	%r8, %rdx	# tmp87, tmp89
	movq	%rdi, %rax	#, tmp90
	movq	%rdx, %rsi	# tmp89,
	movq	%rax, %rdx	# tmp90,
	leaq	v1(%rip), %rax	#, tmp91
	movq	%rax, %rdi	# tmp91,
.LEHB14:
	call	_ZNSt6vectorIiSaIiEEC1ESt16initializer_listIiERKS0_	#
.LEHE14:
# /usr/include/c++/13/bits/allocator.h:184:       ~allocator() _GLIBCXX_NOTHROW { }
	leaq	-49(%rbp), %rax	#, tmp92
	movq	%rax, %rdi	# tmp92,
	call	_ZNSt15__new_allocatorIiED2Ev	#
	nop	
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:6: auto v1 = vector<int>{1, 2, 3};
	leaq	__dso_handle(%rip), %rax	#, tmp93
	movq	%rax, %rdx	# tmp93,
	leaq	v1(%rip), %rax	#, tmp94
	movq	%rax, %rsi	# tmp94,
	leaq	_ZNSt6vectorIiSaIiEED1Ev(%rip), %rax	#, tmp95
	movq	%rax, %rdi	# tmp95,
	call	__cxa_atexit@PLT	#
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	nop	
	movq	-24(%rbp), %rax	# D.45722, tmp99
	subq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp99
	je	.L168	#,
	jmp	.L170	#
.L169:
	endbr64	
# /usr/include/c++/13/bits/allocator.h:184:       ~allocator() _GLIBCXX_NOTHROW { }
	movq	%rax, %rbx	#, tmp97
	leaq	-49(%rbp), %rax	#, tmp96
	movq	%rax, %rdi	# tmp96,
	call	_ZNSt15__new_allocatorIiED2Ev	#
	nop	
	movq	%rbx, %rax	# tmp97, D.45721
	movq	-24(%rbp), %rdx	# D.45722, tmp100
	subq	%fs:40, %rdx	# MEM[(<address-space-1> long unsigned int *)40B], tmp100
	je	.L167	#,
	call	__stack_chk_fail@PLT	#
.L167:
	movq	%rax, %rdi	# D.45721,
.LEHB15:
	call	_Unwind_Resume@PLT	#
.LEHE15:
.L170:
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	call	__stack_chk_fail@PLT	#
.L168:
	movq	-8(%rbp), %rbx	#,
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2347:
	.section	.gcc_except_table
.LLSDA2347:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE2347-.LLSDACSB2347
.LLSDACSB2347:
	.uleb128 .LEHB14-.LFB2347
	.uleb128 .LEHE14-.LEHB14
	.uleb128 .L169-.LFB2347
	.uleb128 0
	.uleb128 .LEHB15-.LFB2347
	.uleb128 .LEHE15-.LEHB15
	.uleb128 0
	.uleb128 0
.LLSDACSE2347:
	.text
	.size	_Z41__static_initialization_and_destruction_0v, .-_Z41__static_initialization_and_destruction_0v
	.section	.rodata
	.type	_ZNSt8__detail30__integer_to_chars_is_unsignedIjEE, @object
	.size	_ZNSt8__detail30__integer_to_chars_is_unsignedIjEE, 1
_ZNSt8__detail30__integer_to_chars_is_unsignedIjEE:
	.byte	1
	.type	_ZNSt8__detail30__integer_to_chars_is_unsignedImEE, @object
	.size	_ZNSt8__detail30__integer_to_chars_is_unsignedImEE, 1
_ZNSt8__detail30__integer_to_chars_is_unsignedImEE:
	.byte	1
	.type	_ZNSt8__detail30__integer_to_chars_is_unsignedIyEE, @object
	.size	_ZNSt8__detail30__integer_to_chars_is_unsignedIyEE, 1
_ZNSt8__detail30__integer_to_chars_is_unsignedIyEE:
	.byte	1
	.text
	.type	_GLOBAL__sub_I_x1, @function
_GLOBAL__sub_I_x1:
.LFB2348:
	.cfi_startproc
	endbr64	
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
# /home/r/Desktop/lecpp/src/83_global_vector_test.cpp:15: int main() { auto v3 = doSomething("hi"); }
	call	_Z41__static_initialization_and_destruction_0v	#
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE2348:
	.size	_GLOBAL__sub_I_x1, .-_GLOBAL__sub_I_x1
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I_x1
	.hidden	DW.ref.__gxx_personality_v0
	.weak	DW.ref.__gxx_personality_v0
	.section	.data.rel.local.DW.ref.__gxx_personality_v0,"awG",@progbits,DW.ref.__gxx_personality_v0,comdat
	.align 8
	.type	DW.ref.__gxx_personality_v0, @object
	.size	DW.ref.__gxx_personality_v0, 8
DW.ref.__gxx_personality_v0:
	.quad	__gxx_personality_v0
	.hidden	__dso_handle
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
