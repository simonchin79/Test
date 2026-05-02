#include "classifier.h"
#include <cmath>
#include <algorithm>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
Classifier::Classifier(double roiFraction)
    : m_roiFraction(std::clamp(roiFraction, 0.1, 1.0))
{
}

// ---------------------------------------------------------------------------
// ROI helpers
// ---------------------------------------------------------------------------
void Classifier::setRoiFraction(double fraction)
{
    m_roiFraction = std::clamp(fraction, 0.1, 1.0);
}

cv::Mat Classifier::applyROI(const cv::Mat &image)
{
    const int h = image.rows;
    const int w = image.cols;
    const int cropH = static_cast<int>(h * m_roiFraction);
    const int cropW = static_cast<int>(w * m_roiFraction);
    const int y1 = (h - cropH) / 2;
    const int x1 = (w - cropW) / 2;
    return image(cv::Rect(x1, y1, cropW, cropH)).clone();
}

// ---------------------------------------------------------------------------
// Stage 1 — mean grayscale brightness
// ---------------------------------------------------------------------------
double Classifier::meanBrightness(const cv::Mat &gray)
{
    return cv::mean(gray)[0];
}

// ---------------------------------------------------------------------------
// Stage 2a — 32-bin histogram entropy
// ---------------------------------------------------------------------------
double Classifier::histogramEntropy(const cv::Mat &gray, int bins)
{
    // Compute histogram with *bins* bins over [0, 256)
    const int channels[] = {0};
    const int histSize[] = {bins};
    const float range[] = {0.f, 256.f};
    const float *ranges[] = {range};

    cv::Mat hist;
    cv::calcHist(&gray, 1, channels, cv::Mat(), hist, 1, histSize, ranges,
                 true,   // uniform
                 false); // not accumulate

    // Normalise to probability distribution
    double total = cv::sum(hist)[0];
    if (total <= 0.0)
        return 0.0;

    double entropy = 0.0;
    for (int i = 0; i < bins; ++i) {
        float p = hist.at<float>(i) / static_cast<float>(total);
        if (p > 0.0f)
            entropy -= static_cast<double>(p) * std::log2(static_cast<double>(p));
    }
    return entropy;
}

// ---------------------------------------------------------------------------
// Stage 2b — Canny edge-density tiebreaker
// ---------------------------------------------------------------------------
double Classifier::cannyEdgeDensity(const cv::Mat &gray,
                                    double low, double high)
{
    cv::Mat edges;
    cv::Canny(gray, edges, low, high);
    const double edgePixels = static_cast<double>(cv::countNonZero(edges));
    return 100.0 * edgePixels / static_cast<double>(edges.total());
}

// ---------------------------------------------------------------------------
// Main classification entry-point
// ---------------------------------------------------------------------------
ClassificationResult Classifier::classifyImage(const std::string &imagePath)
{
    ClassificationResult result;
    result.filename = imagePath;

    // Load image
    cv::Mat img = cv::imread(imagePath, cv::IMREAD_COLOR);
    if (img.empty()) {
        result.label = "ERROR";
        return result;
    }

    // ---- ROI crop ----
    img = applyROI(img);

    // Convert to grayscale once
    cv::Mat gray;
    if (img.channels() == 3)
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    else
        gray = img;

    // ---- Stage 1: P50 vs (P80 / P150) ----
    result.brightness = meanBrightness(gray);
    if (result.brightness > BRIGHTNESS_THRESHOLD) {
        result.label = "P50";
        // Compute remaining metrics for informational display
        result.entropy = histogramEntropy(gray, 32);
        result.cannyEdgeDensity = cannyEdgeDensity(gray);
        return result;
    }

    // ---- Stage 2: P80 vs P150 ----
    result.entropy = histogramEntropy(gray, 32);

    // Stage 2a — entropy primary
    if (result.entropy > ENTROPY_AMBIG_HIGH) {
        result.label = "P80";
        result.cannyEdgeDensity = cannyEdgeDensity(gray);
        return result;
    }
    if (result.entropy < ENTROPY_AMBIG_LOW) {
        result.label = "P150";
        result.cannyEdgeDensity = cannyEdgeDensity(gray);
        return result;
    }

    // Stage 2b — Canny tiebreaker (entropy in ambiguity zone)
    result.usedTiebreaker = true;
    result.cannyEdgeDensity = cannyEdgeDensity(gray);
    if (result.cannyEdgeDensity > CANNY_TIEBREAK_THRESHOLD)
        result.label = "P150";  // more edges → truly P150
    else
        result.label = "P80";   // fewer edges → truly P80
    return result;
}
