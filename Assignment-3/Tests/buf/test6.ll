; ModuleID = './test6.ll'
source_filename = "test6.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

@.str = private unnamed_addr constant [7 x i8] c"abcdef\00", align 1, !dbg !0

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !16 {
entry:
  %buffer = alloca [10 x i8], align 1
  call void @llvm.dbg.declare(metadata ptr %buffer, metadata !21, metadata !DIExpression()), !dbg !25
  call void @llvm.memset.p0.i64(ptr align 1 %buffer, i8 0, i64 10, i1 false), !dbg !25
  %arraydecay = getelementptr inbounds [10 x i8], ptr %buffer, i64 0, i64 0, !dbg !26
  call void @mem_insert(ptr noundef %arraydecay, ptr noundef @.str, i32 noundef 3, i32 noundef 5), !dbg !27
  %arrayidx = getelementptr inbounds [10 x i8], ptr %buffer, i64 0, i64 5, !dbg !28
  %0 = load i8, ptr %arrayidx, align 1, !dbg !28
  %conv = sext i8 %0 to i32, !dbg !28
  %cmp = icmp eq i32 %conv, 97, !dbg !29
  call void @svf_assert(i1 noundef zeroext %cmp), !dbg !30
  %arrayidx2 = getelementptr inbounds [10 x i8], ptr %buffer, i64 0, i64 6, !dbg !31
  %1 = load i8, ptr %arrayidx2, align 1, !dbg !31
  %conv3 = sext i8 %1 to i32, !dbg !31
  %cmp4 = icmp eq i32 %conv3, 98, !dbg !32
  call void @svf_assert(i1 noundef zeroext %cmp4), !dbg !33
  %arrayidx6 = getelementptr inbounds [10 x i8], ptr %buffer, i64 0, i64 7, !dbg !34
  %2 = load i8, ptr %arrayidx6, align 1, !dbg !34
  %conv7 = sext i8 %2 to i32, !dbg !34
  %cmp8 = icmp eq i32 %conv7, 99, !dbg !35
  call void @svf_assert(i1 noundef zeroext %cmp8), !dbg !36
  %arrayidx10 = getelementptr inbounds [10 x i8], ptr %buffer, i64 0, i64 8, !dbg !37
  %3 = load i8, ptr %arrayidx10, align 1, !dbg !37
  %conv11 = sext i8 %3 to i32, !dbg !37
  %cmp12 = icmp ne i32 %conv11, 100, !dbg !38
  call void @svf_assert(i1 noundef zeroext %cmp12), !dbg !39
  ret i32 0, !dbg !40
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

declare void @mem_insert(ptr noundef, ptr noundef, i32 noundef, i32 noundef) #3

declare void @svf_assert(i1 noundef zeroext) #3

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #3 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.dbg.cu = !{!7}
!llvm.module.flags = !{!9, !10, !11, !12, !13, !14}
!llvm.ident = !{!15}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(scope: null, file: !2, line: 7, type: !3, isLocal: true, isDefinition: true)
!2 = !DIFile(filename: "test6.c", directory: "/Users/z5489735/2023/0718/Software-Security-Analysis/teaching/Assignment-3/Tests/buf")
!3 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 56, elements: !5)
!4 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!5 = !{!6}
!6 = !DISubrange(count: 7)
!7 = distinct !DICompileUnit(language: DW_LANG_C11, file: !2, producer: "Homebrew clang version 16.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !8, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!8 = !{!0}
!9 = !{i32 7, !"Dwarf Version", i32 4}
!10 = !{i32 2, !"Debug Info Version", i32 3}
!11 = !{i32 1, !"wchar_size", i32 4}
!12 = !{i32 8, !"PIC Level", i32 2}
!13 = !{i32 7, !"uwtable", i32 1}
!14 = !{i32 7, !"frame-pointer", i32 1}
!15 = !{!"Homebrew clang version 16.0.6"}
!16 = distinct !DISubprogram(name: "main", scope: !2, file: !2, line: 5, type: !17, scopeLine: 5, spFlags: DISPFlagDefinition, unit: !7, retainedNodes: !20)
!17 = !DISubroutineType(types: !18)
!18 = !{!19}
!19 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!20 = !{}
!21 = !DILocalVariable(name: "buffer", scope: !16, file: !2, line: 6, type: !22)
!22 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 80, elements: !23)
!23 = !{!24}
!24 = !DISubrange(count: 10)
!25 = !DILocation(line: 6, column: 7, scope: !16)
!26 = !DILocation(line: 7, column: 13, scope: !16)
!27 = !DILocation(line: 7, column: 2, scope: !16)
!28 = !DILocation(line: 8, column: 16, scope: !16)
!29 = !DILocation(line: 8, column: 26, scope: !16)
!30 = !DILocation(line: 8, column: 5, scope: !16)
!31 = !DILocation(line: 9, column: 16, scope: !16)
!32 = !DILocation(line: 9, column: 26, scope: !16)
!33 = !DILocation(line: 9, column: 5, scope: !16)
!34 = !DILocation(line: 10, column: 16, scope: !16)
!35 = !DILocation(line: 10, column: 26, scope: !16)
!36 = !DILocation(line: 10, column: 5, scope: !16)
!37 = !DILocation(line: 11, column: 16, scope: !16)
!38 = !DILocation(line: 11, column: 26, scope: !16)
!39 = !DILocation(line: 11, column: 5, scope: !16)
!40 = !DILocation(line: 12, column: 5, scope: !16)
