#!/usr/bin/env python3
"""Fail CI if coverage.xml (gcovr) is below threshold.

Usage:
  python scripts/enforce_coverage.py --file coverage.xml --min 70

Exits:
 0 success
 1 missing file / parse error
 2 below threshold
"""
from __future__ import annotations
import argparse, sys
import xml.etree.ElementTree as ET
from pathlib import Path

def parse_args():
    ap = argparse.ArgumentParser()
    ap.add_argument('--file', default='coverage.xml', help='Path to gcovr XML report')
    ap.add_argument('--min', type=float, required=True, help='Minimum line coverage percent required')
    return ap.parse_args()

def main() -> int:
    args = parse_args()
    path = Path(args.file)
    if not path.exists():
        print(f'coverage file {path} missing', file=sys.stderr)
        return 1
    try:
        root = ET.parse(path).getroot()
        rate = float(root.attrib['line-rate']) * 100.0
    except Exception as e:
        print(f'parse error: {e}', file=sys.stderr)
        return 1
    if rate < args.min:
        print(f'❌ Coverage {rate:.2f}% < threshold {args.min:.2f}%')
        return 2
    print(f'✅ Coverage {rate:.2f}% >= threshold {args.min:.2f}%')
    return 0

if __name__ == '__main__':
    raise SystemExit(main())
