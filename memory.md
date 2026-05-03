# memory.md — Project Memory

This file stores durable project memory that persists across agent sessions.
It is always loaded as context so the agent can recall past work,
decisions, open issues, and next steps.

## Recent Changes
- **2026-05-02 (batch dir picker fix)**: Fixed `Main.qml` `batchDirDialog` to
  use `currentFolder` as fallback when `selectedFolder` is empty. On macOS,
  `FileDialog.OpenFolder` can fail to populate `selectedFolder` when the user
  navigates INTO a directory and clicks Open, causing the dialog to select a
  file (image) inside the directory instead of the directory itself. The fix
  ensures the correct directory path is always captured.
- **2026-05-02 (later)**: Diagnosed the real root cause of the macOS crash. The
  crash persisted on the main thread even after setting `QSG_RENDER_LOOP=basic`
  because the actual issue is a **libpng symbol conflict** between Homebrew's
  `libpng16.16.dylib` (loaded transitively by OpenCV) and Apple's internal libpng
  inside ImageIO. When CoreText renders emoji glyphs (CopyEmojiImage), ImageIO's
  PNG plugin resolves libpng functions to Homebrew's incompatible symbols,
  corrupting a function pointer to the freed-memory sentinel `0xbad4007`.
- **Fix**: (1) Removed all emoji characters from `Main.qml` (🔬🖼📁📂🔄⏳✕⚡)
  to avoid triggering the CopyEmojiImage→ImageIO→libpng path. (2) Added
  `-Wl,-twolevel_namespace -Wl,-bind_at_load` linker flags in `CMakeLists.txt`
  to enforce strict symbol resolution. (3) Updated the comment in `main.cpp` to
  accurately describe the root cause.
- **2026-05-02**: Fixed crash (macOS 26.4.1 Apple Silicon). Initially diagnosed
  as QSGRenderThread thread-safety with CoreText/ImageIO. Set
  `QSG_RENDER_LOOP=basic`. This was partially correct — the basic render loop
  avoids thread-safety issues, but the deeper issue is the libpng symbol
  conflict (see above).
- **2025-01-21**: Built out `QtClassify/QtClassify/` C++/QML application using
  Qt 6.10.2 and OpenCV 4.13.0. Full three-stage classifier (brightness →
  entropy → Canny tiebreaker) ported from `classify.py` to C++. QML UI with
  single-image mode (preview + results) and batch-directory mode (progress bar
  + results table). Builds successfully on macOS arm64.
- **2025-01-21**: Added Canny edge-density tiebreaker to `classify.py` Stage 2,
  resolving all 6 hard-overlap P80/P150 cases. Achieves **100% accuracy**
  across all 629 images (P50: 240/240, P80: 216/216, P150: 173/173).

## Status
- `classify.py` — three-step classifier using mean brightness (Stage 1),
  histogram entropy (Stage 2a), and Canny edge density tiebreaker (Stage 2b),
  with ROI cropping applied first.
- **QtClassify C++/QML application** — fully built and compiled.
  - Emoji crash fix applied (see Recent Changes). App should now launch
    without crashing on macOS 26.4.1 / Apple Silicon.
  - `QtClassify/QtClassify/classifier.h` / `.cpp` — classification logic
    (identical thresholds to `classify.py`).
  - `QtClassify/QtClassify/classifierbackend.h` / `.cpp` — QObject bridge
    exposing classifier to QML.
  - `QtClassify/QtClassify/Main.qml` — dark-themed UI, emoji-free.
  - `QtClassify/QtClassify/main.cpp` — entry point with render loop fix.
  - `QtClassify/QtClassify/CMakeLists.txt` — build with linker hardening.
- **All 629 images classified correctly (100.0% accuracy).**

## macOS Crash Root Cause (libpng symbol conflict)
- Homebrew OpenCV links against Homebrew `libpng16.16.dylib`.
- macOS ImageIO framework contains its own internal libpng for PNG decoding
  (used when CoreText renders Apple Color Emoji glyphs as PNG bitmaps).
- At runtime, both Homebrew libpng and system libpng are loaded. When ImageIO's
  PNG plugin calls libpng API functions, dyld may resolve them to Homebrew's
  symbols instead of the system's (due to flat-namespace lookups within
  ImageIO's plugin architecture).
- The two libpng builds have incompatible internal struct layouts, causing
  corrupted function pointers → jump to `0xbad4007` (malloc freed-memory
  sentinel) → `EXC_BAD_ACCESS` SIGBUS (`EXC_ARM_DA_ALIGN`).
- The crash occurs even with `QSG_RENDER_LOOP=basic` because the symbol
  conflict is independent of threading — it's a linking/dyld issue.
- Fix: remove emoji from QML (avoids triggering the path) + linker hardening.

## Decisions
- Stage 1 threshold: brightness > 86 → P50 (100% accurate; P50 min=89.2,
  non-P50 max=85.8).
- Stage 2a: entropy > 3.845 → P80, entropy < 3.77 → P150, else ambiguity zone.
- Stage 2b (Canny tiebreaker): canny edge density > 16.0% → P150, else P80.
  - Canny thresholds: low=50, high=150.
  - Ambiguity zone [3.77, 3.845] contains 28 images; all correctly classified
    by the tiebreaker.
- ROI: central 80% crop (10% trimmed from each side) applied as the very first step.
- QtClassify app: ROI configurable via spinbox (10–100%, default 80%).
- QtClassify UI: dark theme (#1e1e2e base), P50=green, P80=cyan, P150=orange,
  ERROR=red.
- QtClassify UI: all emoji characters removed to avoid macOS libpng crash.

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
- Rebuild and re-run the QtClassify app to confirm the emoji removal + linker
  hardening fixes the crash.
- Add unit tests for the C++ classifier.
- Run the QtClassify app against Dataset/ to verify 100% accuracy from C++ side.
- Consider adding drag-and-drop support to the QML UI.
- The `classify_v2.py` experimental strategies could be cleaned up.


### 🐛 Known Issue (2026-05-03 06:29:27 UTC)
**2026-05-02 (batch dir picker — real fix)**: The batch directory picker was using
  `FileDialog` with `fileMode: FileDialog.OpenFolder`.  In Qt 6, `FileDialog`
  from `QtQuick.Dialogs` does **not** have an `OpenFolder` file mode — the valid
  modes are `OpenFile`, `OpenFiles`, and `SaveFile`.  The `OpenFolder` enum value
  is undefined, so the dialog silently fell back to file-selection mode.  On
  macOS this meant the dialog was selecting whatever image file was highlighted
  inside the directory rather than the directory itself.  **Fix**: replaced
  `FileDialog` with `FolderDialog` (also from `QtQuick.Dialogs`, introduced in
  Qt 6.4) for `batchDirDialog` in `Main.qml`.  Kept the `selectedFolder` →
  `currentFolder` fallback for robustness on macOS.

### 🧠 Decision (2026-05-03 07:02:29 UTC)
**2026-05-03 (batch threading)**: Moved batch classification (`classifyDirectory`) off
  the main thread to prevent GUI freezes. Created `ClassifierWorker : QObject`
  in `classifierbackend.h/.cpp` that runs in a `QThread`. The worker emits
  `started`, `progress`, `resultReady`, and `finished` signals; the backend
  connects them on the main thread to update the `ClassificationResultModel`
  and progress bar live. `ClassificationResult` is now registered as a Qt
  meta-type via `Q_DECLARE_METATYPE` + `qRegisterMetaType` for cross-thread
  signal delivery. `ClassifierBackend` gained `m_workerThread`, `m_worker`,
  `m_batchRunning`, and `stopWorkerThread()` for lifecycle management.
  Single-image `classifyCurrentImage()` remains on the main thread (fast
  enough for one image).

### 🧠 Decision (2026-05-03 08:40:40 UTC)
**2026-05-03 (batch-to-single click-through)**: Added click-to-view feature in batch
  results `ListView`. Each batch result row now has a `MouseArea` that, on click,
  switches `swipeView.currentIndex` to 0 (Single Image tab) and sets
  `backend.imagePath` to the clicked image's full path. Since `setImagePath()`
  auto-calls `classifyCurrentImage()`, the single-image preview and result card
  populate immediately. Also fixed `TabButton.checked` bindings: both tabs now
  bind `checked` to `swipeView.currentIndex === 0` / `=== 1` instead of
  hardcoding `checked: true` on singleTab, so the tab highlight correctly follows
  the active page when switching programmatically. Added a subtle blue hover
  highlight (`#7c8aff20`) and `Qt.PointingHandCursor` to indicate rows are
  clickable.

### 🧠 Decision (2026-05-03 09:42:41 UTC)
**2026-05-03 (batch sort fix)**: Fixed batch results column sorting not working. Root cause: (1) `addResult()` always appended to end, breaking any active sort order during batch processing when new results arrived from the worker thread. (2) QML local `sortColumn`/`sortAscending` properties were never reset on Clear/Classify All, causing visual indicator state drift. Fix: (1) `addResult()` now uses `std::lower_bound` with a shared `resultLessThan()` comparator to insert at the correct sorted position when `m_sortColumn >= 0`. Extracted comparator into anonymous-namespace helper and refactored `sortByColumn()` to use it too. (2) QML "Classify All" and "Clear" button handlers now reset `sortColumn = -1; sortAscending = true`. Build verified clean.