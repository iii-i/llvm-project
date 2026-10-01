// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float \
// RUN:   -target-feature +experimental-kernel-abi-struct-ret \
// RUN:   -emit-llvm -o - %s | FileCheck %s

struct empty { };
empty ret_empty() { return empty(); }
// CHECK-LABEL: define{{.*}} noext i8 @_Z9ret_emptyv()

struct plain { long a, b; };
plain ret_plain() { return plain(); }
// CHECK-LABEL: define{{.*}} { i64, i64 } @_Z9ret_plainv()

struct dtor { long a; ~dtor(); };
dtor ret_dtor() { return dtor(); }
// CHECK-LABEL: define{{.*}} void @_Z8ret_dtorv(ptr {{.*}}sret(%struct.dtor)

struct base { long a; };
struct derived : base { int b; };
derived ret_derived() { return derived(); }
// CHECK-LABEL: define{{.*}} { i64, i64 } @_Z11ret_derivedv()

struct A;
typedef void (A::*memfn)();
memfn ret_memfn(memfn *p) { return *p; }
// CHECK-LABEL: define{{.*}} void @_Z9ret_memfnPM1AFvvE(ptr {{.*}}sret({ i64, i64 })
