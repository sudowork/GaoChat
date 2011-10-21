/****************************************************************************
** Meta object code from reading C++ file 'clientgui.h'
**
** Created: Fri Oct 21 04:29:16 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../clientgui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clientgui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ClientGUI[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,
      18,   10,   10,   10, 0x08,
      33,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ClientGUI[] = {
    "ClientGUI\0\0quit()\0saveSettings()\0"
    "processMsg()\0"
};

const QMetaObject ClientGUI::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ClientGUI,
      qt_meta_data_ClientGUI, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClientGUI::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClientGUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClientGUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClientGUI))
        return static_cast<void*>(const_cast< ClientGUI*>(this));
    return QWidget::qt_metacast(_clname);
}

int ClientGUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: quit(); break;
        case 1: saveSettings(); break;
        case 2: processMsg(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
