# memory.md — Project Memory

This file stores durable project memory that persists across agent sessions.
It is always loaded as context so the agent can recall past work,
decisions, open issues, and next steps.

## Recent Changes
- **2025-01-21**: Added Canny edge-density tiebreaker to `classify.py` Stage 2, resolving all 6 hard-overlap P80/P150 cases. Achieves **100% accuracy** across all 629 images (P50: 240/240, P80: 216/216, P150: 173/173).
- Usage: `python classify.py --p80p150 Dataset/P80 Dataset/P150`
- The `--p80p150` output now shows canny edge density and tiebreaker notes (← FIXED!, ← tie OK).

## Status
- `classify.py` — three-step classifier using mean brightness (Stage 1), histogram entropy (Stage 2a), and Canny edge density tiebreaker (Stage 2b), with ROI cropping applied first.
- Can classify single images or entire directories of images.
- ROI crop (default 80% central region) applied before any analysis to exclude non-critical side areas.
- **All 629 images classified correctly (100.0% accuracy).**

## Decisions
- Stage 1 threshold: brightness > 86 → P50 (100% accurate; P50 min=89.2, non-P50 max=85.8).
- Stage 2a: entropy > 3.845 → P80, entropy < 3.77 → P150, else ambiguity zone.
- Stage 2b (Canny tiebreaker): canny edge density > 16.0% → P150, else P80.
  - Canny thresholds: low=50, high=150.
  - Ambiguity zone [3.77, 3.845] contains 28 images; all correctly classified by the tiebreaker.
  - Of those 28, 6 were originally misclassified by entropy alone (now all fixed).
- ROI: central 80% crop (10% trimmed from each side) applied as the very first step in `classify_image()`.
- ROI fraction configurable via `--roi <0.0–1.0>` flag after the path argument.

## Resolved Hard Cases (all 6 now correctly classified by Canny tiebreaker)
P80 (was misclassified as P150, now fixed):
  - 20230524_002059_022_CH0_CLS3_ORT0_Ok.png (entropy=3.7733, canny=14.45)
  - 20230530_162951_046_ID121_CH0_CLS2_ORT0_Ok.png (entropy=3.8028, canny=14.80)
  - 20230531_092850_081_ID4_CH0_CLS2_ORT0_Ok.png (entropy=3.7748, canny=15.40)
P150 (was misclassified as P80, now fixed):
  - 20230530_100811_348_ID12_CH0_CLS3_ORT0_Ok.png (entropy=3.8293, canny=17.45)
  - 20230530_103051_435_ID34_CH0_CLS3_ORT0_Ok.png (entropy=3.8293, canny=17.41)
  - 20230530_103529_434_ID42_CH0_CLS3_ORT0_Ok.png (entropy=3.8353, canny=16.72)

## Open Issues / Next Steps
- Add unit tests.
- Consider the 3 P150 images with entropy 3.766–3.768 and low canny (14.9–15.9) that sit just below the ambiguity zone. They are correctly classified by entropy alone, but represent a boundary case worth monitoring.
- The `classify_v2.py` experimental strategies could be cleaned up or removed now that the main classifier achieves 100%.
