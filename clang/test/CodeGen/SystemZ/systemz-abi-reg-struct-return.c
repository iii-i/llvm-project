// Test the alternative Linux kernel ABI, in which composite values of up to
// 16 bytes are returned in %r2 and %r3 instead of in memory.

// RUN: %clang_cc1 -no-enable-noundef-analysis -triple s390x-linux-gnu \
// RUN:   -freg-struct-return -emit-llvm -o - %s | FileCheck %s \
// RUN:   --check-prefixes=CHECK,NO-VECTOR
// RUN: %clang_cc1 -no-enable-noundef-analysis -triple s390x-linux-gnu \
// RUN:   -freg-struct-return -target-cpu z13 -emit-llvm -o - %s | FileCheck %s \
// RUN:   --check-prefixes=CHECK,VECTOR
// RUN: %clang_cc1 -no-enable-noundef-analysis -triple s390x-linux-gnu \
// RUN:   -freg-struct-return -mfloat-abi soft -emit-llvm -o - %s | FileCheck %s \
// RUN:   --check-prefixes=CHECK,NO-VECTOR

// The default ABI, which -fpcc-struct-return selects explicitly, is unchanged.

// RUN: %clang_cc1 -no-enable-noundef-analysis -triple s390x-linux-gnu \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefix=MEMORY
// RUN: %clang_cc1 -no-enable-noundef-analysis -triple s390x-linux-gnu \
// RUN:   -fpcc-struct-return -emit-llvm -o - %s | FileCheck %s \
// RUN:   --check-prefix=MEMORY


// Scalar types are not composites, so they are unaffected.

int ret_int(void) { return 0; }
// CHECK-LABEL: define{{.*}} signext i32 @ret_int()

__int128 ret_int128(void) { return 0; }
// CHECK-LABEL: define{{.*}} void @ret_int128(ptr dead_on_unwind noalias writable sret(i128) align 8 %{{.*}})

double ret_double(void) { return 0; }
// CHECK-LABEL: define{{.*}} double @ret_double()

long double ret_longdouble(void) { return 0; }
// CHECK-LABEL: define{{.*}} void @ret_longdouble(ptr dead_on_unwind noalias writable sret(fp128) align 8 %{{.*}})


// Aggregates of up to 8 bytes are returned in %r2, and aggregates of up to
// 16 bytes in %r2 and %r3.  Each register holds its part of the value in its
// low-order bits.

struct agg_1byte { char a[1]; };
struct agg_1byte ret_agg_1byte(void) { struct agg_1byte r; return r; }
// CHECK-LABEL: define{{.*}} noext i8 @ret_agg_1byte()

struct agg_2byte { char a[2]; };
struct agg_2byte ret_agg_2byte(void) { struct agg_2byte r; return r; }
// CHECK-LABEL: define{{.*}} noext i16 @ret_agg_2byte()

struct agg_3byte { char a[3]; };
struct agg_3byte ret_agg_3byte(void) { struct agg_3byte r; return r; }
// CHECK-LABEL: define{{.*}} noext i24 @ret_agg_3byte()

struct agg_4byte { char a[4]; };
struct agg_4byte ret_agg_4byte(void) { struct agg_4byte r; return r; }
// CHECK-LABEL: define{{.*}} noext i32 @ret_agg_4byte()
// MEMORY-LABEL: define{{.*}} void @ret_agg_4byte(ptr dead_on_unwind noalias writable sret(%struct.agg_4byte) align 1 %{{.*}})

struct agg_5byte { char a[5]; };
struct agg_5byte ret_agg_5byte(void) { struct agg_5byte r; return r; }
// CHECK-LABEL: define{{.*}} i40 @ret_agg_5byte()

struct agg_8byte { char a[8]; };
struct agg_8byte ret_agg_8byte(void) { struct agg_8byte r; return r; }
// CHECK-LABEL: define{{.*}} i64 @ret_agg_8byte()

struct agg_9byte { char a[9]; };
struct agg_9byte ret_agg_9byte(void) { struct agg_9byte r; return r; }
// CHECK-LABEL: define{{.*}} { i64, i8 } @ret_agg_9byte()

struct agg_12byte { char a[12]; };
struct agg_12byte ret_agg_12byte(void) { struct agg_12byte r; return r; }
// CHECK-LABEL: define{{.*}} { i64, i32 } @ret_agg_12byte()
// MEMORY-LABEL: define{{.*}} void @ret_agg_12byte(ptr dead_on_unwind noalias writable sret(%struct.agg_12byte) align 1 %{{.*}})

struct agg_16byte { char a[16]; };
struct agg_16byte ret_agg_16byte(void) { struct agg_16byte r; return r; }
// CHECK-LABEL: define{{.*}} { i64, i64 } @ret_agg_16byte()
// MEMORY-LABEL: define{{.*}} void @ret_agg_16byte(ptr dead_on_unwind noalias writable sret(%struct.agg_16byte) align 1 %{{.*}})

// Larger aggregates are still returned in memory.
struct agg_17byte { char a[17]; };
struct agg_17byte ret_agg_17byte(void) { struct agg_17byte r; return r; }
// CHECK-LABEL: define{{.*}} void @ret_agg_17byte(ptr dead_on_unwind noalias writable sret(%struct.agg_17byte) align 1 %{{.*}})

// The third int ends up in the low half of %r3, where it can be used without
// shifting it into place first.
struct agg_3int { int a, b, c; };
struct agg_3int ret_agg_3int(void) { struct agg_3int r; return r; }
// CHECK-LABEL: define{{.*}} { i64, i32 } @ret_agg_3int()

union union_8byte { int a; double b; };
union union_8byte ret_union_8byte(void) { union union_8byte r; return r; }
// CHECK-LABEL: define{{.*}} i64 @ret_union_8byte()

// Floating point members are returned in general purpose registers, too.

struct agg_float { float a; };
struct agg_float ret_agg_float(void) { struct agg_float r; return r; }
// CHECK-LABEL: define{{.*}} noext i32 @ret_agg_float()

struct agg_double { double a; };
struct agg_double ret_agg_double(void) { struct agg_double r; return r; }
// CHECK-LABEL: define{{.*}} i64 @ret_agg_double()
// MEMORY-LABEL: define{{.*}} void @ret_agg_double(ptr dead_on_unwind noalias writable sret(%struct.agg_double) align 8 %{{.*}})

struct agg_2double { double a, b; };
struct agg_2double ret_agg_2double(void) { struct agg_2double r; return r; }
// CHECK-LABEL: define{{.*}} { i64, i64 } @ret_agg_2double()

struct agg_longdouble { long double a; };
struct agg_longdouble ret_agg_longdouble(void) { struct agg_longdouble r; return r; }
// CHECK-LABEL: define{{.*}} { i64, i64 } @ret_agg_longdouble()

// Aggregates of variable length, and aggregates without any members, are
// returned in memory.

struct agg_flexarray { int a; int b[]; };
struct agg_flexarray ret_agg_flexarray(void) { struct agg_flexarray r; return r; }
// CHECK-LABEL: define{{.*}} void @ret_agg_flexarray(ptr dead_on_unwind noalias writable sret(%struct.agg_flexarray) align 4 %{{.*}})

struct agg_empty { };
struct agg_empty ret_agg_empty(void) { struct agg_empty r; return r; }
// CHECK-LABEL: define{{.*}} void @ret_agg_empty(ptr dead_on_unwind noalias writable sret(%struct.agg_empty) align 1 %{{.*}})


// Complex values are composites as well.

_Complex char ret_complex_char(void) { return 0; }
// CHECK-LABEL: define{{.*}} noext i16 @ret_complex_char()

_Complex int ret_complex_int(void) { return 0; }
// CHECK-LABEL: define{{.*}} i64 @ret_complex_int()

_Complex float ret_complex_float(void) { return 0; }
// CHECK-LABEL: define{{.*}} i64 @ret_complex_float()

_Complex double ret_complex_double(void) { return 0; }
// CHECK-LABEL: define{{.*}} { i64, i64 } @ret_complex_double()
// MEMORY-LABEL: define{{.*}} void @ret_complex_double(ptr dead_on_unwind noalias writable sret({ double, double }) align 8 %{{.*}})

_Complex long double ret_complex_longdouble(void) { return 0; }
// CHECK-LABEL: define{{.*}} void @ret_complex_longdouble(ptr dead_on_unwind noalias writable sret({ fp128, fp128 }) align 8 %{{.*}})


// Vectors are still returned in %v24 when the vector facility is available.

typedef __attribute__((vector_size(8))) int v2i32;
v2i32 ret_v2i32(void) { return (v2i32){ 0 }; }
// NO-VECTOR-LABEL: define{{.*}} i64 @ret_v2i32()
// VECTOR-LABEL: define{{.*}} <2 x i32> @ret_v2i32()

typedef __attribute__((vector_size(16))) int v4i32;
v4i32 ret_v4i32(void) { return (v4i32){ 0 }; }
// NO-VECTOR-LABEL: define{{.*}} { i64, i64 } @ret_v4i32()
// VECTOR-LABEL: define{{.*}} <4 x i32> @ret_v4i32()

typedef __attribute__((vector_size(32))) int v8i32;
v8i32 ret_v8i32(void) { return (v8i32){ 0 }; }
// CHECK-LABEL: define{{.*}} void @ret_v8i32(ptr dead_on_unwind noalias writable sret(<8 x i32>) align {{[0-9]+}} %{{.*}})


// The caller coerces the value back into its natural type.

struct agg_12byte call_agg_12byte(struct agg_12byte (*fn)(void)) { return fn(); }
// CHECK-LABEL: define{{.*}} { i64, i32 } @call_agg_12byte(ptr %{{.*}})
// CHECK: call { i64, i32 } %{{.*}}()
