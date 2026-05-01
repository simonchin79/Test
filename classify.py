#!/usr/bin/env python3
"""
classify.py — Two-stage image classifier for P50, P80, and P150.

Usage:
    python classify.py <image_path>          # Classify a single image
    python classify.py <directory_path>      # Classify all images in a directory

Classification Strategy:
    Stage 1: P50 vs (P80/P150)  — mean grayscale brightness (threshold ~75)
    Stage 2: P80 vs P150        — histogram entropy (threshold ~3.62, 32 bins)
"""

import os
import sys
import cv2
import numpy as np


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

    path = sys.argv[1]
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
