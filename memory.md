# memory.md — Project Memory

This file stores durable project memory that persists across agent sessions.
It is always loaded as context so the agent can recall past work,
decisions, open issues, and next steps.

## Recent Changes
- **2025-01-21**: Built out `QtClassify/QtClassify/` C++/QML application using Qt 6.10.2 and OpenCV 4.13.0. Full three-stage classifier (brightness → entropy → Canny tiebreaker) ported from `classify.py` to C++. QML UI with single-image mode (preview + results) and batch-directory mode (progress bar + results table). Builds successfully on macOS arm64.
- **2025-01-21**: Added Canny edge-density tiebreaker to `classify.py` Stage 2, resolving all 6 hard-overlap P80/P150 cases. Achieves **100% accuracy** across all 629 images (P50: 240/240, P80: 216/216, P150: 173/173).

## Status
- `classify.py` — three-step classifier using mean brightness (Stage 1), histogram entropy (Stage 2a), and Canny edge density tiebreaker (Stage 2b), with ROI cropping applied first.
- **QtClassify C++/QML application** — fully built and compiles clean.
  - `QtClassify/QtClassify/classifier.h` / `.cpp` — classification logic (identical thresholds to `classify.py`).
  - `QtClassify/QtClassify/classifierbackend.h` / `.cpp` — QObject bridge exposing classifier to QML.
  - `QtClassify/QtClassify/Main.qml` — dark-themed UI with Single Image and Batch Directory tabs.
  - `QtClassify/QtClassify/main.cpp` — entry point, registers `ClassifierBackend` as QML context property.
  - Build: `cmake -S QtClassify/QtClassify -B QtClassify/QtClassify/build/Debug -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6 -DOpenCV_DIR=/opt/homebrew/opt/opencv -G Ninja && cmake --build QtClassify/QtClassify/build/Debug`
- **All 629 images classified correctly (100.0% accuracy).**

## Decisions
- Stage 1 threshold: brightness > 86 → P50 (100% accurate; P50 min=89.2, non-P50 max=85.8).
- Stage 2a: entropy > 3.845 → P80, entropy < 3.77 → P150, else ambiguity zone.
- Stage 2b (Canny tiebreaker): canny edge density > 16.0% → P150, else P80.
  - Canny thresholds: low=50, high=150.
  - Ambiguity zone [3.77, 3.845] contains 28 images; all correctly classified by the tiebreaker.
- ROI: central 80% crop (10% trimmed from each side) applied as the very first step.
- QtClassify app: ROI configurable via spinbox (10–100%, default 80%).
- QtClassify UI: dark theme (#1e1e2e base), P50=green, P80=cyan, P150=orange, ERROR=red.

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
- Add unit tests for the C++ classifier.
- Run the QtClassify app against Dataset/ to verify 100% accuracy from C++ side.
- Consider adding drag-and-drop support to the QML UI.
- The `classify_v2.py` experimental strategies could be cleaned up.
