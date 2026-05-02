#!/usr/bin/env python3
"""
Analyze multiple feature distributions for P80 vs P150 images.
Helps determine optimal thresholds and scoring strategies.
"""
import os, sys
import cv2
import numpy as np

ROI_FRACTION = 0.80

def apply_roi(image, fraction=ROI_FRACTION):
    h, w = image.shape[:2]
    crop_h = int(h * fraction)
    crop_w = int(w * fraction)
    y1 = (h - crop_h) // 2
    x1 = (w - crop_w) // 2
    return image[y1:y1+crop_h, x1:x1+crop_w]

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
    """Mean gradient magnitude using Sobel operator."""
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image
    sobelx = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=3)
    sobely = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=3)
    mag = np.sqrt(sobelx**2 + sobely**2)
    return float(np.mean(mag))

def laplacian_variance(image):
    """Variance of Laplacian — focus/texture measure."""
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image
    laplacian = cv2.Laplacian(gray, cv2.CV_64F)
    return float(laplacian.var())

def midrange_pixel_percentage(image, low=50, high=150):
    """Percentage of pixels in the mid-range brightness band."""
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image
    total = gray.size
    mid = np.sum((gray >= low) & (gray <= high))
    return 100.0 * mid / total

def percent_bright(image, threshold=100):
    """Percentage of pixels above a brightness threshold."""
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image
    return 100.0 * np.sum(gray > threshold) / gray.size

def analyze_directory(directory, label_name):
    print(f"\n{'='*70}")
    print(f"  {label_name}  (directory: {directory})")
    print(f"{'='*70}")
    
    valid_exts = {".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".tif"}
    features = []
    
    for fname in sorted(os.listdir(directory)):
        ext = os.path.splitext(fname)[1].lower()
        if ext not in valid_exts:
            continue
        fpath = os.path.join(directory, fname)
        img = cv2.imread(fpath)
        if img is None:
            continue
        img = apply_roi(img, ROI_FRACTION)
        
        feat = {
            "file": fname,
            "brightness": mean_brightness(img),
            "entropy": histogram_entropy(img, 32),
            "sobel_grad": sobel_mean_gradient(img),
            "laplacian_var": laplacian_variance(img),
            "midrange_pct": midrange_pixel_percentage(img, 50, 150),
            "pct_bright_100": percent_bright(img, 100),
        }
        features.append(feat)
    
    if not features:
        print("  No images found.")
        return
    
    # Print header
    print(f"{'File':45s} {'Bright':>7s} {'Entropy':>7s} {'SobelG':>8s} {'Laplac':>8s} {'Mid%':>6s} {'%>100':>6s}")
    print("-"*90)
    
    for f in features:
        print(f"{f['file']:45s} {f['brightness']:7.1f} {f['entropy']:7.3f} {f['sobel_grad']:8.2f} {f['laplacian_var']:8.1f} {f['midrange_pct']:6.1f} {f['pct_bright_100']:6.1f}")
    
    # Statistics
    names = ["brightness", "entropy", "sobel_grad", "laplacian_var", "midrange_pct", "pct_bright_100"]
    print(f"\n  Statistics for {label_name} ({len(features)} images):")
    for name in names:
        vals = [f[name] for f in features]
        print(f"    {name:20s}: min={min(vals):8.3f}  max={max(vals):8.3f}  mean={np.mean(vals):8.3f}  std={np.std(vals):8.3f}")


if __name__ == "__main__":
    base = "Dataset"
    analyze_directory(os.path.join(base, "P50"), "P50 (bright)")
    analyze_directory(os.path.join(base, "P80"), "P80 (dark+texture)")
    analyze_directory(os.path.join(base, "P150"), "P150 (dark+smooth)")
