# implementation.md — Architecture & Implementation Notes

This file records the project architecture, design decisions, and
implementation notes. Keep it up to date as the project evolves.

## Overview
Image classification project using OpenCV (cv2) to classify images into three groups: P50, P80, and P150.

## Focus Mode (P80 vs P150)
`classify.py` now supports a `--p80p150` flag that classifies both P80 and P150 directories in one pass, showing per-image brightness/entropy/canny, a confusion matrix, and listing all misclassified images. Usage: `python classify.py --p80p150 Dataset/P80 Dataset/P150`

## Architecture

### Dataset
- `Dataset/P50/` — 240 images, bright (mean brightness ~116)
- `Dataset/P80/` — 216 images, darker with texture (mean brightness ~54, entropy ~4.00)
- `Dataset/P150/` — 173 images, dark and smooth (mean brightness ~49, entropy ~3.71)

### Classification Strategy (two-stage with Canny tiebreaker)

**Preprocessing — ROI Crop**
- Before any analysis, the image is cropped to the central region (default 80% of width and height).
- Discards non-critical side areas that could skew brightness or entropy measurements.
- Configurable via `--roi <0.0–1.0>` flag (e.g. `--roi 0.70` for a tighter crop).

**Stage 1: P50 vs (P80/P150)**
- Feature: Mean grayscale brightness (computed on ROI-cropped image)
- Threshold: 86 (P50 min brightness=89.2, non-P50 max brightness=85.8)
- Result: 100% accurate — no overlap between P50 and other groups

**Stage 2: P80 vs P150**
- Stage 2a (primary): 32-bin histogram entropy
  - entropy > 3.845 → P80
  - entropy < 3.77  → P150
  - entropy in [3.77, 3.845] → ambiguity zone, goes to Stage 2b
- Stage 2b (tiebreaker): Canny edge density (Canny 50/150)
  - canny ≤ 16.0% → P80 (unusually smooth P80)
  - canny > 16.0%  → P150 (unusually textured P150)
- The 6 original hard-overlap cases have a clean gap in canny edge density:
  - P80→P150 misclassified: canny 14.4–15.4%
  - P150→P80 misclassified: canny 16.7–17.5%
- 28 images total fall in the [3.77, 3.845] ambiguity zone; canny tiebreaker correctly classifies all 28.

### Performance Summary (ROI=0.80)
| Class  | Correct/Total | Accuracy |
|--------|---------------|----------|
| P50    | 240/240       | 100.0%   |
| P80    | 216/216       | 100.0%   |
| P150   | 173/173       | 100.0%   |
| **Overall** | **629/629** | **100.0%** |

### Classifier Script
- `classify.py` — Main script using cv2 to classify images

## Key Files & Modules
- `classify.py` — Image classification script (entropy + Canny tiebreaker)
- `classify_v2.py` — Experimental script testing multiple alternative strategies
- `analyze_features.py` — Feature distribution analysis across all three classes
- `diagnose_hard_cases.py` — Diagnostic tool for analyzing hard cases with extra features

## Key Decisions
- ROI applied first, before any analysis, to exclude non-critical side regions.
- Default ROI fraction 0.80 (10% cropped from each side).
- P50 brightness threshold 86 (safe gap between P50 min 89.2 and non-P50 max 85.8).
- Using histogram entropy for P80 vs P150 distinction because it captures texture complexity best.
- 32-bin histogram for entropy calculation (good balance of detail vs noise).
- Canny edge density (thresholds 50/150) as tiebreaker in entropy ambiguity zone [3.77, 3.845].
- Canny threshold 16.0% chosen with a clean 1.3% gap from the nearest misclassified case on each side.
- Two-stage (three-step) classifier achieves 100% accuracy across all 629 images.
