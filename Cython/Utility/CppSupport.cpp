/////////////// CppExceptionConversion.proto ///////////////

#ifndef __Pyx_CppExn2PyErr
#include <new>
#include <typeinfo>
#include <stdexcept>
#include <ios>

static void __Pyx_CppExn2PyErr() {
  // Catch a handful of different errors here and turn them into the
  // equivalent Python errors.
  try {
    if (PyErr_Occurred())
      ; // let the latest Python exn pass through and ignore the current one
    else
      throw;
  } catch (const std::bad_alloc& exn) {
    PyErr_SetString(PyExc_MemoryError, exn.what());
  } catch (const std::bad_cast& exn) {
    PyErr_SetString(PyExc_TypeError, exn.what());
  } catch (const std::bad_typeid& exn) {
    PyErr_SetString(PyExc_TypeError, exn.what());
  } catch (const std::domain_error& exn) {
    PyErr_SetString(PyExc_ValueError, exn.what());
  } catch (const std::invalid_argument& exn) {
    PyErr_SetString(PyExc_ValueError, exn.what());
  } catch (const std::ios_base::failure& exn) {
    // Unfortunately, in standard C++ we have no way of distinguishing EOF
    // from other errors here; be careful with the exception mask
    PyErr_SetString(PyExc_IOError, exn.what());
  } catch (const std::out_of_range& exn) {
    // Change out_of_range to IndexError
    PyErr_SetString(PyExc_IndexError, exn.what());
  } catch (const std::overflow_error& exn) {
    PyErr_SetString(PyExc_OverflowError, exn.what());
  } catch (const std::range_error& exn) {
    PyErr_SetString(PyExc_ArithmeticError, exn.what());
  } catch (const std::underflow_error& exn) {
    PyErr_SetString(PyExc_ArithmeticError, exn.what());
  } catch (const std::exception& exn) {
    PyErr_SetString(PyExc_RuntimeError, exn.what());
  }
  catch (...)
  {
    PyErr_SetString(PyExc_RuntimeError, "Unknown exception");
  }
}
#endif

/////////////// PythranConversion.proto ///////////////

template <class T>
auto __Pyx_pythran_to_python(T &&value) -> decltype(to_python(
      typename pythonic::returnable<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::type{std::forward<T>(value)}))
{
  using returnable_type = typename pythonic::returnable<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::type;
  return to_python(returnable_type{std::forward<T>(value)});
}

#define __Pyx_PythranShapeAccessor(x) (x.shape().array())

////////////// MoveIfSupported.proto //////////////////

#if __cplusplus >= 201103L
  #include <utility>
  #define __PYX_STD_MOVE_IF_SUPPORTED(x) std::move(x)
#else
  #define __PYX_STD_MOVE_IF_SUPPORTED(x) x
#endif

/////////////// CheckedResult.proto ///////////////

#include <type_traits>

template <typename T, bool = std::is_void<T>::value>
struct CheckedResult {};

template <typename T>
class CheckedResult<T, false> {
  enum Status { Ok, Err };

  private:
    enum Status status;

  public:
    T result;

    CheckedResult(const T& value) : status(Ok), result(value) {}
    CheckedResult() : status(Err) {}
    operator T() { return result; }
    void set_error() { status = Err; }
    bool is_error() { return status == Err; }
};

template <typename T>
class CheckedResult<T, true> {
  enum Status { Ok, Err };

  public:
    CheckedResult() : status(Ok) {}
    void set_error() { status = Err; }
    bool is_error() { return status == Err; }

  private:
    enum Status status;
};
