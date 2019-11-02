#include "Python.h"

// CPython 3.8 API backports

#if PY_VERSION_HEX < 0x03080000

/* Convenience function to get a builtin from its name.
 * Note this was backported at some point to 3.6.x and 3.7.x,
 * so it's possible to get a compilation warning.
 */
PyObject *
_PyEval_GetBuiltinId(_Py_Identifier *name)
{
    PyObject *attr = _PyDict_GetItemIdWithError(PyEval_GetBuiltins(), name);
    if (attr) {
        Py_INCREF(attr);
    }
    else if (!PyErr_Occurred()) {
        PyErr_SetObject(PyExc_AttributeError, _PyUnicode_FromId(name));
    }
    return attr;
}

#endif

// CPython 3.7 API backports

#if PY_VERSION_HEX < 0x03070000

#define Py_UNREACHABLE() abort()

#define PyDict_GET_SIZE PyDict_Size

static int
_PyObject_LookupAttr(PyObject *v, PyObject *name, PyObject **result)
{
    PyTypeObject *tp = Py_TYPE(v);

    if (!PyUnicode_Check(name)) {
        PyErr_Format(PyExc_TypeError,
                     "attribute name must be string, not '%.200s'",
                     name->ob_type->tp_name);
        *result = NULL;
        return -1;
    }

    if (tp->tp_getattro == PyObject_GenericGetAttr) {
        *result = _PyObject_GenericGetAttrWithDict(v, name, NULL);
        if (*result != NULL) {
            return 1;
        }
        if (PyErr_ExceptionMatches(PyExc_AttributeError)) {
            PyErr_Clear();
        }
        else if (PyErr_Occurred()) {
            return -1;
        }
        return 0;
    }
    if (tp->tp_getattro != NULL) {
        *result = (*tp->tp_getattro)(v, name);
    }
    else if (tp->tp_getattr != NULL) {
        const char *name_str = PyUnicode_AsUTF8(name);
        if (name_str == NULL) {
            *result = NULL;
            return -1;
        }
        *result = (*tp->tp_getattr)(v, (char *)name_str);
    }
    else {
        *result = NULL;
        return 0;
    }

    if (*result != NULL) {
        return 1;
    }
    if (!PyErr_ExceptionMatches(PyExc_AttributeError)) {
        return -1;
    }
    PyErr_Clear();
    return 0;
}

static int
_PyObject_LookupAttrId(PyObject *v, _Py_Identifier *name, PyObject **result)
{
    PyObject *oname = _PyUnicode_FromId(name); /* borrowed */
    if (!oname) {
        *result = NULL;
        return -1;
    }
    return  _PyObject_LookupAttr(v, oname, result);
}

#endif
