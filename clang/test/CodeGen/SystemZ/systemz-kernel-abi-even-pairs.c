// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float \
// RUN:   -target-feature +experimental-kernel-abi-struct-arg \
// RUN:   -target-feature +experimental-kernel-abi-int128 \
// RUN:   -target-feature +experimental-kernel-abi-even-pairs \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,R6
// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float \
// RUN:   -target-feature +experimental-kernel-abi-struct-arg \
// RUN:   -target-feature +experimental-kernel-abi-int128 \
// RUN:   -target-feature +experimental-kernel-abi-even-pairs \
// RUN:   -target-feature +experimental-kernel-abi-r6-clobbered \
// RUN:   -target-feature +experimental-kernel-abi-r7-arg \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,R7

#include <stdarg.h>

// The GPR counter is rounded up to an even number before the check, and
// only stored if the pair gets GPRs.
struct pair { long a, b; };
struct pair va_pair(va_list ap) { return va_arg(ap, struct pair); }
// CHECK-LABEL: define{{.*}} void @va_pair(
// CHECK: %reg_count = load i64, ptr %reg_count_ptr
// CHECK: [[INC:%.*]] = add i64 %reg_count, 1
// CHECK: %even_reg_count = and i64 [[INC]], -2
// R6: %fits_in_regs = icmp ule i64 %even_reg_count, 3
// R7: %fits_in_regs = icmp ule i64 %even_reg_count, 4
// CHECK: vaarg.in_reg:
// CHECK: %scaled_reg_count = mul i64 %even_reg_count, 8
// CHECK: [[NEW:%.*]] = add i64 %even_reg_count, 2
// CHECK: store i64 [[NEW]], ptr %reg_count_ptr
// CHECK: vaarg.in_mem:
// CHECK-NOT: store i64 {{.*}}, ptr %reg_count_ptr
// CHECK: vaarg.end:

__int128 va_int128(va_list ap) { return va_arg(ap, __int128); }
// CHECK-LABEL: define{{.*}} i128 @va_int128(
// CHECK: %even_reg_count = and i64

// A single GPR is not affected.
long va_long(va_list ap) { return va_arg(ap, long); }
// CHECK-LABEL: define{{.*}} i64 @va_long(
// CHECK-NOT: even_reg_count
// CHECK: ret i64

// Nor is a composite of more than two GPRs.
struct agg5 { char c[5]; };
struct agg5 va_agg5(va_list ap) { return va_arg(ap, struct agg5); }
// CHECK-LABEL: define{{.*}} void @va_agg5(
// CHECK-NOT: even_reg_count
// CHECK: ret void
