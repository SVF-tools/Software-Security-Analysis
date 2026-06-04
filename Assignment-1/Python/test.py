from Assignment_1 import *
def check_icfg_case(module_name, result, expected):
    assert len(result) == len(expected), f"Wrong paths generated - {module_name} failed!"
    for path in result:
        assert path in expected, f"Wrong paths generated - {module_name} failed!"
    print(f"Test case {module_name} passed!")


def test_icfg(argv):
    pysvf.buildSVFModule(argv)  # Build Program Assignment Graph (SVFIR)
    pag = pysvf.getPAG()
    icfg = pag.getICFG()  # Get ICFG
    gt = Ass1_ICFGTraversal(pag)  # Create ICFG Traversal object

    config_path = os.path.join(os.path.dirname(__file__), "../Tests/SrcSnk.txt")
    gt.readSrcSnkFromFile(config_path)

    for src in gt.identifySources():
        for snk in gt.identifySinks():
            gt.reachability(src, snk)
    for arg in argv:
        # not start with -
        if not arg.startswith('-'):
            module_name_vec = arg

    module_name = os.path.basename(module_name_vec)
    if module_name == "test1.ll":
        expected = {
            "START->6->7->8->9->10->1->5->2->11->14->END",
            "START->6->7->8->9->12->1->5->2->13->16->END"
        }
        check_icfg_case(module_name, gt.getPaths(), expected)

    elif module_name == "test2.ll":
        expected = {"START->17->1->7->END"}
        actual_paths = gt.getPaths()
        assert len(actual_paths) == len(expected), " \n wrong paths generated - test2 failed !"
        for path in actual_paths:
            assert path in expected, " \n wrong paths generated - test2 failed !"

    else:
        print(f"Test case {module_name} not found!")


def test_pta(argv):
    pysvf.buildSVFModule(argv)  # Build Program Assignment Graph (SVFIR)
    pag = pysvf.getPAG()
    andersen_pta = Ass1_Andersen(pag)
    andersen_pta.analyze()  # Run Andersen pointer analysis
    del andersen_pta


def test_taint(argv):
    pysvf.buildSVFModule(argv)  # Build Program Assignment Graph (SVFIR)
    pag = pysvf.getPAG()
    taint = Ass1_ICFGTraversal(pag)
    taint.taintChecking()  # Perform taint analysis

    for arg in argv:
        # not start with -
        if not arg.startswith('-'):
            module_name_vec = arg
    module_name_vec = os.path.basename(module_name_vec)
    print(taint.getPaths())
    if module_name_vec == "test1.ll":
        expected = {"START->6->1->5->2->7->8->9->10->END"}
        assert taint.getPaths() == expected, " \n wrong paths generated - test1 failed !"
        print("\n test1 passed !")
    elif module_name_vec == "test2.ll":
        expected = {"START->6->1->5->2->7->8->9->10->11->13->14->END"}
        assert taint.getPaths() == expected, " \n wrong paths generated - test2 failed !"
        print("\n test2 passed !")
    elif module_name_vec == "test2.ll" or module_name_vec == "test3.ll":
        expected = set()
        assert taint.getPaths() == expected, " \n wrong paths generated - test3 or test4 failed !"
        print("\n test3 or test4 passed !")


    print(f"###################### Tainted Information Flow ({len(taint.getPaths())} found) ######################")
    print("---------------------------------------------")
    for path in taint.getPaths():
        origin_path = path
        prefix = "START->"
        suffix = "->END"

        if path.startswith(prefix):
            path = path[len(prefix):]
        if path.endswith(suffix):
            path = path[:-len(suffix)]

        tokens = path.split("->")
        src_id = int(tokens[0])
        dst_id = int(tokens[-1])
        src_node = pag.getICFG().getGNode(src_id)
        dst_node = pag.getICFG().getGNode(dst_id)


        print(
            f"{origin_path}\nSource: {src_node.toString()}\nSink: {dst_node.toString()}\n---------------------------------------------")

    if not taint.getPaths():
        print("No tainted information flow found")


def main():
    pta_enabled = False
    taint_enabled = False
    icfg_enabled = False
    module_name_vec = ""
    argv = []

    args = sys.argv[1:]

    for arg in args:
        if arg == "-pta":
            pta_enabled = True
        elif arg == "-taint":
            taint_enabled = True
        elif arg == "-icfg":
            icfg_enabled = True
        else:
            argv.append(arg)

    # Default to taint analysis if none specified
    if not (pta_enabled or taint_enabled or icfg_enabled):
        assert False, "No analysis specified. Please specify -pta, -taint, or -icfg."

    assert (pta_enabled + taint_enabled + icfg_enabled) == 1, "Only one analysis can be enabled."

    if argv == []:
        assert False, "No module specified. Please specify a module to analyze."

    if pta_enabled:
        test_pta(argv)
    elif taint_enabled:
        test_taint(argv)
    elif icfg_enabled:
        test_icfg(argv)


if __name__ == "__main__":
    main()
