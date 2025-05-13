from Assignment3 import *

if __name__ == "__main__":
    # check sys.argv and print friendly error message if not enough arguments
    if len(sys.argv) < 2:
        print("Usage: python3 Main.py <path-to-bc-file>")
        sys.exit(1)
    pysvf.buildSVFModule(sys.argv[1:])
    pag = pysvf.getPAG()
    ass3 = Assignment3(pag)
    ass3.analyse()
    pysvf.releasePAG()