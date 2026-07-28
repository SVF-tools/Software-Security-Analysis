#!/usr/bin/env python3

"""Regression checks for Assignment 3's typed report contract."""

from pathlib import Path
import sys


PYTHON_DIR = Path(__file__).resolve().parents[1] / "Python"
sys.path.insert(0, str(PYTHON_DIR))

from AEReporter import AEReporter  # noqa: E402


def main() -> None:
    reporter = AEReporter(None)
    access = object()

    reporter.reportBufOverflow(access, "first buffer report")
    reporter.reportBufOverflow(access, "duplicate buffer report")
    reporter.reportNullDeref(access, "null report at the same node")

    assert reporter.getReportCount() == 2
    assert reporter.getReportCount("buffer-overflow") == 1
    assert reporter.getReportCount("nullptr-deref") == 1

    reporter.noteExpectedReport("buffer-overflow")
    reporter.noteExpectedReport("nullptr-deref")
    reporter.noteExpectedReport("nullptr-deref")

    assert reporter.getExpectedReportCount("buffer-overflow") == 1
    assert reporter.getExpectedReportCount("nullptr-deref") == 2
    assert reporter.getExpectedReportCount("unknown-kind") == 0


if __name__ == "__main__":
    main()
