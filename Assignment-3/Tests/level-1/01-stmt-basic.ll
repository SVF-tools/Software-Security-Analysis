; ModuleID = '01-stmt-basic.c'
source_filename = "01-stmt-basic.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
entry:
  %retval = alloca i32, align 4
  %c = alloca i8, align 1
  %wide = alloca i32, align 4
  %result = alloca i32, align 4
  store i32 0, ptr %retval, align 4
    #dbg_declare(ptr %c, !15, !DIExpression(), !17)
  store i8 -1, ptr %c, align 1, !dbg !17
    #dbg_declare(ptr %wide, !18, !DIExpression(), !20)
  %0 = load i8, ptr %c, align 1, !dbg !21
  %conv = zext i8 %0 to i32, !dbg !21
  store i32 %conv, ptr %wide, align 4, !dbg !20
    #dbg_declare(ptr %result, !22, !DIExpression(), !23)
  %1 = load i32, ptr %wide, align 4, !dbg !24
  %add = add i32 %1, 1, !dbg !25
  store i32 %add, ptr %result, align 4, !dbg !23
  %2 = load i32, ptr %result, align 4, !dbg !26
  %cmp = icmp eq i32 %2, 256, !dbg !27
  call void @svf_assert(i1 noundef %cmp), !dbg !28
  ret i32 0, !dbg !29
}

declare void @svf_assert(i1 noundef) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "01-stmt-basic.c", directory: ".", checksumkind: CSK_MD5, checksum: "0e7bce636e687d93d521ebd287aa94db")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 1}
!9 = !{!"clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)"}
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
