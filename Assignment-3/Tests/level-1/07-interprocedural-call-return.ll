; ModuleID = '07-interprocedural-call-return.c'
source_filename = "07-interprocedural-call-return.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
entry:
  %retval = alloca i32, align 4
  %result = alloca i32, align 4
  store i32 0, ptr %retval, align 4
    #dbg_declare(ptr %result, !15, !DIExpression(), !16)
  %call = call i32 @add_one(i32 noundef 41) #2, !dbg !17
  store i32 %call, ptr %result, align 4, !dbg !16
  %0 = load i32, ptr %result, align 4, !dbg !18
  %cmp = icmp eq i32 %0, 42, !dbg !19
  call void @svf_assert(i1 noundef %cmp) #2, !dbg !20
  ret i32 0, !dbg !21
}

; Function Attrs: noinline nounwind optnone uwtable
define internal i32 @add_one(i32 noundef %value) #0 !dbg !22 {
entry:
  %value.addr = alloca i32, align 4
  store i32 %value, ptr %value.addr, align 4
    #dbg_declare(ptr %value.addr, !25, !DIExpression(), !26)
  %0 = load i32, ptr %value.addr, align 4, !dbg !27
  %add = add nsw i32 %0, 1, !dbg !28
  ret i32 %add, !dbg !29
}

declare void @svf_assert(i1 noundef) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #1 = { "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #2 = { nobuiltin "no-builtins" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "07-interprocedural-call-return.c", directory: "/workspace/Assignment-3/Tests/level-1", checksumkind: CSK_MD5, checksum: "0ef343c6dc46b77255fecb23f4877a4b")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 1}
!9 = !{!"clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 9, type: !11, scopeLine: 9, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "result", scope: !10, file: !1, line: 10, type: !13)
!16 = !DILocation(line: 10, column: 9, scope: !10)
!17 = !DILocation(line: 10, column: 18, scope: !10)
!18 = !DILocation(line: 11, column: 16, scope: !10)
!19 = !DILocation(line: 11, column: 23, scope: !10)
!20 = !DILocation(line: 11, column: 5, scope: !10)
!21 = !DILocation(line: 12, column: 5, scope: !10)
!22 = distinct !DISubprogram(name: "add_one", scope: !1, file: !1, line: 5, type: !23, scopeLine: 5, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !0, retainedNodes: !14)
!23 = !DISubroutineType(types: !24)
!24 = !{!13, !13}
!25 = !DILocalVariable(name: "value", arg: 1, scope: !22, file: !1, line: 5, type: !13)
!26 = !DILocation(line: 5, column: 24, scope: !22)
!27 = !DILocation(line: 6, column: 12, scope: !22)
!28 = !DILocation(line: 6, column: 18, scope: !22)
!29 = !DILocation(line: 6, column: 5, scope: !22)
