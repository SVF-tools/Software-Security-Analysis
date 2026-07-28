; ModuleID = '08-memory-summary.c'
source_filename = "08-memory-summary.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

@__const.main.source = private unnamed_addr constant [2 x i8] c"\07\09", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
entry:
  %retval = alloca i32, align 4
  %source = alloca [2 x i8], align 1
  %destination = alloca [2 x i8], align 1
  store i32 0, ptr %retval, align 4
    #dbg_declare(ptr %source, !15, !DIExpression(), !20)
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %source, ptr align 1 @__const.main.source, i64 2, i1 false), !dbg !20
    #dbg_declare(ptr %destination, !21, !DIExpression(), !22)
  call void @llvm.memset.p0.i64(ptr align 1 %destination, i8 0, i64 2, i1 false), !dbg !22
  %arraydecay = getelementptr inbounds [2 x i8], ptr %destination, i64 0, i64 0, !dbg !23
  %arraydecay1 = getelementptr inbounds [2 x i8], ptr %source, i64 0, i64 0, !dbg !24
  %call = call ptr @memcpy(ptr noundef %arraydecay, ptr noundef %arraydecay1, i64 noundef 2) #4, !dbg !25
  %arrayidx = getelementptr inbounds [2 x i8], ptr %destination, i64 0, i64 0, !dbg !26
  %0 = load i8, ptr %arrayidx, align 1, !dbg !26
  %conv = zext i8 %0 to i32, !dbg !26
  %cmp = icmp eq i32 %conv, 7, !dbg !27
  call void @svf_assert(i1 noundef %cmp) #4, !dbg !28
  %arrayidx3 = getelementptr inbounds [2 x i8], ptr %destination, i64 0, i64 1, !dbg !29
  %1 = load i8, ptr %arrayidx3, align 1, !dbg !29
  %conv4 = zext i8 %1 to i32, !dbg !29
  %cmp5 = icmp eq i32 %conv4, 9, !dbg !30
  call void @svf_assert(i1 noundef %cmp5) #4, !dbg !31
  ret i32 0, !dbg !32
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #2

declare ptr @memcpy(ptr noundef, ptr noundef, i64 noundef) #3

declare void @svf_assert(i1 noundef) #3

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #3 = { "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #4 = { nobuiltin "no-builtins" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "08-memory-summary.c", directory: "/workspace/Assignment-3/Tests/level-1", checksumkind: CSK_MD5, checksum: "4a6be870c323fee62857605a34465964")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 1}
!9 = !{!"clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 7, type: !11, scopeLine: 7, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "source", scope: !10, file: !1, line: 8, type: !16)
!16 = !DICompositeType(tag: DW_TAG_array_type, baseType: !17, size: 16, elements: !18)
!17 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!18 = !{!19}
!19 = !DISubrange(count: 2)
!20 = !DILocation(line: 8, column: 19, scope: !10)
!21 = !DILocalVariable(name: "destination", scope: !10, file: !1, line: 9, type: !16)
!22 = !DILocation(line: 9, column: 19, scope: !10)
!23 = !DILocation(line: 11, column: 12, scope: !10)
!24 = !DILocation(line: 11, column: 25, scope: !10)
!25 = !DILocation(line: 11, column: 5, scope: !10)
!26 = !DILocation(line: 12, column: 16, scope: !10)
!27 = !DILocation(line: 12, column: 31, scope: !10)
!28 = !DILocation(line: 12, column: 5, scope: !10)
!29 = !DILocation(line: 13, column: 16, scope: !10)
!30 = !DILocation(line: 13, column: 31, scope: !10)
!31 = !DILocation(line: 13, column: 5, scope: !10)
!32 = !DILocation(line: 14, column: 5, scope: !10)
