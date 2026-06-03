; ModuleID = 'null_deref.c'
source_filename = "null_deref.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 !dbg !13 {
  %1 = alloca i32, align 4
  %2 = alloca ptr, align 8
  store i32 0, ptr %1, align 4
    #dbg_declare(ptr %2, !17, !DIExpression(), !18)
  store ptr null, ptr %2, align 8, !dbg !18
  %3 = load ptr, ptr %2, align 8, !dbg !19
  call void @UNSAFE_LOAD(ptr noundef %3), !dbg !20
  ret i32 0, !dbg !21
}

declare void @UNSAFE_LOAD(ptr noundef) #1

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!5, !6, !7, !8, !9, !10, !11}
!llvm.ident = !{!12}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 4f7056e8ada487923d1c8f9bc38df6472008eda3)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "null_deref.c", directory: "/mnt/scratch/PAG/Wjw/vibe/ass3-template-wt/Assignment-3/Tests", checksumkind: CSK_MD5, checksum: "69381433b6fc6047d3e75f8d46b18ae0")
!2 = !{!3}
!3 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
!4 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!5 = !{i32 7, !"Dwarf Version", i32 5}
!6 = !{i32 2, !"Debug Info Version", i32 3}
!7 = !{i32 1, !"wchar_size", i32 4}
!8 = !{i32 8, !"PIC Level", i32 2}
!9 = !{i32 7, !"PIE Level", i32 2}
!10 = !{i32 7, !"uwtable", i32 2}
!11 = !{i32 7, !"frame-pointer", i32 2}
!12 = !{!"clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 4f7056e8ada487923d1c8f9bc38df6472008eda3)"}
!13 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 3, type: !14, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !16)
!14 = !DISubroutineType(types: !15)
!15 = !{!4}
!16 = !{}
!17 = !DILocalVariable(name: "p", scope: !13, file: !1, line: 4, type: !3)
!18 = !DILocation(line: 4, column: 10, scope: !13)
!19 = !DILocation(line: 5, column: 17, scope: !13)
!20 = !DILocation(line: 5, column: 5, scope: !13)
!21 = !DILocation(line: 6, column: 5, scope: !13)
