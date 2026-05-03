#include "classifierbackend.h"
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <algorithm>

// ============================================================================
// ClassificationResultModel
// ============================================================================
ClassificationResultModel::ClassificationResultModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ClassificationResultModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_results.size();
}

QVariant ClassificationResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_results.size())
        return {};

    const auto &r = m_results.at(index.row());

    switch (role) {
    case FilenameRole:       return QString::fromStdString(r.filename);
    case LabelRole:          return QString::fromStdString(r.label);
    case BrightnessRole:     return r.brightness;
    case EntropyRole:        return r.entropy;
    case CannyRole:          return r.cannyEdgeDensity;
    case UsedTiebreakerRole: return r.usedTiebreaker;
    case IsErrorRole:        return r.label == "ERROR";
    case DisplayTextRole: {
        // Build a one-line summary for the list view
        QString fname = QFileInfo(QString::fromStdString(r.filename)).fileName();
        return QStringLiteral("%1  |  %2  |  B:%3  E:%4  C:%5%")
            .arg(fname,
                 QString::fromStdString(r.label))
            .arg(r.brightness, 0, 'f', 1)
            .arg(r.entropy, 0, 'f', 4)
            .arg(r.cannyEdgeDensity, 0, 'f', 2);
    }
    default: break;
    }
    return {};
}

QHash<int, QByteArray> ClassificationResultModel::roleNames() const
{
    return {
        {FilenameRole,       "filename"},
        {LabelRole,          "label"},
        {BrightnessRole,     "brightness"},
        {EntropyRole,        "entropy"},
        {CannyRole,          "canny"},
        {UsedTiebreakerRole, "usedTiebreaker"},
        {IsErrorRole,        "isError"},
        {DisplayTextRole,    "displayText"}
    };
}

void ClassificationResultModel::addResult(const ClassificationResult &result)
{
    beginInsertRows(QModelIndex(), m_results.size(), m_results.size());
    m_results.append(result);
    endInsertRows();

    // Update counters
    const auto &label = result.label;
    if (label == "P50")       ++m_countP50;
    else if (label == "P80")  ++m_countP80;
    else if (label == "P150") ++m_countP150;
    else                      ++m_countErrors;
}

void ClassificationResultModel::clear()
{
    beginResetModel();
    m_results.clear();
    m_countP50 = 0;
    m_countP80 = 0;
    m_countP150 = 0;
    m_countErrors = 0;
    endResetModel();
}

// ============================================================================
// ClassifierBackend
// ============================================================================
ClassifierBackend::ClassifierBackend(QObject *parent)
    : QObject(parent)
    , m_batchModel(new ClassificationResultModel(this))
{
}

// --- Single-image properties ---

QString ClassifierBackend::imagePath() const
{
    return m_imagePath;
}

void ClassifierBackend::setImagePath(const QString &path)
{
    const QString cleaned = localFilePathFromUrl(path);
    if (cleaned != m_imagePath) {
        m_imagePath = cleaned;
        emit imagePathChanged();
        classifyCurrentImage();
    }
}

QString ClassifierBackend::classificationLabel() const
{
    return QString::fromStdString(m_lastResult.label);
}

double ClassifierBackend::brightness() const
{
    return m_lastResult.brightness;
}

double ClassifierBackend::entropy() const
{
    return m_lastResult.entropy;
}

double ClassifierBackend::cannyEdgeDensity() const
{
    return m_lastResult.cannyEdgeDensity;
}

bool ClassifierBackend::usedTiebreaker() const
{
    return m_lastResult.usedTiebreaker;
}

QString ClassifierBackend::lastError() const
{
    return m_lastError;
}

double ClassifierBackend::roiFraction() const
{
    return m_classifier.roiFraction();
}

void ClassifierBackend::setRoiFraction(double fraction)
{
    if (std::abs(m_classifier.roiFraction() - fraction) > 0.001) {
        m_classifier.setRoiFraction(fraction);
        emit roiFractionChanged();
    }
}

// --- Batch ---

QObject *ClassifierBackend::batchModel() const
{
    return m_batchModel;
}

int ClassifierBackend::batchCountP50() const  { return m_batchModel->countP50(); }
int ClassifierBackend::batchCountP80() const  { return m_batchModel->countP80(); }
int ClassifierBackend::batchCountP150() const { return m_batchModel->countP150(); }
int ClassifierBackend::batchCountErrors() const { return m_batchModel->countErrors(); }
int ClassifierBackend::batchTotal() const     { return m_batchModel->totalCount(); }

// --- Slots ---

void ClassifierBackend::classifyCurrentImage()
{
    if (m_imagePath.isEmpty()) {
        m_lastError = QStringLiteral("No image selected.");
        emit lastErrorChanged();
        return;
    }

    if (!QFileInfo::exists(m_imagePath)) {
        m_lastError = QStringLiteral("File not found: %1").arg(m_imagePath);
        emit lastErrorChanged();
        return;
    }

    try {
        m_lastResult = m_classifier.classifyImage(m_imagePath.toStdString());
        m_lastError.clear();
    } catch (const std::exception &e) {
        m_lastResult = ClassificationResult{};
        m_lastResult.label = "ERROR";
        m_lastResult.filename = m_imagePath.toStdString();
        m_lastError = QString::fromStdString(e.what());
    }

    emit classificationChanged();
    if (!m_lastError.isEmpty())
        emit lastErrorChanged();
}

void ClassifierBackend::classifyDirectory(const QString &dirPath)
{
    const QString dir = localFilePathFromUrl(dirPath);
    QDir qdir(dir);
    if (!qdir.exists()) {
        m_lastError = QStringLiteral("Directory not found: %1").arg(dir);
        emit lastErrorChanged();
        return;
    }

    QStringList nameFilters;
    nameFilters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.tiff" << "*.tif";
    qdir.setNameFilters(nameFilters);
    qdir.setFilter(QDir::Files | QDir::Readable);
    qdir.setSorting(QDir::Name);

    const QFileInfoList files = qdir.entryInfoList();
    const int total = files.size();

    m_batchModel->clear();
    emit batchModelChanged();
    emit batchClassifyStarted(total);

    for (int i = 0; i < total; ++i) {
        const QString path = files.at(i).absoluteFilePath();
        emit batchClassifyProgress(i + 1, total,
                                   files.at(i).fileName());

        ClassificationResult res;
        try {
            res = m_classifier.classifyImage(path.toStdString());
        } catch (const std::exception &e) {
            res.label = "ERROR";
            res.filename = path.toStdString();
        }
        res.filename = path.toStdString();  // store full path for display
        m_batchModel->addResult(res);
    }

    emit batchModelChanged();
    emit batchClassifyFinished();
}

void ClassifierBackend::clearBatchResults()
{
    m_batchModel->clear();
    emit batchModelChanged();
}

// --- Helpers ---

QString ClassifierBackend::localFilePathFromUrl(const QString &urlOrPath) const
{
    QUrl url(urlOrPath);
    if (url.isLocalFile())
        return url.toLocalFile();
    // Strip "file://" prefix if present (handles various Qt versions)
    if (urlOrPath.startsWith("file://"))
        return QUrl(urlOrPath).toLocalFile();
    return urlOrPath;
}
