# implementation.md — Architecture & Implementation Notes

This file records the project architecture, design decisions, and
implementation notes. Keep it up to date as the project evolves.

## Overview
Image classification project using OpenCV (cv2) to classify images into three groups: P50, P80, and P150.

## Architecture

### Dataset
- `Dataset/P50/` — 240 images, bright (mean brightness ~102)
- `Dataset/P80/` — 216 images, dark with texture (mean brightness ~46, entropy ~3.78)
- `Dataset/P150/` — 173 images, dark and smooth (mean brightness ~41, entropy ~3.53)

### Classification Strategy (two-stage)

**Stage 1: P50 vs (P80/P150)**
- Feature: Mean grayscale brightness
- Threshold: ~75 (P50 always > 90, others always < 75)
- Result: 100% accurate

**Stage 2: P80 vs P150**
- Feature: Entropy (texture complexity via 32-bin histogram)
- Threshold: ~3.62
- Accuracy: ~95.6% (7/216 P80 misclassified, 10/173 P150 misclassified)
- Secondary fallback features: Sobel mean gradient, mid-range pixel percentage

### Classifier Script
- `classify.py` — Main script using cv2 to classify images

## Key Files & Modules
- `classify.py` — Image classification script

## Key Decisions
- Using histogram entropy for P80 vs P150 distinction because it captures texture complexity best
- Using 32-bin histogram for entropy calculation (good balance of detail vs noise)
- Two-stage classifier for cleaner separation
