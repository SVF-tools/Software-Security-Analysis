; ModuleID = './test1.ll'
source_filename = "./test1.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !9 {
entry:
  %arr = alloca [5 x i32], align 4
  call void @llvm.dbg.declare(metadata ptr %arr, metadata !15, metadata !DIExpression()), !dbg !19
  %arraydecay = getelementptr inbounds [5 x i32], ptr %arr, i64 0, i64 0, !dbg !20
  call void @OVERFLOW(ptr noundef %arraydecay, i32 noundef 20), !dbg !21
  %arrayidx = getelementptr inbounds [5 x i32], ptr %arr, i64 0, i64 5, !dbg !22
  store i32 10, ptr %arrayidx, align 4, !dbg !23
  ret i32 0, !dbg !24
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare void @OVERFLOW(ptr noundef, i32 noundef) #2

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Homebrew clang version 16.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!1 = !DIFile(filename: "test1.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-3/Tests/buf")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 1}
!8 = !{!"Homebrew clang version 16.0.6"}
!9 = distinct !DISubprogram(name: "main", scope: !10, file: !10, line: 2, type: !11, scopeLine: 2, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!10 = !DIFile(filename: "./test1.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-3/Tests/buf")
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "arr", scope: !9, file: !10, line: 3, type: !16)
!16 = !DICompositeType(tag: DW_TAG_array_type, baseType: !13, size: 160, elements: !17)
!17 = !{!18}
!18 = !DISubrange(count: 5)
!19 = !DILocation(line: 3, column: 9, scope: !9)
!20 = !DILocation(line: 4, column: 14, scope: !9)
!21 = !DILocation(line: 4, column: 5, scope: !9)
!22 = !DILocation(line: 5, column: 5, scope: !9)
!23 = !DILocation(line: 5, column: 12, scope: !9)
!24 = !DILocation(line: 6, column: 1, scope: !9)
