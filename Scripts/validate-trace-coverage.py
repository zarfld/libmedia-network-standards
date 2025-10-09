#!/usr/bin/env python3
"""Validate traceability coverage thresholds.

Reads build/traceability.json and enforces minimum linkage coverage for:
 - Requirements (REQ-*) overall
 - ADR linkage (percent requirements with ≥1 ADR)
 - Scenario linkage (percent requirements with ≥1 scenario)
 - Test linkage (percent requirements with ≥1 test)

The traceability.json generator must include a metrics block similar to:
{
    "metrics": {
        "requirement": {"coverage_pct": 82.0},
        "requirement_to_ADR": {"coverage_pct": 75.0},
        "requirement_to_scenario": {"coverage_pct": 60.0},
        "requirement_to_test": {"coverage_pct": 40.0}
    }
}

Exit Codes:
    0 - All thresholds satisfied
    1 - Missing or malformed traceability.json
    2 - Requirements coverage threshold failed
    3 - ADR linkage threshold failed
    4 - Scenario linkage threshold failed
    5 - Test linkage threshold failed

Usage:
    python scripts/validate-trace-coverage.py \
        --min-req 80 --min-req-adr 70 --min-req-scenario 50 --min-req-test 40
"""
from __future__ import annotations
import argparse, json, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TRACE_JSON = ROOT / 'build' / 'traceability.json'

def parse_args():
    ap = argparse.ArgumentParser()
    ap.add_argument('--min-req', type=float, default=80.0, help='Minimum percent of requirements with at least one link (any)')
    ap.add_argument('--min-req-adr', type=float, default=70.0, help='Minimum percent of requirements linked to ≥1 ADR')
    ap.add_argument('--min-req-scenario', type=float, default=60.0, help='Minimum percent of requirements linked to ≥1 scenario')
    ap.add_argument('--min-req-test', type=float, default=40.0, help='Minimum percent of requirements linked to ≥1 test')
    return ap.parse_args()

def main() -> int:
    args = parse_args()
    if not TRACE_JSON.exists():
        print('traceability.json missing; ensure spec-generation job ran first', file=sys.stderr)
        return 1
    data = json.loads(TRACE_JSON.read_text(encoding='utf-8'))
    metrics = data.get('metrics', {})
    def get_cov(key_aliases):
        for k in key_aliases:
            if k in metrics and isinstance(metrics[k], dict):
                val = metrics[k].get('coverage_pct')
                if val is not None:
                    return val
        return None

    overall = get_cov(['requirement','REQ','REQ-'])
    adr_cov = get_cov(['requirement_to_ADR','req_to_adr'])
    scen_cov = get_cov(['requirement_to_scenario','req_to_scenario'])
    test_cov = get_cov(['requirement_to_test','req_to_test'])

    if overall is None:
        print('No overall requirement coverage metrics found', file=sys.stderr)
        return 1

    status = 0
    if overall < args.min_req:
        print(f"❌ Requirements overall coverage {overall:.2f}% < {args.min_req:.2f}%")
        status = max(status, 2)
    else:
        print(f"✅ Requirements overall coverage {overall:.2f}% >= {args.min_req:.2f}%")

    if adr_cov is not None:
        if adr_cov < args.min_req_adr:
            print(f"❌ ADR linkage coverage {adr_cov:.2f}% < {args.min_req_adr:.2f}%")
            status = max(status, 3)
        else:
            print(f"✅ ADR linkage coverage {adr_cov:.2f}% >= {args.min_req_adr:.2f}%")
    else:
        print('⚠️ No ADR linkage metric present')

    if scen_cov is not None:
        if scen_cov < args.min_req_scenario:
            print(f"❌ Scenario linkage coverage {scen_cov:.2f}% < {args.min_req_scenario:.2f}%")
            status = max(status, 4)
        else:
            print(f"✅ Scenario linkage coverage {scen_cov:.2f}% >= {args.min_req_scenario:.2f}%")
    else:
        print('⚠️ No Scenario linkage metric present')

    if test_cov is not None:
        if test_cov < args.min_req_test:
            print(f"❌ Test linkage coverage {test_cov:.2f}% < {args.min_req_test:.2f}%")
            status = max(status, 5)
        else:
            print(f"✅ Test linkage coverage {test_cov:.2f}% >= {args.min_req_test:.2f}%")
    else:
        print('⚠️ No Test linkage metric present')

    return status

if __name__ == '__main__':
    raise SystemExit(main())
