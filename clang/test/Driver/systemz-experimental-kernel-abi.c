// RUN: %clang --target=s390x-linux-gnu -msoft-float -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=NONE %s
// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi= -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=NONE %s
// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-ret -mexperimental-kernel-abi= \
// RUN:   -### -c %s 2>&1 | FileCheck --check-prefix=NONE %s
// NONE-NOT: error:
// NONE-NOT: "+experimental-kernel-abi

// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-ret -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=STRUCT-RET %s
// RUN: %clang --target=s390x-unknown-none -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-ret,struct-ret -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=STRUCT-RET %s
// STRUCT-RET-NOT: error:
// STRUCT-RET: "-target-feature" "+experimental-kernel-abi-struct-ret"
// STRUCT-RET-NOT: "+experimental-kernel-abi

// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-arg -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=STRUCT-ARG %s
// STRUCT-ARG-NOT: error:
// STRUCT-ARG: "-target-feature" "+experimental-kernel-abi-struct-arg"
// STRUCT-ARG-NOT: "+experimental-kernel-abi

// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-arg,struct-ret -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=STRUCT %s
// STRUCT-NOT: error:
// STRUCT: "-target-feature" "+experimental-kernel-abi-struct-ret" "-target-feature" "+experimental-kernel-abi-struct-arg"
// STRUCT-NOT: "+experimental-kernel-abi

// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=int128 -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=INT128 %s
// INT128-NOT: error:
// INT128: "-target-feature" "+experimental-kernel-abi-int128"
// INT128-NOT: "+experimental-kernel-abi

// RUN: not %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-ret,foo -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=UNKNOWN %s
// UNKNOWN: error: unsupported argument 'foo' to option '-mexperimental-kernel-abi='

// RUN: not %clang --target=s390x-linux-gnu \
// RUN:   -mexperimental-kernel-abi=struct-ret -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=HARD-FLOAT %s
// RUN: not %clang --target=s390x-linux-gnu -msoft-float -mhard-float \
// RUN:   -mexperimental-kernel-abi= -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=HARD-FLOAT %s
// HARD-FLOAT: error: invalid argument '-mexperimental-kernel-abi={{(struct-ret)?}}' only allowed with '-msoft-float'

// RUN: not %clang --target=s390x-ibm-zos -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-ret -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=ZOS %s
// ZOS: error: unsupported option '-mexperimental-kernel-abi=' for target 's390x-ibm-zos'

// RUN: not %clang --target=x86_64-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-ret -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=X86 %s
// X86: error: unsupported option '-mexperimental-kernel-abi=' for target 'x86_64-linux-gnu'
