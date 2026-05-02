#!/usr/bin/env python3
"""
Diagnose the 6 hard-overlap P80/P150 cases with additional features
to find the best tiebreaker feature(s).
"""
import os
import cv2
import numpy as np

ROI_FRACTION = 0.80

# --- Known hard cases ---
HARD_CASES = [
    # P80 misclassified as P150 (entropy below 3.815)
    ("Dataset/P80/20230524_002059_022_CH0_CLS3_ORT0_Ok.png", "P80"),
    ("Dataset/P80/20230530_162951_046_ID121_CH0_CLS2_ORT0_Ok.png", "P80"),
    ("Dataset/P80/20230531_092850_081_ID4_CH0_CLS2_ORT0_Ok.png", "P80"),
    # P150 misclassified as P80 (entropy above 3.815)
    ("Dataset/P150/20230530_100811_348_ID12_CH0_CLS3_ORT0_Ok.png", "P150"),
    ("Dataset/P150/20230530_103051_435_ID34_CH0_CLS3_ORT0_Ok.png", "P150"),
    ("Dataset/P150/20230530_103529_434_ID42_CH0_CLS3_ORT0_Ok.png", "P150"),
]


def apply_roi(image, fraction=ROI_FRACTION):
    h, w = image.shape[:2]
    crop_h = int(h * fraction)
    crop_w = int(w * fraction)
    y1 = (h - crop_h) // 2
    x1 = (w - crop_w) // 2
    return image[y1:y1 + crop_h, x1:x1 + crop_w]


def to_gray(image):
    if len(image.shape) == 3:
        return cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    return image


def mean_brightness(gray):
    return float(np.mean(gray))


def histogram_entropy(gray, bins=32):
    hist = cv2.calcHist([gray], [0], None, [bins], [0, 256])
    hist = hist.flatten()
    hist = hist / hist.sum()
    nonzero = hist[hist > 0]
    return -float(np.sum(nonzero * np.log2(nonzero)))


def sobel_mean_gradient(gray):
    sobelx = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=3)
    sobely = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=3)
    mag = np.sqrt(sobelx**2 + sobely**2)
    return float(np.mean(mag))


def sobel_std_gradient(gray):
    """Standard deviation of gradient magnitude."""
    sobelx = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=3)
    sobely = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=3)
    mag = np.sqrt(sobelx**2 + sobely**2)
    return float(np.std(mag))


def gradient_entropy(gray, bins=32):
    """Entropy of gradient magnitude image — measures edge complexity."""
    sobelx = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=3)
    sobely = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=3)
    mag = np.sqrt(sobelx**2 + sobely**2)
    # Scale to 0-255
    mag_scaled = np.clip(mag, 0, 255).astype(np.uint8)
    hist = cv2.calcHist([mag_scaled], [0], None, [bins], [0, 256])
    hist = hist.flatten()
    hist = hist / hist.sum()
    nonzero = hist[hist > 0]
    return -float(np.sum(nonzero * np.log2(nonzero)))


def canny_edge_density(gray, low=50, high=150):
    """Percentage of edge pixels using Canny edge detection."""
    edges = cv2.Canny(gray, low, high)
    return 100.0 * np.sum(edges > 0) / edges.size


def local_std_stats(gray, block_size=16):
    """Mean and std of local standard deviations (block-based)."""
    h, w = gray.shape
    block_stds = []
    for y in range(0, h - block_size + 1, block_size):
        for x in range(0, w - block_size + 1, block_size):
            block = gray[y:y + block_size, x:x + block_size]
            block_stds.append(float(np.std(block)))
    if not block_stds:
        return 0.0, 0.0
    return float(np.mean(block_stds)), float(np.std(block_stds))


def total_variation(gray):
    """Normalized total variation (L1 norm of gradient)."""
    diff_x = np.abs(np.diff(gray, axis=1))
    diff_y = np.abs(np.diff(gray, axis=0))
    tv = np.sum(diff_x) + np.sum(diff_y)
    return tv / gray.size


def laplacian_variance(gray):
    lap = cv2.Laplacian(gray, cv2.CV_64F)
    return float(lap.var())


def midrange_pixel_percentage(gray, low=50, high=150):
    total = gray.size
    mid = np.sum((gray >= low) & (gray <= high))
    return 100.0 * mid / total


def percent_bright(gray, threshold=100):
    return 100.0 * np.sum(gray > threshold) / gray.size


def analyze_image(image_path, label):
    img = cv2.imread(image_path)
    if img is None:
        print(f"Cannot read: {image_path}")
        return
    img = apply_roi(img)
    gray = to_gray(img)

    features = {
        "file": os.path.basename(image_path),
        "actual": label,
        "brightness": mean_brightness(gray),
        "entropy_32": histogram_entropy(gray, 32),
        "entropy_64": histogram_entropy(gray, 64),
        "sobel_mean": sobel_mean_gradient(gray),
        "sobel_std": sobel_std_gradient(gray),
        "gradient_entropy_32": gradient_entropy(gray, 32),
        "gradient_entropy_64": gradient_entropy(gray, 64),
        "canny_50_150": canny_edge_density(gray, 50, 150),
        "canny_30_90": canny_edge_density(gray, 30, 90),
        "local_std_mean": None,  # filled below
        "local_std_std": None,
        "total_variation": total_variation(gray),
        "laplacian_var": laplacian_variance(gray),
        "midrange_pct": midrange_pixel_percentage(gray),
        "pct_bright_100": percent_bright(gray),
    }

    lsm, lss = local_std_stats(gray, 16)
    features["local_std_mean"] = lsm
    features["local_std_std"] = lss

    return features


def main():
    print("=" * 120)
    print("  DIAGNOSTIC: Additional Features for 6 Hard-Overlap Cases")
    print("=" * 120)

    all_features = []
    for path, label in HARD_CASES:
        feat = analyze_image(path, label)
        if feat:
            all_features.append(feat)

    # Print header
    header = (
        f"{'File':50s} {'Act':>4s} "
        f"{'Brt':>6s} {'Ent32':>7s} {'Ent64':>7s} "
        f"{'SblM':>7s} {'SblS':>7s} "
        f"{'GrdE32':>7s} {'GrdE64':>7s} "
        f"{'Cny50':>7s} {'Cny30':>7s} "
        f"{'LclM':>7s} {'LclS':>7s} "
        f"{'TV':>7s} {'LapV':>8s} "
        f"{'Mid%':>6s} {'%>100':>6s}"
    )
    print(header)
    print("-" * len(header))

    for f in all_features:
        print(
            f"{f['file']:50s} {f['actual']:>4s} "
            f"{f['brightness']:6.1f} {f['entropy_32']:7.4f} {f['entropy_64']:7.4f} "
            f"{f['sobel_mean']:7.2f} {f['sobel_std']:7.2f} "
            f"{f['gradient_entropy_32']:7.4f} {f['gradient_entropy_64']:7.4f} "
            f"{f['canny_50_150']:7.2f} {f['canny_30_90']:7.2f} "
            f"{f['local_std_mean']:7.2f} {f['local_std_std']:7.2f} "
            f"{f['total_variation']:7.2f} {f['laplacian_var']:8.1f} "
            f"{f['midrange_pct']:6.1f} {f['pct_bright_100']:6.1f}"
        )

    # Also analyze a few correctly-classified cases for comparison
    print("\n" + "=" * 120)
    print("  REFERENCE: A few correctly-classified images from each class")
    print("=" * 120)

    # P80 correctly classified (entropy near threshold)
    p80_refs = [
        ("Dataset/P80/20230524_003107_032_CH0_CLS3_ORT0_Ok.png", "P80"),
        ("Dataset/P80/20230530_155704_068_ID108_CH0_CLS2_ORT0_Ok.png", "P80"),
        ("Dataset/P80/20230531_092859_080_ID5_CH0_CLS2_ORT0_Ok.png", "P80"),
    ]
    # P150 correctly classified (entropy near threshold)
    p150_refs = [
        ("Dataset/P150/20230530_100847_352_ID16_CH0_CLS3_ORT0_Ok.png", "P150"),
        ("Dataset/P150/20230530_101721_385_ID23_CH0_CLS3_ORT0_Ok.png", "P150"),
        ("Dataset/P150/20230529_145009_455_CH0_CLS3_ORT0_Ok.png", "P150"),
    ]

    ref_features = []
    for path, label in p80_refs + p150_refs:
        feat = analyze_image(path, label)
        if feat:
            ref_features.append(feat)

    print(header)
    print("-" * len(header))
    for f in ref_features:
        print(
            f"{f['file']:50s} {f['actual']:>4s} "
            f"{f['brightness']:6.1f} {f['entropy_32']:7.4f} {f['entropy_64']:7.4f} "
            f"{f['sobel_mean']:7.2f} {f['sobel_std']:7.2f} "
            f"{f['gradient_entropy_32']:7.4f} {f['gradient_entropy_64']:7.4f} "
            f"{f['canny_50_150']:7.2f} {f['canny_30_90']:7.2f} "
            f"{f['local_std_mean']:7.2f} {f['local_std_std']:7.2f} "
            f"{f['total_variation']:7.2f} {f['laplacian_var']:8.1f} "
            f"{f['midrange_pct']:6.1f} {f['pct_bright_100']:6.1f}"
        )

    # --- Feature separation analysis ---
    print("\n" + "=" * 120)
    print("  SEPARATION ANALYSIS: For each feature, check if it separates the 6 hard cases")
    print("=" * 120)
    print(f"{'Feature':25s} {'P80→P150 min':>14s} {'P80→P150 max':>14s} {'P150→P80 min':>14s} {'P150→P80 max':>14s} {'Gap?':>8s}")
    print("-" * 80)

    p80_err = [f for f in all_features if f["actual"] == "P80"]
    p150_err = [f for f in all_features if f["actual"] == "P150"]

    feature_keys = [
        "brightness", "entropy_32", "entropy_64",
        "sobel_mean", "sobel_std",
        "gradient_entropy_32", "gradient_entropy_64",
        "canny_50_150", "canny_30_90",
        "local_std_mean", "local_std_std",
        "total_variation", "laplacian_var",
        "midrange_pct", "pct_bright_100",
    ]

    for key in feature_keys:
        p80_vals = [f[key] for f in p80_err]
        p150_vals = [f[key] for f in p150_err]
        p80_min, p80_max = min(p80_vals), max(p80_vals)
        p150_min, p150_max = min(p150_vals), max(p150_vals)

        # Check if ranges overlap
        overlap = not (p80_max < p150_min or p150_max < p80_min)
        gap = "OVERLAP" if overlap else "SEPARATED"

        print(f"{key:25s} {p80_min:14.4f} {p80_max:14.4f} {p150_min:14.4f} {p150_max:14.4f} {gap:>8s}")


if __name__ == "__main__":
    main()
