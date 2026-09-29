// RUN: %clang -### --target=s390x-linux-gnu -freg-struct-return %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=REG
// RUN: %clang -### --target=s390x-linux-gnu -fpcc-struct-return %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=PCC
// RUN: not %clang -### --target=s390x-ibm-zos -freg-struct-return %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=ZOS-REG
// RUN: not %clang -### --target=s390x-ibm-zos -fpcc-struct-return %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=ZOS-PCC

// REG: "-cc1"{{.*}} "-freg-struct-return"
// PCC: "-cc1"{{.*}} "-fpcc-struct-return"
// ZOS-REG: error: unsupported option '-freg-struct-return' for target 's390x-ibm-zos'
// ZOS-PCC: error: unsupported option '-fpcc-struct-return' for target 's390x-ibm-zos'
