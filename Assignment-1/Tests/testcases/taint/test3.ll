; ModuleID = 'test3.ll'
source_filename = "test3.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

@.str = private unnamed_addr constant [6 x i8] c"hello\00", align 1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define ptr @foo(ptr noundef %token) #0 {
entry:
  ret ptr %token
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %call = call ptr @getchar()
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %tobool = trunc i8 1 to i1
  br i1 %tobool, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %tobool1 = trunc i8 0 to i1
  br i1 %tobool1, label %if.then, label %if.else

if.then:                                          ; preds = %while.body
  %call2 = call ptr @foo(ptr noundef %call)
  br label %if.end

if.else:                                          ; preds = %while.body
  %call3 = call ptr @foo(ptr noundef @.str)
  call void @broadcast(ptr noundef %call3)
  call void @NOALIAS(ptr noundef %call, ptr noundef %call3)
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  br label %while.cond, !llvm.loop !5

while.end:                                        ; preds = %while.cond
  ret i32 0
}

declare ptr @getchar(...) #1

declare void @broadcast(ptr noundef) #1

declare void @NOALIAS(ptr noundef, ptr noundef) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 16.0.6"}
!5 = distinct !{!5, !6}
!6 = !{!"llvm.loop.mustprogress"}
