; ModuleID = './test4.ll'
source_filename = "./test4.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !9 {
entry:
  call void @llvm.dbg.value(metadata i32 1, metadata !15, metadata !DIExpression()), !dbg !16
  br label %while.cond, !dbg !17

while.cond:                                       ; preds = %while.body, %entry
  %x.0 = phi i32 [ 1, %entry ], [ %inc, %while.body ], !dbg !16
  call void @llvm.dbg.value(metadata i32 %x.0, metadata !15, metadata !DIExpression()), !dbg !16
  %cmp = icmp slt i32 %x.0, 10000, !dbg !18
  br i1 %cmp, label %while.body, label %while.end, !dbg !17

while.body:                                       ; preds = %while.cond
  %inc = add nsw i32 %x.0, 1, !dbg !19
  call void @llvm.dbg.value(metadata i32 %inc, metadata !15, metadata !DIExpression()), !dbg !16
  br label %while.cond, !dbg !17, !llvm.loop !21

while.end:                                        ; preds = %while.cond
  %cmp1 = icmp eq i32 %x.0, 10000, !dbg !24
  call void @svf_assert(i1 noundef zeroext %cmp1), !dbg !25
  ret i32 0, !dbg !26
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare void @svf_assert(i1 noundef zeroext) #2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Homebrew clang version 16.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!1 = !DIFile(filename: "test4.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-3/Tests/ae")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 1}
!8 = !{!"Homebrew clang version 16.0.6"}
!9 = distinct !DISubprogram(name: "main", scope: !10, file: !10, line: 4, type: !11, scopeLine: 4, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!10 = !DIFile(filename: "./test4.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-3/Tests/ae")
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "x", scope: !9, file: !10, line: 5, type: !13)
!16 = !DILocation(line: 0, scope: !9)
!17 = !DILocation(line: 7, column: 5, scope: !9)
!18 = !DILocation(line: 7, column: 12, scope: !9)
!19 = !DILocation(line: 8, column: 10, scope: !20)
!20 = distinct !DILexicalBlock(scope: !9, file: !10, line: 7, column: 20)
!21 = distinct !{!21, !17, !22, !23}
!22 = !DILocation(line: 9, column: 5, scope: !9)
!23 = !{!"llvm.loop.mustprogress"}
!24 = !DILocation(line: 10, column: 18, scope: !9)
!25 = !DILocation(line: 10, column: 5, scope: !9)
!26 = !DILocation(line: 11, column: 5, scope: !9)
