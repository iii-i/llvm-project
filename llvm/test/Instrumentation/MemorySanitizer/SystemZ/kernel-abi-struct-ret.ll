; RUN: opt < %s -S -mcpu=z13 -msan-kernel=1 -float-abi=soft -passes=msan 2>&1 | FileCheck %s

target datalayout = "E-m:e-i1:8:16-i8:8:16-i64:64-f128:64-a:8:16-n32:64"
target triple = "s390x-unknown-linux-gnu"

define void @Store8(ptr %p, i64 %x) sanitize_memory #0 {
entry:
  store i64 %x, ptr %p
  ret void
}

; CHECK-LABEL: define {{[^@]+}}@Store8(
; CHECK-NOT: alloca
; CHECK: [[META:%[a-z0-9_]+]] = call { ptr, ptr } @__msan_metadata_ptr_for_store_8(ptr %p)
; CHECK: [[SHADOW:%[a-z0-9_]+]] = extractvalue { ptr, ptr } [[META]], 0
; CHECK: store i64 {{.+}}, ptr [[SHADOW]]
; CHECK: ret void

define i64 @Load16(ptr %p) sanitize_memory #0 {
entry:
  %v = load i128, ptr %p
  %t = trunc i128 %v to i64
  ret i64 %t
}

; CHECK-LABEL: define {{[^@]+}}@Load16(
; CHECK: call { ptr, ptr } @__msan_metadata_ptr_for_load_n(ptr %p, i64 16)

; CHECK: declare { ptr, ptr } @__msan_metadata_ptr_for_load_1(ptr)

attributes #0 = { "target-features"="+soft-float,+experimental-kernel-abi-struct-ret" }
