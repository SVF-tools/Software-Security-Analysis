#!/usr/bin/env python3

"""Regression checks for Assignment 3's typed report contract."""

from pathlib import Path
import sys
import types


PYTHON_DIR = Path(__file__).resolve().parents[1] / "Python"
sys.path.insert(0, str(PYTHON_DIR))

try:
    import pysvf  # noqa: F401
except ModuleNotFoundError:
    # Reporting/counting is intentionally independent of the SVF domain.
    # Lightweight stand-ins let CI execute this contract test even when it
    # builds only the C++ SVF package and has no PySVF wheel installed.
    fake_pysvf = types.ModuleType("pysvf")
    for type_name in (
            "AbstractState", "AbstractValue", "AddrStmt", "AddressValue",
            "GepStmt", "IntervalValue", "SVFIR", "SVFVar"):
        setattr(fake_pysvf, type_name, type(type_name, (), {}))
    fake_pysvf.Options = type("Options", (), {})
    sys.modules["pysvf"] = fake_pysvf

    fake_ae_state = types.ModuleType("AEState")
    fake_ae_state.AEState = type("AEState", (), {})
    fake_ae_state.unwrap_state = lambda state: state
    sys.modules["AEState"] = fake_ae_state

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

    buffer_checkpoint = object()
    null_checkpoint_1 = object()
    null_checkpoint_2 = object()
    reporter.noteExpectedReport("buffer-overflow", buffer_checkpoint)
    reporter.noteExpectedReport("nullptr-deref", null_checkpoint_1)
    reporter.noteExpectedReport("nullptr-deref", null_checkpoint_2)
    reporter.noteExpectedReport("nullptr-deref", null_checkpoint_1)

    assert reporter.getExpectedReportCount("buffer-overflow") == 1
    assert reporter.getExpectedReportCount("nullptr-deref") == 2
    assert reporter.getExpectedReportCount("unknown-kind") == 0


if __name__ == "__main__":
    main()
