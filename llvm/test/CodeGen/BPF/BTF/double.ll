; RUN: llc -march=bpfel -filetype=asm -o - %s | FileCheck -check-prefixes=CHECK %s
; RUN: llc -march=bpfeb -filetype=asm -o - %s | FileCheck -check-prefixes=CHECK %s

; Source code:
;   double a;
; Compilation flag:
;   clang -target bpf -O2 -g -S -emit-llvm t.c

@a = dso_local local_unnamed_addr global double 0.000000e+00, align 8, !dbg !0

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!7, !8, !9}
!llvm.ident = !{!10}

; CHECK:             .section .BTF,"",@progbits
; CHECK-NEXT:        .short 60319 # 0xeb9f
; CHECK-NEXT:        .byte 1
; CHECK-NEXT:        .byte 0
; CHECK-NEXT:        .long 24
; CHECK-NEXT:        .long 0
; CHECK-NEXT:        .long 96
; CHECK-NEXT:        .long 96
; CHECK-NEXT:        .long 42
; [1] __BYTE_TYPE__, int, size=1 byte, unsigned, 8 bits
; CHECK-NEXT:        .long 1 # BTF_KIND_INT(id = 1)
; CHECK-NEXT:        .long 16777216 # 0x1000000
; CHECK-NEXT:        .long 1
; CHECK-NEXT:        .long 8 # 0x8
; [2] __ARRAY_SIZE_TYPE__, int, size=4 bytes, unsigned, 32 bits
; CHECK-NEXT:        .long 15 # BTF_KIND_INT(id = 2)
; CHECK-NEXT:        .long 16777216 # 0x1000000
; CHECK-NEXT:        .long 4
; CHECK-NEXT:        .long 32 # 0x20
; [3] array, type=__BYTE_TYPE__, index_type=__ARRAY_SIZE_TYPE__, nelems=8
; CHECK-NEXT:        .long 0 # BTF_KIND_ARRAY(id = 3)
; CHECK-NEXT:        .long 50331648 # 0x3000000
; CHECK-NEXT:        .long 0
; CHECK-NEXT:        .long 1
; CHECK-NEXT:        .long 2
; CHECK-NEXT:        .long 8
; [4] a, var, type=__BYTE_TYPE__[8], global
; CHECK-NEXT:        .long 35 # BTF_KIND_VAR(id = 4)
; CHECK-NEXT:        .long 234881024 # 0xe000000
; CHECK-NEXT:        .long 3
; CHECK-NEXT:        .long 1
; [5] .bss, datasec, 1 var, {a, offset=&a, size=8 bytes}
; CHECK-NEXT:        .long 37 # BTF_KIND_DATASEC(id = 5)
; CHECK-NEXT:        .long 251658241 # 0xf000001
; CHECK-NEXT:        .long 0
; CHECK-NEXT:        .long 4
; CHECK-NEXT:        .long a
; CHECK-NEXT:        .long 8
; CHECK-NEXT:        .byte 0 # string offset=0
; CHECK-NEXT:        .ascii "__BYTE_TYPE__" # string offset=1
; CHECK-NEXT:        .byte 0
; CHECK-NEXT:        .ascii "__ARRAY_SIZE_TYPE__" # string offset=15
; CHECK-NEXT:        .byte 0
; CHECK-NEXT:        .byte 97 # string offset=35
; CHECK-NEXT:        .byte 0
; CHECK-NEXT:        .ascii ".bss" # string offset=37
; CHECK-NEXT:        .byte 0

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "a", scope: !2, file: !3, line: 1, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 11.0.0 ", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "t.c", directory: "/home/yhs/tmp")
!4 = !{}
!5 = !{!0}
!6 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!7 = !{i32 7, !"Dwarf Version", i32 4}
!8 = !{i32 2, !"Debug Info Version", i32 3}
!9 = !{i32 1, !"wchar_size", i32 4}
!10 = !{!"clang version 11.0.0 "}
