// With -freg-struct-return a composite is returned where it would be passed
// as the first argument, and in memory if it would be passed by reference.

// RUN: %clang_cc1 -triple s390x-linux-gnu -freg-struct-return -emit-llvm \
// RUN:   -o - %s | FileCheck %s --check-prefixes=CHECK,HARD
// RUN: %clang_cc1 -triple s390x-linux-gnu -freg-struct-return -target-cpu z13 \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,HARD,VECTOR
// RUN: %clang_cc1 -triple s390x-linux-gnu -freg-struct-return -mfloat-abi soft \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,SOFT
// RUN: %clang_cc1 -triple s390x-linux-gnu -emit-llvm -o - %s \
// RUN:   | FileCheck %s --check-prefix=MEMORY
// RUN: %clang_cc1 -triple s390x-linux-gnu -fpcc-struct-return -emit-llvm \
// RUN:   -o - %s | FileCheck %s --check-prefix=MEMORY

struct agg_1byte { char a[1]; };
struct agg_1byte ret_agg_1byte(void) { struct agg_1byte r; return r; }
// CHECK-LABEL: define{{.*}} noext i8 @ret_agg_1byte()

struct agg_3byte { char a[3]; };
struct agg_3byte ret_agg_3byte(void) { struct agg_3byte r; return r; }
// CHECK-LABEL: define{{.*}} void @ret_agg_3byte(ptr dead_on_unwind noalias writable sret(%struct.agg_3byte) align 1 %{{.*}})

struct agg_4byte { char a[4]; };
struct agg_4byte ret_agg_4byte(void) { struct agg_4byte r; return r; }
// CHECK-LABEL: define{{.*}} noext i32 @ret_agg_4byte()

struct agg_padded { int x; char y; };
struct agg_padded ret_agg_padded(void) { struct agg_padded r; return r; }
// CHECK-LABEL: define{{.*}} i64 @ret_agg_padded()
// MEMORY-LABEL: define{{.*}} void @ret_agg_padded(ptr dead_on_unwind noalias writable sret(%struct.agg_padded) align 4 %{{.*}})

struct agg_8byte { char a[8]; };
struct agg_8byte ret_agg_8byte(void) { struct agg_8byte r; return r; }
// CHECK-LABEL: define{{.*}} i64 @ret_agg_8byte()

struct agg_12byte { int a, b, c; };
struct agg_12byte ret_agg_12byte(void) { struct agg_12byte r; return r; }
// CHECK-LABEL: define{{.*}} void @ret_agg_12byte(ptr dead_on_unwind noalias writable sret(%struct.agg_12byte) align 4 %{{.*}})

struct agg_16byte { long a, b; };
struct agg_16byte ret_agg_16byte(void) { struct agg_16byte r; return r; }
// CHECK-LABEL: define{{.*}} void @ret_agg_16byte(ptr dead_on_unwind noalias writable sret(%struct.agg_16byte) align 8 %{{.*}})

struct agg_float { float a; };
struct agg_float ret_agg_float(void) { struct agg_float r; return r; }
// HARD-LABEL: define{{.*}} float @ret_agg_float()
// SOFT-LABEL: define{{.*}} noext i32 @ret_agg_float()

struct agg_double { double a; };
struct agg_double ret_agg_double(void) { struct agg_double r; return r; }
// HARD-LABEL: define{{.*}} double @ret_agg_double()
// SOFT-LABEL: define{{.*}} i64 @ret_agg_double()
// MEMORY-LABEL: define{{.*}} void @ret_agg_double(ptr dead_on_unwind noalias writable sret(%struct.agg_double) align 8 %{{.*}})

struct agg_nested_double { struct { double a; } in; };
struct agg_nested_double ret_agg_nested_double(void) { struct agg_nested_double r; return r; }
// HARD-LABEL: define{{.*}} double @ret_agg_nested_double()

struct agg_float_padded { float a; } __attribute__((aligned(8)));
struct agg_float_padded ret_agg_float_padded(void) { struct agg_float_padded r; return r; }
// HARD-LABEL: define{{.*}} double @ret_agg_float_padded()

union union_double { double a; };
union union_double ret_union_double(void) { union union_double r; return r; }
// CHECK-LABEL: define{{.*}} i64 @ret_union_double()

struct agg_float_pair { float a, b; };
struct agg_float_pair ret_agg_float_pair(void) { struct agg_float_pair r; return r; }
// CHECK-LABEL: define{{.*}} i64 @ret_agg_float_pair()

struct agg_empty { };
struct agg_empty ret_agg_empty(void) { struct agg_empty r; return r; }
// CHECK-LABEL: define{{.*}} void @ret_agg_empty(ptr dead_on_unwind noalias writable sret(%struct.agg_empty) align 1 %{{.*}})

struct agg_fam { long a; int b[]; };
struct agg_fam ret_agg_fam(void) { struct agg_fam r; return r; }
// CHECK-LABEL: define{{.*}} void @ret_agg_fam(ptr dead_on_unwind noalias writable sret(%struct.agg_fam) align 8 %{{.*}})

typedef __attribute__((vector_size(16))) int v4i32;
struct agg_vector { v4i32 a; };
struct agg_vector ret_agg_vector(void) { struct agg_vector r; return r; }
// VECTOR-LABEL: define{{.*}} <4 x i32> @ret_agg_vector()

_Complex float ret_complex_float(void) { return 0; }
// CHECK-LABEL: define{{.*}} void @ret_complex_float(ptr dead_on_unwind noalias writable sret({ float, float }) align 4 %{{.*}})

struct agg_4byte get_agg_4byte(void);
void take_agg_4byte(struct agg_4byte);
void pass_agg_4byte(void) { take_agg_4byte(get_agg_4byte()); }
// CHECK-LABEL: define{{.*}} void @pass_agg_4byte()
// CHECK: call noext i32 @get_agg_4byte()
// CHECK: call void @take_agg_4byte(i32 noext %{{.*}})
