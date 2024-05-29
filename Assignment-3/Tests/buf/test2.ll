; ModuleID = './test2.ll'
source_filename = "./test2.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @CWE121_Stack_Based_Buffer_Overflow__CWE805_int64_t_declare_loop_01_bad() #0 !dbg !9 {
entry:
  %dataBadBuffer = alloca [50 x i64], align 8
  %source = alloca [100 x i64], align 8
  call void @llvm.dbg.declare(metadata ptr %dataBadBuffer, metadata !14, metadata !DIExpression()), !dbg !21
  call void @llvm.dbg.declare(metadata ptr %source, metadata !22, metadata !DIExpression()), !dbg !26
  call void @llvm.memset.p0.i64(ptr align 8 %source, i8 0, i64 800, i1 false), !dbg !26
  %arraydecay = getelementptr inbounds [50 x i64], ptr %dataBadBuffer, i64 0, i64 0, !dbg !27
  call void @OVERFLOW(ptr noundef %arraydecay, i32 noundef 800), !dbg !28
  call void @llvm.dbg.value(metadata i64 0, metadata !29, metadata !DIExpression()), !dbg !35
  br label %for.cond, !dbg !36

for.cond:                                         ; preds = %for.inc, %entry
  %i.0 = phi i64 [ 0, %entry ], [ %inc, %for.inc ], !dbg !38
  call void @llvm.dbg.value(metadata i64 %i.0, metadata !29, metadata !DIExpression()), !dbg !35
  %cmp = icmp ult i64 %i.0, 100, !dbg !39
  br i1 %cmp, label %for.body, label %for.end, !dbg !41

for.body:                                         ; preds = %for.cond
  %arrayidx = getelementptr inbounds [100 x i64], ptr %source, i64 0, i64 %i.0, !dbg !42
  %0 = load i64, ptr %arrayidx, align 8, !dbg !42
  %arrayidx1 = getelementptr inbounds [50 x i64], ptr %dataBadBuffer, i64 0, i64 %i.0, !dbg !44
  store i64 %0, ptr %arrayidx1, align 8, !dbg !45
  br label %for.inc, !dbg !46

for.inc:                                          ; preds = %for.body
  %inc = add i64 %i.0, 1, !dbg !47
  call void @llvm.dbg.value(metadata i64 %inc, metadata !29, metadata !DIExpression()), !dbg !35
  br label %for.cond, !dbg !48, !llvm.loop !49

for.end:                                          ; preds = %for.cond
  ret void, !dbg !52
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

declare void @OVERFLOW(ptr noundef, i32 noundef) #3

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !53 {
entry:
  call void @CWE121_Stack_Based_Buffer_Overflow__CWE805_int64_t_declare_loop_01_bad(), !dbg !57
  ret i32 0, !dbg !58
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #3 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Homebrew clang version 16.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!1 = !DIFile(filename: "test2.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-3/Tests/buf")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 1}
!8 = !{!"Homebrew clang version 16.0.6"}
!9 = distinct !DISubprogram(name: "CWE121_Stack_Based_Buffer_Overflow__CWE805_int64_t_declare_loop_01_bad", scope: !10, file: !10, line: 6, type: !11, scopeLine: 6, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !13)
!10 = !DIFile(filename: "./test2.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-3/Tests/buf")
!11 = !DISubroutineType(types: !12)
!12 = !{null}
!13 = !{}
!14 = !DILocalVariable(name: "dataBadBuffer", scope: !9, file: !10, line: 7, type: !15)
!15 = !DICompositeType(tag: DW_TAG_array_type, baseType: !16, size: 3200, elements: !19)
!16 = !DIDerivedType(tag: DW_TAG_typedef, name: "int64_t", file: !17, line: 30, baseType: !18)
!17 = !DIFile(filename: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk/usr/include/sys/_types/_int64_t.h", directory: "")
!18 = !DIBasicType(name: "long long", size: 64, encoding: DW_ATE_signed)
!19 = !{!20}
!20 = !DISubrange(count: 50)
!21 = !DILocation(line: 7, column: 13, scope: !9)
!22 = !DILocalVariable(name: "source", scope: !9, file: !10, line: 9, type: !23)
!23 = !DICompositeType(tag: DW_TAG_array_type, baseType: !16, size: 6400, elements: !24)
!24 = !{!25}
!25 = !DISubrange(count: 100)
!26 = !DILocation(line: 9, column: 13, scope: !9)
!27 = !DILocation(line: 13, column: 14, scope: !9)
!28 = !DILocation(line: 13, column: 5, scope: !9)
!29 = !DILocalVariable(name: "i", scope: !9, file: !10, line: 10, type: !30)
!30 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !31, line: 31, baseType: !32)
!31 = !DIFile(filename: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk/usr/include/sys/_types/_size_t.h", directory: "")
!32 = !DIDerivedType(tag: DW_TAG_typedef, name: "__darwin_size_t", file: !33, line: 70, baseType: !34)
!33 = !DIFile(filename: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk/usr/include/arm/_types.h", directory: "")
!34 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!35 = !DILocation(line: 0, scope: !9)
!36 = !DILocation(line: 14, column: 10, scope: !37)
!37 = distinct !DILexicalBlock(scope: !9, file: !10, line: 14, column: 5)
!38 = !DILocation(line: 14, scope: !37)
!39 = !DILocation(line: 14, column: 19, scope: !40)
!40 = distinct !DILexicalBlock(scope: !37, file: !10, line: 14, column: 5)
!41 = !DILocation(line: 14, column: 5, scope: !37)
!42 = !DILocation(line: 15, column: 28, scope: !43)
!43 = distinct !DILexicalBlock(scope: !40, file: !10, line: 14, column: 31)
!44 = !DILocation(line: 15, column: 9, scope: !43)
!45 = !DILocation(line: 15, column: 26, scope: !43)
!46 = !DILocation(line: 16, column: 5, scope: !43)
!47 = !DILocation(line: 14, column: 27, scope: !40)
!48 = !DILocation(line: 14, column: 5, scope: !40)
!49 = distinct !{!49, !41, !50, !51}
!50 = !DILocation(line: 16, column: 5, scope: !37)
!51 = !{!"llvm.loop.mustprogress"}
!52 = !DILocation(line: 17, column: 1, scope: !9)
!53 = distinct !DISubprogram(name: "main", scope: !10, file: !10, line: 19, type: !54, scopeLine: 19, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !13)
!54 = !DISubroutineType(types: !55)
!55 = !{!56}
!56 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!57 = !DILocation(line: 20, column: 5, scope: !53)
!58 = !DILocation(line: 21, column: 5, scope: !53)
