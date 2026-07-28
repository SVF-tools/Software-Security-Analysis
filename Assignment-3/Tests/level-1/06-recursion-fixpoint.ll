; ModuleID = '06-recursion-fixpoint.c'
source_filename = "06-recursion-fixpoint.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
entry:
  %retval = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  call void @count_down(i32 noundef 3) #2, !dbg !14
  ret i32 0, !dbg !15
}

; Function Attrs: noinline nounwind optnone uwtable
define internal void @count_down(i32 noundef %value) #0 !dbg !16 {
entry:
  %value.addr = alloca i32, align 4
  store i32 %value, ptr %value.addr, align 4
    #dbg_declare(ptr %value.addr, !20, !DIExpression(), !21)
  %0 = load i32, ptr %value.addr, align 4, !dbg !22
  %cmp = icmp sle i32 %0, 0, !dbg !24
  br i1 %cmp, label %if.then, label %if.end, !dbg !24

if.then:                                          ; preds = %entry
  %1 = load i32, ptr %value.addr, align 4, !dbg !25
  %cmp1 = icmp sle i32 %1, 0, !dbg !27
  call void @svf_assert(i1 noundef %cmp1) #2, !dbg !28
  br label %return, !dbg !29

if.end:                                           ; preds = %entry
  %2 = load i32, ptr %value.addr, align 4, !dbg !30
  %sub = sub nsw i32 %2, 1, !dbg !31
  call void @count_down(i32 noundef %sub) #2, !dbg !32
  br label %return, !dbg !33

return:                                           ; preds = %if.end, %if.then
  ret void, !dbg !33
}

declare void @svf_assert(i1 noundef) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #1 = { "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #2 = { nobuiltin "no-builtins" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "06-recursion-fixpoint.c", directory: "/workspace/Assignment-3/Tests/level-1", checksumkind: CSK_MD5, checksum: "5c0f5c29d1fe6224bb1d5e07fb03ac76")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 1}
!9 = !{!"clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 13, type: !11, scopeLine: 13, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !DILocation(line: 14, column: 5, scope: !10)
!15 = !DILocation(line: 15, column: 5, scope: !10)
!16 = distinct !DISubprogram(name: "count_down", scope: !1, file: !1, line: 5, type: !17, scopeLine: 5, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !0, retainedNodes: !19)
!17 = !DISubroutineType(types: !18)
!18 = !{null, !13}
!19 = !{}
!20 = !DILocalVariable(name: "value", arg: 1, scope: !16, file: !1, line: 5, type: !13)
!21 = !DILocation(line: 5, column: 28, scope: !16)
!22 = !DILocation(line: 6, column: 9, scope: !23)
!23 = distinct !DILexicalBlock(scope: !16, file: !1, line: 6, column: 9)
!24 = !DILocation(line: 6, column: 15, scope: !23)
!25 = !DILocation(line: 7, column: 20, scope: !26)
!26 = distinct !DILexicalBlock(scope: !23, file: !1, line: 6, column: 21)
!27 = !DILocation(line: 7, column: 26, scope: !26)
!28 = !DILocation(line: 7, column: 9, scope: !26)
!29 = !DILocation(line: 8, column: 9, scope: !26)
!30 = !DILocation(line: 10, column: 16, scope: !16)
!31 = !DILocation(line: 10, column: 22, scope: !16)
!32 = !DILocation(line: 10, column: 5, scope: !16)
!33 = !DILocation(line: 11, column: 1, scope: !16)
