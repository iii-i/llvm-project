// RUN: %clang --target=s390x-linux-gnu -c -### %s -freg-struct-return 2>&1 \
// RUN:   | FileCheck %s --check-prefix=REG
// RUN: %clang --target=s390x-linux-gnu -c -### %s -fpcc-struct-return 2>&1 \
// RUN:   | FileCheck %s --check-prefix=PCC
// RUN: %clang --target=s390x-linux-gnu -c -### %s -freg-struct-return \
// RUN:   -fpcc-struct-return 2>&1 | FileCheck %s --check-prefix=PCC
// RUN: %clang --target=s390x-linux-gnu -c -### %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=DEFAULT
// RUN: not %clang --target=s390x-ibm-zos -c -### %s -freg-struct-return 2>&1 \
// RUN:   | FileCheck %s --check-prefix=ZOS-REG
// RUN: not %clang --target=s390x-ibm-zos -c -### %s -fpcc-struct-return 2>&1 \
// RUN:   | FileCheck %s --check-prefix=ZOS-PCC

// REG: "-freg-struct-return"
// PCC: "-fpcc-struct-return"
// DEFAULT-NOT: "-f{{pcc|reg}}-struct-return"
// ZOS-REG: error: unsupported option '-freg-struct-return' for target 's390x-ibm-zos'
// ZOS-PCC: error: unsupported option '-fpcc-struct-return' for target 's390x-ibm-zos'
