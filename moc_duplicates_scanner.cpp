/****************************************************************************
** Meta object code from reading C++ file 'duplicates_scanner.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "duplicates_scanner.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'duplicates_scanner.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_duplicates_scanner_t {
    QByteArrayData data[6];
    char stringdata0[82];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_duplicates_scanner_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_duplicates_scanner_t qt_meta_stringdata_duplicates_scanner = {
    {
QT_MOC_LITERAL(0, 0, 18), // "duplicates_scanner"
QT_MOC_LITERAL(1, 19, 15), // "onFileProcessed"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 16), // "onDuplicateFound"
QT_MOC_LITERAL(4, 53, 14), // "onScanComplete"
QT_MOC_LITERAL(5, 68, 13) // "startScanning"

    },
    "duplicates_scanner\0onFileProcessed\0\0"
    "onDuplicateFound\0onScanComplete\0"
    "startScanning"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_duplicates_scanner[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       3,    2,   37,    2, 0x06 /* Public */,
       4,    0,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    2,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void duplicates_scanner::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        duplicates_scanner *_t = static_cast<duplicates_scanner *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onFileProcessed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->onDuplicateFound((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->onScanComplete(); break;
        case 3: _t->startScanning(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (duplicates_scanner::*_t)(QString const & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&duplicates_scanner::onFileProcessed)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (duplicates_scanner::*_t)(QString const & , QString const & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&duplicates_scanner::onDuplicateFound)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (duplicates_scanner::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&duplicates_scanner::onScanComplete)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject duplicates_scanner::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_duplicates_scanner.data,
      qt_meta_data_duplicates_scanner,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *duplicates_scanner::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *duplicates_scanner::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_duplicates_scanner.stringdata0))
        return static_cast<void*>(const_cast< duplicates_scanner*>(this));
    return QObject::qt_metacast(_clname);
}

int duplicates_scanner::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void duplicates_scanner::onFileProcessed(QString const & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void duplicates_scanner::onDuplicateFound(QString const & _t1, QString const & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void duplicates_scanner::onScanComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
