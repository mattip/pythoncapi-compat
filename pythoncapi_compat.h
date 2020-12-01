// Header file providing new functions of the Python C API to Python 3.6.
//
// File distributed under the MIT license.
//
// Homepage:
// https://github.com/pythoncapi/pythoncapi_compat
//
// Latest version:
// https://raw.githubusercontent.com/pythoncapi/pythoncapi_compat/master/pythoncapi_compat.h

#ifndef PYTHONCAPI_COMPAT
#define PYTHONCAPI_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "frameobject.h"          // PyFrameObject, PyFrame_GetBack()


// Cast argument to PyObject* type.
#ifndef _PyObject_CAST
#  define _PyObject_CAST(op) ((PyObject*)(op))
#endif


// bpo-42262 added Py_NewRef() and Py_XNewRef() to Python 3.10.0a3
#if PY_VERSION_HEX < 0x030a00A3
static inline PyObject* Py_NewRef(PyObject *obj)
{
    Py_INCREF(obj);
    return obj;
}

static inline PyObject* Py_XNewRef(PyObject *obj)
{
    Py_XINCREF(obj);
    return obj;
}
#endif


// bpo-39573: Py_TYPE(), Py_REFCNT() and Py_SIZE() can no longer be used
// as l-value in Python 3.10.
#if PY_VERSION_HEX < 0x030900A4
static inline void _Py_SET_REFCNT(PyObject *ob, Py_ssize_t refcnt)
{
    ob->ob_refcnt = refcnt;
}
#define Py_SET_REFCNT(ob, refcnt) _Py_SET_REFCNT((PyObject*)(ob), refcnt)


static inline void
_Py_SET_TYPE(PyObject *ob, PyTypeObject *type)
{
    ob->ob_type = type;
}
#define Py_SET_TYPE(ob, type) _Py_SET_TYPE((PyObject*)(ob), type)

static inline void
_Py_SET_SIZE(PyVarObject *ob, Py_ssize_t size)
{
    ob->ob_size = size;
}
#define Py_SET_SIZE(ob, size) _Py_SET_SIZE((PyVarObject*)(ob), size)

#endif  // PY_VERSION_HEX < 0x030900A4


#if PY_VERSION_HEX < 0x030900B1
static inline PyCodeObject*
PyFrame_GetCode(PyFrameObject *frame)
{
    assert(frame != NULL);
    PyCodeObject *code = frame->f_code;
    assert(code != NULL);
    Py_INCREF(code);
    return code;
}
#endif

static inline PyCodeObject*
_PyFrame_GetCodeBorrow(PyFrameObject *frame)
{
    PyCodeObject *code = PyFrame_GetCode(frame);
    Py_DECREF(code);
    return code;  // borrowed reference
}


#if PY_VERSION_HEX < 0x030900B1
static inline PyFrameObject*
PyFrame_GetBack(PyFrameObject *frame)
{
    assert(frame != NULL);
    PyFrameObject *back = frame->f_back;
    Py_XINCREF(back);
    return back;
}
#endif

static inline PyFrameObject*
_PyFrame_GetBackBorrow(PyFrameObject *frame)
{
    PyFrameObject *back = PyFrame_GetBack(frame);
    Py_XDECREF(back);
    return back;  // borrowed reference
}


#if PY_VERSION_HEX < 0x030900A5
static inline PyInterpreterState *
PyThreadState_GetInterpreter(PyThreadState *tstate)
{
    assert(tstate != NULL);
    return tstate->interp;
}
#endif


#if PY_VERSION_HEX < 0x030900B1
static inline PyFrameObject*
PyThreadState_GetFrame(PyThreadState *tstate)
{
    assert(tstate != NULL);
    PyFrameObject *frame = tstate->frame;
    Py_XINCREF(frame);
    return frame;
}
#endif

static inline PyFrameObject*
_PyThreadState_GetFrameBorrow(PyThreadState *tstate)
{
    PyFrameObject *frame = PyThreadState_GetFrame(tstate);
    Py_XDECREF(frame);
    return frame;  // borrowed reference
}


#if PY_VERSION_HEX < 0x030900A5
static inline PyInterpreterState *
PyInterpreterState_Get(void)
{
    PyThreadState *tstate = PyThreadState_GET();
    if (tstate == NULL) {
        Py_FatalError("GIL released (tstate is NULL)");
    }
    PyInterpreterState *interp = tstate->interp;
    if (interp == NULL) {
        Py_FatalError("no current interpreter");
    }
    return interp;
}
#endif


#if 0x030700A1 <= PY_VERSION_HEX && PY_VERSION_HEX < 0x030900A6
static inline uint64_t
PyThreadState_GetID(PyThreadState *tstate)
{
    assert(tstate != NULL);
    return tstate->id;
}
#endif


#if PY_VERSION_HEX < 0x030900A1
static inline PyObject*
PyObject_CallNoArgs(PyObject *func)
{
    return PyObject_CallFunctionObjArgs(func, NULL);
}
#endif


#if PY_VERSION_HEX < 0x030900A4
static inline PyObject*
PyObject_CallOneArg(PyObject *func, PyObject *arg)
{
    return PyObject_CallFunctionObjArgs(func, arg, NULL);
}
#endif


#if PY_VERSION_HEX < 0x030900A5
static inline int
PyModule_AddType(PyObject *module, PyTypeObject *type)
{
    if (PyType_Ready(type) < 0) {
        return -1;
    }

    // inline _PyType_Name()
    const char *name = type->tp_name;
    assert(name != NULL);
    const char *dot = strrchr(name, '.');
    if (dot != NULL) {
        name = dot + 1;
    }

    Py_INCREF(type);
    if (PyModule_AddObject(module, name, (PyObject *)type) < 0) {
        Py_DECREF(type);
        return -1;
    }

    return 0;
}
#endif


#if PY_VERSION_HEX < 0x030900A6
static inline int
PyObject_GC_IsTracked(PyObject* obj)
{
    return (PyObject_IS_GC(obj) && _PyObject_GC_IS_TRACKED(obj));
}

static inline int
PyObject_GC_IsFinalized(PyObject *obj)
{
    return (PyObject_IS_GC(obj) && _PyGCHead_FINALIZED((PyGC_Head *)(obj)-1));
}
#endif  // PY_VERSION_HEX < 0x030900A6


#if PY_VERSION_HEX < 0x030900A4
static inline int
_Py_IS_TYPE(const PyObject *ob, const PyTypeObject *type) {
    return ob->ob_type == type;
}
#define Py_IS_TYPE(ob, type) _Py_IS_TYPE((const PyObject*)(ob), type)
#endif


#ifdef __cplusplus
}
#endif
#endif  // PYTHONCAPI_COMPAT
