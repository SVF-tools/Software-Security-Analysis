import os
import subprocess
import glob

def run_tests(Failed_test_cases:list = []):
    assert isinstance(Failed_test_cases, list), "L should be a list"
    # Define the base directory containing the test cases
    base_dir = os.path.dirname(__file__)
    total_test_cases = 0
    # Define test case types and their corresponding subdirectories and flags
    test_types = {
        "pta": {"subdir": "../Tests/testcases/pta", "flag": "-pta"},
        "icfg": {"subdir": "../Tests/testcases/icfg", "flag": "-icfg"},
        "taint": {"subdir": "../Tests/testcases/taint", "flag": "-taint"},
    }
    # Iterate over each test type
    for test_type, config in test_types.items():
        testcases_dir = os.path.join(base_dir, config["subdir"])
        test_files = glob.glob(os.path.join(testcases_dir, "*.ll"))

        # Run tests for each file in the test type
        for filename in test_files:
            print(f"Adding Python test for {test_type}: {filename}")
            total_test_cases += 1
            # Construct the command to run the test
            main_py_path = os.path.join(os.path.dirname(__file__), "Main.py")
            command = [
                "python3", main_py_path, config["flag"], filename
            ]

            # Run the command and capture the output
            result = subprocess.run(command, cwd=os.path.join(base_dir, ".."), text=True)
            # Check the result
            if result.returncode == 0:
                print(f"Test passed for {filename}")
            else:
                Failed_test_cases.append(filename)
                #assert False, f"Test failed for {filename} with error code {result.returncode}"
    return total_test_cases



if __name__ == "__main__":
    Failed_test_cases = []
    total_test_cases = 0
    total_test_cases += run_tests(Failed_test_cases)
    if len(Failed_test_cases) == 0:
        print("\033[92mAll tests passed!\033[0m")
    else:
        print(f"\033[91m {len(Failed_test_cases)} out of {total_test_cases} failed\033[0m")
        print("\033[91mFailed tests are:\033[0m")
        for failed_test_case in Failed_test_cases:
            print(f"\033[91m{failed_test_case}\033[0m")