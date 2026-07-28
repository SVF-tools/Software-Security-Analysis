; ModuleID = '03-buffer-overflow.c'
source_filename = "03-buffer-overflow.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !12 {
entry:
  %retval = alloca i32, align 4
  %buf = alloca [1 x i8], align 1
  %unsafe = alloca ptr, align 8
  %observed = alloca i8, align 1
  store i32 0, ptr %retval, align 4
    #dbg_declare(ptr %buf, !17, !DIExpression(), !22)
  call void @llvm.memset.p0.i64(ptr align 1 %buf, i8 0, i64 1, i1 false), !dbg !22
    #dbg_declare(ptr %unsafe, !23, !DIExpression(), !26)
  %arrayidx = getelementptr inbounds [1 x i8], ptr %buf, i64 0, i64 1, !dbg !27
  store ptr %arrayidx, ptr %unsafe, align 8, !dbg !26
    #dbg_declare(ptr %observed, !28, !DIExpression(), !29)
  %0 = load ptr, ptr %unsafe, align 8, !dbg !30
  %1 = load volatile i8, ptr %0, align 1, !dbg !31
  store volatile i8 %1, ptr %observed, align 1, !dbg !29
  %2 = load volatile i8, ptr %observed, align 1, !dbg !32
  %3 = load ptr, ptr %unsafe, align 8, !dbg !33
  call void @UNSAFE_BUFACCESS(ptr noundef %3, i64 noundef 1) #3, !dbg !34
  ret i32 0, !dbg !35
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #1

declare void @UNSAFE_BUFACCESS(ptr noundef, i64 noundef) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #2 = { "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #3 = { nobuiltin "no-builtins" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!4, !5, !6, !7, !8, !9, !10}
!llvm.ident = !{!11}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "03-buffer-overflow.c", directory: "/workspace/Assignment-3/Tests/level-1", checksumkind: CSK_MD5, checksum: "da36a99d6f2e7e8f3d7c061ced18a972")
!2 = !{!3}
!3 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!4 = !{i32 7, !"Dwarf Version", i32 5}
!5 = !{i32 2, !"Debug Info Version", i32 3}
!6 = !{i32 1, !"wchar_size", i32 4}
!7 = !{i32 8, !"PIC Level", i32 2}
!8 = !{i32 7, !"PIE Level", i32 2}
!9 = !{i32 7, !"uwtable", i32 2}
!10 = !{i32 7, !"frame-pointer", i32 1}
!11 = !{!"clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)"}
!12 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 3, type: !13, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !16)
!13 = !DISubroutineType(types: !14)
!14 = !{!15}
!15 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!16 = !{}
!17 = !DILocalVariable(name: "buf", scope: !12, file: !1, line: 4, type: !18)
!18 = !DICompositeType(tag: DW_TAG_array_type, baseType: !19, size: 8, elements: !20)
!19 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_unsigned_char)
!20 = !{!21}
!21 = !DISubrange(count: 1)
!22 = !DILocation(line: 4, column: 10, scope: !12)
!23 = !DILocalVariable(name: "unsafe", scope: !12, file: !1, line: 5, type: !24)
!24 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !25, size: 64)
!25 = !DIDerivedType(tag: DW_TAG_volatile_type, baseType: !19)
!26 = !DILocation(line: 5, column: 20, scope: !12)
!27 = !DILocation(line: 5, column: 30, scope: !12)
!28 = !DILocalVariable(name: "observed", scope: !12, file: !1, line: 6, type: !25)
!29 = !DILocation(line: 6, column: 19, scope: !12)
!30 = !DILocation(line: 6, column: 31, scope: !12)
!31 = !DILocation(line: 6, column: 30, scope: !12)
!32 = !DILocation(line: 7, column: 11, scope: !12)
!33 = !DILocation(line: 8, column: 30, scope: !12)
!34 = !DILocation(line: 8, column: 5, scope: !12)
!35 = !DILocation(line: 9, column: 5, scope: !12)
