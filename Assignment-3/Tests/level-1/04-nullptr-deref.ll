; ModuleID = '04-nullptr-deref.c'
source_filename = "04-nullptr-deref.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !15 {
entry:
  %retval = alloca i32, align 4
  %ptr = alloca ptr, align 8
  %observed = alloca i32, align 4
  store i32 0, ptr %retval, align 4
    #dbg_declare(ptr %ptr, !19, !DIExpression(), !20)
  store ptr null, ptr %ptr, align 8, !dbg !20
    #dbg_declare(ptr %observed, !21, !DIExpression(), !22)
  %0 = load ptr, ptr %ptr, align 8, !dbg !23
  %1 = load volatile i32, ptr %0, align 4, !dbg !24
  store volatile i32 %1, ptr %observed, align 4, !dbg !22
  %2 = load volatile i32, ptr %observed, align 4, !dbg !25
  %3 = load ptr, ptr %ptr, align 8, !dbg !26
  call void @UNSAFE_PTRDEREF(ptr noundef %3) #2, !dbg !27
  ret i32 0, !dbg !28
}

declare void @UNSAFE_PTRDEREF(ptr noundef) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #1 = { "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #2 = { nobuiltin "no-builtins" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!7, !8, !9, !10, !11, !12, !13}
!llvm.ident = !{!14}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "04-nullptr-deref.c", directory: "/workspace/Assignment-3/Tests/level-1", checksumkind: CSK_MD5, checksum: "029a96a2ae8851f5ad370ed7ecc944d0")
!2 = !{!3, !6}
!3 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
!4 = !DIDerivedType(tag: DW_TAG_volatile_type, baseType: !5)
!5 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!6 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!7 = !{i32 7, !"Dwarf Version", i32 5}
!8 = !{i32 2, !"Debug Info Version", i32 3}
!9 = !{i32 1, !"wchar_size", i32 4}
!10 = !{i32 8, !"PIC Level", i32 2}
!11 = !{i32 7, !"PIE Level", i32 2}
!12 = !{i32 7, !"uwtable", i32 2}
!13 = !{i32 7, !"frame-pointer", i32 1}
!14 = !{!"clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)"}
!15 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 3, type: !16, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !18)
!16 = !DISubroutineType(types: !17)
!17 = !{!5}
!18 = !{}
!19 = !DILocalVariable(name: "ptr", scope: !15, file: !1, line: 4, type: !3)
!20 = !DILocation(line: 4, column: 19, scope: !15)
!21 = !DILocalVariable(name: "observed", scope: !15, file: !1, line: 5, type: !4)
!22 = !DILocation(line: 5, column: 18, scope: !15)
!23 = !DILocation(line: 5, column: 30, scope: !15)
!24 = !DILocation(line: 5, column: 29, scope: !15)
!25 = !DILocation(line: 6, column: 11, scope: !15)
!26 = !DILocation(line: 7, column: 29, scope: !15)
!27 = !DILocation(line: 7, column: 5, scope: !15)
!28 = !DILocation(line: 8, column: 5, scope: !15)
