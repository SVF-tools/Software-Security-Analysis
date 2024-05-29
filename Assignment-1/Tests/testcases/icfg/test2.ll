; ModuleID = './test2.ll'
source_filename = "./test2.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @source(ptr noundef %s) #0 !dbg !9 {
entry:
  call void @llvm.dbg.value(metadata ptr %s, metadata !16, metadata !DIExpression()), !dbg !17
  call void @sink(), !dbg !18
  ret void, !dbg !19
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @sink() #0 !dbg !20 {
entry:
  ret void, !dbg !23
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !24 {
entry:
  %a = alloca i32, align 4
  call void @llvm.dbg.declare(metadata ptr %a, metadata !27, metadata !DIExpression()), !dbg !28
  store i32 1, ptr %a, align 4, !dbg !28
  br label %while.cond, !dbg !29

while.cond:                                       ; preds = %while.body, %entry
  %0 = load i32, ptr %a, align 4, !dbg !30
  %cmp = icmp sle i32 %0, 1, !dbg !31
  br i1 %cmp, label %while.body, label %while.end, !dbg !29

while.body:                                       ; preds = %while.cond
  call void @source(ptr noundef %a), !dbg !32
  %1 = load i32, ptr %a, align 4, !dbg !34
  %inc = add nsw i32 %1, 1, !dbg !34
  store i32 %inc, ptr %a, align 4, !dbg !34
  br label %while.cond, !dbg !29, !llvm.loop !35

while.end:                                        ; preds = %while.cond
  ret i32 0, !dbg !38
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Homebrew clang version 16.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!1 = !DIFile(filename: "test2.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-1/Tests/testcases/icfg")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 1}
!8 = !{!"Homebrew clang version 16.0.6"}
!9 = distinct !DISubprogram(name: "source", scope: !10, file: !10, line: 3, type: !11, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!10 = !DIFile(filename: "./test2.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-1/Tests/testcases/icfg")
!11 = !DISubroutineType(types: !12)
!12 = !{null, !13}
!13 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
!14 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!15 = !{}
!16 = !DILocalVariable(name: "s", arg: 1, scope: !9, file: !10, line: 3, type: !13)
!17 = !DILocation(line: 0, scope: !9)
!18 = !DILocation(line: 4, column: 5, scope: !9)
!19 = !DILocation(line: 5, column: 1, scope: !9)
!20 = distinct !DISubprogram(name: "sink", scope: !10, file: !10, line: 6, type: !21, scopeLine: 6, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!21 = !DISubroutineType(types: !22)
!22 = !{null}
!23 = !DILocation(line: 7, column: 1, scope: !20)
!24 = distinct !DISubprogram(name: "main", scope: !10, file: !10, line: 9, type: !25, scopeLine: 9, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!25 = !DISubroutineType(types: !26)
!26 = !{!14}
!27 = !DILocalVariable(name: "a", scope: !24, file: !10, line: 10, type: !14)
!28 = !DILocation(line: 10, column: 9, scope: !24)
!29 = !DILocation(line: 11, column: 5, scope: !24)
!30 = !DILocation(line: 11, column: 13, scope: !24)
!31 = !DILocation(line: 11, column: 15, scope: !24)
!32 = !DILocation(line: 12, column: 9, scope: !33)
!33 = distinct !DILexicalBlock(scope: !24, file: !10, line: 11, column: 19)
!34 = !DILocation(line: 13, column: 10, scope: !33)
!35 = distinct !{!35, !29, !36, !37}
!36 = !DILocation(line: 14, column: 5, scope: !24)
!37 = !{!"llvm.loop.mustprogress"}
!38 = !DILocation(line: 15, column: 5, scope: !24)
