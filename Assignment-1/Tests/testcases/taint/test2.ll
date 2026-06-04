; ModuleID = './test2.ll'
source_filename = "./test2.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

@.str = private unnamed_addr constant [4 x i8] c"/0 \00", align 1, !dbg !0
@.str.1 = private unnamed_addr constant [6 x i8] c"hello\00", align 1, !dbg !7

; Function Attrs: noinline nounwind ssp uwtable(sync)
define ptr @getchar() #0 !dbg !22 {
entry:
  ret ptr @.str, !dbg !27
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !28 {
entry:
  call void @llvm.dbg.value(metadata i8 1, metadata !32, metadata !DIExpression()), !dbg !34
  call void @llvm.dbg.value(metadata i8 1, metadata !35, metadata !DIExpression()), !dbg !34
  %call = call ptr @getchar(), !dbg !36
  call void @llvm.dbg.value(metadata ptr %call, metadata !37, metadata !DIExpression()), !dbg !34
  br label %while.cond, !dbg !38

while.cond:                                       ; preds = %if.end, %entry
  %tobool = trunc i8 1 to i1, !dbg !39
  br i1 %tobool, label %while.body, label %while.end, !dbg !38

while.body:                                       ; preds = %while.cond
  %tobool1 = trunc i8 1 to i1, !dbg !40
  br i1 %tobool1, label %if.then, label %if.else, !dbg !43

if.then:                                          ; preds = %while.body
  call void @llvm.dbg.value(metadata ptr %call, metadata !44, metadata !DIExpression()), !dbg !46
  call void @broadcast(ptr noundef %call), !dbg !47
  call void @MAYALIAS(ptr noundef %call, ptr noundef %call), !dbg !48
  br label %if.end, !dbg !49

if.else:                                          ; preds = %while.body
  call void @llvm.dbg.value(metadata ptr @.str.1, metadata !50, metadata !DIExpression()), !dbg !52
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  br label %while.cond, !dbg !38, !llvm.loop !53

while.end:                                        ; preds = %while.cond
  ret i32 0, !dbg !56
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare void @broadcast(ptr noundef) #2

declare void @MAYALIAS(ptr noundef, ptr noundef) #2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.dbg.cu = !{!12}
!llvm.module.flags = !{!15, !16, !17, !18, !19, !20}
!llvm.ident = !{!21}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(scope: null, file: !2, line: 4, type: !3, isLocal: true, isDefinition: true)
!2 = !DIFile(filename: "./test2.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-1/Tests/testcases/taint")
!3 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 32, elements: !5)
!4 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!5 = !{!6}
!6 = !DISubrange(count: 4)
!7 = !DIGlobalVariableExpression(var: !8, expr: !DIExpression())
!8 = distinct !DIGlobalVariable(scope: null, file: !2, line: 17, type: !9, isLocal: true, isDefinition: true)
!9 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 48, elements: !10)
!10 = !{!11}
!11 = !DISubrange(count: 6)
!12 = distinct !DICompileUnit(language: DW_LANG_C11, file: !13, producer: "Homebrew clang version 16.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !14, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!13 = !DIFile(filename: "test2.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-1/Tests/testcases/taint")
!14 = !{!0, !7}
!15 = !{i32 7, !"Dwarf Version", i32 4}
!16 = !{i32 2, !"Debug Info Version", i32 3}
!17 = !{i32 1, !"wchar_size", i32 4}
!18 = !{i32 8, !"PIC Level", i32 2}
!19 = !{i32 7, !"uwtable", i32 1}
!20 = !{i32 7, !"frame-pointer", i32 1}
!21 = !{!"Homebrew clang version 16.0.6"}
!22 = distinct !DISubprogram(name: "getchar", scope: !2, file: !2, line: 4, type: !23, scopeLine: 4, spFlags: DISPFlagDefinition, unit: !12, retainedNodes: !26)
!23 = !DISubroutineType(types: !24)
!24 = !{!25}
!25 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
!26 = !{}
!27 = !DILocation(line: 4, column: 21, scope: !22)
!28 = distinct !DISubprogram(name: "main", scope: !2, file: !2, line: 6, type: !29, scopeLine: 6, spFlags: DISPFlagDefinition, unit: !12, retainedNodes: !26)
!29 = !DISubroutineType(types: !30)
!30 = !{!31}
!31 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!32 = !DILocalVariable(name: "loopCondition", scope: !28, file: !2, line: 7, type: !33)
!33 = !DIBasicType(name: "_Bool", size: 8, encoding: DW_ATE_boolean)
!34 = !DILocation(line: 0, scope: !28)
!35 = !DILocalVariable(name: "BranchCondition", scope: !28, file: !2, line: 8, type: !33)
!36 = !DILocation(line: 9, column: 25, scope: !28)
!37 = !DILocalVariable(name: "secretToken", scope: !28, file: !2, line: 9, type: !25)
!38 = !DILocation(line: 10, column: 5, scope: !28)
!39 = !DILocation(line: 10, column: 11, scope: !28)
!40 = !DILocation(line: 11, column: 12, scope: !41)
!41 = distinct !DILexicalBlock(scope: !42, file: !2, line: 11, column: 12)
!42 = distinct !DILexicalBlock(scope: !28, file: !2, line: 10, column: 25)
!43 = !DILocation(line: 11, column: 12, scope: !42)
!44 = !DILocalVariable(name: "a", scope: !45, file: !2, line: 12, type: !25)
!45 = distinct !DILexicalBlock(scope: !41, file: !2, line: 11, column: 28)
!46 = !DILocation(line: 0, scope: !45)
!47 = !DILocation(line: 13, column: 13, scope: !45)
!48 = !DILocation(line: 14, column: 13, scope: !45)
!49 = !DILocation(line: 15, column: 9, scope: !45)
!50 = !DILocalVariable(name: "b", scope: !51, file: !2, line: 17, type: !25)
!51 = distinct !DILexicalBlock(scope: !41, file: !2, line: 16, column: 13)
!52 = !DILocation(line: 0, scope: !51)
!53 = distinct !{!53, !38, !54, !55}
!54 = !DILocation(line: 19, column: 5, scope: !28)
!55 = !{!"llvm.loop.mustprogress"}
!56 = !DILocation(line: 20, column: 1, scope: !28)
