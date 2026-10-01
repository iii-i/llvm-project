// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float \
// RUN:   -target-feature +experimental-kernel-abi-struct-arg \
// RUN:   -emit-llvm -o - %s | FileCheck %s

struct empty { };
void arg_empty(empty a) { }
// CHECK-LABEL: define{{.*}} void @_Z9arg_empty5empty(i8 noext %a.coerce)

struct plain { long a; int b; };
void arg_plain(plain a) { }
// CHECK-LABEL: define{{.*}} void @_Z9arg_plain5plain({ i64, i64 } %a.coerce)

struct dtor { long a; ~dtor(); };
void arg_dtor(dtor a) { }
// CHECK-LABEL: define{{.*}} void @_Z8arg_dtor4dtor(ptr
