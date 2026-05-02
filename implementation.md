# implementation.md — Architecture & Implementation Notes

This file records the project architecture, design decisions, and
implementation notes. Keep it up to date as the project evolves.

## Overview
Image classification project using OpenCV (cv2) to classify images into three groups: P50, P80, and P150.

## Focus Mode (P80 vs P150)
`classify.py` now supports a `--p80p150` flag that classifies both P80 and P150 directories in one pass, showing per-image brightness/entropy, a confusion matrix, and listing all misclassified images. Usage: `python classify.py --p80p150 Dataset/P80 Dataset/P150`

## Architecture

### Dataset
- `Dataset/P50/` — 240 images, bright (mean brightness ~116)
- `Dataset/P80/` — 216 images, darker with texture (mean brightness ~54, entropy ~4.00)
- `Dataset/P150/` — 173 images, dark and smooth (mean brightness ~49, entropy ~3.71)

### Classification Strategy (two-stage with ROI)

**Preprocessing — ROI Crop**
- Before any analysis, the image is cropped to the central region (default 80% of width and height).
- Discards non-critical side areas that could skew brightness or entropy measurements.
- Configurable via `--roi <0.0–1.0>` flag (e.g. `--roi 0.70` for a tighter crop).

**Stage 1: P50 vs (P80/P150)**
- Feature: Mean grayscale brightness (computed on ROI-cropped image)
- Threshold: 86 (P50 min brightness=89.2, non-P50 max brightness=85.8)
- Result: 100% accurate — no overlap between P50 and other groups

**Stage 2: P80 vs P150**
- Feature: Entropy (texture complexity via 32-bin histogram, computed on ROI-cropped image)
- Threshold: 3.815
- Accuracy: 98.5% (383/389 correct — 3/216 P80 misclassified, 3/173 P150 misclassified)
- The 6 hard-overlap cases have similar entropy in the 3.77–3.84 range

### Performance Summary (ROI=0.80)
| Class  | Correct/Total | Accuracy |
|--------|---------------|----------|
| P50    | 240/240       | 100.0%   |
| P80    | 213/216       | 98.6%    |
| P150   | 170/173       | 98.3%    |
| **Overall** | **623/629** | **99.0%** |

### Classifier Script
- `classify.py` — Main script using cv2 to classify images

## Key Files & Modules
- `classify.py` — Image classification script (optimized thresholds)

## Key Decisions
- ROI applied first, before any analysis, to exclude non-critical side regions.
- Default ROI fraction 0.80 (10% cropped from each side).
- P50 brightness threshold 86 (safe gap between P50 min 89.2 and non-P50 max 85.8).
- Using histogram entropy for P80 vs P150 distinction because it captures texture complexity best.
- 32-bin histogram for entropy calculation (good balance of detail vs noise).
- Entropy threshold 3.815 yields 98.5% accuracy on P80 vs P150.
- Two-stage classifier for cleaner separation.
