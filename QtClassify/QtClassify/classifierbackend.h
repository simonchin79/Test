#ifndef CLASSIFIERBACKEND_H
#define CLASSIFIERBACKEND_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QList>
#include <QAbstractListModel>
#include <QUrl>
#include "classifier.h"

// ============================================================================
// ClassificationResultModel — QAbstractListModel for batch results
// ============================================================================
class ClassificationResultModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        FilenameRole       = Qt::UserRole + 1,
        LabelRole,
        BrightnessRole,
        EntropyRole,
        CannyRole,
        UsedTiebreakerRole,
        IsErrorRole,
        DisplayTextRole
    };

    explicit ClassificationResultModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Sort-column state as Q_PROPERTY so QML binding expressions auto-update
    Q_PROPERTY(int sortColumn READ sortColumn NOTIFY sortChanged)
    Q_PROPERTY(bool sortAscending READ sortAscending NOTIFY sortChanged)

    Q_INVOKABLE void sortByColumn(int column);
    int sortColumn() const { return m_sortColumn; }
    bool sortAscending() const { return m_sortOrder == Qt::AscendingOrder; }

    void addResult(const ClassificationResult &result);
    void clear();

    int countP50()   const { return m_countP50; }
    int countP80()   const { return m_countP80; }
    int countP150()  const { return m_countP150; }
    int countErrors() const { return m_countErrors; }
    int totalCount() const { return m_results.size(); }

signals:
    void sortChanged();

private:
    QList<ClassificationResult> m_results;
    int m_countP50   = 0;
    int m_countP80   = 0;
    int m_countP150  = 0;
    int m_countErrors = 0;
    int m_sortColumn = -1;        // -1 = natural (insertion) order
    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;
};

// ============================================================================
// ClassifierWorker — runs batch classification in a worker thread
// ============================================================================
class ClassifierWorker : public QObject
{
    Q_OBJECT

public:
    explicit ClassifierWorker(QObject *parent = nullptr);

    void setDirectory(const QString &dirPath);
    void setRoiFraction(double fraction);

public slots:
    void process();

signals:
    void started(int totalFiles);
    void progress(int current, int total, const QString &filename);
    void resultReady(ClassificationResult result);
    void finished();
    void errorOccurred(const QString &errorMessage);

private:
    QString m_dirPath;
    double m_roiFraction = 0.80;

    QString localFilePathFromUrl(const QString &urlOrPath) const;

    static const QStringList &imageNameFilters();
};

// ============================================================================
// ClassifierBackend — main QML-callable backend object
// ============================================================================
class ClassifierBackend : public QObject
{
    Q_OBJECT

    // --- Current single-image result ---
    Q_PROPERTY(QString imagePath READ imagePath
               WRITE setImagePath NOTIFY imagePathChanged)
    Q_PROPERTY(QString classificationLabel READ classificationLabel
               NOTIFY classificationChanged)
    Q_PROPERTY(double brightness READ brightness
               NOTIFY classificationChanged)
    Q_PROPERTY(double entropy READ entropy
               NOTIFY classificationChanged)
    Q_PROPERTY(double cannyEdgeDensity READ cannyEdgeDensity
               NOTIFY classificationChanged)
    Q_PROPERTY(bool usedTiebreaker READ usedTiebreaker
               NOTIFY classificationChanged)
    Q_PROPERTY(QString lastError READ lastError
               NOTIFY lastErrorChanged)

    // --- Settings ---
    Q_PROPERTY(double roiFraction READ roiFraction
               WRITE setRoiFraction NOTIFY roiFractionChanged)

    // --- Batch model ---
    Q_PROPERTY(QObject *batchModel READ batchModel CONSTANT)

    // --- Batch summary ---
    Q_PROPERTY(int batchCountP50 READ batchCountP50 NOTIFY batchModelChanged)
    Q_PROPERTY(int batchCountP80 READ batchCountP80 NOTIFY batchModelChanged)
    Q_PROPERTY(int batchCountP150 READ batchCountP150 NOTIFY batchModelChanged)
    Q_PROPERTY(int batchCountErrors READ batchCountErrors NOTIFY batchModelChanged)
    Q_PROPERTY(int batchTotal READ batchTotal NOTIFY batchModelChanged)

public:
    explicit ClassifierBackend(QObject *parent = nullptr);

    // Single-image properties
    QString imagePath() const;
    void setImagePath(const QString &path);

    QString classificationLabel() const;
    double brightness() const;
    double entropy() const;
    double cannyEdgeDensity() const;
    bool usedTiebreaker() const;
    QString lastError() const;

    // Settings
    double roiFraction() const;
    void setRoiFraction(double fraction);

    // Batch
    QObject *batchModel() const;
    int batchCountP50() const;
    int batchCountP80() const;
    int batchCountP150() const;
    int batchCountErrors() const;
    int batchTotal() const;

public slots:
    void classifyCurrentImage();
    void classifyDirectory(const QString &dirPath);
    void clearBatchResults();

    Q_INVOKABLE void sortBatchModel(int column);

signals:
    void imagePathChanged();
    void classificationChanged();
    void lastErrorChanged();
    void roiFractionChanged();
    void batchModelChanged();

    void batchClassifyStarted(int totalFiles);
    void batchClassifyProgress(int current, int total,
                               const QString &filename);
    void batchClassifyFinished();

private:
    QString localFilePathFromUrl(const QString &urlOrPath) const;

    Classifier m_classifier;
    QString m_imagePath;
    ClassificationResult m_lastResult;
    QString m_lastError;
    ClassificationResultModel *m_batchModel = nullptr;

    // Worker-thread state
    QThread          *m_workerThread = nullptr;
    ClassifierWorker *m_worker       = nullptr;
    bool              m_batchRunning = false;

    void stopWorkerThread();
};

#endif // CLASSIFIERBACKEND_H
