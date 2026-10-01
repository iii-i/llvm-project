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

// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=no-ext -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=NO-EXT %s
// NO-EXT-NOT: error:
// NO-EXT: "-target-feature" "+experimental-kernel-abi-no-ext"
// NO-EXT-NOT: "+experimental-kernel-abi

// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=r6-clobbered -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=R6-CLOBBERED %s
// R6-CLOBBERED-NOT: error:
// R6-CLOBBERED: "-target-feature" "+experimental-kernel-abi-r6-clobbered"
// R6-CLOBBERED-NOT: "+experimental-kernel-abi

// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=r7-arg,r6-clobbered -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=R7-ARG %s
// R7-ARG-NOT: error:
// R7-ARG: "-target-feature" "+experimental-kernel-abi-r6-clobbered" "-target-feature" "+experimental-kernel-abi-r7-arg"
// R7-ARG-NOT: "+experimental-kernel-abi

// RUN: not %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=r7-arg -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=R7-ARG-ONLY %s
// R7-ARG-ONLY: error: invalid argument 'r7-arg' only allowed with 'r6-clobbered'

// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=even-pairs,int128 -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=EVEN-PAIRS-INT128 %s
// EVEN-PAIRS-INT128-NOT: error:
// EVEN-PAIRS-INT128: "-target-feature" "+experimental-kernel-abi-int128" "-target-feature" "+experimental-kernel-abi-even-pairs"
// EVEN-PAIRS-INT128-NOT: "+experimental-kernel-abi
// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=even-pairs,struct-arg -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=EVEN-PAIRS-STRUCT-ARG %s
// EVEN-PAIRS-STRUCT-ARG-NOT: error:
// EVEN-PAIRS-STRUCT-ARG: "-target-feature" "+experimental-kernel-abi-struct-arg" "-target-feature" "+experimental-kernel-abi-even-pairs"
// EVEN-PAIRS-STRUCT-ARG-NOT: "+experimental-kernel-abi

// RUN: not %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=even-pairs,struct-ret -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=EVEN-PAIRS-ONLY %s
// EVEN-PAIRS-ONLY: error: invalid argument 'even-pairs' only allowed with 'struct-arg' or 'int128'

// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-32,struct-arg,struct-ret -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=STRUCT-32 %s
// STRUCT-32-NOT: error:
// STRUCT-32: "-target-feature" "+experimental-kernel-abi-struct-ret" "-target-feature" "+experimental-kernel-abi-struct-arg" "-target-feature" "+experimental-kernel-abi-struct-32"
// STRUCT-32-NOT: "+experimental-kernel-abi

// RUN: not %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-32 -### -c %s 2>&1 \
// RUN:   | FileCheck --check-prefix=STRUCT-32-ONLY %s
// STRUCT-32-ONLY: error: invalid argument 'struct-32' only allowed with 'struct-ret'
// STRUCT-32-ONLY: error: invalid argument 'struct-32' only allowed with 'struct-arg'

// RUN: %clang --target=s390x-linux-gnu -msoft-float \
// RUN:   -mexperimental-kernel-abi=struct-32,even-pairs,r7-arg,r6-clobbered,no-ext,int128,struct-arg,struct-ret \
// RUN:   -### -c %s 2>&1 | FileCheck --check-prefix=ALL %s
// ALL-NOT: error:
// ALL: "-target-feature" "+experimental-kernel-abi-struct-ret" "-target-feature" "+experimental-kernel-abi-struct-arg" "-target-feature" "+experimental-kernel-abi-int128" "-target-feature" "+experimental-kernel-abi-no-ext" "-target-feature" "+experimental-kernel-abi-r6-clobbered" "-target-feature" "+experimental-kernel-abi-r7-arg" "-target-feature" "+experimental-kernel-abi-even-pairs" "-target-feature" "+experimental-kernel-abi-struct-32"

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
