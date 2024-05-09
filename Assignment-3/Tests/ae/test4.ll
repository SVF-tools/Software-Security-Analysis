; ModuleID = 'branch.ll'
source_filename = "branch.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %call = call i32 @nd()
  %tobool = icmp ne i32 %call, 0
  br i1 %tobool, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %inc = add nsw i32 1, 1
  %inc1 = add nsw i32 1, 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %x.0 = phi i32 [ %inc, %if.then ], [ 1, %entry ]
  %y.0 = phi i32 [ %inc1, %if.then ], [ 1, %entry ]
  %cmp = icmp sle i32 %x.0, 10
  br i1 %cmp, label %land.lhs.true, label %land.end

land.lhs.true:                                    ; preds = %if.end
  %cmp2 = icmp sge i32 %x.0, %y.0
  br i1 %cmp2, label %land.rhs, label %land.end

land.rhs:                                         ; preds = %land.lhs.true
  %cmp3 = icmp sle i32 %x.0, %y.0
  br label %land.end

land.end:                                         ; preds = %land.rhs, %land.lhs.true, %if.end
  %0 = phi i1 [ false, %land.lhs.true ], [ false, %if.end ], [ %cmp3, %land.rhs ]
  call void @svf_assert(i1 noundef zeroext %0)
  ret i32 0
}

declare i32 @nd() #1

declare void @svf_assert(i1 noundef zeroext) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 16.0.6"}
