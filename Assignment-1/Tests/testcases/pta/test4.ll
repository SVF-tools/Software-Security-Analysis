; ModuleID = 'Assignment-1/Tests/testcases/pta/test4.ll'
source_filename = "Assignment-1/Tests/testcases/pta/test4.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

%struct.MyStruct = type { ptr, i32, ptr }

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %s1 = alloca %struct.MyStruct, align 8
  %s2 = alloca %struct.MyStruct, align 8
  store i32 10, ptr %a, align 4
  store i32 20, ptr %b, align 4
  %ptr1 = getelementptr inbounds %struct.MyStruct, ptr %s1, i32 0, i32 0
  store ptr %a, ptr %ptr1, align 8
  %ptr2 = getelementptr inbounds %struct.MyStruct, ptr %s1, i32 0, i32 2
  store ptr %a, ptr %ptr2, align 8
  %ptr11 = getelementptr inbounds %struct.MyStruct, ptr %s2, i32 0, i32 0
  store ptr %b, ptr %ptr11, align 8
  %ptr22 = getelementptr inbounds %struct.MyStruct, ptr %s2, i32 0, i32 2
  store ptr %a, ptr %ptr22, align 8
  %ptr13 = getelementptr inbounds %struct.MyStruct, ptr %s1, i32 0, i32 0
  %0 = load ptr, ptr %ptr13, align 8
  %ptr24 = getelementptr inbounds %struct.MyStruct, ptr %s1, i32 0, i32 2
  %1 = load ptr, ptr %ptr24, align 8
  call void @MAYALIAS(ptr noundef %0, ptr noundef %1)
  %ptr25 = getelementptr inbounds %struct.MyStruct, ptr %s1, i32 0, i32 2
  %2 = load ptr, ptr %ptr25, align 8
  %ptr26 = getelementptr inbounds %struct.MyStruct, ptr %s2, i32 0, i32 2
  %3 = load ptr, ptr %ptr26, align 8
  call void @MAYALIAS(ptr noundef %2, ptr noundef %3)
  ret i32 0
}

declare void @MAYALIAS(ptr noundef, ptr noundef) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 16.0.6"}
