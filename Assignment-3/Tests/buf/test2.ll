; ModuleID = 'Assignment-3/Tests/buf/test2.ll'
source_filename = "Assignment-3/Tests/buf/test2.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str.1 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@.str.2 = private unnamed_addr constant [32 x i8] c"ERROR: Array index is negative.\00", align 1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @printIntLine(i32 noundef %value) #0 {
entry:
  %call = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %value)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @printLine(ptr noundef %message) #0 {
entry:
  %call = call i32 (ptr, ...) @printf(ptr noundef @.str.1, ptr noundef %message)
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @CWE121_Stack_Based_Buffer_Overflow__CWE129_rand_01_bad() #0 {
entry:
  %buffer = alloca [10 x i32], align 4
  %call = call i64 @time(ptr noundef null)
  %conv = trunc i64 %call to i32
  call void @srand(i32 noundef %conv)
  %call1 = call i32 @rand()
  %rem = srem i32 %call1, 100
  call void @llvm.memset.p0.i64(ptr align 4 %buffer, i8 0, i64 40, i1 false)
  %cmp = icmp sge i32 %rem, 0
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %arraydecay = getelementptr inbounds [10 x i32], ptr %buffer, i64 0, i64 0
  call void @OVERFLOW(ptr noundef %arraydecay, i32 noundef 396)
  %idxprom = sext i32 %rem to i64
  %arrayidx = getelementptr inbounds [10 x i32], ptr %buffer, i64 0, i64 %idxprom
  store i32 1, ptr %arrayidx, align 4
  br label %if.end

if.else:                                          ; preds = %entry
  call void @printLine(ptr noundef @.str.2)
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %if.end
  %i.0 = phi i32 [ 0, %if.end ], [ %inc, %for.inc ]
  %cmp3 = icmp slt i32 %i.0, 10
  br i1 %cmp3, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %idxprom5 = sext i32 %i.0 to i64
  %arrayidx6 = getelementptr inbounds [10 x i32], ptr %buffer, i64 0, i64 %idxprom5
  %0 = load i32, ptr %arrayidx6, align 4
  call void @printIntLine(i32 noundef %0)
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %inc = add nsw i32 %i.0, 1
  br label %for.cond, !llvm.loop !5

for.end:                                          ; preds = %for.cond
  ret void
}

declare void @srand(i32 noundef) #1

declare i64 @time(ptr noundef) #1

declare i32 @rand() #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

declare void @OVERFLOW(ptr noundef, i32 noundef) #1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  call void @CWE121_Stack_Based_Buffer_Overflow__CWE129_rand_01_bad()
  ret i32 0
}

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 16.0.6"}
!5 = distinct !{!5, !6}
!6 = !{!"llvm.loop.mustprogress"}
