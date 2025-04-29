from Assignment1 import *
def check_icfg_case(module_name, result, expected):
    assert len(result) == len(expected), f"Wrong paths generated - {module_name} failed!"
    for path in result:
        assert path in expected, f"Wrong paths generated - {module_name} failed!"
    print(f"Test case {module_name} passed!")


def test_icfg(module_name_vec):
    pag = pysvf.get_pag(module_name_vec)  # Build Program Assignment Graph (SVFIR)
    icfg = pag.get_icfg()  # Get ICFG
    gt = Ass1_ICFGTraversal(pag)  # Create ICFG Traversal object

    config_path = os.path.join(os.path.dirname(__file__), "../Tests/SrcSnk.txt")
    gt.read_srcsnk_from_file(config_path)

    for src in gt.identify_sources():
        for snk in gt.identify_sinks():
            gt.reachability(src, snk)

    module_name = os.path.basename(module_name_vec)
    if module_name == "test1.ll":
        expected = {"START->3->4->5->END"}
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test2.ll":
        expected = {
            "START->3->4->5->6->7->8->9->END",
            "START->3->4->5->6->7->END",
            "START->5->6->7->8->9->END",
            "START->5->6->7->END"
        }
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test3.ll":
        expected = {"START->6->7->8->1->5->2->9->10->END"}
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test4.ll":
        expected = {"START->12->13->14->3->8->9->1->7->2->10->11->4->15->16->END"}
        check_icfg_case(module_name, gt.get_paths(), expected)
    # Add further test cases as needed...
    elif module_name == "test5.ll":
        expected = {
            "START->6->7->8->9->10->1->5->2->11->14->END",
            "START->6->7->8->9->12->1->5->2->13->16->END",
        }
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test6.ll":
        expected = {
            "START->12->13->14->15->16->3->8->9->1->7->2->10->11->4->17->20->END",
            "START->12->13->14->15->18->3->8->9->1->7->2->10->11->4->19->22->END",
        }
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif  module_name == "test7.ll":
        expected = {"START->17->1->7->END"}
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test8.ll":
        expected = {
            "START->6->7->8->9->10->1->5->2->11->14->END",
            "START->6->7->8->9->12->1->5->2->13->16->END",
        }
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test9.ll":
        expected = {"START->7->8->9->10->11->14->END"}
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test10.ll":
        expected = {
            "START->3->4->5->6->7->9->11->END",
            "START->3->4->5->6->8->10->14->17->END",
        }
        check_icfg_case(module_name, gt.get_paths(), expected)

    else:
        print(f"Test case {module_name} not found!")


def test_pta(module_name_vec):
    pag = pysvf.get_pag(module_name_vec)  # Build Program Assignment Graph (SVFIR)
    andersen_pta = Ass1_Andersen(pag)
    andersen_pta.analyze()  # Run Andersen pointer analysis
    del andersen_pta


def test_taint(module_name_vec):
    pag = pysvf.get_pag(module_name_vec)  # Build Program Assignment Graph (SVFIR)

    taint = Ass1_ICFGTraversal(pag)
    taint.taint_checking()  # Perform taint analysis

    module_name_vec = os.path.basename(module_name_vec)
    print(taint.get_paths())
    if module_name_vec == "test1.ll":
        expected = {"START->6->1->5->2->7->8->9->10->END"}
        assert taint.get_paths() == expected, " \n wrong paths generated - test1 failed !"
        print("\n test1 passed !")
    elif module_name_vec == "test4.ll":
        expected = {"START->6->1->5->2->7->8->9->10->11->13->14->END"}
        assert taint.get_paths() == expected, " \n wrong paths generated - test4 failed !"
        print("\n test2 passed !")
    elif module_name_vec == "test2.ll" or module_name_vec == "test3.ll":
        expected = set()
        assert taint.get_paths() == expected, " \n wrong paths generated - test2 or test3 failed !"
        print("\n test2 or test3 passed !")


    print(f"###################### Tainted Information Flow ({len(taint.get_paths())} found) ######################")
    print("---------------------------------------------")
    for path in taint.get_paths():
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
        src_node = pag.get_icfg().get_gnode(src_id)
        dst_node = pag.get_icfg().get_gnode(dst_id)

        print(
            f"{origin_path}\nSource: {src_node.to_string()}\nSink: {dst_node.to_string()}\n---------------------------------------------")

    if not taint.get_paths():
        print("No tainted information flow found")


def main():
    pta_enabled = False
    taint_enabled = False
    icfg_enabled = False
    module_name_vec = ""

    args = sys.argv[1:]

    for arg in args:
        if arg == "-pta":
            pta_enabled = True
        elif arg == "-taint":
            taint_enabled = True
        elif arg == "-icfg":
            icfg_enabled = True
        else:
            module_name_vec = arg

    # Default to taint analysis if none specified
    if not (pta_enabled or taint_enabled or icfg_enabled):
        assert False, "No analysis specified. Please specify -pta, -taint, or -icfg."

    assert (pta_enabled + taint_enabled + icfg_enabled) == 1, "Only one analysis can be enabled."

    if module_name_vec == "":
        assert False, "No module specified. Please specify a module to analyze."

    if pta_enabled:
        test_pta(module_name_vec)
    elif taint_enabled:
        test_taint(module_name_vec)
    elif icfg_enabled:
        test_icfg(module_name_vec)


if __name__ == "__main__":
    main()