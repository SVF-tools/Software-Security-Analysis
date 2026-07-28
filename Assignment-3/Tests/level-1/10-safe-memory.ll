; ModuleID = '10-safe-memory.c'
source_filename = "10-safe-memory.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

@__const.main.buffer = private unnamed_addr constant [2 x i8] c"\04\05", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !14 {
entry:
  %retval = alloca i32, align 4
  %buffer = alloca [2 x i8], align 1
  %byte = alloca i8, align 1
  %value = alloca i32, align 4
  %observed = alloca i32, align 4
  store i32 0, ptr %retval, align 4
    #dbg_declare(ptr %buffer, !18, !DIExpression(), !23)
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %buffer, ptr align 1 @__const.main.buffer, i64 2, i1 false), !dbg !23
    #dbg_declare(ptr %byte, !24, !DIExpression(), !26)
  %arrayidx = getelementptr inbounds [2 x i8], ptr %buffer, i64 0, i64 1, !dbg !27
  %0 = load i8, ptr %arrayidx, align 1, !dbg !27
  store volatile i8 %0, ptr %byte, align 1, !dbg !26
  %1 = load volatile i8, ptr %byte, align 1, !dbg !28
  %conv = zext i8 %1 to i32, !dbg !28
  %cmp = icmp eq i32 %conv, 5, !dbg !29
  call void @svf_assert(i1 noundef %cmp) #3, !dbg !30
  %arrayidx2 = getelementptr inbounds [2 x i8], ptr %buffer, i64 0, i64 1, !dbg !31
  call void @SAFE_BUFACCESS(ptr noundef %arrayidx2, i64 noundef 1) #3, !dbg !32
    #dbg_declare(ptr %value, !33, !DIExpression(), !34)
  store i32 8, ptr %value, align 4, !dbg !34
    #dbg_declare(ptr %observed, !35, !DIExpression(), !36)
  %2 = load volatile i32, ptr %value, align 4, !dbg !37
  store volatile i32 %2, ptr %observed, align 4, !dbg !36
  %3 = load volatile i32, ptr %observed, align 4, !dbg !38
  %cmp3 = icmp eq i32 %3, 8, !dbg !39
  call void @svf_assert(i1 noundef %cmp3) #3, !dbg !40
  call void @SAFE_PTRDEREF(ptr noundef %value) #3, !dbg !41
  ret i32 0, !dbg !42
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #1

declare void @svf_assert(i1 noundef) #2

declare void @SAFE_BUFACCESS(ptr noundef, i64 noundef) #2

declare void @SAFE_PTRDEREF(ptr noundef) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #3 = { nobuiltin "no-builtins" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!6, !7, !8, !9, !10, !11, !12}
!llvm.ident = !{!13}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "10-safe-memory.c", directory: "/workspace/Assignment-3/Tests/level-1", checksumkind: CSK_MD5, checksum: "71708700e208be20b4f9300472b66d04")
!2 = !{!3}
!3 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
!4 = !DIDerivedType(tag: DW_TAG_volatile_type, baseType: !5)
!5 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!6 = !{i32 7, !"Dwarf Version", i32 5}
!7 = !{i32 2, !"Debug Info Version", i32 3}
!8 = !{i32 1, !"wchar_size", i32 4}
!9 = !{i32 8, !"PIC Level", i32 2}
!10 = !{i32 7, !"PIE Level", i32 2}
!11 = !{i32 7, !"uwtable", i32 2}
!12 = !{i32 7, !"frame-pointer", i32 1}
!13 = !{!"clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)"}
!14 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 7, type: !15, scopeLine: 7, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !17)
!15 = !DISubroutineType(types: !16)
!16 = !{!5}
!17 = !{}
!18 = !DILocalVariable(name: "buffer", scope: !14, file: !1, line: 8, type: !19)
!19 = !DICompositeType(tag: DW_TAG_array_type, baseType: !20, size: 16, elements: !21)
!20 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_unsigned_char)
!21 = !{!22}
!22 = !DISubrange(count: 2)
!23 = !DILocation(line: 8, column: 10, scope: !14)
!24 = !DILocalVariable(name: "byte", scope: !14, file: !1, line: 9, type: !25)
!25 = !DIDerivedType(tag: DW_TAG_volatile_type, baseType: !20)
!26 = !DILocation(line: 9, column: 19, scope: !14)
!27 = !DILocation(line: 9, column: 26, scope: !14)
!28 = !DILocation(line: 10, column: 16, scope: !14)
!29 = !DILocation(line: 10, column: 21, scope: !14)
!30 = !DILocation(line: 10, column: 5, scope: !14)
!31 = !DILocation(line: 11, column: 21, scope: !14)
!32 = !DILocation(line: 11, column: 5, scope: !14)
!33 = !DILocalVariable(name: "value", scope: !14, file: !1, line: 13, type: !5)
!34 = !DILocation(line: 13, column: 9, scope: !14)
!35 = !DILocalVariable(name: "observed", scope: !14, file: !1, line: 14, type: !4)
!36 = !DILocation(line: 14, column: 18, scope: !14)
!37 = !DILocation(line: 14, column: 29, scope: !14)
!38 = !DILocation(line: 15, column: 16, scope: !14)
!39 = !DILocation(line: 15, column: 25, scope: !14)
!40 = !DILocation(line: 15, column: 5, scope: !14)
!41 = !DILocation(line: 16, column: 5, scope: !14)
!42 = !DILocation(line: 17, column: 5, scope: !14)
