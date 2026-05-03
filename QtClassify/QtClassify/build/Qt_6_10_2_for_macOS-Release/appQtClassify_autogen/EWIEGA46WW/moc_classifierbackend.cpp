/****************************************************************************
** Meta object code from reading C++ file 'classifierbackend.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../classifierbackend.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'classifierbackend.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN25ClassificationResultModelE_t {};
} // unnamed namespace

template <> constexpr inline auto ClassificationResultModel::qt_create_metaobjectdata<qt_meta_tag_ZN25ClassificationResultModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ClassificationResultModel",
        "sortChanged",
        "",
        "sortByColumn",
        "column",
        "sortColumn",
        "sortAscending"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'sortChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'sortByColumn'
        QtMocHelpers::MethodData<void(int)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'sortColumn'
        QtMocHelpers::PropertyData<int>(5, QMetaType::Int, QMC::DefaultPropertyFlags, 0),
        // property 'sortAscending'
        QtMocHelpers::PropertyData<bool>(6, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ClassificationResultModel, qt_meta_tag_ZN25ClassificationResultModelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ClassificationResultModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25ClassificationResultModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25ClassificationResultModelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN25ClassificationResultModelE_t>.metaTypes,
    nullptr
} };

void ClassificationResultModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ClassificationResultModel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->sortChanged(); break;
        case 1: _t->sortByColumn((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ClassificationResultModel::*)()>(_a, &ClassificationResultModel::sortChanged, 0))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_v) = _t->sortColumn(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->sortAscending(); break;
        default: break;
        }
    }
}

const QMetaObject *ClassificationResultModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClassificationResultModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25ClassificationResultModelE_t>.strings))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int ClassificationResultModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ClassificationResultModel::sortChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
namespace {
struct qt_meta_tag_ZN16ClassifierWorkerE_t {};
} // unnamed namespace

template <> constexpr inline auto ClassifierWorker::qt_create_metaobjectdata<qt_meta_tag_ZN16ClassifierWorkerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ClassifierWorker",
        "started",
        "",
        "totalFiles",
        "progress",
        "current",
        "total",
        "filename",
        "resultReady",
        "ClassificationResult",
        "result",
        "finished",
        "errorOccurred",
        "errorMessage",
        "process"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'started'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'progress'
        QtMocHelpers::SignalData<void(int, int, const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::Int, 6 }, { QMetaType::QString, 7 },
        }}),
        // Signal 'resultReady'
        QtMocHelpers::SignalData<void(ClassificationResult)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Signal 'finished'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'process'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ClassifierWorker, qt_meta_tag_ZN16ClassifierWorkerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ClassifierWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16ClassifierWorkerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16ClassifierWorkerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16ClassifierWorkerE_t>.metaTypes,
    nullptr
} };

void ClassifierWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ClassifierWorker *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->started((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->progress((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 2: _t->resultReady((*reinterpret_cast<std::add_pointer_t<ClassificationResult>>(_a[1]))); break;
        case 3: _t->finished(); break;
        case 4: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->process(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ClassificationResult >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ClassifierWorker::*)(int )>(_a, &ClassifierWorker::started, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ClassifierWorker::*)(int , int , const QString & )>(_a, &ClassifierWorker::progress, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ClassifierWorker::*)(ClassificationResult )>(_a, &ClassifierWorker::resultReady, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ClassifierWorker::*)()>(_a, &ClassifierWorker::finished, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ClassifierWorker::*)(const QString & )>(_a, &ClassifierWorker::errorOccurred, 4))
            return;
    }
}

const QMetaObject *ClassifierWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClassifierWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16ClassifierWorkerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ClassifierWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ClassifierWorker::started(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void ClassifierWorker::progress(int _t1, int _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2, _t3);
}

// SIGNAL 2
void ClassifierWorker::resultReady(ClassificationResult _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void ClassifierWorker::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ClassifierWorker::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN17ClassifierBackendE_t {};
} // unnamed namespace

template <> constexpr inline auto ClassifierBackend::qt_create_metaobjectdata<qt_meta_tag_ZN17ClassifierBackendE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ClassifierBackend",
        "imagePathChanged",
        "",
        "classificationChanged",
        "lastErrorChanged",
        "roiFractionChanged",
        "batchModelChanged",
        "batchClassifyStarted",
        "totalFiles",
        "batchClassifyProgress",
        "current",
        "total",
        "filename",
        "batchClassifyFinished",
        "classifyCurrentImage",
        "classifyDirectory",
        "dirPath",
        "clearBatchResults",
        "sortBatchModel",
        "column",
        "imagePath",
        "classificationLabel",
        "brightness",
        "entropy",
        "cannyEdgeDensity",
        "usedTiebreaker",
        "lastError",
        "roiFraction",
        "batchModel",
        "batchCountP50",
        "batchCountP80",
        "batchCountP150",
        "batchCountErrors",
        "batchTotal"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'imagePathChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'classificationChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'lastErrorChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'roiFractionChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'batchModelChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'batchClassifyStarted'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Signal 'batchClassifyProgress'
        QtMocHelpers::SignalData<void(int, int, const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 10 }, { QMetaType::Int, 11 }, { QMetaType::QString, 12 },
        }}),
        // Signal 'batchClassifyFinished'
        QtMocHelpers::SignalData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'classifyCurrentImage'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'classifyDirectory'
        QtMocHelpers::SlotData<void(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 16 },
        }}),
        // Slot 'clearBatchResults'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'sortBatchModel'
        QtMocHelpers::SlotData<void(int)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 19 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'imagePath'
        QtMocHelpers::PropertyData<QString>(20, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'classificationLabel'
        QtMocHelpers::PropertyData<QString>(21, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'brightness'
        QtMocHelpers::PropertyData<double>(22, QMetaType::Double, QMC::DefaultPropertyFlags, 1),
        // property 'entropy'
        QtMocHelpers::PropertyData<double>(23, QMetaType::Double, QMC::DefaultPropertyFlags, 1),
        // property 'cannyEdgeDensity'
        QtMocHelpers::PropertyData<double>(24, QMetaType::Double, QMC::DefaultPropertyFlags, 1),
        // property 'usedTiebreaker'
        QtMocHelpers::PropertyData<bool>(25, QMetaType::Bool, QMC::DefaultPropertyFlags, 1),
        // property 'lastError'
        QtMocHelpers::PropertyData<QString>(26, QMetaType::QString, QMC::DefaultPropertyFlags, 2),
        // property 'roiFraction'
        QtMocHelpers::PropertyData<double>(27, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'batchModel'
        QtMocHelpers::PropertyData<QObject*>(28, QMetaType::QObjectStar, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'batchCountP50'
        QtMocHelpers::PropertyData<int>(29, QMetaType::Int, QMC::DefaultPropertyFlags, 4),
        // property 'batchCountP80'
        QtMocHelpers::PropertyData<int>(30, QMetaType::Int, QMC::DefaultPropertyFlags, 4),
        // property 'batchCountP150'
        QtMocHelpers::PropertyData<int>(31, QMetaType::Int, QMC::DefaultPropertyFlags, 4),
        // property 'batchCountErrors'
        QtMocHelpers::PropertyData<int>(32, QMetaType::Int, QMC::DefaultPropertyFlags, 4),
        // property 'batchTotal'
        QtMocHelpers::PropertyData<int>(33, QMetaType::Int, QMC::DefaultPropertyFlags, 4),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ClassifierBackend, qt_meta_tag_ZN17ClassifierBackendE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ClassifierBackend::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ClassifierBackendE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ClassifierBackendE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17ClassifierBackendE_t>.metaTypes,
    nullptr
} };

void ClassifierBackend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ClassifierBackend *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->imagePathChanged(); break;
        case 1: _t->classificationChanged(); break;
        case 2: _t->lastErrorChanged(); break;
        case 3: _t->roiFractionChanged(); break;
        case 4: _t->batchModelChanged(); break;
        case 5: _t->batchClassifyStarted((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->batchClassifyProgress((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 7: _t->batchClassifyFinished(); break;
        case 8: _t->classifyCurrentImage(); break;
        case 9: _t->classifyDirectory((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->clearBatchResults(); break;
        case 11: _t->sortBatchModel((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ClassifierBackend::*)()>(_a, &ClassifierBackend::imagePathChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ClassifierBackend::*)()>(_a, &ClassifierBackend::classificationChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ClassifierBackend::*)()>(_a, &ClassifierBackend::lastErrorChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ClassifierBackend::*)()>(_a, &ClassifierBackend::roiFractionChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ClassifierBackend::*)()>(_a, &ClassifierBackend::batchModelChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ClassifierBackend::*)(int )>(_a, &ClassifierBackend::batchClassifyStarted, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ClassifierBackend::*)(int , int , const QString & )>(_a, &ClassifierBackend::batchClassifyProgress, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (ClassifierBackend::*)()>(_a, &ClassifierBackend::batchClassifyFinished, 7))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->imagePath(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->classificationLabel(); break;
        case 2: *reinterpret_cast<double*>(_v) = _t->brightness(); break;
        case 3: *reinterpret_cast<double*>(_v) = _t->entropy(); break;
        case 4: *reinterpret_cast<double*>(_v) = _t->cannyEdgeDensity(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->usedTiebreaker(); break;
        case 6: *reinterpret_cast<QString*>(_v) = _t->lastError(); break;
        case 7: *reinterpret_cast<double*>(_v) = _t->roiFraction(); break;
        case 8: *reinterpret_cast<QObject**>(_v) = _t->batchModel(); break;
        case 9: *reinterpret_cast<int*>(_v) = _t->batchCountP50(); break;
        case 10: *reinterpret_cast<int*>(_v) = _t->batchCountP80(); break;
        case 11: *reinterpret_cast<int*>(_v) = _t->batchCountP150(); break;
        case 12: *reinterpret_cast<int*>(_v) = _t->batchCountErrors(); break;
        case 13: *reinterpret_cast<int*>(_v) = _t->batchTotal(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setImagePath(*reinterpret_cast<QString*>(_v)); break;
        case 7: _t->setRoiFraction(*reinterpret_cast<double*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *ClassifierBackend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClassifierBackend::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ClassifierBackendE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ClassifierBackend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void ClassifierBackend::imagePathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ClassifierBackend::classificationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ClassifierBackend::lastErrorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ClassifierBackend::roiFractionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ClassifierBackend::batchModelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ClassifierBackend::batchClassifyStarted(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void ClassifierBackend::batchClassifyProgress(int _t1, int _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2, _t3);
}

// SIGNAL 7
void ClassifierBackend::batchClassifyFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
