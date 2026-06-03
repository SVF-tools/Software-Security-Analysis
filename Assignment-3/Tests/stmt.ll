; ModuleID = 'stmt.c'
source_filename = "stmt.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 !dbg !10 {
  %1 = alloca i32, align 4
  %2 = alloca i8, align 1
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 0, ptr %1, align 4
    #dbg_declare(ptr %2, !15, !DIExpression(), !17)
  store i8 -1, ptr %2, align 1, !dbg !17
    #dbg_declare(ptr %3, !18, !DIExpression(), !20)
  %5 = load i8, ptr %2, align 1, !dbg !21
  %6 = zext i8 %5 to i32, !dbg !21
  store i32 %6, ptr %3, align 4, !dbg !20
    #dbg_declare(ptr %4, !22, !DIExpression(), !23)
  %7 = load i32, ptr %3, align 4, !dbg !24
  %8 = add i32 %7, 1, !dbg !25
  store i32 %8, ptr %4, align 4, !dbg !23
  %9 = load i32, ptr %4, align 4, !dbg !26
  %10 = icmp eq i32 %9, 256, !dbg !27
  call void @svf_assert(i1 noundef zeroext %10), !dbg !28
  ret i32 0, !dbg !29
}

declare void @svf_assert(i1 noundef zeroext) #1

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 4f7056e8ada487923d1c8f9bc38df6472008eda3)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "stmt.c", directory: "/mnt/scratch/PAG/Wjw/vibe/ass3-template-wt/Assignment-3/Tests", checksumkind: CSK_MD5, checksum: "0e7bce636e687d93d521ebd287aa94db")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 4f7056e8ada487923d1c8f9bc38df6472008eda3)"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 4, type: !11, scopeLine: 4, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "c", scope: !10, file: !1, line: 5, type: !16)
!16 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!17 = !DILocation(line: 5, column: 19, scope: !10)
!18 = !DILocalVariable(name: "wide", scope: !10, file: !1, line: 6, type: !19)
!19 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!20 = !DILocation(line: 6, column: 18, scope: !10)
!21 = !DILocation(line: 6, column: 25, scope: !10)
!22 = !DILocalVariable(name: "result", scope: !10, file: !1, line: 7, type: !19)
!23 = !DILocation(line: 7, column: 18, scope: !10)
!24 = !DILocation(line: 7, column: 27, scope: !10)
!25 = !DILocation(line: 7, column: 32, scope: !10)
!26 = !DILocation(line: 8, column: 16, scope: !10)
!27 = !DILocation(line: 8, column: 23, scope: !10)
!28 = !DILocation(line: 8, column: 5, scope: !10)
!29 = !DILocation(line: 9, column: 5, scope: !10)
