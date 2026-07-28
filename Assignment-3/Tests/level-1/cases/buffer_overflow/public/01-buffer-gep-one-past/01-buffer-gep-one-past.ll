; ModuleID = '01-buffer-gep-one-past.c'
source_filename = "01-buffer-gep-one-past.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
  %1 = alloca i32, align 4
  %2 = alloca [4 x i8], align 1
  %3 = alloca ptr, align 8
  store i32 0, ptr %1, align 4
  call void @llvm.dbg.declare(metadata ptr %2, metadata !15, metadata !DIExpression()), !dbg !20
  call void @llvm.memset.p0.i64(ptr align 1 %2, i8 0, i64 4, i1 false), !dbg !20
  call void @llvm.dbg.declare(metadata ptr %3, metadata !21, metadata !DIExpression()), !dbg !23
  %4 = getelementptr inbounds [4 x i8], ptr %2, i64 0, i64 4, !dbg !24
  store ptr %4, ptr %3, align 8, !dbg !23
  %5 = load ptr, ptr %3, align 8, !dbg !25
  call void @UNSAFE_BUFACCESS(ptr noundef %5, i64 noundef 1), !dbg !26
  ret i32 0, !dbg !27
}

; Function Attrs: nocallback nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: argmemonly nocallback nofree nounwind willreturn writeonly
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

declare void @UNSAFE_BUFACCESS(ptr noundef, i64 noundef) #3

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { argmemonly nocallback nofree nounwind willreturn writeonly }
attributes #3 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "Debian clang version 15.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "01-buffer-gep-one-past.c", directory: ".", checksumkind: CSK_MD5, checksum: "7be16a8c9da1a752d57f5ebab88111ff")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"Debian clang version 15.0.6"}
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
!21 = !DILocalVariable(name: "q", scope: !10, file: !1, line: 5, type: !22)
!22 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !17, size: 64)
!23 = !DILocation(line: 5, column: 11, scope: !10)
!24 = !DILocation(line: 5, column: 16, scope: !10)
!25 = !DILocation(line: 6, column: 22, scope: !10)
!26 = !DILocation(line: 6, column: 5, scope: !10)
!27 = !DILocation(line: 7, column: 5, scope: !10)
