/****************************************************************************
** Meta object code from reading C++ file 'can_send_manager.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "can_send_manager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'can_send_manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_can_send_manager[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      30,   18,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      57,   17,   17,   17, 0x0a,
      73,   17,   17,   17, 0x0a,
      90,   17,   17,   17, 0x0a,
     108,   17,   17,   17, 0x0a,
     127,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_can_send_manager[] = {
    "can_send_manager\0\0p_can_frame\0"
    "send_can_frame(can_frame*)\0send_can_data()\0"
    "send_10ms_info()\0send_100ms_info()\0"
    "send_1000ms_info()\0"
    "accept_modify_can_info(obu_can_info_update*)\0"
};

void can_send_manager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        can_send_manager *_t = static_cast<can_send_manager *>(_o);
        switch (_id) {
        case 0: _t->send_can_frame((*reinterpret_cast< can_frame*(*)>(_a[1]))); break;
        case 1: _t->send_can_data(); break;
        case 2: _t->send_10ms_info(); break;
        case 3: _t->send_100ms_info(); break;
        case 4: _t->send_1000ms_info(); break;
        case 5: _t->accept_modify_can_info((*reinterpret_cast< obu_can_info_update*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData can_send_manager::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject can_send_manager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_can_send_manager,
      qt_meta_data_can_send_manager, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &can_send_manager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *can_send_manager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *can_send_manager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_can_send_manager))
        return static_cast<void*>(const_cast< can_send_manager*>(this));
    return QObject::qt_metacast(_clname);
}

int can_send_manager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void can_send_manager::send_can_frame(can_frame * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
