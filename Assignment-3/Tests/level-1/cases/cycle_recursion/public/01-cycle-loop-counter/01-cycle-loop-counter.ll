; ModuleID = '01-cycle-loop-counter.c'
source_filename = "01-cycle-loop-counter.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  call void @llvm.dbg.declare(metadata ptr %2, metadata !15, metadata !DIExpression()), !dbg !16
  store i32 0, ptr %2, align 4, !dbg !16
  br label %3, !dbg !17

3:                                                ; preds = %6, %0
  %4 = load i32, ptr %2, align 4, !dbg !18
  %5 = icmp slt i32 %4, 8, !dbg !19
  br i1 %5, label %6, label %9, !dbg !17

6:                                                ; preds = %3
  %7 = load i32, ptr %2, align 4, !dbg !20
  %8 = add nsw i32 %7, 1, !dbg !20
  store i32 %8, ptr %2, align 4, !dbg !20
  br label %3, !dbg !17, !llvm.loop !22

9:                                                ; preds = %3
  %10 = load i32, ptr %2, align 4, !dbg !25
  %11 = icmp sge i32 %10, 8, !dbg !26
  call void @svf_assert(i1 noundef zeroext %11), !dbg !27
  ret i32 0, !dbg !28
}

; Function Attrs: nocallback nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare void @svf_assert(i1 noundef zeroext) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "Debian clang version 15.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "01-cycle-loop-counter.c", directory: ".", checksumkind: CSK_MD5, checksum: "a3c50ec27673ca6a6fcda4e6d6861a52")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"Debian clang version 15.0.6"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 4, type: !11, scopeLine: 4, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "i", scope: !10, file: !1, line: 5, type: !13)
!16 = !DILocation(line: 5, column: 9, scope: !10)
!17 = !DILocation(line: 6, column: 5, scope: !10)
!18 = !DILocation(line: 6, column: 12, scope: !10)
!19 = !DILocation(line: 6, column: 14, scope: !10)
!20 = !DILocation(line: 7, column: 10, scope: !21)
!21 = distinct !DILexicalBlock(scope: !10, file: !1, line: 6, column: 19)
!22 = distinct !{!22, !17, !23, !24}
!23 = !DILocation(line: 8, column: 5, scope: !10)
!24 = !{!"llvm.loop.mustprogress"}
!25 = !DILocation(line: 9, column: 16, scope: !10)
!26 = !DILocation(line: 9, column: 18, scope: !10)
!27 = !DILocation(line: 9, column: 5, scope: !10)
!28 = !DILocation(line: 10, column: 5, scope: !10)
