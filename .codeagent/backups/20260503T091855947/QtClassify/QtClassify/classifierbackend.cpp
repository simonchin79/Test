#include "classifierbackend.h"
#include <QDir>
#include <QFileInfo>
#include <QMetaObject>
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
    m_sortColumn = -1;
    m_sortOrder = Qt::AscendingOrder;
    endResetModel();
}

void ClassificationResultModel::sortByColumn(int column)
{
    if (column < 0 || column > 5)
        return;

    // Toggle order if same column clicked; otherwise default to ascending
    if (column == m_sortColumn) {
        m_sortOrder = (m_sortOrder == Qt::AscendingOrder)
                          ? Qt::DescendingOrder
                          : Qt::AscendingOrder;
    } else {
        m_sortColumn = column;
        m_sortOrder = Qt::AscendingOrder;
    }

    beginResetModel();
    std::sort(m_results.begin(), m_results.end(),
              [this](const ClassificationResult &a,
                     const ClassificationResult &b) {
        bool lessThan = false;
        switch (m_sortColumn) {
        case 0: // Filename
            lessThan = a.filename < b.filename;
            break;
        case 1: // Label
            lessThan = a.label < b.label;
            break;
        case 2: // Brightness
            lessThan = a.brightness < b.brightness;
            break;
        case 3: // Entropy
            lessThan = a.entropy < b.entropy;
            break;
        case 4: // Canny edge density
            lessThan = a.cannyEdgeDensity < b.cannyEdgeDensity;
            break;
        case 5: // Tiebreaker
            lessThan = static_cast<int>(a.usedTiebreaker)
                       < static_cast<int>(b.usedTiebreaker);
            break;
        }
        return m_sortOrder == Qt::AscendingOrder
                   ? lessThan
                   : !lessThan;
    });
    endResetModel();
}

// ============================================================================
// ClassifierWorker
// ============================================================================

const QStringList &ClassifierWorker::imageNameFilters()
{
    static const QStringList filters = {
        "*.png", "*.jpg", "*.jpeg", "*.bmp", "*.tiff", "*.tif"
    };
    return filters;
}

ClassifierWorker::ClassifierWorker(QObject *parent)
    : QObject(parent)
{
}

void ClassifierWorker::setDirectory(const QString &dirPath)
{
    m_dirPath = localFilePathFromUrl(dirPath);
}

void ClassifierWorker::setRoiFraction(double fraction)
{
    m_roiFraction = fraction;
}

void ClassifierWorker::process()
{
    QDir qdir(m_dirPath);
    if (!qdir.exists()) {
        emit errorOccurred(
            QStringLiteral("Directory not found: %1").arg(m_dirPath));
        emit finished();
        return;
    }

    qdir.setNameFilters(imageNameFilters());
    qdir.setFilter(QDir::Files | QDir::Readable);
    qdir.setSorting(QDir::Name);

    const QFileInfoList files = qdir.entryInfoList();
    const int total = files.size();

    emit started(total);

    Classifier classifier(m_roiFraction);

    for (int i = 0; i < total; ++i) {
        const QString path = files.at(i).absoluteFilePath();
        const QString filename = files.at(i).fileName();

        emit progress(i + 1, total, filename);

        ClassificationResult res = classifier.classifyImage(path.toStdString());
        res.filename = path.toStdString();  // store full path for display
        emit resultReady(res);
    }

    emit finished();
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
    if (m_batchRunning) {
        // Already processing — button is disabled in QML, but guard anyway
        return;
    }

    const QString dir = localFilePathFromUrl(dirPath);

    // Clean up any previous worker/thread
    stopWorkerThread();

    // Clear previous batch results
    m_batchModel->clear();
    emit batchModelChanged();

    m_batchRunning = true;

    // Create worker + thread
    m_worker = new ClassifierWorker();  // no parent — will be moved to thread
    m_workerThread = new QThread(this);
    m_worker->moveToThread(m_workerThread);

    // Pass parameters to worker
    m_worker->setDirectory(dir);
    m_worker->setRoiFraction(m_classifier.roiFraction());

    // --- Connect signals (all cross-thread, queued automatically) ---

    // Thread lifecycle
    QObject::connect(m_workerThread, &QThread::started,
                     m_worker,       &ClassifierWorker::process);
    QObject::connect(m_worker,       &ClassifierWorker::finished,
                     m_workerThread, &QThread::quit);
    QObject::connect(m_worker,       &ClassifierWorker::finished,
                     m_worker,       &QObject::deleteLater);
    QObject::connect(m_workerThread, &QThread::finished,
                     m_workerThread, &QObject::deleteLater);

    // Forward signals to backend
    QObject::connect(m_worker, &ClassifierWorker::started,
                     this,     &ClassifierBackend::batchClassifyStarted);
    QObject::connect(m_worker, &ClassifierWorker::progress,
                     this,     &ClassifierBackend::batchClassifyProgress);

    // Handle each result on the main thread — update model + notify QML
    QObject::connect(m_worker, &ClassifierWorker::resultReady,
                     this,     [this](ClassificationResult res) {
        m_batchModel->addResult(res);
        emit batchModelChanged();
    });

    // Error handling
    QObject::connect(m_worker, &ClassifierWorker::errorOccurred,
                     this,     [this](const QString &msg) {
        m_lastError = msg;
        emit lastErrorChanged();
    });

    // When done, reset state
    QObject::connect(m_worker, &ClassifierWorker::finished,
                     this,     [this]() {
        m_batchRunning = false;
        m_worker = nullptr;
        m_workerThread = nullptr;
        emit batchModelChanged();
        emit batchClassifyFinished();
    });

    m_workerThread->start();
}

void ClassifierBackend::sortBatchModel(int column)
{
    m_batchModel->sortByColumn(column);
}

void ClassifierBackend::stopWorkerThread()
{
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait(5000);  // wait up to 5 s for graceful shutdown
        // If still running after wait, terminate (shouldn't happen normally)
        if (m_workerThread->isRunning()) {
            m_workerThread->terminate();
            m_workerThread->wait(2000);
        }
        m_workerThread = nullptr;
        m_worker = nullptr;
        m_batchRunning = false;
    }
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

QString ClassifierWorker::localFilePathFromUrl(const QString &urlOrPath) const
{
    QUrl url(urlOrPath);
    if (url.isLocalFile())
        return url.toLocalFile();
    if (urlOrPath.startsWith("file://"))
        return QUrl(urlOrPath).toLocalFile();
    return urlOrPath;
}
