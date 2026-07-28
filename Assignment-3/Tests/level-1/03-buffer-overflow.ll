; ModuleID = '03-buffer-overflow.c'
source_filename = "03-buffer-overflow.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !15 {
entry:
  %retval = alloca i32, align 4
  %buf = alloca [1 x i8], align 1
  %observed = alloca i32, align 4
  store i32 0, ptr %retval, align 4
    #dbg_declare(ptr %buf, !19, !DIExpression(), !24)
  call void @llvm.memset.p0.i64(ptr align 1 %buf, i8 0, i64 1, i1 false), !dbg !24
    #dbg_declare(ptr %observed, !25, !DIExpression(), !26)
  %arraydecay = getelementptr inbounds [1 x i8], ptr %buf, i64 0, i64 0, !dbg !27
  %0 = load volatile i32, ptr %arraydecay, align 4, !dbg !28
  store volatile i32 %0, ptr %observed, align 4, !dbg !26
  %1 = load volatile i32, ptr %observed, align 4, !dbg !29
  %arraydecay1 = getelementptr inbounds [1 x i8], ptr %buf, i64 0, i64 0, !dbg !30
  call void @UNSAFE_BUFACCESS(ptr noundef %arraydecay1, i64 noundef 4) #3, !dbg !31
  ret i32 0, !dbg !32
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #1

declare void @UNSAFE_BUFACCESS(ptr noundef, i64 noundef) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #2 = { "frame-pointer"="non-leaf" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #3 = { nobuiltin "no-builtins" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!7, !8, !9, !10, !11, !12, !13}
!llvm.ident = !{!14}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.0 (https://github.com/bjjwwang/LLVM-compile 56e2582eb90f98cb21f19cd6d72cca341b06d1bf)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "03-buffer-overflow.c", directory: "/workspace/Assignment-3/Tests/level-1", checksumkind: CSK_MD5, checksum: "11a2826ffe4359ef566adcbe40a679b1")
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
!19 = !DILocalVariable(name: "buf", scope: !15, file: !1, line: 4, type: !20)
!20 = !DICompositeType(tag: DW_TAG_array_type, baseType: !21, size: 8, elements: !22)
!21 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_unsigned_char)
!22 = !{!23}
!23 = !DISubrange(count: 1)
!24 = !DILocation(line: 4, column: 10, scope: !15)
!25 = !DILocalVariable(name: "observed", scope: !15, file: !1, line: 5, type: !4)
!26 = !DILocation(line: 5, column: 18, scope: !15)
!27 = !DILocation(line: 5, column: 54, scope: !15)
!28 = !DILocation(line: 5, column: 29, scope: !15)
!29 = !DILocation(line: 6, column: 11, scope: !15)
!30 = !DILocation(line: 7, column: 30, scope: !15)
!31 = !DILocation(line: 7, column: 5, scope: !15)
!32 = !DILocation(line: 8, column: 5, scope: !15)
