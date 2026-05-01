#!/usr/bin/env python3
"""
classify.py — Two-stage image classifier for P50, P80, and P150 (ROI-cropped).

Usage:
    python classify.py                          # Show this help
    python classify.py <image_path>             # Classify a single image
    python classify.py <directory_path>         # Classify all images in a directory
    python classify.py <path> --roi <0.0-1.0>   # Override ROI central fraction

Classification Strategy:
    Stage 1: P50 vs (P80/P150)  — mean grayscale brightness (threshold ~75)
    Stage 2: P80 vs P150        — histogram entropy (threshold ~3.62, 32 bins)

ROI: The image is cropped to a central region (default 80% of width and height)
     before any analysis, discarding non-critical side areas that could skew
     brightness or entropy calculations.
"""

import os
import sys
import cv2
import numpy as np

ROI_FRACTION = 0.80  # fraction of width and height to keep from the centre


def apply_roi(image: np.ndarray, fraction: float = ROI_FRACTION) -> np.ndarray:
    """Crop *image* to the central *fraction* of width and height.

    Crops away (1 - fraction)/2 from each side.  fraction=1.0 keeps the
    full image; fraction=0.5 keeps the middle half.
    """
    h, w = image.shape[:2]
    crop_h = int(h * fraction)
    crop_w = int(w * fraction)
    y1 = (h - crop_h) // 2
    x1 = (w - crop_w) // 2
    return image[y1:y1 + crop_h, x1:x1 + crop_w]


def mean_brightness(image: np.ndarray) -> float:
    """Return the mean grayscale brightness of an image."""
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image
    return float(np.mean(gray))


def histogram_entropy(image: np.ndarray, bins: int = 32) -> float:
    """Compute entropy of a grayscale image histogram using *bins* bins."""
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image

    hist = cv2.calcHist([gray], [0], None, [bins], [0, 256])
    hist = hist.flatten()
    hist = hist / hist.sum()  # normalise to probability distribution

    # Avoid log(0) by masking zero entries
    nonzero = hist[hist > 0]
    entropy = -float(np.sum(nonzero * np.log2(nonzero)))
    return entropy


def classify_image(image_path: str) -> str:
    """Return 'P50', 'P80', or 'P150' for the image at *image_path*."""
    img = cv2.imread(image_path)
    if img is None:
        raise ValueError(f"Cannot read image: {image_path}")

    # ---- ROI crop: discard non-critical side areas first ----
    img = apply_roi(img, ROI_FRACTION)

    # ---- Stage 1: P50 vs (P80/P150) ----
    brightness = mean_brightness(img)
    if brightness > 75.0:
        return "P50"

    # ---- Stage 2: P80 vs P150 ----
    entropy = histogram_entropy(img, bins=32)
    if entropy > 3.62:
        return "P80"
    else:
        return "P150"


def classify_directory(directory: str) -> None:
    """Classify every image file in *directory* and print a summary."""
    valid_exts = {".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".tif"}
    results: dict[str, list[str]] = {"P50": [], "P80": [], "P150": [], "ERROR": []}

    for fname in sorted(os.listdir(directory)):
        ext = os.path.splitext(fname)[1].lower()
        if ext not in valid_exts:
            continue
        fpath = os.path.join(directory, fname)
        try:
            label = classify_image(fpath)
        except Exception as exc:
            label = "ERROR"
            print(f"ERROR  {fname}: {exc}")
        results[label].append(fname)
        print(f"{label:5s}  {fname}")

    # Summary
    total = sum(len(v) for v in results.values())
    print(f"\n{'='*40}")
    print(f"Total images : {total}")
    for cls in ("P50", "P80", "P150", "ERROR"):
        count = len(results[cls])
        pct = 100.0 * count / total if total else 0.0
        print(f"  {cls:6s} : {count:3d}  ({pct:5.1f}%)")


def main() -> None:
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)

    global ROI_FRACTION
    args = list(sys.argv[1:])

    # Check for --roi override (e.g. --roi 0.70)
    if "--roi" in args:
        idx = args.index("--roi")
        if idx + 1 < len(args):
            ROI_FRACTION = float(args[idx + 1])
            # Remove flag and its value so they don't interfere with path detection
            args = args[:idx] + args[idx + 2:]

    path = args[0] if args else ""
    if os.path.isdir(path):
        classify_directory(path)
    elif os.path.isfile(path):
        label = classify_image(path)
        print(f"{label}  {path}")
    else:
        print(f"Path not found: {path}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
