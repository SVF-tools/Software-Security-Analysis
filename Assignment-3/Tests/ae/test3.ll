; ModuleID = './test3.ll'
source_filename = "test3.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @foo() #0 !dbg !9 {
entry:
  ret i32 1, !dbg !14
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !15 {
entry:
  call void @llvm.dbg.value(metadata i32 1, metadata !16, metadata !DIExpression()), !dbg !17
  call void @llvm.dbg.value(metadata i32 0, metadata !18, metadata !DIExpression()), !dbg !17
  %call = call i32 @foo(), !dbg !19
  switch i32 %call, label %sw.default [
    i32 0, label %sw.bb
    i32 1, label %sw.bb1
    i32 2, label %sw.bb3
  ], !dbg !20

sw.bb:                                            ; preds = %entry
  %add = add nsw i32 1, 1, !dbg !21
  call void @llvm.dbg.value(metadata i32 %add, metadata !16, metadata !DIExpression()), !dbg !17
  br label %sw.epilog, !dbg !23

sw.bb1:                                           ; preds = %entry
  %add2 = add nsw i32 1, 0, !dbg !24
  call void @llvm.dbg.value(metadata i32 %add2, metadata !16, metadata !DIExpression()), !dbg !17
  br label %sw.epilog, !dbg !25

sw.bb3:                                           ; preds = %entry
  %sub = sub nsw i32 1, 0, !dbg !26
  call void @llvm.dbg.value(metadata i32 %sub, metadata !16, metadata !DIExpression()), !dbg !17
  br label %sw.epilog, !dbg !27

sw.default:                                       ; preds = %entry
  %inc = add nsw i32 1, 1, !dbg !28
  call void @llvm.dbg.value(metadata i32 %inc, metadata !16, metadata !DIExpression()), !dbg !17
  %inc4 = add nsw i32 0, 1, !dbg !29
  call void @llvm.dbg.value(metadata i32 %inc4, metadata !18, metadata !DIExpression()), !dbg !17
  br label %sw.epilog, !dbg !30

sw.epilog:                                        ; preds = %sw.default, %sw.bb3, %sw.bb1, %sw.bb
  %x.0 = phi i32 [ %inc, %sw.default ], [ %sub, %sw.bb3 ], [ %add2, %sw.bb1 ], [ %add, %sw.bb ], !dbg !31
  %y.0 = phi i32 [ %inc4, %sw.default ], [ 0, %sw.bb3 ], [ 0, %sw.bb1 ], [ 0, %sw.bb ], !dbg !17
  call void @llvm.dbg.value(metadata i32 %y.0, metadata !18, metadata !DIExpression()), !dbg !17
  call void @llvm.dbg.value(metadata i32 %x.0, metadata !16, metadata !DIExpression()), !dbg !17
  %cmp = icmp sge i32 %x.0, %y.0, !dbg !32
  call void @svf_assert(i1 noundef zeroext %cmp), !dbg !33
  ret i32 0, !dbg !34
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
!1 = !DIFile(filename: "test3.c", directory: "/Users/z5489735/2023/0718/Software-Security-Analysis/teaching/Assignment-3/Tests/ae")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 1}
!8 = !{!"Homebrew clang version 16.0.6"}
!9 = distinct !DISubprogram(name: "foo", scope: !1, file: !1, line: 4, type: !10, scopeLine: 5, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !13)
!10 = !DISubroutineType(types: !11)
!11 = !{!12}
!12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!13 = !{}
!14 = !DILocation(line: 6, column: 5, scope: !9)
!15 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 9, type: !10, scopeLine: 9, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !13)
!16 = !DILocalVariable(name: "x", scope: !15, file: !1, line: 10, type: !12)
!17 = !DILocation(line: 0, scope: !15)
!18 = !DILocalVariable(name: "y", scope: !15, file: !1, line: 10, type: !12)
!19 = !DILocation(line: 13, column: 11, scope: !15)
!20 = !DILocation(line: 13, column: 3, scope: !15)
!21 = !DILocation(line: 16, column: 11, scope: !22)
!22 = distinct !DILexicalBlock(scope: !15, file: !1, line: 14, column: 3)
!23 = !DILocation(line: 17, column: 9, scope: !22)
!24 = !DILocation(line: 19, column: 11, scope: !22)
!25 = !DILocation(line: 20, column: 9, scope: !22)
!26 = !DILocation(line: 22, column: 11, scope: !22)
!27 = !DILocation(line: 23, column: 9, scope: !22)
!28 = !DILocation(line: 25, column: 10, scope: !22)
!29 = !DILocation(line: 26, column: 10, scope: !22)
!30 = !DILocation(line: 27, column: 9, scope: !22)
!31 = !DILocation(line: 0, scope: !22)
!32 = !DILocation(line: 29, column: 18, scope: !15)
!33 = !DILocation(line: 29, column: 5, scope: !15)
!34 = !DILocation(line: 30, column: 5, scope: !15)
