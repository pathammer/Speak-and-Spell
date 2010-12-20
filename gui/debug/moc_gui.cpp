/****************************************************************************
** Meta object code from reading C++ file 'gui.h'
**
** Created: Mon Dec 20 14:51:31 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../gui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_gui[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       5,    4,    4,    4, 0x0a,
      18,    4,    4,    4, 0x0a,
      55,   39,   32,    4, 0x0a,
      79,    4,    4,    4, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_gui[] = {
    "gui\0\0CorrectAll()\0CorrectWord()\0string\0"
    "text,colorscale\0ColorText(string,float)\0"
    "Analysis_()\0"
};

const QMetaObject gui::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_gui,
      qt_meta_data_gui, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &gui::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *gui::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *gui::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_gui))
        return static_cast<void*>(const_cast< gui*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int gui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: CorrectAll(); break;
        case 1: CorrectWord(); break;
        case 2: { string _r = ColorText((*reinterpret_cast< string(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< string*>(_a[0]) = _r; }  break;
        case 3: Analysis_(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
