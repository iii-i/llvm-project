// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float -emit-llvm -o - %s \
// RUN:   | FileCheck %s --check-prefixes=CHECK,ELF
// RUN: %clang_cc1 -triple s390x-linux-gnu -mfloat-abi soft \
// RUN:   -target-feature +soft-float \
// RUN:   -target-feature +experimental-kernel-abi-no-ext \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,KABI

char pass_char(char a) { return a; }
// ELF-LABEL: define{{.*}} signext i8 @pass_char(i8 noundef signext %a)
// KABI-LABEL: define{{.*}} noext i8 @pass_char(i8 noext noundef %a)

signed char pass_schar(signed char a) { return a; }
// KABI-LABEL: define{{.*}} noext i8 @pass_schar(i8 noext noundef %a)

short pass_short(short a) { return a; }
// ELF-LABEL: define{{.*}} signext i16 @pass_short(i16 noundef signext %a)
// KABI-LABEL: define{{.*}} noext i16 @pass_short(i16 noext noundef %a)

int pass_int(int a) { return a; }
// ELF-LABEL: define{{.*}} signext i32 @pass_int(i32 noundef signext %a)
// KABI-LABEL: define{{.*}} noext i32 @pass_int(i32 noext noundef %a)

unsigned pass_uint(unsigned a) { return a; }
// ELF-LABEL: define{{.*}} zeroext i32 @pass_uint(i32 noundef zeroext %a)
// KABI-LABEL: define{{.*}} noext i32 @pass_uint(i32 noext noundef %a)

enum e { E0, E1 };
enum e pass_enum(enum e a) { return a; }
// KABI-LABEL: define{{.*}} noext i32 @pass_enum(i32 noext noundef %a)

_BitInt(17) pass_bitint(_BitInt(17) a) { return a; }
// KABI-LABEL: define{{.*}} noext i17 @pass_bitint(i17 noext noundef %a)

long pass_long(long a) { return a; }
// CHECK-LABEL: define{{.*}} i64 @pass_long(i64 noundef %a)

float pass_float(float a) { return a; }
// CHECK-LABEL: define{{.*}} float @pass_float(float noundef %a)

// _Bool is 0 or 1 in the low byte, which the backend ensures.
_Bool pass_bool(_Bool a) { return a; }
// ELF-LABEL: define{{.*}} zeroext i1 @pass_bool(i1 noundef zeroext %a)
// KABI-LABEL: define{{.*}} noext i1 @pass_bool(i1 noext noundef %a)

int call_int(int a) { return pass_int(a); }
// KABI-LABEL: define{{.*}} noext i32 @call_int(
// KABI: call noext i32 @pass_int(i32 noext noundef %{{.*}})
