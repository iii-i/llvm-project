// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float \
// RUN:   -target-feature +experimental-kernel-abi-struct-ret \
// RUN:   -target-feature +experimental-kernel-abi-struct-arg \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,NO32
// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float \
// RUN:   -target-feature +experimental-kernel-abi-struct-ret \
// RUN:   -target-feature +experimental-kernel-abi-struct-arg \
// RUN:   -target-feature +experimental-kernel-abi-struct-32 \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,S32

#include <stdarg.h>

#define AGG(n) struct agg##n { char c[n]; };                                 \
  struct agg##n pass_agg##n(struct agg##n a) { return a; }

AGG(16)
// CHECK-LABEL: define{{.*}} { i64, i64 } @pass_agg16({ i64, i64 } %a.coerce)
AGG(17)
// NO32-LABEL: define{{.*}} void @pass_agg17(ptr {{.*}}sret(%struct.agg17) {{.*}}, ptr
// S32-LABEL: define{{.*}} { i64, i64, i8 } @pass_agg17({ i64, i64, i8 } %a.coerce)
AGG(20)
// S32-LABEL: define{{.*}} { i64, i64, i32 } @pass_agg20({ i64, i64, i32 } %a.coerce)
AGG(24)
// S32-LABEL: define{{.*}} { i64, i64, i64 } @pass_agg24({ i64, i64, i64 } %a.coerce)
AGG(25)
// S32-LABEL: define{{.*}} { i64, i64, i64, i8 } @pass_agg25({ i64, i64, i64, i8 } %a.coerce)
AGG(31)
// S32-LABEL: define{{.*}} { i64, i64, i64, i56 } @pass_agg31({ i64, i64, i64, i56 } %a.coerce)
AGG(32)
// S32-LABEL: define{{.*}} { i64, i64, i64, i64 } @pass_agg32({ i64, i64, i64, i64 } %a.coerce)
AGG(33)
// CHECK-LABEL: define{{.*}} void @pass_agg33(ptr {{.*}}sret(%struct.agg33) {{.*}}, ptr

struct bvec_iter { long sector; unsigned size, done, idx, bvec_done; };
struct bvec_iter pass_bvec_iter(struct bvec_iter a) { return a; }
// S32-LABEL: define{{.*}} { i64, i64, i64 } @pass_bvec_iter({ i64, i64, i64 } %a.coerce)

_Complex long double pass_cldouble(_Complex long double a) { return a; }
// NO32-LABEL: define{{.*}} void @pass_cldouble(ptr {{.*}}sret({ fp128, fp128 }) {{.*}}, ptr
// S32-LABEL: define{{.*}} { i64, i64, i64, i64 } @pass_cldouble({ i64, i64, i64, i64 } noundef %a.coerce)

// Three slots of the register save area, or 24 bytes of the overflow area.
struct agg20 va_agg20(va_list ap) { return va_arg(ap, struct agg20); }
// S32-LABEL: define{{.*}} { i64, i64, i32 } @va_agg20(
// S32: [[TMP:%va_arg.tmp]] = alloca %struct.agg20, align 1
// S32: %fits_in_regs = icmp ule i64 [[RC:%.*]], 2
// S32: vaarg.in_reg:
// S32: call void @llvm.memcpy.p0.p0.i64(ptr align 1 [[TMP]], ptr align 8 %raw_reg_addr, i64 16, i1 false)
// S32: [[DST:%.*]] = getelementptr inbounds i8, ptr [[TMP]], i64 16
// S32: [[SRC:%.*]] = getelementptr inbounds i8, ptr %raw_reg_addr, i64 20
// S32: call void @llvm.memcpy.p0.p0.i64(ptr align 1 [[DST]], ptr align 4 [[SRC]], i64 4, i1 false)
// S32: add i64 [[RC]], 3
// S32: vaarg.in_mem:
// S32: getelementptr i8, ptr %overflow_arg_area, i64 24
