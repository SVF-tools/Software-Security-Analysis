; ModuleID = './test1.ll'
source_filename = "./test1.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @bar(i32 noundef %s) #0 !dbg !9 {
entry:
  call void @llvm.dbg.value(metadata i32 %s, metadata !15, metadata !DIExpression()), !dbg !16
  ret i32 %s, !dbg !17
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !18 {
entry:
  %call = call i32 @source(), !dbg !21
  call void @llvm.dbg.value(metadata i32 %call, metadata !22, metadata !DIExpression()), !dbg !23
  %cmp = icmp sgt i32 %call, 0, !dbg !24
  br i1 %cmp, label %if.then, label %if.else, !dbg !26

if.then:                                          ; preds = %entry
  %call1 = call i32 @bar(i32 noundef %call), !dbg !27
  call void @llvm.dbg.value(metadata i32 %call1, metadata !29, metadata !DIExpression()), !dbg !30
  call void @sink(i32 noundef %call1), !dbg !31
  br label %if.end, !dbg !32

if.else:                                          ; preds = %entry
  %call2 = call i32 @bar(i32 noundef %call), !dbg !33
  call void @llvm.dbg.value(metadata i32 %call2, metadata !35, metadata !DIExpression()), !dbg !36
  call void @sink(i32 noundef %call2), !dbg !37
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret i32 0, !dbg !38
}

declare i32 @source(...) #2

declare void @sink(i32 noundef) #2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Homebrew clang version 16.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!1 = !DIFile(filename: "test1.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-1/Tests/testcases/icfg")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 1}
!8 = !{!"Homebrew clang version 16.0.6"}
!9 = distinct !DISubprogram(name: "bar", scope: !10, file: !10, line: 3, type: !11, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!10 = !DIFile(filename: "./test1.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-1/Tests/testcases/icfg")
!11 = !DISubroutineType(types: !12)
!12 = !{!13, !13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "s", arg: 1, scope: !9, file: !10, line: 3, type: !13)
!16 = !DILocation(line: 0, scope: !9)
!17 = !DILocation(line: 4, column: 5, scope: !9)
!18 = distinct !DISubprogram(name: "main", scope: !10, file: !10, line: 6, type: !19, scopeLine: 6, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!19 = !DISubroutineType(types: !20)
!20 = !{!13}
!21 = !DILocation(line: 7, column: 13, scope: !18)
!22 = !DILocalVariable(name: "a", scope: !18, file: !10, line: 7, type: !13)
!23 = !DILocation(line: 0, scope: !18)
!24 = !DILocation(line: 8, column: 11, scope: !25)
!25 = distinct !DILexicalBlock(scope: !18, file: !10, line: 8, column: 9)
!26 = !DILocation(line: 8, column: 9, scope: !18)
!27 = !DILocation(line: 9, column: 17, scope: !28)
!28 = distinct !DILexicalBlock(scope: !25, file: !10, line: 8, column: 15)
!29 = !DILocalVariable(name: "p", scope: !28, file: !10, line: 9, type: !13)
!30 = !DILocation(line: 0, scope: !28)
!31 = !DILocation(line: 10, column: 9, scope: !28)
!32 = !DILocation(line: 11, column: 5, scope: !28)
!33 = !DILocation(line: 12, column: 17, scope: !34)
!34 = distinct !DILexicalBlock(scope: !25, file: !10, line: 11, column: 10)
!35 = !DILocalVariable(name: "q", scope: !34, file: !10, line: 12, type: !13)
!36 = !DILocation(line: 0, scope: !34)
!37 = !DILocation(line: 13, column: 9, scope: !34)
!38 = !DILocation(line: 15, column: 1, scope: !18)
