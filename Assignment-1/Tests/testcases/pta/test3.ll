; ModuleID = './test3.ll'
source_filename = "./test3.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @foo(ptr noundef %m, ptr noundef %n) #0 !dbg !9 {
entry:
  call void @llvm.dbg.value(metadata ptr %m, metadata !16, metadata !DIExpression()), !dbg !17
  call void @llvm.dbg.value(metadata ptr %n, metadata !18, metadata !DIExpression()), !dbg !17
  call void @MAYALIAS(ptr noundef %m, ptr noundef %n), !dbg !19
  %0 = load i32, ptr %n, align 4, !dbg !20
  call void @llvm.dbg.value(metadata i32 %0, metadata !21, metadata !DIExpression()), !dbg !17
  %1 = load i32, ptr %m, align 4, !dbg !22
  call void @llvm.dbg.value(metadata i32 %1, metadata !23, metadata !DIExpression()), !dbg !17
  store i32 %0, ptr %m, align 4, !dbg !24
  store i32 %1, ptr %n, align 4, !dbg !25
  ret void, !dbg !26
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare void @MAYALIAS(ptr noundef, ptr noundef) #2

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !27 {
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  call void @llvm.dbg.declare(metadata ptr %a, metadata !30, metadata !DIExpression()), !dbg !31
  call void @llvm.dbg.declare(metadata ptr %b, metadata !32, metadata !DIExpression()), !dbg !33
  call void @llvm.dbg.declare(metadata ptr %c, metadata !34, metadata !DIExpression()), !dbg !35
  %0 = load i32, ptr %c, align 4, !dbg !36
  %tobool = icmp ne i32 %0, 0, !dbg !36
  br i1 %tobool, label %if.then, label %if.else, !dbg !38

if.then:                                          ; preds = %entry
  call void @llvm.dbg.value(metadata ptr %a, metadata !39, metadata !DIExpression()), !dbg !40
  call void @llvm.dbg.value(metadata ptr %b, metadata !41, metadata !DIExpression()), !dbg !40
  call void @foo(ptr noundef %a, ptr noundef %b), !dbg !42
  br label %if.end, !dbg !44

if.else:                                          ; preds = %entry
  call void @llvm.dbg.value(metadata ptr %b, metadata !39, metadata !DIExpression()), !dbg !40
  call void @llvm.dbg.value(metadata ptr %c, metadata !41, metadata !DIExpression()), !dbg !40
  call void @foo(ptr noundef %b, ptr noundef %c), !dbg !45
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret i32 0, !dbg !47
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Homebrew clang version 16.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!1 = !DIFile(filename: "test3.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-1/Tests/testcases/pta")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 1}
!8 = !{!"Homebrew clang version 16.0.6"}
!9 = distinct !DISubprogram(name: "foo", scope: !10, file: !10, line: 3, type: !11, scopeLine: 4, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!10 = !DIFile(filename: "./test3.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-1/Tests/testcases/pta")
!11 = !DISubroutineType(types: !12)
!12 = !{null, !13, !13}
!13 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
!14 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!15 = !{}
!16 = !DILocalVariable(name: "m", arg: 1, scope: !9, file: !10, line: 3, type: !13)
!17 = !DILocation(line: 0, scope: !9)
!18 = !DILocalVariable(name: "n", arg: 2, scope: !9, file: !10, line: 3, type: !13)
!19 = !DILocation(line: 5, column: 5, scope: !9)
!20 = !DILocation(line: 7, column: 9, scope: !9)
!21 = !DILocalVariable(name: "x", scope: !9, file: !10, line: 6, type: !14)
!22 = !DILocation(line: 8, column: 9, scope: !9)
!23 = !DILocalVariable(name: "y", scope: !9, file: !10, line: 6, type: !14)
!24 = !DILocation(line: 9, column: 8, scope: !9)
!25 = !DILocation(line: 10, column: 8, scope: !9)
!26 = !DILocation(line: 11, column: 1, scope: !9)
!27 = distinct !DISubprogram(name: "main", scope: !10, file: !10, line: 13, type: !28, scopeLine: 14, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!28 = !DISubroutineType(types: !29)
!29 = !{!14}
!30 = !DILocalVariable(name: "a", scope: !27, file: !10, line: 16, type: !14)
!31 = !DILocation(line: 16, column: 9, scope: !27)
!32 = !DILocalVariable(name: "b", scope: !27, file: !10, line: 16, type: !14)
!33 = !DILocation(line: 16, column: 12, scope: !27)
!34 = !DILocalVariable(name: "c", scope: !27, file: !10, line: 16, type: !14)
!35 = !DILocation(line: 16, column: 15, scope: !27)
!36 = !DILocation(line: 17, column: 9, scope: !37)
!37 = distinct !DILexicalBlock(scope: !27, file: !10, line: 17, column: 9)
!38 = !DILocation(line: 17, column: 9, scope: !27)
!39 = !DILocalVariable(name: "p", scope: !27, file: !10, line: 15, type: !13)
!40 = !DILocation(line: 0, scope: !27)
!41 = !DILocalVariable(name: "q", scope: !27, file: !10, line: 15, type: !13)
!42 = !DILocation(line: 20, column: 9, scope: !43)
!43 = distinct !DILexicalBlock(scope: !37, file: !10, line: 17, column: 12)
!44 = !DILocation(line: 21, column: 5, scope: !43)
!45 = !DILocation(line: 25, column: 9, scope: !46)
!46 = distinct !DILexicalBlock(scope: !37, file: !10, line: 22, column: 10)
!47 = !DILocation(line: 27, column: 5, scope: !27)
