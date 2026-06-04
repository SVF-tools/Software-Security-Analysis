; ModuleID = 'buf_overflow.c'
source_filename = "buf_overflow.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 !dbg !10 {
  %1 = alloca i32, align 4
  %2 = alloca [4 x i8], align 1
  store i32 0, ptr %1, align 4
    #dbg_declare(ptr %2, !15, !DIExpression(), !20)
  call void @llvm.memset.p0.i64(ptr align 1 %2, i8 0, i64 4, i1 false), !dbg !20
  %3 = getelementptr inbounds [4 x i8], ptr %2, i64 0, i64 0, !dbg !21
  %4 = getelementptr inbounds i8, ptr %3, i64 4, !dbg !22
  call void @UNSAFE_BUFACCESS(ptr noundef %4, i32 noundef 1), !dbg !23
  %5 = getelementptr inbounds [4 x i8], ptr %2, i64 0, i64 4, !dbg !24
  store i8 120, ptr %5, align 1, !dbg !25
  ret i32 0, !dbg !26
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #1

declare void @UNSAFE_BUFACCESS(ptr noundef, i32 noundef) #2

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Homebrew clang version 21.1.8", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "buf_overflow.c", directory: "/Users/z5489735/2023/0603/template-SSA/Assignment-3/Tests", checksumkind: CSK_MD5, checksum: "85ee42df20341852cdffa71c1f7089a7")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"Homebrew clang version 21.1.8"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 3, type: !11, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "buf", scope: !10, file: !1, line: 4, type: !16)
!16 = !DICompositeType(tag: DW_TAG_array_type, baseType: !17, size: 32, elements: !18)
!17 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!18 = !{!19}
!19 = !DISubrange(count: 4)
!20 = !DILocation(line: 4, column: 10, scope: !10)
!21 = !DILocation(line: 5, column: 22, scope: !10)
!22 = !DILocation(line: 5, column: 26, scope: !10)
!23 = !DILocation(line: 5, column: 5, scope: !10)
!24 = !DILocation(line: 6, column: 5, scope: !10)
!25 = !DILocation(line: 6, column: 12, scope: !10)
!26 = !DILocation(line: 7, column: 5, scope: !10)
