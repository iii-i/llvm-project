// Test the alternative Linux kernel ABI, in which composite values of up to
// 16 bytes are returned in %r2 and %r3 instead of in memory.  Classes that the
// C++ ABI requires to be returned in memory are not affected.

// RUN: %clang_cc1 -no-enable-noundef-analysis -triple s390x-linux-gnu \
// RUN:   -freg-struct-return -emit-llvm -o - %s | FileCheck %s

struct pod { int a, b, c; };
pod ret_pod() { return pod(); }
// CHECK-LABEL: define{{.*}} { i64, i32 } @_Z7ret_podv()

struct base { int a; };
struct derived : base { int b; };
derived ret_derived() { return derived(); }
// CHECK-LABEL: define{{.*}} i64 @_Z11ret_derivedv()

struct empty { };
empty ret_empty() { return empty(); }
// CHECK-LABEL: define{{.*}} noext i8 @_Z9ret_emptyv()

// Classes that are not trivially copyable are returned in memory.
struct nontrivial { int a; nontrivial(const nontrivial &); };
nontrivial ret_nontrivial();
nontrivial call_nontrivial() { return ret_nontrivial(); }
// CHECK-LABEL: define{{.*}} void @_Z15call_nontrivialv(ptr dead_on_unwind noalias writable sret(%struct.nontrivial) align 4 %{{.*}})

struct polymorphic { virtual void f(); int a; };
polymorphic ret_polymorphic();
polymorphic call_polymorphic() { return ret_polymorphic(); }
// CHECK-LABEL: define{{.*}} void @_Z16call_polymorphicv(ptr dead_on_unwind noalias writable sret(%struct.polymorphic) align 8 %{{.*}})
