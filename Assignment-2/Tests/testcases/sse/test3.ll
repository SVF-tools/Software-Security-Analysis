; ModuleID = './test3.ll'
source_filename = "./test3.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @test(i32 noundef %a, i32 noundef %b) #0 !dbg !9 {
entry:
  call void @llvm.dbg.value(metadata i32 %a, metadata !15, metadata !DIExpression()), !dbg !16
  call void @llvm.dbg.value(metadata i32 %b, metadata !17, metadata !DIExpression()), !dbg !16
  call void @llvm.dbg.value(metadata i32 1, metadata !18, metadata !DIExpression()), !dbg !16
  call void @llvm.dbg.value(metadata i32 1, metadata !19, metadata !DIExpression()), !dbg !16
  %cmp = icmp sgt i32 %a, %b, !dbg !20
  br i1 %cmp, label %if.then, label %if.else, !dbg !22

if.then:                                          ; preds = %entry
  %inc = add nsw i32 1, 1, !dbg !23
  call void @llvm.dbg.value(metadata i32 %inc, metadata !18, metadata !DIExpression()), !dbg !16
  %inc1 = add nsw i32 1, 1, !dbg !25
  call void @llvm.dbg.value(metadata i32 %inc1, metadata !19, metadata !DIExpression()), !dbg !16
  %cmp2 = icmp eq i32 %inc, %inc1, !dbg !26
  call void @svf_assert(i1 noundef zeroext %cmp2), !dbg !27
  br label %if.end, !dbg !28

if.else:                                          ; preds = %entry
  %inc3 = add nsw i32 1, 1, !dbg !29
  call void @llvm.dbg.value(metadata i32 %inc3, metadata !18, metadata !DIExpression()), !dbg !16
  %cmp4 = icmp eq i32 %inc3, 2, !dbg !31
  call void @svf_assert(i1 noundef zeroext %cmp4), !dbg !32
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret i32 0, !dbg !33
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare void @svf_assert(i1 noundef zeroext) #2

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !34 {
entry:
  call void @llvm.dbg.value(metadata i32 1, metadata !37, metadata !DIExpression()), !dbg !38
  call void @llvm.dbg.value(metadata i32 2, metadata !39, metadata !DIExpression()), !dbg !38
  %call = call i32 @test(i32 noundef 1, i32 noundef 2), !dbg !40
  ret i32 0, !dbg !41
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Homebrew clang version 16.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!1 = !DIFile(filename: "test3.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-2/Tests/testcases/sse")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 1}
!8 = !{!"Homebrew clang version 16.0.6"}
!9 = distinct !DISubprogram(name: "test", scope: !10, file: !10, line: 8, type: !11, scopeLine: 8, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!10 = !DIFile(filename: "./test3.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-2/Tests/testcases/sse")
!11 = !DISubroutineType(types: !12)
!12 = !{!13, !13, !13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "a", arg: 1, scope: !9, file: !10, line: 8, type: !13)
!16 = !DILocation(line: 0, scope: !9)
!17 = !DILocalVariable(name: "b", arg: 2, scope: !9, file: !10, line: 8, type: !13)
!18 = !DILocalVariable(name: "x", scope: !9, file: !10, line: 9, type: !13)
!19 = !DILocalVariable(name: "y", scope: !9, file: !10, line: 9, type: !13)
!20 = !DILocation(line: 12, column: 11, scope: !21)
!21 = distinct !DILexicalBlock(scope: !9, file: !10, line: 12, column: 9)
!22 = !DILocation(line: 12, column: 9, scope: !9)
!23 = !DILocation(line: 13, column: 10, scope: !24)
!24 = distinct !DILexicalBlock(scope: !21, file: !10, line: 12, column: 16)
!25 = !DILocation(line: 14, column: 10, scope: !24)
!26 = !DILocation(line: 15, column: 23, scope: !24)
!27 = !DILocation(line: 15, column: 9, scope: !24)
!28 = !DILocation(line: 16, column: 5, scope: !24)
!29 = !DILocation(line: 17, column: 10, scope: !30)
!30 = distinct !DILexicalBlock(scope: !21, file: !10, line: 16, column: 12)
!31 = !DILocation(line: 18, column: 23, scope: !30)
!32 = !DILocation(line: 18, column: 9, scope: !30)
!33 = !DILocation(line: 20, column: 5, scope: !9)
!34 = distinct !DISubprogram(name: "main", scope: !10, file: !10, line: 23, type: !35, scopeLine: 23, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!35 = !DISubroutineType(types: !36)
!36 = !{!13}
!37 = !DILocalVariable(name: "a", scope: !34, file: !10, line: 24, type: !13)
!38 = !DILocation(line: 0, scope: !34)
!39 = !DILocalVariable(name: "b", scope: !34, file: !10, line: 25, type: !13)
!40 = !DILocation(line: 26, column: 5, scope: !34)
!41 = !DILocation(line: 27, column: 5, scope: !34)
