#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <string>
#include <opencv2/opencv.hpp>

struct ClassificationResult
{
    std::string label;       // "P50", "P80", "P150", or "ERROR"
    std::string filename;
    double brightness = 0.0;
    double entropy = 0.0;
    double cannyEdgeDensity = 0.0;
    bool usedTiebreaker = false;
};

class Classifier
{
public:
    explicit Classifier(double roiFraction = 0.80);

    ClassificationResult classifyImage(const std::string &imagePath);

    void setRoiFraction(double fraction);
    double roiFraction() const { return m_roiFraction; }

private:
    cv::Mat applyROI(const cv::Mat &image);
    double meanBrightness(const cv::Mat &gray);
    double histogramEntropy(const cv::Mat &gray, int bins = 32);
    double cannyEdgeDensity(const cv::Mat &gray,
                            double low = 50.0, double high = 150.0);

    double m_roiFraction;

    static constexpr double BRIGHTNESS_THRESHOLD     = 86.0;
    static constexpr double ENTROPY_AMBIG_LOW        = 3.77;
    static constexpr double ENTROPY_AMBIG_HIGH       = 3.845;
    static constexpr double CANNY_TIEBREAK_THRESHOLD = 16.0;
};

#endif // CLASSIFIER_H
