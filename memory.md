# memory.md — Project Memory

This file stores durable project memory that persists across agent sessions.
It is always loaded as context so the agent can recall past work,
decisions, open issues, and next steps.

## Recent Changes
- Added `--p80p150` focus mode to `classify.py` that processes both P80 and P150 directories, shows per-image details (brightness + entropy), prints a confusion matrix, and lists all 6 hard-overlap misclassified cases.
- Usage: `python classify.py --p80p150 Dataset/P80 Dataset/P150`

## Status
- `classify.py` — two-stage classifier using mean brightness (Stage 1) and histogram entropy (Stage 2), with ROI cropping applied first.
- Can classify single images or entire directories of images.
- ROI crop (default 80% central region) applied before any analysis to exclude non-critical side areas.

## Decisions
- Stage 1 threshold: brightness > 86 → P50 (100% accurate; P50 min=89.2, non-P50 max=85.8).
- Stage 2 threshold: entropy > 3.815 → P80, else P150 (32-bin histogram). Accuracy: 98.5%.
- ROI: central 80% crop (10% trimmed from each side) applied as the very first step in `classify_image()`.
- ROI fraction configurable via `--roi <0.0–1.0>` flag after the path argument.
- Overall accuracy on 629 images across P50/P80/P150: 99.0% (623/629 correct).

## Hard Cases (6 images in entropy overlap zone 3.77–3.84)
P80 misclassified as P150 (3):
  - 20230524_002059_022_CH0_CLS3_ORT0_Ok.png (entropy=3.7733, sobel=68.26)
  - 20230530_162951_046_ID121_CH0_CLS2_ORT0_Ok.png (entropy=3.8028, sobel=68.89)
  - 20230531_092850_081_ID4_CH0_CLS2_ORT0_Ok.png (entropy=3.7748, sobel=71.61)
P150 misclassified as P80 (3):
  - 20230530_100811_348_ID12_CH0_CLS3_ORT0_Ok.png (entropy=3.8293, sobel=72.57)
  - 20230530_103051_435_ID34_CH0_CLS3_ORT0_Ok.png (entropy=3.8293, sobel=72.57)
  - 20230530_103529_434_ID42_CH0_CLS3_ORT0_Ok.png (entropy=3.8353, sobel=69.45)

## Open Issues / Next Steps
- The 6 hard-overlap cases (confirmed in `--p80p150` focus mode) could potentially be resolved with additional features (Sobel gradient, Laplacian variance) as a tiebreaker in the entropy overlap zone (~3.77–3.84).
- Add unit tests.
