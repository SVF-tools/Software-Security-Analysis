; ModuleID = '02-branch-feasible.c'
source_filename = "02-branch-feasible.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
entry:
  %retval = alloca i32, align 4
  %x = alloca i32, align 4
  store i32 0, ptr %retval, align 4
    #dbg_declare(ptr %x, !15, !DIExpression(), !16)
  %call = call i32 @nd(), !dbg !17
  store i32 %call, ptr %x, align 4, !dbg !16
  %0 = load i32, ptr %x, align 4, !dbg !18
  %cmp = icmp slt i32 %0, 10, !dbg !20
  br i1 %cmp, label %if.then, label %if.end, !dbg !20

if.then:                                          ; preds = %entry
  %1 = load i32, ptr %x, align 4, !dbg !21
  %cmp1 = icmp sle i32 %1, 9, !dbg !23
  call void @svf_assert(i1 noundef %cmp1), !dbg !24
  br label %if.end, !dbg !25

if.end:                                           ; preds = %if.then, %entry
  ret i32 0, !dbg !26
}

declare i32 @nd() #1

declare void @svf_assert(i1 noundef) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "02-branch-feasible.c", directory: ".", checksumkind: CSK_MD5, checksum: "7f048c755f539011b393aa597133530b")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 1}
!9 = !{!"clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)"}
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
!21 = !DILocation(line: 8, column: 20, scope: !22)
!22 = distinct !DILexicalBlock(scope: !19, file: !1, line: 7, column: 17)
!23 = !DILocation(line: 8, column: 22, scope: !22)
!24 = !DILocation(line: 8, column: 9, scope: !22)
!25 = !DILocation(line: 9, column: 5, scope: !22)
!26 = !DILocation(line: 10, column: 5, scope: !10)
