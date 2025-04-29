from Assignment3 import *

if __name__ == "__main__":
    # check sys.argv and print friendly error message if not enough arguments
    if len(sys.argv) != 2:
        print("Usage: python3 Assignment3.py <path-to-bc-file>")
        sys.exit(1)
    bc_file = sys.argv[1]
    pag = pysvf.get_pag(bc_file)
    ass3 = Assignment3(pag)
    ass3.analyse()
    pysvf.release_pag()