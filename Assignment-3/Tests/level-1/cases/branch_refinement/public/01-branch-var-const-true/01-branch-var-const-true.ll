; ModuleID = '01-branch-var-const-true.c'
source_filename = "01-branch-var-const-true.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  call void @llvm.dbg.declare(metadata ptr %2, metadata !15, metadata !DIExpression()), !dbg !16
  %3 = call i32 @nd(), !dbg !17
  store i32 %3, ptr %2, align 4, !dbg !16
  %4 = load i32, ptr %2, align 4, !dbg !18
  %5 = icmp slt i32 %4, 10, !dbg !20
  br i1 %5, label %6, label %9, !dbg !21

6:                                                ; preds = %0
  %7 = load i32, ptr %2, align 4, !dbg !22
  %8 = icmp sle i32 %7, 9, !dbg !24
  call void @svf_assert(i1 noundef zeroext %8), !dbg !25
  br label %9, !dbg !26

9:                                                ; preds = %6, %0
  ret i32 0, !dbg !27
}

; Function Attrs: nocallback nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @nd() #2

declare void @svf_assert(i1 noundef zeroext) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "Debian clang version 15.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "01-branch-var-const-true.c", directory: ".", checksumkind: CSK_MD5, checksum: "7f048c755f539011b393aa597133530b")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"Debian clang version 15.0.6"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 5, type: !11, scopeLine: 5, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "x", scope: !10, file: !1, line: 6, type: !13)
!16 = !DILocation(line: 6, column: 9, scope: !10)
!17 = !DILocation(line: 6, column: 13, scope: !10)
!18 = !DILocation(line: 7, column: 9, scope: !19)
!19 = distinct !DILexicalBlock(scope: !10, file: !1, line: 7, column: 9)
!20 = !DILocation(line: 7, column: 11, scope: !19)
!21 = !DILocation(line: 7, column: 9, scope: !10)
!22 = !DILocation(line: 8, column: 20, scope: !23)
!23 = distinct !DILexicalBlock(scope: !19, file: !1, line: 7, column: 17)
!24 = !DILocation(line: 8, column: 22, scope: !23)
!25 = !DILocation(line: 8, column: 9, scope: !23)
!26 = !DILocation(line: 9, column: 5, scope: !23)
!27 = !DILocation(line: 10, column: 5, scope: !10)
