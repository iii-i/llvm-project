// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float -emit-llvm -o - %s \
// RUN:   | FileCheck %s --check-prefixes=CHECK,ELF
// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float \
// RUN:   -target-feature +experimental-kernel-abi-struct-ret \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,KABI

#define AGG(n) struct agg##n { char c[n]; };                                 \
  struct agg##n ret_agg##n(void) { struct agg##n r = {}; return r; }

struct agg0 { };
struct agg0 ret_agg0(void) { struct agg0 r; return r; }
// ELF-LABEL: define{{.*}} void @ret_agg0(ptr {{.*}}sret(%struct.agg0)
// KABI-LABEL: define{{.*}} void @ret_agg0()
// KABI: ret void

AGG(1)
// ELF-LABEL: define{{.*}} void @ret_agg1(ptr {{.*}}sret(%struct.agg1)
// KABI-LABEL: define{{.*}} noext i8 @ret_agg1()
AGG(2)
// KABI-LABEL: define{{.*}} noext i16 @ret_agg2()
AGG(3)
// ELF-LABEL: define{{.*}} void @ret_agg3(ptr {{.*}}sret(%struct.agg3)
// KABI-LABEL: define{{.*}} noext i24 @ret_agg3()
AGG(4)
// KABI-LABEL: define{{.*}} noext i32 @ret_agg4()
AGG(5)
// KABI-LABEL: define{{.*}} i40 @ret_agg5()
AGG(7)
// KABI-LABEL: define{{.*}} i56 @ret_agg7()
AGG(8)
// ELF-LABEL: define{{.*}} void @ret_agg8(ptr {{.*}}sret(%struct.agg8)
// KABI-LABEL: define{{.*}} i64 @ret_agg8()
AGG(9)
// ELF-LABEL: define{{.*}} void @ret_agg9(ptr {{.*}}sret(%struct.agg9)
// KABI-LABEL: define{{.*}} { i64, i8 } @ret_agg9()
AGG(11)
// KABI-LABEL: define{{.*}} { i64, i24 } @ret_agg11()
AGG(12)
// KABI-LABEL: define{{.*}} { i64, i32 } @ret_agg12()
AGG(15)
// KABI-LABEL: define{{.*}} { i64, i56 } @ret_agg15()
AGG(16)
// KABI-LABEL: define{{.*}} { i64, i64 } @ret_agg16()
AGG(17)
// CHECK-LABEL: define{{.*}} void @ret_agg17(ptr {{.*}}sret(%struct.agg17)

// The second register holds bytes 8-11, right-justified.
struct s12 { int a, b, c; };
struct s12 ret_s12(struct s12 *p) { return *p; }
// KABI-LABEL: define{{.*}} { i64, i32 } @ret_s12(ptr
// KABI: [[TMP:%.*]] = alloca { i64, i32 }, align 8
// KABI: call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[TMP]], ptr align 4 %{{.*}}, i64 12, i1 false)
// KABI: [[RET:%.*]] = load { i64, i32 }, ptr [[TMP]], align 8
// KABI: ret { i64, i32 } [[RET]]

struct s12 call_s12(void) { return ret_s12(0); }
// KABI-LABEL: define{{.*}} { i64, i32 } @call_s12()
// KABI: [[RES:%.*]] = call { i64, i32 } @ret_s12(ptr noundef null)
// KABI: store { i64, i32 } [[RES]], ptr [[TMP:%.*]], align 8
// KABI: call void @llvm.memcpy.p0.p0.i64(ptr align 4 %{{.*}}, ptr align 8 [[TMP]], i64 12, i1 false)

// A flexible array member does not count.
struct fam { long n; int d[]; };
struct fam ret_fam(struct fam *p) { return *p; }
// ELF-LABEL: define{{.*}} void @ret_fam(ptr {{.*}}sret(%struct.fam)
// KABI-LABEL: define{{.*}} i64 @ret_fam(ptr

union u12 { int i[3]; char c; };
union u12 ret_u12(union u12 *p) { return *p; }
// ELF-LABEL: define{{.*}} void @ret_u12(ptr {{.*}}sret(%union.u12)
// KABI-LABEL: define{{.*}} { i64, i32 } @ret_u12(ptr

struct __attribute__((packed)) packed { char c; long l; };
struct packed ret_packed(struct packed *p) { return *p; }
// KABI-LABEL: define{{.*}} { i64, i8 } @ret_packed(ptr

struct single_double { double d; };
struct single_double ret_single_double(struct single_double *p) { return *p; }
// KABI-LABEL: define{{.*}} i64 @ret_single_double(ptr

_Complex char ret_cchar(_Complex char *p) { return *p; }
// ELF-LABEL: define{{.*}} void @ret_cchar(ptr {{.*}}sret({ i8, i8 })
// KABI-LABEL: define{{.*}} noext i16 @ret_cchar(ptr

_Complex float ret_cfloat(_Complex float *p) { return *p; }
// ELF-LABEL: define{{.*}} void @ret_cfloat(ptr {{.*}}sret({ float, float })
// KABI-LABEL: define{{.*}} i64 @ret_cfloat(ptr

_Complex double ret_cdouble(_Complex double *p) { return *p; }
// ELF-LABEL: define{{.*}} void @ret_cdouble(ptr {{.*}}sret({ double, double })
// KABI-LABEL: define{{.*}} { i64, i64 } @ret_cdouble(ptr

_Complex long double ret_cldouble(_Complex long double *p) { return *p; }
// CHECK-LABEL: define{{.*}} void @ret_cldouble(ptr {{.*}}sret({ fp128, fp128 })

// Not composites.
long double ret_ldouble(long double *p) { return *p; }
// CHECK-LABEL: define{{.*}} void @ret_ldouble(ptr {{.*}}sret(fp128)
__int128 ret_int128(__int128 *p) { return *p; }
// CHECK-LABEL: define{{.*}} void @ret_int128(ptr {{.*}}sret(i128)
typedef int v2si __attribute__((vector_size(8)));
v2si ret_v2si(v2si *p) { return *p; }
// CHECK-LABEL: define{{.*}} void @ret_v2si(ptr {{.*}}sret(<2 x i32>)
int ret_int(int *p) { return *p; }
// CHECK-LABEL: define{{.*}} signext i32 @ret_int(ptr

// Arguments are not affected.
void arg_agg12(struct agg12 a) { }
// CHECK-LABEL: define{{.*}} void @arg_agg12(ptr {{.*}}dead_on_return
