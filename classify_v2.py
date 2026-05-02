#!/usr/bin/env python3
"""
classify_v2.py — Improved two-stage image classifier with multiple enhancement strategies.

Usage:
    python classify_v2.py <directory> --strategy <name>

Strategies:
    1. optimize_entropy_threshold  — try entropy thresholds 3.70-3.85
    2. dual_feature_sobel          — entropy + Sobel gradient composite score
    3. triple_feature              — entropy + Sobel + midrange% composite
    4. adaptive_threshold          — brightness-band-aware entropy threshold
    5. multi_bin_entropy           — try 16, 32, 64 bins
    6. roi_tuning                  — try ROI 0.70, 0.80, 0.90
    7. confidence_scoring          — ensemble scoring with confidence levels
"""

import os, sys
import cv2
import numpy as np

ROI_FRACTION = 0.80
STRATEGY = "baseline"


def apply_roi(image, fraction=ROI_FRACTION):
    h, w = image.shape[:2]
    crop_h = int(h * fraction)
    crop_w = int(w * fraction)
    y1 = (h - crop_h) // 2
    x1 = (w - crop_w) // 2
    return image[y1:y1 + crop_h, x1:x1 + crop_w]


def mean_brightness(image):
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image
    return float(np.mean(gray))


def histogram_entropy(image, bins=32):
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image
    hist = cv2.calcHist([gray], [0], None, [bins], [0, 256])
    hist = hist.flatten()
    hist = hist / hist.sum()
    nonzero = hist[hist > 0]
    return -float(np.sum(nonzero * np.log2(nonzero)))


def sobel_mean_gradient(image):
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image
    sobelx = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=3)
    sobely = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=3)
    mag = np.sqrt(sobelx**2 + sobely**2)
    return float(np.mean(mag))


def laplacian_variance(image):
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image
    lap = cv2.Laplacian(gray, cv2.CV_64F)
    return float(lap.var())


def midrange_pixel_percentage(image, low=50, high=150):
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image
    total = gray.size
    mid = np.sum((gray >= low) & (gray <= high))
    return 100.0 * mid / total


def percent_bright(image, threshold=100):
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image
    return 100.0 * np.sum(gray > threshold) / gray.size


# ============================================================
# Strategy implementations
# ============================================================

def classify_image_baseline(image_path):
    """Original classifier (entropy threshold 3.62)."""
    img = cv2.imread(image_path)
    if img is None:
        raise ValueError(f"Cannot read image: {image_path}")
    img = apply_roi(img, ROI_FRACTION)
    brightness = mean_brightness(img)
    if brightness > 75.0:
        return "P50"
    entropy = histogram_entropy(img, bins=32)
    if entropy > 3.62:
        return "P80"
    return "P150"


def classify_image_entropy_threshold(image_path, threshold=3.80):
    """Strategy 1: Tuned entropy threshold."""
    img = cv2.imread(image_path)
    if img is None:
        raise ValueError(f"Cannot read image: {image_path}")
    img = apply_roi(img, ROI_FRACTION)
    brightness = mean_brightness(img)
    if brightness > 75.0:
        return "P50"
    entropy = histogram_entropy(img, bins=32)
    if entropy > threshold:
        return "P80"
    return "P150"


def classify_image_dual_feature(image_path):
    """
    Strategy 2: Dual-feature score combining entropy and Sobel gradient.
    Normalize both to z-scores and use a weighted sum.
    """
    # Reference stats from P80/P150 analysis (ROI=0.80)
    ENTROPY_MEAN_P150, ENTROPY_STD_P150 = 3.707, 0.067
    ENTROPY_MEAN_P80, ENTROPY_STD_P80 = 4.001, 0.074
    SOBEL_MEAN_P150, SOBEL_STD_P150 = 65.259, 4.646
    SOBEL_MEAN_P80, SOBEL_STD_P80 = 77.181, 4.818

    img = cv2.imread(image_path)
    if img is None:
        raise ValueError(f"Cannot read image: {image_path}")
    img = apply_roi(img, ROI_FRACTION)

    brightness = mean_brightness(img)
    if brightness > 75.0:
        return "P50"

    entropy = histogram_entropy(img, bins=32)
    sobel = sobel_mean_gradient(img)

    # Likelihood scores: how many std from P150 mean vs P80 mean
    # Positive = more P80-like, negative = more P150-like
    score_entropy = ((entropy - ENTROPY_MEAN_P150) / ENTROPY_STD_P150 -
                     (entropy - ENTROPY_MEAN_P80) / ENTROPY_STD_P80)
    score_sobel = ((sobel - SOBEL_MEAN_P150) / SOBEL_STD_P150 -
                   (sobel - SOBEL_MEAN_P80) / SOBEL_STD_P80)

    # Weighted: entropy (weight 0.7) + sobel (weight 0.3)
    composite = 0.7 * score_entropy + 0.3 * score_sobel
    return "P80" if composite > 0 else "P150"


def classify_image_triple_feature(image_path):
    """
    Strategy 3: Triple-feature score (entropy + Sobel + midrange% + %bright>100).
    """
    ENTROPY_MEAN, ENTROPY_STD = 3.707, 0.067
    SOBEL_MEAN, SOBEL_STD = 65.259, 4.646
    MID_MEAN, MID_STD = 27.893, 2.239
    PCTB_MEAN, PCTB_STD = 8.633, 2.531

    img = cv2.imread(image_path)
    if img is None:
        raise ValueError(f"Cannot read image: {image_path}")
    img = apply_roi(img, ROI_FRACTION)

    brightness = mean_brightness(img)
    if brightness > 75.0:
        return "P50"

    entropy = histogram_entropy(img, bins=32)
    sobel = sobel_mean_gradient(img)
    mid = midrange_pixel_percentage(img)
    pctb = percent_bright(img, 100)

    # Z-scores relative to P150 mean (higher = more P80-like)
    z_entropy = (entropy - ENTROPY_MEAN) / ENTROPY_STD
    z_sobel = (sobel - SOBEL_MEAN) / SOBEL_STD
    z_mid = (mid - MID_MEAN) / MID_STD
    z_pctb = (pctb - PCTB_MEAN) / PCTB_STD

    composite = (0.5 * z_entropy + 0.25 * z_sobel +
                 0.15 * z_mid + 0.10 * z_pctb)
    return "P80" if composite > 1.5 else "P150"


def classify_image_adaptive_threshold(image_path):
    """
    Strategy 4: Adaptive entropy threshold based on brightness band.
    Darker images need a lower entropy threshold; brighter ones need higher.
    """
    img = cv2.imread(image_path)
    if img is None:
        raise ValueError(f"Cannot read image: {image_path}")
    img = apply_roi(img, ROI_FRACTION)

    brightness = mean_brightness(img)
    if brightness > 75.0:
        return "P50"

    entropy = histogram_entropy(img, bins=32)
    sobel = sobel_mean_gradient(img)

    # Adaptive threshold: brighter images use higher entropy threshold
    # Base threshold 3.75, adjusting by brightness
    if brightness < 43:
        adj_threshold = 3.70
    elif brightness < 48:
        adj_threshold = 3.75
    elif brightness < 55:
        adj_threshold = 3.78
    else:
        adj_threshold = 3.80

    # Also use Sobel as tiebreaker near boundary
    if entropy > adj_threshold + 0.05:
        return "P80"
    elif entropy < adj_threshold - 0.05:
        return "P150"
    else:
        # Tiebreaker zone: use Sobel gradient
        return "P80" if sobel > 68 else "P150"


def classify_image_multi_bin_entropy(image_path):
    """
    Strategy 5: Use 64-bin entropy for better resolution.
    Threshold tuned for 64-bin.
    """
    img = cv2.imread(image_path)
    if img is None:
        raise ValueError(f"Cannot read image: {image_path}")
    img = apply_roi(img, ROI_FRACTION)

    brightness = mean_brightness(img)
    if brightness > 75.0:
        return "P50"

    entropy64 = histogram_entropy(img, bins=64)
    if entropy64 > 3.90:
        return "P80"
    return "P150"


def classify_image_confidence_scoring(image_path):
    """
    Strategy 7: Ensemble confidence scoring.
    Uses multiple features and multiple bin counts, votes on result.
    """
    img = cv2.imread(image_path)
    if img is None:
        raise ValueError(f"Cannot read image: {image_path}")
    img = apply_roi(img, ROI_FRACTION)

    brightness = mean_brightness(img)
    if brightness > 75.0:
        return "P50"

    entropy32 = histogram_entropy(img, bins=32)
    entropy64 = histogram_entropy(img, bins=64)
    entropy16 = histogram_entropy(img, bins=16)
    sobel = sobel_mean_gradient(img)
    mid = midrange_pixel_percentage(img)
    pctb = percent_bright(img)

    # Votes: each classifier casts a vote
    votes = {"P80": 0, "P150": 0}

    # Vote 1: entropy32 > 3.75
    if entropy32 > 3.75:
        votes["P80"] += 1
    else:
        votes["P150"] += 1

    # Vote 2: entropy64 > 4.0 (64-bin tends to be higher)
    if entropy64 > 4.0:
        votes["P80"] += 1
    else:
        votes["P150"] += 1

    # Vote 3: entropy16 > 3.5
    if entropy16 > 3.5:
        votes["P80"] += 1
    else:
        votes["P150"] += 1

    # Vote 4: Sobel gradient > 68
    if sobel > 68:
        votes["P80"] += 1
    else:
        votes["P150"] += 1

    # Vote 5: Midrange > 28%
    if mid > 28:
        votes["P80"] += 1
    else:
        votes["P150"] += 1

    # Vote 6: % bright > 100 threshold
    if pctb > 9:
        votes["P80"] += 1
    else:
        votes["P150"] += 1

    # Vote 7: Laplacian variance > 160
    lap_var = laplacian_variance(img)
    if lap_var > 160:
        votes["P80"] += 1
    else:
        votes["P150"] += 1

    return max(votes, key=votes.get)


def classify_single(image_path):
    """Dispatch to the selected strategy."""
    global STRATEGY
    if STRATEGY == "baseline":
        return classify_image_baseline(image_path)
    elif STRATEGY == "entropy_3.70":
        return classify_image_entropy_threshold(image_path, 3.70)
    elif STRATEGY == "entropy_3.75":
        return classify_image_entropy_threshold(image_path, 3.75)
    elif STRATEGY == "entropy_3.78":
        return classify_image_entropy_threshold(image_path, 3.78)
    elif STRATEGY == "entropy_3.80":
        return classify_image_entropy_threshold(image_path, 3.80)
    elif STRATEGY == "entropy_3.82":
        return classify_image_entropy_threshold(image_path, 3.82)
    elif STRATEGY == "entropy_3.85":
        return classify_image_entropy_threshold(image_path, 3.85)
    elif STRATEGY == "dual_feature":
        return classify_image_dual_feature(image_path)
    elif STRATEGY == "triple_feature":
        return classify_image_triple_feature(image_path)
    elif STRATEGY == "adaptive":
        return classify_image_adaptive_threshold(image_path)
    elif STRATEGY == "multi_bin":
        return classify_image_multi_bin_entropy(image_path)
    elif STRATEGY == "confidence":
        return classify_image_confidence_scoring(image_path)
    else:
        raise ValueError(f"Unknown strategy: {STRATEGY}")


def classify_directory(directory, label_name):
    """Classify all images and return stats."""
    valid_exts = {".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".tif"}
    results = {"P50": 0, "P80": 0, "P150": 0, "ERROR": 0}
    total = 0
    misclassified = []

    for fname in sorted(os.listdir(directory)):
        ext = os.path.splitext(fname)[1].lower()
        if ext not in valid_exts:
            continue
        fpath = os.path.join(directory, fname)
        total += 1
        try:
            label = classify_single(fpath)
        except Exception:
            label = "ERROR"
        results[label] += 1
        if label != label_name:
            misclassified.append((fname, label))

    return total, results, misclassified


def test_strategy(strategy_name, roi=0.80):
    """Test a strategy across all datasets."""
    global STRATEGY, ROI_FRACTION
    STRATEGY = strategy_name
    ROI_FRACTION = roi

    base = "Dataset"
    datasets = [
        ("P50", os.path.join(base, "P50")),
        ("P80", os.path.join(base, "P80")),
        ("P150", os.path.join(base, "P150")),
    ]

    print(f"\n{'='*60}")
    print(f"  Strategy: {strategy_name}  (ROI={roi})")
    print(f"{'='*60}")

    total_correct = 0
    total_images = 0

    for label_name, dir_path in datasets:
        total, results, misclassified = classify_directory(dir_path, label_name)
        correct = results[label_name]
        pct = 100.0 * correct / total if total else 0.0
        total_correct += correct
        total_images += total

        print(f"  {label_name:5s}: {correct:3d}/{total:3d} correct ({pct:5.1f}%)  "
              f"(P50={results['P50']}, P80={results['P80']}, P150={results['P150']})")
        if misclassified:
            for fname, pred in misclassified[:5]:
                print(f"    MIS: {fname} → {pred}")
            if len(misclassified) > 5:
                print(f"    ... and {len(misclassified)-5} more")

    overall = 100.0 * total_correct / total_images if total_images else 0.0
    print(f"  TOTAL   : {total_correct:3d}/{total_images:3d} correct ({overall:5.1f}%)")
    return total_correct, total_images


if __name__ == "__main__":
    # Test all strategies
    strategies = [
        "baseline",
        "entropy_3.70",
        "entropy_3.75",
        "entropy_3.78",
        "entropy_3.80",
        "entropy_3.82",
        "entropy_3.85",
        "dual_feature",
        "triple_feature",
        "adaptive",
        "multi_bin",
        "confidence",
    ]

    rois = [0.80]

    best_acc = 0
    best = None

    for roi in rois:
        for strat in strategies:
            correct, total = test_strategy(strat, roi)
            acc = 100.0 * correct / total
            if acc > best_acc:
                best_acc = acc
                best = (strat, roi, correct, total, acc)

    print(f"\n{'='*60}")
    print(f"  BEST: {best[0]} (ROI={best[1]}) → {best[2]}/{best[3]} = {best[4]:.1f}%")
    print(f"{'='*60}")
