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

// CPython 3.6 API backports


#if PY_VERSION_HEX < 0x03060000

#include <stddef.h> /* For offsetof */

#define Py_BUILD_ASSERT(cond) do { (void)Py_BUILD_ASSERT_EXPR(cond); } while(0)

PyObject *_PyObject_CallNoArg(PyObject *func) {
    PyObject *empty_tuple = PyTuple_New(0);
    PyObject *data = PyObject_Call((func), empty_tuple, NULL);
    Py_DECREF(empty_tuple);
    return data;
}

/* _PyBytesWriter API */

#ifndef Py_LIMITED_API
/* The _PyBytesWriter structure is big: it contains an embedded "stack buffer".
   A _PyBytesWriter variable must be declared at the end of variables in a
   function to optimize the memory allocation on the stack. */
typedef struct {
    /* bytes, bytearray or NULL (when the small buffer is used) */
    PyObject *buffer;

    /* Number of allocated size. */
    Py_ssize_t allocated;

    /* Minimum number of allocated bytes,
       incremented by _PyBytesWriter_Prepare() */
    Py_ssize_t min_size;

    /* If non-zero, use a bytearray instead of a bytes object for buffer. */
    int use_bytearray;

    /* If non-zero, overallocate the buffer (default: 0).
       This flag must be zero if use_bytearray is non-zero. */
    int overallocate;

    /* Stack buffer */
    int use_small_buffer;
    char small_buffer[512];
} _PyBytesWriter;


#ifdef MS_WINDOWS
   /* On Windows, overallocate by 50% is the best factor */
#  define OVERALLOCATE_FACTOR 2
#else
   /* On Linux, overallocate by 25% is the best factor */
#  define OVERALLOCATE_FACTOR 4
#endif

void
_PyBytesWriter_Init(_PyBytesWriter *writer)
{
    /* Set all attributes before small_buffer to 0 */
    memset(writer, 0, offsetof(_PyBytesWriter, small_buffer));
#ifdef Py_DEBUG
    memset(writer->small_buffer, 0xCB, sizeof(writer->small_buffer));
#endif
}

void
_PyBytesWriter_Dealloc(_PyBytesWriter *writer)
{
    Py_CLEAR(writer->buffer);
}

Py_LOCAL_INLINE(char*)
_PyBytesWriter_AsString(_PyBytesWriter *writer)
{
    if (writer->use_small_buffer) {
        assert(writer->buffer == NULL);
        return writer->small_buffer;
    }
    else if (writer->use_bytearray) {
        assert(writer->buffer != NULL);
        return PyByteArray_AS_STRING(writer->buffer);
    }
    else {
        assert(writer->buffer != NULL);
        return PyBytes_AS_STRING(writer->buffer);
    }
}

Py_LOCAL_INLINE(Py_ssize_t)
_PyBytesWriter_GetSize(_PyBytesWriter *writer, char *str)
{
    char *start = _PyBytesWriter_AsString(writer);
    assert(str != NULL);
    assert(str >= start);
    assert(str - start <= writer->allocated);
    return str - start;
}

Py_LOCAL_INLINE(void)
_PyBytesWriter_CheckConsistency(_PyBytesWriter *writer, char *str)
{
#ifdef Py_DEBUG
    char *start, *end;

    if (writer->use_small_buffer) {
        assert(writer->buffer == NULL);
    }
    else {
        assert(writer->buffer != NULL);
        if (writer->use_bytearray)
            assert(PyByteArray_CheckExact(writer->buffer));
        else
            assert(PyBytes_CheckExact(writer->buffer));
        assert(Py_REFCNT(writer->buffer) == 1);
    }

    if (writer->use_bytearray) {
        /* bytearray has its own overallocation algorithm,
           writer overallocation must be disabled */
        assert(!writer->overallocate);
    }

    assert(0 <= writer->allocated);
    assert(0 <= writer->min_size && writer->min_size <= writer->allocated);
    /* the last byte must always be null */
    start = _PyBytesWriter_AsString(writer);
    assert(start[writer->allocated] == 0);

    end = start + writer->allocated;
    assert(str != NULL);
    assert(start <= str && str <= end);
#endif
}

void*
_PyBytesWriter_Resize(_PyBytesWriter *writer, void *str, Py_ssize_t size)
{
    Py_ssize_t allocated, pos;

    _PyBytesWriter_CheckConsistency(writer, str);
    assert(writer->allocated < size);

    allocated = size;
    if (writer->overallocate
        && allocated <= (PY_SSIZE_T_MAX - allocated / OVERALLOCATE_FACTOR)) {
        /* overallocate to limit the number of realloc() */
        allocated += allocated / OVERALLOCATE_FACTOR;
    }

    pos = _PyBytesWriter_GetSize(writer, str);
    if (!writer->use_small_buffer) {
        if (writer->use_bytearray) {
            if (PyByteArray_Resize(writer->buffer, allocated))
                goto error;
            /* writer->allocated can be smaller than writer->buffer->ob_alloc,
               but we cannot use ob_alloc because bytes may need to be moved
               to use the whole buffer. bytearray uses an internal optimization
               to avoid moving or copying bytes when bytes are removed at the
               beginning (ex: del bytearray[:1]). */
        }
        else {
            if (_PyBytes_Resize(&writer->buffer, allocated))
                goto error;
        }
    }
    else {
        /* convert from stack buffer to bytes object buffer */
        assert(writer->buffer == NULL);

        if (writer->use_bytearray)
            writer->buffer = PyByteArray_FromStringAndSize(NULL, allocated);
        else
            writer->buffer = PyBytes_FromStringAndSize(NULL, allocated);
        if (writer->buffer == NULL)
            goto error;

        if (pos != 0) {
            char *dest;
            if (writer->use_bytearray)
                dest = PyByteArray_AS_STRING(writer->buffer);
            else
                dest = PyBytes_AS_STRING(writer->buffer);
            memcpy(dest,
                      writer->small_buffer,
                      pos);
        }

        writer->use_small_buffer = 0;
#ifdef Py_DEBUG
        memset(writer->small_buffer, 0xDB, sizeof(writer->small_buffer));
#endif
    }
    writer->allocated = allocated;

    str = _PyBytesWriter_AsString(writer) + pos;
    _PyBytesWriter_CheckConsistency(writer, str);
    return str;

error:
    _PyBytesWriter_Dealloc(writer);
    return NULL;
}

void*
_PyBytesWriter_Prepare(_PyBytesWriter *writer, void *str, Py_ssize_t size)
{
    Py_ssize_t new_min_size;

    _PyBytesWriter_CheckConsistency(writer, str);
    assert(size >= 0);

    if (size == 0) {
        /* nothing to do */
        return str;
    }

    if (writer->min_size > PY_SSIZE_T_MAX - size) {
        PyErr_NoMemory();
        _PyBytesWriter_Dealloc(writer);
        return NULL;
    }
    new_min_size = writer->min_size + size;

    if (new_min_size > writer->allocated)
        str = _PyBytesWriter_Resize(writer, str, new_min_size);

    writer->min_size = new_min_size;
    return str;
}

/* Allocate the buffer to write size bytes.
   Return the pointer to the beginning of buffer data.
   Raise an exception and return NULL on error. */
void*
_PyBytesWriter_Alloc(_PyBytesWriter *writer, Py_ssize_t size)
{
    /* ensure that _PyBytesWriter_Alloc() is only called once */
    assert(writer->min_size == 0 && writer->buffer == NULL);
    assert(size >= 0);

    writer->use_small_buffer = 1;
#ifdef Py_DEBUG
    writer->allocated = sizeof(writer->small_buffer) - 1;
    /* In debug mode, don't use the full small buffer because it is less
       efficient than bytes and bytearray objects to detect buffer underflow
       and buffer overflow. Use 10 bytes of the small buffer to test also
       code using the smaller buffer in debug mode.

       Don't modify the _PyBytesWriter structure (use a shorter small buffer)
       in debug mode to also be able to detect stack overflow when running
       tests in debug mode. The _PyBytesWriter is large (more than 512 bytes),
       if Py_EnterRecursiveCall() is not used in deep C callback, we may hit a
       stack overflow. */
    writer->allocated = Py_MIN(writer->allocated, 10);
    /* _PyBytesWriter_CheckConsistency() requires the last byte to be 0,
       to detect buffer overflow */
    writer->small_buffer[writer->allocated] = 0;
#else
    writer->allocated = sizeof(writer->small_buffer);
#endif
    return _PyBytesWriter_Prepare(writer, writer->small_buffer, size);
}

PyObject *
_PyBytesWriter_Finish(_PyBytesWriter *writer, void *str)
{
    Py_ssize_t size;
    PyObject *result;

    _PyBytesWriter_CheckConsistency(writer, str);

    size = _PyBytesWriter_GetSize(writer, str);
    if (size == 0 && !writer->use_bytearray) {
        Py_CLEAR(writer->buffer);
        /* Get the empty byte string singleton */
        result = PyBytes_FromStringAndSize(NULL, 0);
    }
    else if (writer->use_small_buffer) {
        if (writer->use_bytearray) {
            result = PyByteArray_FromStringAndSize(writer->small_buffer, size);
        }
        else {
            result = PyBytes_FromStringAndSize(writer->small_buffer, size);
        }
    }
    else {
        result = writer->buffer;
        writer->buffer = NULL;

        if (size != writer->allocated) {
            if (writer->use_bytearray) {
                if (PyByteArray_Resize(result, size)) {
                    Py_DECREF(result);
                    return NULL;
                }
            }
            else {
                if (_PyBytes_Resize(&result, size)) {
                    assert(result == NULL);
                    return NULL;
                }
            }
        }
    }
    return result;
}

void*
_PyBytesWriter_WriteBytes(_PyBytesWriter *writer, void *ptr,
                          const void *bytes, Py_ssize_t size)
{
    char *str = (char *)ptr;

    str = _PyBytesWriter_Prepare(writer, str, size);
    if (str == NULL)
        return NULL;

    memcpy(str, bytes, size);
    str += size;

    return str;
}
#endif   /* Py_LIMITED_API */

#endif
