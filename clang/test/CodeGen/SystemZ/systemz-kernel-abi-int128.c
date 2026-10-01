// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float -emit-llvm -o - %s \
// RUN:   | FileCheck %s --check-prefixes=CHECK,ELF
// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float \
// RUN:   -target-feature +experimental-kernel-abi-int128 \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,KABI

#include <stdarg.h>

__int128 pass_int128(__int128 a) { return a; }
// ELF-LABEL: define{{.*}} void @pass_int128(ptr {{.*}}sret(i128) {{.*}}, ptr
// KABI-LABEL: define{{.*}} i128 @pass_int128(i128 noundef %a)

unsigned __int128 pass_uint128(int x, unsigned __int128 a) { return a; }
// KABI-LABEL: define{{.*}} i128 @pass_uint128(i32 noundef signext %x, i128 noundef %a)

unsigned __int128 call_uint128(unsigned __int128 a) { return pass_uint128(1, a); }
// KABI-LABEL: define{{.*}} i128 @call_uint128(
// KABI: [[R:%.*]] = call i128 @pass_uint128(i32 noundef signext 1, i128 noundef %{{.*}})
// KABI: ret i128 [[R]]

// Not __int128.
struct s128 { __int128 a; };
struct s128 pass_s128(struct s128 a) { return a; }
// CHECK-LABEL: define{{.*}} void @pass_s128(ptr {{.*}}sret(%struct.s128) {{.*}}, ptr
_BitInt(128) pass_bitint128(_BitInt(128) a) { return a; }
// CHECK-LABEL: define{{.*}} void @pass_bitint128(ptr {{.*}}sret(i128) {{.*}}, ptr
long double pass_ldouble(long double a) { return a; }
// CHECK-LABEL: define{{.*}} void @pass_ldouble(ptr {{.*}}sret(fp128) {{.*}}, ptr

__int128 va_int128(va_list ap) { return va_arg(ap, __int128); }
// KABI-LABEL: define{{.*}} i128 @va_int128(
// KABI: %fits_in_regs = icmp ule i64 [[RC:%.*]], 3
// KABI: vaarg.in_reg:
// KABI: %raw_reg_addr = getelementptr i8, ptr %reg_save_area, i64 %reg_offset
// KABI: add i64 [[RC]], 2
// KABI: vaarg.in_mem:
// KABI: getelementptr i8, ptr %overflow_arg_area, i64 16
// KABI: vaarg.end:
// KABI: %va_arg.addr = phi ptr [ %raw_reg_addr, %vaarg.in_reg ], [ %overflow_arg_area, %vaarg.in_mem ]
// KABI: load i128, ptr %va_arg.addr, align 8
