; ModuleID = '03-buffer-overflow.c'
source_filename = "03-buffer-overflow.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
entry:
  %retval = alloca i32, align 4
  %buf = alloca [1 x i8], align 1
  store i32 0, ptr %retval, align 4
    #dbg_declare(ptr %buf, !15, !DIExpression(), !20)
  call void @llvm.memset.p0.i64(ptr align 1 %buf, i8 0, i64 1, i1 false), !dbg !20
  %arrayidx = getelementptr inbounds [1 x i8], ptr %buf, i64 0, i64 1, !dbg !21
  call void @UNSAFE_BUFACCESS(ptr noundef %arrayidx, i64 noundef 1), !dbg !22
  ret i32 0, !dbg !23
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #1

declare void @UNSAFE_BUFACCESS(ptr noundef, i64 noundef) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "03-buffer-overflow.c", directory: ".", checksumkind: CSK_MD5, checksum: "9aaf7853a32fe73b43d66fde4884f191")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 1}
!9 = !{!"clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 3, type: !11, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "buf", scope: !10, file: !1, line: 4, type: !16)
!16 = !DICompositeType(tag: DW_TAG_array_type, baseType: !17, size: 8, elements: !18)
!17 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_unsigned_char)
!18 = !{!19}
!19 = !DISubrange(count: 1)
!20 = !DILocation(line: 4, column: 10, scope: !10)
!21 = !DILocation(line: 5, column: 23, scope: !10)
!22 = !DILocation(line: 5, column: 5, scope: !10)
!23 = !DILocation(line: 6, column: 5, scope: !10)
