// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float \
// RUN:   -target-feature +experimental-kernel-abi-r6-clobbered \
// RUN:   -target-feature +experimental-kernel-abi-r7-arg \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK
// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float \
// RUN:   -target-feature +experimental-kernel-abi-r6-clobbered \
// RUN:   -target-feature +experimental-kernel-abi-r7-arg \
// RUN:   -target-feature +experimental-kernel-abi-struct-arg \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,PAIR

#include <stdarg.h>

// Six GPR varargs.
long va_long(va_list ap) { return va_arg(ap, long); }
// CHECK-LABEL: define{{.*}} i64 @va_long(
// CHECK: %fits_in_regs = icmp ult i64 %reg_count, 6

struct pair { long a, b; };
struct pair va_pair(va_list ap) { return va_arg(ap, struct pair); }
// PAIR-LABEL: define{{.*}} void @va_pair(
// PAIR: %fits_in_regs = icmp ule i64 %reg_count, 4
