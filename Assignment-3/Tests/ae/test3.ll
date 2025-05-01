; ModuleID = './test3.ll'
source_filename = "./test3.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

%struct.A = type { i32, [2 x i32] }

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @getValue(ptr noundef %arr, i32 noundef %x) #0 !dbg !9 {
entry:
  call void @llvm.dbg.value(metadata ptr %arr, metadata !24, metadata !DIExpression()), !dbg !25
  call void @llvm.dbg.value(metadata i32 %x, metadata !26, metadata !DIExpression()), !dbg !25
  %b = getelementptr inbounds %struct.A, ptr %arr, i32 0, i32 1, !dbg !27
  %idxprom = sext i32 %x to i64, !dbg !28
  %arrayidx = getelementptr inbounds [2 x i32], ptr %b, i64 0, i64 %idxprom, !dbg !28
  %0 = load i32, ptr %arrayidx, align 4, !dbg !28
  ret i32 %0, !dbg !29
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !30 {
entry:
  %a = alloca %struct.A, align 4
  call void @llvm.dbg.declare(metadata ptr %a, metadata !33, metadata !DIExpression()), !dbg !34
  %a1 = getelementptr inbounds %struct.A, ptr %a, i32 0, i32 0, !dbg !35
  store i32 0, ptr %a1, align 4, !dbg !36
  %b = getelementptr inbounds %struct.A, ptr %a, i32 0, i32 1, !dbg !37
  %arrayidx = getelementptr inbounds [2 x i32], ptr %b, i64 0, i64 0, !dbg !38
  store i32 1, ptr %arrayidx, align 4, !dbg !39
  %b2 = getelementptr inbounds %struct.A, ptr %a, i32 0, i32 1, !dbg !40
  %arrayidx3 = getelementptr inbounds [2 x i32], ptr %b2, i64 0, i64 1, !dbg !41
  store i32 2, ptr %arrayidx3, align 4, !dbg !42
  %call = call i32 @getValue(ptr noundef %a, i32 noundef 1), !dbg !43
  %cmp = icmp eq i32 %call, 2, !dbg !44
  call void @svf_assert(i1 noundef zeroext %cmp), !dbg !45
  ret i32 0, !dbg !46
}

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
!1 = !DIFile(filename: "test3.c", directory: "/Users/z5489735/2023/0522/Software-Security-Analysis/Assignment-3/Tests/ae")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 1}
!8 = !{!"Homebrew clang version 16.0.6"}
!9 = distinct !DISubprogram(name: "getValue", scope: !10, file: !10, line: 13, type: !11, scopeLine: 13, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !23)
!10 = !DIFile(filename: "./test3.c", directory: "/Users/z5489735/2023/0522/Software-Security-Analysis/Assignment-3/Tests/ae")
!11 = !DISubroutineType(types: !12)
!12 = !{!13, !14, !13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !15, size: 64)
!15 = !DIDerivedType(tag: DW_TAG_typedef, name: "A", file: !10, line: 11, baseType: !16)
!16 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !10, line: 8, size: 96, elements: !17)
!17 = !{!18, !19}
!18 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !16, file: !10, line: 9, baseType: !13, size: 32)
!19 = !DIDerivedType(tag: DW_TAG_member, name: "b", scope: !16, file: !10, line: 10, baseType: !20, size: 64, offset: 32)
!20 = !DICompositeType(tag: DW_TAG_array_type, baseType: !13, size: 64, elements: !21)
!21 = !{!22}
!22 = !DISubrange(count: 2)
!23 = !{}
!24 = !DILocalVariable(name: "arr", arg: 1, scope: !9, file: !10, line: 13, type: !14)
!25 = !DILocation(line: 0, scope: !9)
!26 = !DILocalVariable(name: "x", arg: 2, scope: !9, file: !10, line: 13, type: !13)
!27 = !DILocation(line: 14, column: 17, scope: !9)
!28 = !DILocation(line: 14, column: 12, scope: !9)
!29 = !DILocation(line: 14, column: 5, scope: !9)
!30 = distinct !DISubprogram(name: "main", scope: !10, file: !10, line: 17, type: !31, scopeLine: 17, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !23)
!31 = !DISubroutineType(types: !32)
!32 = !{!13}
!33 = !DILocalVariable(name: "a", scope: !30, file: !10, line: 18, type: !15)
!34 = !DILocation(line: 18, column: 7, scope: !30)
!35 = !DILocation(line: 19, column: 7, scope: !30)
!36 = !DILocation(line: 19, column: 9, scope: !30)
!37 = !DILocation(line: 20, column: 7, scope: !30)
!38 = !DILocation(line: 20, column: 5, scope: !30)
!39 = !DILocation(line: 20, column: 12, scope: !30)
!40 = !DILocation(line: 21, column: 7, scope: !30)
!41 = !DILocation(line: 21, column: 5, scope: !30)
!42 = !DILocation(line: 21, column: 12, scope: !30)
!43 = !DILocation(line: 22, column: 16, scope: !30)
!44 = !DILocation(line: 22, column: 32, scope: !30)
!45 = !DILocation(line: 22, column: 5, scope: !30)
!46 = !DILocation(line: 23, column: 5, scope: !30)
