// RUN: %clang --target=s390x-linux-gnu -msoft-float -E -dM %s -o - \
// RUN:   | FileCheck --check-prefix=NONE %s
// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-ret -E -dM %s -o - \
// RUN:   | FileCheck --check-prefix=STRUCT-RET %s
// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-ret -E -dM -x assembler-with-cpp \
// RUN:   %s -o - | FileCheck --check-prefix=STRUCT-RET %s

// NONE-NOT: __S390_EXPERIMENTAL_KERNEL_ABI

// STRUCT-RET: #define __S390_EXPERIMENTAL_KERNEL_ABI_STRUCT_RET__ 1
