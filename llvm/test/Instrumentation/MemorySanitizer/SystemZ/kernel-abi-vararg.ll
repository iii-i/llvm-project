; RUN: opt < %s -S -mcpu=z13 -msan-kernel=1 -float-abi=soft -passes=msan 2>&1 | FileCheck %s

target datalayout = "E-m:e-i1:8:16-i8:8:16-i64:64-f128:64-a:8:16-n32:64"
target triple = "s390x-unknown-linux-gnu"

declare void @vararg(i64, ...)

; The first pair is in %r3:%r4 and its tail is right-justified, the second
; one does not fit in %r5:%r6 and is on the stack as its memory image.
define void @Pair(i64 %a, { i64, i32 } %b, { i64, i32 } %c) sanitize_memory #0 {
  call void (i64, ...) @vararg(i64 %a, { i64, i32 } %b, i64 %a, i64 %a, { i64, i32 } %c)
  ret void
}

; CHECK-LABEL: @Pair(
; CHECK: [[B:%.*]] = load { i64, i32 }, ptr
; CHECK: [[C:%.*]] = load { i64, i32 }, ptr
; CHECK: [[B0:%.*]] = extractvalue { i64, i32 } [[B]], 0
; CHECK: [[P0:%.*]] = getelementptr i8, ptr %va_arg_shadow, i64 24
; CHECK: store i64 [[B0]], ptr [[P0]]
; CHECK: [[B1:%.*]] = extractvalue { i64, i32 } [[B]], 1
; CHECK: [[P1:%.*]] = getelementptr i8, ptr %va_arg_shadow, i64 36
; CHECK: store i32 [[B1]], ptr [[P1]]
; CHECK: add i64 {{.*}}, 40
; CHECK: add i64 {{.*}}, 48
; CHECK: [[P4:%.*]] = getelementptr i8, ptr %va_arg_shadow, i64 160
; CHECK: store { i64, i32 } [[C]], ptr [[P4]]
; CHECK: store i64 16, ptr %va_arg_overflow_size

define void @Int128(i64 %a, i128 %b) sanitize_memory #1 {
  call void (i64, ...) @vararg(i64 %a, i128 %b, i64 %a, i64 %a, i128 %b)
  ret void
}

; CHECK-LABEL: @Int128(
; CHECK: [[B:%.*]] = load i128, ptr
; CHECK: [[P0:%.*]] = getelementptr i8, ptr %va_arg_shadow, i64 24
; CHECK: store i128 [[B]], ptr [[P0]]
; CHECK: [[P1:%.*]] = getelementptr i8, ptr %va_arg_shadow, i64 160
; CHECK: store i128 [[B]], ptr [[P1]]
; CHECK: store i64 16, ptr %va_arg_overflow_size

attributes #0 = { "target-features"="+soft-float,+experimental-kernel-abi-struct-arg" }
attributes #1 = { "target-features"="+soft-float,+experimental-kernel-abi-int128" }
