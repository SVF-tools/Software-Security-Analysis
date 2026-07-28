; ModuleID = '01-extapi-memcpy-propagates.c'
source_filename = "01-extapi-memcpy-propagates.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@__const.main.src = private unnamed_addr constant [4 x i8] c"abc\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
  %1 = alloca i32, align 4
  %2 = alloca [4 x i8], align 1
  %3 = alloca [4 x i8], align 1
  store i32 0, ptr %1, align 4
  call void @llvm.dbg.declare(metadata ptr %2, metadata !15, metadata !DIExpression()), !dbg !20
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %2, ptr align 1 @__const.main.src, i64 4, i1 false), !dbg !20
  call void @llvm.dbg.declare(metadata ptr %3, metadata !21, metadata !DIExpression()), !dbg !22
  call void @llvm.memset.p0.i64(ptr align 1 %3, i8 0, i64 4, i1 false), !dbg !22
  %4 = getelementptr inbounds [4 x i8], ptr %3, i64 0, i64 0, !dbg !23
  %5 = getelementptr inbounds [4 x i8], ptr %2, i64 0, i64 0, !dbg !23
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %4, ptr align 1 %5, i64 4, i1 false), !dbg !23
  %6 = getelementptr inbounds [4 x i8], ptr %3, i64 0, i64 2, !dbg !24
  %7 = load i8, ptr %6, align 1, !dbg !24
  %8 = sext i8 %7 to i32, !dbg !24
  %9 = icmp eq i32 %8, 99, !dbg !25
  call void @svf_assert(i1 noundef zeroext %9), !dbg !26
  ret i32 0, !dbg !27
}

; Function Attrs: nocallback nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: argmemonly nocallback nofree nounwind willreturn
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #2

; Function Attrs: argmemonly nocallback nofree nounwind willreturn writeonly
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #3

declare void @svf_assert(i1 noundef zeroext) #4

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { argmemonly nocallback nofree nounwind willreturn }
attributes #3 = { argmemonly nocallback nofree nounwind willreturn writeonly }
attributes #4 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "Debian clang version 15.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "01-extapi-memcpy-propagates.c", directory: ".", checksumkind: CSK_MD5, checksum: "766ea68e69bb2dabadfa1495302fb6cc")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"Debian clang version 15.0.6"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 5, type: !11, scopeLine: 5, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "src", scope: !10, file: !1, line: 6, type: !16)
!16 = !DICompositeType(tag: DW_TAG_array_type, baseType: !17, size: 32, elements: !18)
!17 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!18 = !{!19}
!19 = !DISubrange(count: 4)
!20 = !DILocation(line: 6, column: 10, scope: !10)
!21 = !DILocalVariable(name: "dst", scope: !10, file: !1, line: 7, type: !16)
!22 = !DILocation(line: 7, column: 10, scope: !10)
!23 = !DILocation(line: 8, column: 5, scope: !10)
!24 = !DILocation(line: 9, column: 16, scope: !10)
!25 = !DILocation(line: 9, column: 23, scope: !10)
!26 = !DILocation(line: 9, column: 5, scope: !10)
!27 = !DILocation(line: 10, column: 5, scope: !10)
