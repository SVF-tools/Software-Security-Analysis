; ModuleID = './test1.ll'
source_filename = "./test1.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

@__const.main.a = private unnamed_addr constant [10 x i32] [i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9], align 4

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 !dbg !9 {
entry:
  %a = alloca [10 x i32], align 4
  call void @llvm.dbg.declare(metadata ptr %a, metadata !15, metadata !DIExpression()), !dbg !19
  call void @llvm.memcpy.p0.p0.i64(ptr align 4 %a, ptr align 4 @__const.main.a, i64 40, i1 false), !dbg !19
  %arraydecay = getelementptr inbounds [10 x i32], ptr %a, i64 0, i64 0, !dbg !20
  %add.ptr = getelementptr inbounds i32, ptr %arraydecay, i64 9, !dbg !21
  call void @llvm.dbg.value(metadata ptr %add.ptr, metadata !22, metadata !DIExpression()), !dbg !24
  store i32 10, ptr %add.ptr, align 4, !dbg !25
  %arrayidx = getelementptr inbounds [10 x i32], ptr %a, i64 0, i64 9, !dbg !26
  %0 = load i32, ptr %arrayidx, align 4, !dbg !26
  %cmp = icmp eq i32 %0, 10, !dbg !27
  call void @svf_assert(i1 noundef zeroext %cmp), !dbg !28
  ret i32 0, !dbg !29
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #2

declare void @svf_assert(i1 noundef zeroext) #3

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #3 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Homebrew clang version 16.0.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!1 = !DIFile(filename: "test1.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-3/Tests/ae")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 1}
!8 = !{!"Homebrew clang version 16.0.6"}
!9 = distinct !DISubprogram(name: "main", scope: !10, file: !10, line: 9, type: !11, scopeLine: 9, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!10 = !DIFile(filename: "./test1.c", directory: "/Users/z5489735/2023/0513/Software-Security-Analysis/Assignment-3/Tests/ae")
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "a", scope: !9, file: !10, line: 10, type: !16)
!16 = !DICompositeType(tag: DW_TAG_array_type, baseType: !13, size: 320, elements: !17)
!17 = !{!18}
!18 = !DISubrange(count: 10)
!19 = !DILocation(line: 10, column: 9, scope: !9)
!20 = !DILocation(line: 11, column: 15, scope: !9)
!21 = !DILocation(line: 11, column: 17, scope: !9)
!22 = !DILocalVariable(name: "p", scope: !9, file: !10, line: 11, type: !23)
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!24 = !DILocation(line: 0, scope: !9)
!25 = !DILocation(line: 12, column: 8, scope: !9)
!26 = !DILocation(line: 13, column: 16, scope: !9)
!27 = !DILocation(line: 13, column: 25, scope: !9)
!28 = !DILocation(line: 13, column: 5, scope: !9)
!29 = !DILocation(line: 14, column: 5, scope: !9)
