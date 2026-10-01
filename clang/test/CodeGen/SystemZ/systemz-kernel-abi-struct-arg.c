// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float -emit-llvm -o - %s \
// RUN:   | FileCheck %s --check-prefixes=CHECK,ELF
// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float \
// RUN:   -target-feature +experimental-kernel-abi-struct-arg \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,KABI

#include <stdarg.h>

#define AGG(n) struct agg##n { char c[n]; };                                 \
  void arg_agg##n(struct agg##n a) { }

struct agg0 { };
void arg_agg0(int x, struct agg0 a, int y) { }
// ELF-LABEL: define{{.*}} void @arg_agg0(i32 noundef signext %x, ptr
// KABI-LABEL: define{{.*}} void @arg_agg0(i32 noundef signext %x, i32 noundef signext %y)

AGG(1)
// CHECK-LABEL: define{{.*}} void @arg_agg1(i8 noext %a.coerce)
AGG(3)
// ELF-LABEL: define{{.*}} void @arg_agg3(ptr
// KABI-LABEL: define{{.*}} void @arg_agg3(i24 noext %a.coerce)
AGG(4)
// CHECK-LABEL: define{{.*}} void @arg_agg4(i32 noext %a.coerce)
AGG(5)
// ELF-LABEL: define{{.*}} void @arg_agg5(ptr
// KABI-LABEL: define{{.*}} void @arg_agg5(i40 %a.coerce)
AGG(8)
// CHECK-LABEL: define{{.*}} void @arg_agg8(i64 %a.coerce)
AGG(9)
// ELF-LABEL: define{{.*}} void @arg_agg9(ptr
// KABI-LABEL: define{{.*}} void @arg_agg9({ i64, i8 } %a.coerce)
AGG(12)
// KABI-LABEL: define{{.*}} void @arg_agg12({ i64, i32 } %a.coerce)
AGG(16)
// KABI-LABEL: define{{.*}} void @arg_agg16({ i64, i64 } %a.coerce)
AGG(17)
// CHECK-LABEL: define{{.*}} void @arg_agg17(ptr

struct fam { long n; int d[]; };
void arg_fam(struct fam a) { }
// ELF-LABEL: define{{.*}} void @arg_fam(ptr
// KABI-LABEL: define{{.*}} void @arg_fam(i64 %a.coerce)

union u12 { int i[3]; char c; };
void arg_u12(union u12 a) { }
// KABI-LABEL: define{{.*}} void @arg_u12({ i64, i32 } %a.coerce)

typedef union { struct agg3 s; char c[3]; } tu __attribute__((transparent_union));
void arg_tu(tu a) { }
// ELF-LABEL: define{{.*}} void @arg_tu(ptr
// KABI-LABEL: define{{.*}} void @arg_tu(i24 noext %a.coerce)

void arg_cfloat(_Complex float a) { }
// ELF-LABEL: define{{.*}} void @arg_cfloat(ptr
// KABI-LABEL: define{{.*}} void @arg_cfloat(i64 noundef %a.coerce)
void arg_cdouble(_Complex double a) { }
// KABI-LABEL: define{{.*}} void @arg_cdouble({ i64, i64 } noundef %a.coerce)
void arg_cldouble(_Complex long double a) { }
// CHECK-LABEL: define{{.*}} void @arg_cldouble(ptr

void arg_ldouble(long double a) { }
// CHECK-LABEL: define{{.*}} void @arg_ldouble(ptr
void arg_int128(__int128 a) { }
// CHECK-LABEL: define{{.*}} void @arg_int128(ptr

// Returns are not affected.
struct agg12 ret_agg12(void) { struct agg12 r = {}; return r; }
// CHECK-LABEL: define{{.*}} void @ret_agg12(ptr {{.*}}sret(%struct.agg12)

// Composites are not flattened.
void call_agg12(struct agg12 *p) { arg_agg12(*p); }
// KABI-LABEL: define{{.*}} void @call_agg12(
// KABI: [[TMP:%.*]] = alloca { i64, i32 }, align 8
// KABI: call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[TMP]], ptr align 1 %{{.*}}, i64 12, i1 false)
// KABI: [[V:%.*]] = load { i64, i32 }, ptr [[TMP]], align 8
// KABI: call void @arg_agg12({ i64, i32 } [[V]])

struct agg0 va_agg0(va_list ap) { return va_arg(ap, struct agg0); }
// KABI-LABEL: define{{.*}} void @va_agg0(
// KABI-NOT: reg_count
// KABI: ret void

// A ONE takes one slot, right-justified.
struct agg3 va_agg3(va_list ap) { return va_arg(ap, struct agg3); }
// KABI-LABEL: define{{.*}} void @va_agg3(
// KABI: [[RC:%.*]] = load i64, ptr %reg_count_ptr
// KABI: icmp ult i64 [[RC]], 5
// KABI: add i64 %scaled_reg_count, 21
// KABI: add i64 [[RC]], 1
// KABI: %raw_mem_addr = getelementptr i8, ptr %overflow_arg_area, i64 5

// A PAIR takes two slots of the register save area, or 16 bytes of the
// overflow area.
struct agg12 va_agg12(va_list ap) { return va_arg(ap, struct agg12); }
// KABI-LABEL: define{{.*}} void @va_agg12(
// KABI: [[TMP:%va_arg.tmp]] = alloca %struct.agg12, align 1
// KABI: [[RC:%.*]] = load i64, ptr %reg_count_ptr
// KABI: %fits_in_regs = icmp ule i64 [[RC]], 3
// KABI: br i1 %fits_in_regs, label %vaarg.in_reg, label %vaarg.in_mem
// KABI: vaarg.in_reg:
// KABI: %scaled_reg_count = mul i64 [[RC]], 8
// KABI: %reg_offset = add i64 %scaled_reg_count, 16
// KABI: %raw_reg_addr = getelementptr i8, ptr %reg_save_area, i64 %reg_offset
// KABI: call void @llvm.memcpy.p0.p0.i64(ptr align 1 [[TMP]], ptr align 8 %raw_reg_addr, i64 8, i1 false)
// KABI: [[DST:%.*]] = getelementptr inbounds i8, ptr [[TMP]], i64 8
// KABI: [[SRC:%.*]] = getelementptr inbounds i8, ptr %raw_reg_addr, i64 12
// KABI: call void @llvm.memcpy.p0.p0.i64(ptr align 1 [[DST]], ptr align 4 [[SRC]], i64 4, i1 false)
// KABI: [[NRC:%.*]] = add i64 [[RC]], 2
// KABI: store i64 [[NRC]], ptr %reg_count_ptr
// KABI: vaarg.in_mem:
// KABI: %overflow_arg_area = load ptr, ptr %overflow_arg_area_ptr
// KABI: [[NEXT:%.*]] = getelementptr i8, ptr %overflow_arg_area, i64 16
// KABI: store ptr [[NEXT]], ptr %overflow_arg_area_ptr
// KABI: vaarg.end:
// KABI: %va_arg.addr = phi ptr [ [[TMP]], %vaarg.in_reg ], [ %overflow_arg_area, %vaarg.in_mem ]
// KABI: call void @llvm.memcpy.p0.p0.i64(ptr align 1 %agg.result, ptr align 1 %va_arg.addr, i64 12, i1 false)

struct agg16 va_agg16(va_list ap) { return va_arg(ap, struct agg16); }
// KABI-LABEL: define{{.*}} void @va_agg16(
// KABI: %fits_in_regs = icmp ule i64 [[RC:%.*]], 3
// KABI: vaarg.in_reg:
// KABI-NOT: memcpy
// KABI: vaarg.end:
// KABI: %va_arg.addr = phi ptr [ %raw_reg_addr, %vaarg.in_reg ], [ %overflow_arg_area, %vaarg.in_mem ]

void call_vararg(void (*f)(int, ...), struct agg12 *p) { f(1, *p); }
// KABI-LABEL: define{{.*}} void @call_vararg(
// KABI: call void (i32, ...) %{{.*}}(i32 noundef signext 1, { i64, i32 } %{{.*}})
