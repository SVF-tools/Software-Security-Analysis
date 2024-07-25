; ModuleID = './test3.ll'
source_filename = "test3.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @CWE121_Stack_Based_Buffer_Overflow__CWE129_rand_01_bad() #0 !dbg !9 {
entry:
  %buffer = alloca [10 x i32], align 4
  call void @llvm.dbg.value(metadata i32 -1, metadata !13, metadata !DIExpression()), !dbg !15
  call void @llvm.dbg.value(metadata i32 99, metadata !13, metadata !DIExpression()), !dbg !15
  call void @llvm.dbg.declare(metadata ptr undef, metadata !16, metadata !DIExpression()), !dbg !17
  call void @llvm.dbg.declare(metadata ptr %buffer, metadata !18, metadata !DIExpression()), !dbg !22
  call void @llvm.memset.p0.i64(ptr align 4 %buffer, i8 0, i64 40, i1 false), !dbg !22
  %cmp = icmp sge i32 99, 0, !dbg !23
  br i1 %cmp, label %if.then, label %if.end, !dbg !25

if.then:                                          ; preds = %entry
  %arraydecay = getelementptr inbounds [10 x i32], ptr %buffer, i64 0, i64 0, !dbg !26
  call void @OVERFLOW(ptr noundef %arraydecay, i32 noundef 396), !dbg !28
  %idxprom = sext i32 99 to i64, !dbg !29
  %arrayidx = getelementptr inbounds [10 x i32], ptr %buffer, i64 0, i64 %idxprom, !dbg !29
  store i32 1, ptr %arrayidx, align 4, !dbg !30
  br label %if.end, !dbg !31

if.end:                                           ; preds = %if.then, %entry
  ret void, !dbg !32
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

declare void @OVERFLOW(ptr noundef, i32 noundef) #3

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !33 {
entry:
  call void @CWE121_Stack_Based_Buffer_Overflow__CWE129_rand_01_bad(), !dbg !36
  ret i32 0, !dbg !37
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #3 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Homebrew clang version 16.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!1 = !DIFile(filename: "test3.c", directory: "/Users/z5489735/2023/0617/solution/Software-Security-Analysis/Assignment-3/Tests/buf")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 1}
!8 = !{!"Homebrew clang version 16.0.6"}
!9 = distinct !DISubprogram(name: "CWE121_Stack_Based_Buffer_Overflow__CWE129_rand_01_bad", scope: !1, file: !1, line: 7, type: !10, scopeLine: 7, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !12)
!10 = !DISubroutineType(types: !11)
!11 = !{null}
!12 = !{}
!13 = !DILocalVariable(name: "data", scope: !9, file: !1, line: 8, type: !14)
!14 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!15 = !DILocation(line: 0, scope: !9)
!16 = !DILocalVariable(name: "i", scope: !9, file: !1, line: 14, type: !14)
!17 = !DILocation(line: 14, column: 9, scope: !9)
!18 = !DILocalVariable(name: "buffer", scope: !9, file: !1, line: 15, type: !19)
!19 = !DICompositeType(tag: DW_TAG_array_type, baseType: !14, size: 320, elements: !20)
!20 = !{!21}
!21 = !DISubrange(count: 10)
!22 = !DILocation(line: 15, column: 9, scope: !9)
!23 = !DILocation(line: 18, column: 14, scope: !24)
!24 = distinct !DILexicalBlock(scope: !9, file: !1, line: 18, column: 9)
!25 = !DILocation(line: 18, column: 9, scope: !9)
!26 = !DILocation(line: 20, column: 18, scope: !27)
!27 = distinct !DILexicalBlock(scope: !24, file: !1, line: 19, column: 5)
!28 = !DILocation(line: 20, column: 9, scope: !27)
!29 = !DILocation(line: 21, column: 9, scope: !27)
!30 = !DILocation(line: 21, column: 22, scope: !27)
!31 = !DILocation(line: 22, column: 5, scope: !27)
!32 = !DILocation(line: 23, column: 1, scope: !9)
!33 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 25, type: !34, scopeLine: 25, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !12)
!34 = !DISubroutineType(types: !35)
!35 = !{!14}
!36 = !DILocation(line: 26, column: 5, scope: !33)
!37 = !DILocation(line: 27, column: 5, scope: !33)
