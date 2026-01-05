// ═══════════════════════════════════════════════════════════════════════════
// FILE: 80_visit_multi_variant.cpp
// TOPIC: std::visit with MULTIPLE variants → 2D dispatch table
// ═══════════════════════════════════════════════════════════════════════════

#include <iostream>
#include <string>
#include <variant>

// ═══════════════════════════════════════════════════════════════════════════
// DERIVATION: std::visit with 2 variants
// ═══════════════════════════════════════════════════════════════════════════
//
// 1 variant:  visit(lambda, v1)        → 1D table, N entries
// 2 variants: visit(lambda, v1, v2)    → 2D table, N×M entries
// 3 variants: visit(lambda, v1, v2, v3)→ 3D table, N×M×K entries
//
// COMBINATORIAL EXPLOSION:
// variant<A,B> × variant<X,Y>       → 2×2 = 4 handlers
// variant<A,B,C> × variant<X,Y,Z>   → 3×3 = 9 handlers
// variant<A,B,C,D> × variant<W,X,Y,Z> → 4×4 = 16 handlers
//
// ═══════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// DISPATCH TABLE VISUALIZATION:
// ═══════════════════════════════════════════════════════════════════════════
//
// v1 = variant<int, double>    → index ∈ {0, 1}
// v2 = variant<string, double> → index ∈ {0, 1}
//
//                      v2.index()
//                  ┌────────┬────────┐
//                  │   0    │   1    │
//                  │ string │ double │
//        ┌─────────┼────────┼────────┤
//        │ 0 (int) │ 0x3100 │ 0x3200 │
// v1.idx ├─────────┼────────┼────────┤
//        │1(double)│ 0x3300 │ 0x3400 │
//        └─────────┴────────┴────────┘
//
// Flattened 1D: table[v1.index() * 2 + v2.index()]
//
// ═══════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// COMPILER GENERATES 4 operator() INSTANTIATIONS:
// ═══════════════════════════════════════════════════════════════════════════
//
// operator()(int&&, std::string&&)    → handler for (int, string)
// operator()(int&&, double&&)         → handler for (int, double)
// operator()(double&&, std::string&&) → handler for (double, string)
// operator()(double&&, double&&)      → handler for (double, double)
//
// Generic lambda (auto&& a, auto&& b) becomes 4 separate functions
//
// ═══════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "=== MULTI-VARIANT std::visit DEMO ===" << std::endl;

    // ═══════════════════════════════════════════════════════════════════════
    // EXAMPLE 1: int × string
    // ═══════════════════════════════════════════════════════════════════════

    std::variant<int, double> v1 = 42;
    std::variant<std::string, double> v2 = std::string("Hello");

    auto visitor = [](auto&& a, auto&& b) {
        std::cout << "Pair: (" << a << ", " << b << ")" << std::endl;
    };

    std::cout << "\nv1.index() = " << v1.index() << " (0=int, 1=double)" << std::endl;
    std::cout << "v2.index() = " << v2.index() << " (0=string, 1=double)" << std::endl;
    std::cout << "dispatch_table index = " << v1.index() << " * 2 + " << v2.index() << " = "
              << (v1.index() * 2 + v2.index()) << std::endl;

    std::cout << "Result: ";
    std::visit(visitor, v1, v2);  // Pair: (42, Hello)

    // ═══════════════════════════════════════════════════════════════════════
    // EXAMPLE 2: double × double
    // ═══════════════════════════════════════════════════════════════════════

    v1 = 3.14;   // now v1 holds double
    v2 = 2.718;  // now v2 holds double

    std::cout << "\nv1.index() = " << v1.index() << " (0=int, 1=double)" << std::endl;
    std::cout << "v2.index() = " << v2.index() << " (0=string, 1=double)" << std::endl;
    std::cout << "dispatch_table index = " << v1.index() << " * 2 + " << v2.index() << " = "
              << (v1.index() * 2 + v2.index()) << std::endl;

    std::cout << "Result: ";
    std::visit(visitor, v1, v2);  // Pair: (3.14, 2.718)

    // ═══════════════════════════════════════════════════════════════════════
    // EXAMPLE 3: int × double
    // ═══════════════════════════════════════════════════════════════════════

    v1 = 100;   // now v1 holds int
    v2 = 9.99;  // v2 still holds double

    std::cout << "\nv1.index() = " << v1.index() << " (0=int, 1=double)" << std::endl;
    std::cout << "v2.index() = " << v2.index() << " (0=string, 1=double)" << std::endl;
    std::cout << "dispatch_table index = " << v1.index() << " * 2 + " << v2.index() << " = "
              << (v1.index() * 2 + v2.index()) << std::endl;

    std::cout << "Result: ";
    std::visit(visitor, v1, v2);  // Pair: (100, 9.99)

    // ═══════════════════════════════════════════════════════════════════════
    // EXAMPLE 4: double × string
    // ═══════════════════════════════════════════════════════════════════════

    v1 = 2.5;                   // v1 holds double
    v2 = std::string("World");  // v2 holds string

    std::cout << "\nv1.index() = " << v1.index() << " (0=int, 1=double)" << std::endl;
    std::cout << "v2.index() = " << v2.index() << " (0=string, 1=double)" << std::endl;
    std::cout << "dispatch_table index = " << v1.index() << " * 2 + " << v2.index() << " = "
              << (v1.index() * 2 + v2.index()) << std::endl;

    std::cout << "Result: ";
    std::visit(visitor, v1, v2);  // Pair: (2.5, World)

    // ═══════════════════════════════════════════════════════════════════════
    // ALL 4 COMBINATIONS DEMONSTRATED
    // ═══════════════════════════════════════════════════════════════════════
    //
    // index=0: (int, string)    → handler @0x3100
    // index=1: (int, double)    → handler @0x3200
    // index=2: (double, string) → handler @0x3300
    // index=3: (double, double) → handler @0x3400
    //
    // SAME jmpq *%rdx instruction, DIFFERENT handler based on runtime indices
    //
    // ═══════════════════════════════════════════════════════════════════════

    // ═══════════════════════════════════════════════════════════════════════
    // MEMORY ANALYSIS
    // ═══════════════════════════════════════════════════════════════════════

    std::cout << "\n=== MEMORY ANALYSIS ===" << std::endl;
    std::cout << "sizeof(variant<int,double>) = " << sizeof(v1) << " bytes" << std::endl;
    std::cout << "sizeof(variant<string,double>) = " << sizeof(v2) << " bytes" << std::endl;
    std::cout << "sizeof(visitor lambda) = " << sizeof(visitor) << " bytes" << std::endl;

    // ═══════════════════════════════════════════════════════════════════════
    // ASSEMBLY TRACE (conceptual):
    // ═══════════════════════════════════════════════════════════════════════
    //
    // movzbl (%rsi), %eax          # read v1.index() → 0 or 1
    // movzbl (%rdx), %ecx          # read v2.index() → 0 or 1
    // imulq $2, %rax, %rax         # row offset = v1.index() * 2
    // addq %rcx, %rax              # flat index = row + col
    // leaq dispatch_table(%rip), %r8
    // movq (%r8,%rax,8), %r9       # load handler address
    // jmpq *%r9                    # indirect jump to handler
    //
    // ═══════════════════════════════════════════════════════════════════════

    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// FAILURE PREDICTIONS:
// ═══════════════════════════════════════════════════════════════════════════
//
// F1. Visitor missing overload for some combination → compile error
// F2. N variants × M types each = M^N handlers → code bloat
// F3. 3 variants × 4 types each = 64 handlers generated
// F4. 4 variants × 5 types each = 625 handlers generated
//
// ═══════════════════════════════════════════════════════════════════════════
// TRIP-UP POINTS:
// ═══════════════════════════════════════════════════════════════════════════
//
// T1. auto&& in visitor → works for all combinations (universal reference)
// T2. dispatch table is FLATTENED to 1D: index = i1*N2 + i2
// T3. Each handler is SEPARATE function → no runtime type checks inside
// T4. Same jmpq *%reg instruction → different target based on runtime index
//
// ═══════════════════════════════════════════════════════════════════════════
