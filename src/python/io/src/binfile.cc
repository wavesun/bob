/**
 * @author <a href="mailto:andre.dos.anjos@gmail.com">Andre Anjos</a> 
 * @author <a href="mailto:Laurent.El-Shafey@idiap.ch">Laurent El Shafey</a> 
 * @date Wed 26 Jan 2011 07:46:17
 *
 * @brief Python bindings to io::BinFile
 */

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <blitz/array.h>

#include "io/BinFile.h"

#include <iostream>

using namespace boost::python;
namespace io = Torch::io;

template <typename T>
static tuple get_shape(const T& f) {
  size_t ndim = f.getNDimensions();
  const size_t* shape = f.getShape();
  switch (ndim) {
    case 1:
      return make_tuple(shape[0]);
    case 2:
      return make_tuple(shape[0], shape[1]);
    case 3:
      return make_tuple(shape[0], shape[1], shape[2]);
    case 4:
      return make_tuple(shape[0], shape[1], shape[2], shape[3]);
    default:
      break;
  }
  return make_tuple();
}

/**
 * Allows us to write BinFile("filename.bin", "rb")
 */
static boost::shared_ptr<io::BinFile>
binfile_make_fromstr(const std::string& filename, const std::string& opmode) {
  io::BinFile::openmode mode = io::_unset;
  for (size_t i=0; i<opmode.size(); ++i) {
    if (opmode[i] == 'r') mode |= io::BinFile::in;
    else if (opmode[i] == 'w') mode |= io::BinFile::out;
    else if (opmode[i] == 'a' || opmode[i] == '+') mode |= io::BinFile::append; 
    else { //anything else is just unsupported for the time being
      PyErr_SetString(PyExc_RuntimeError, "Supported flags are 'r' (read), 'w' (write) or 'a'/'+' (append) or combination of those");
      boost::python::throw_error_already_set();
    }
  }
  return boost::make_shared<io::BinFile>(filename, mode);
}

/**
 * Allows us to write BinFile("filename.bin") and open that file for reading
 */
static boost::shared_ptr<io::BinFile>
binfile_make_readable(const std::string& filename) {
  return boost::make_shared<io::BinFile>(filename, io::_in);
}

/**
 * blitz::Array<> writing
 */
template <typename T, int D> 
static void bzwrite(io::BinFile& f, blitz::Array<T,D>& bz) {
  f.write(io::detail::InlinedArrayImpl(bz));
}


static const char* ARRAY_READ_DOC = "Reads data in the binary file and return a blitz::Array with a copy of this data.";
static const char* ARRAY_WRITE_DOC = "Writes a single blitz::Array<> into the binary file. Please note that this array should conform to the shape and element type of the arrays already inserted. If no array was inserted, the element type and shape will be defined when you first write an array to this binary file.";
#define ARRAY_DEF(T,N,D) .def(BOOST_PP_STRINGIZE(__getitem_ ## N ## _ ## D ## __), (blitz::Array<T,D> (io::BinFile::*)(size_t))&io::BinFile::read<T,D>, (arg("self"), arg("index")), ARRAY_READ_DOC) \
.def("write", &bzwrite<T,D>, (arg("self"), arg("array")), ARRAY_WRITE_DOC)

void bind_io_binfile() {
  class_<io::BinFile, boost::shared_ptr<io::BinFile>, boost::noncopyable>("BinFile", "A BinFile allows users to read and write data from and to files containing standard Torch binary coded data", no_init)
    .def("__init__", make_constructor(binfile_make_fromstr, default_call_policies(), (arg("filename"), arg("openmode_string"))), "Opens a new file for reading (pass 'r' as second parameter), writing (pass 'w') or appending (pass 'a') depending on the given flag.")
    .def("__init__", make_constructor(binfile_make_readable, default_call_policies(), (arg("filename"))), "Opens a new file for reading")
    .add_property("shape", &get_shape<io::BinFile>, "The shape of arrays in this binary file. Please note all arrays in the file have necessarily the same shape.")
    .add_property("elementType", &io::BinFile::getElementType, "The type of array elements contained in this binary file. This would be equivalent to the 'T' bit in blitz::Array<T,D>.")
    .def("__len__", &io::BinFile::getNSamples, "The number of arrays in this binary file.")
    ARRAY_DEF(bool, bool, 1)
    ARRAY_DEF(int8_t, int8, 1)
    ARRAY_DEF(int16_t, int16, 1)
    ARRAY_DEF(int32_t, int32, 1)
    ARRAY_DEF(int64_t, int64, 1)
    ARRAY_DEF(uint8_t, uint8, 1)
    ARRAY_DEF(uint16_t, uint16, 1)
    ARRAY_DEF(uint32_t, uint32, 1)
    ARRAY_DEF(uint64_t, uint64, 1)
    ARRAY_DEF(float, float32, 1)
    ARRAY_DEF(double, float64, 1)
    ARRAY_DEF(double, float128, 1)
    ARRAY_DEF(std::complex<float>, complex64, 1)
    ARRAY_DEF(std::complex<double>, complex128, 1)
    ARRAY_DEF(std::complex<double>, complex256, 1)
    ARRAY_DEF(bool, bool, 2)
    ARRAY_DEF(int8_t, int8, 2)
    ARRAY_DEF(int16_t, int16, 2)
    ARRAY_DEF(int32_t, int32, 2)
    ARRAY_DEF(int64_t, int64, 2)
    ARRAY_DEF(uint8_t, uint8, 2)
    ARRAY_DEF(uint16_t, uint16, 2)
    ARRAY_DEF(uint32_t, uint32, 2)
    ARRAY_DEF(uint64_t, uint64, 2)
    ARRAY_DEF(float, float32, 2)
    ARRAY_DEF(double, float64, 2)
    ARRAY_DEF(double, float128, 2)
    ARRAY_DEF(std::complex<float>, complex64, 2)
    ARRAY_DEF(std::complex<double>, complex128, 2)
    ARRAY_DEF(std::complex<double>, complex256, 2)
    ARRAY_DEF(bool, bool, 3)
    ARRAY_DEF(int8_t, int8, 3)
    ARRAY_DEF(int16_t, int16, 3)
    ARRAY_DEF(int32_t, int32, 3)
    ARRAY_DEF(int64_t, int64, 3)
    ARRAY_DEF(uint8_t, uint8, 3)
    ARRAY_DEF(uint16_t, uint16, 3)
    ARRAY_DEF(uint32_t, uint32, 3)
    ARRAY_DEF(uint64_t, uint64, 3)
    ARRAY_DEF(float, float32, 3)
    ARRAY_DEF(double, float64, 3)
    ARRAY_DEF(double, float128, 3)
    ARRAY_DEF(std::complex<float>, complex64, 3)
    ARRAY_DEF(std::complex<double>, complex128, 3)
    ARRAY_DEF(std::complex<double>, complex256, 3)
    ARRAY_DEF(bool, bool, 4)
    ARRAY_DEF(int8_t, int8, 4)
    ARRAY_DEF(int16_t, int16, 4)
    ARRAY_DEF(int32_t, int32, 4)
    ARRAY_DEF(int64_t, int64, 4)
    ARRAY_DEF(uint8_t, uint8, 4)
    ARRAY_DEF(uint16_t, uint16, 4)
    ARRAY_DEF(uint32_t, uint32, 4)
    ARRAY_DEF(uint64_t, uint64, 4)
    ARRAY_DEF(float, float32, 4)
    ARRAY_DEF(double, float64, 4)
    ARRAY_DEF(double, float128, 4)
    ARRAY_DEF(std::complex<float>, complex64, 4)
    ARRAY_DEF(std::complex<double>, complex128, 4)
    ARRAY_DEF(std::complex<double>, complex256, 4)
    ;

}