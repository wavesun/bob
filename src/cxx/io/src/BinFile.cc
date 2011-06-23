/**
 * @file src/cxx/io/src/BinFile.cc
 * @author <a href="mailto:Laurent.El-Shafey@idiap.ch">Laurent El Shafey</a> 
 *
 * @brief This class can be used to store and load multiarrays into/from files.
 */

#include "core/logging.h"
#include "core/array_type.h"

#include "io/BinFile.h"

namespace io = Torch::io;
namespace core = Torch::core;

io::BinFile::BinFile(const std::string& filename, io::BinFile::openmode flag):
  m_header_init(false),
  m_current_array(0),
  m_n_arrays_written(0),
  m_openmode(flag)
{
  if((flag & io::BinFile::out) && (flag & io::BinFile::in)) {
    m_stream.open(filename.c_str(), std::ios::in | std::ios::out | 
        std::ios::binary);
    if(m_stream)
    {
      m_header.read(m_stream);
      m_header_init = true;
      m_n_arrays_written = m_header.m_n_samples;

      if (flag & io::BinFile::append) {
        m_stream.seekp(0, std::ios::end);
        m_current_array = m_header.m_n_samples;
      }
    }
  }
  else if(flag & io::BinFile::out) {  
    if(m_stream && (flag & io::BinFile::append)) {
      m_stream.open(filename.c_str(), std::ios::out | std::ios::in);
      m_header.read(m_stream);
      m_header_init = true;
      m_n_arrays_written = m_header.m_n_samples;
      m_stream.seekp(0, std::ios::end);
      m_current_array = m_header.m_n_samples;
    }
    else
      m_stream.open(filename.c_str(), std::ios::out | std::ios::binary);
  }
  else if(flag & io::BinFile::in) {
    m_stream.open(filename.c_str(), std::ios::in | std::ios::binary);
    if(m_stream) {
      m_header.read(m_stream);
      m_header_init = true;
      m_n_arrays_written = m_header.m_n_samples;

      if (flag & io::BinFile::append) {
        core::error << "Cannot append data in read only mode." << std::endl;
        throw core::Exception();
      }
    }
  }
  else
  {
    core::error << "Invalid combination of flags." << std::endl;
    throw core::Exception();
  }
}

io::BinFile::~BinFile() {
  close();
}

void io::BinFile::close() {
  // Rewrite the header and update the number of samples
  m_header.m_n_samples = m_n_arrays_written;
  if(m_openmode & io::BinFile::out) m_header.write(m_stream);

  m_stream.close();
}

void io::BinFile::initHeader(const Torch::core::array::ElementType type, 
    size_t ndim, const size_t* shape) {
  // Check that data have not already been written
  if (m_n_arrays_written > 0 ) {
    Torch::core::error << "Cannot init the header of an output stream in which data" <<
      " have already been written." << std::endl;
    throw Torch::core::Exception();
  }

  // Initialize header
  m_header.m_elem_type = type;
  m_header.m_elem_sizeof = Torch::core::array::getElementSize(type);
  m_header.setShape(ndim, shape);
  m_header.write(m_stream);
  m_header_init = true;
}

template <typename T>
void write_inlined(const io::detail::InlinedArrayImpl& data, std::ostream& s) {

  switch(data.getNDim()) {
    case 1:
      {
        const blitz::Array<T,1>& bz = data.get<T,1>();
        for (int i=0; i<(int)data.getShape()[0]; ++i) {
          T val = bz(i);
          s.write((const char*)&val, core::array::getElementSize<T>());
        }
        break;
      }
    case 2:
      {
        const blitz::Array<T,2>& bz = data.get<T,2>();
        for (int i=0; i<(int)data.getShape()[0]; ++i)
          for (int j=0; j<(int)data.getShape()[1]; ++j) {
            T val = bz(i,j);
            s.write((const char*)&val, core::array::getElementSize<T>());
          }
        break;
      }
    case 3:
      {
        const blitz::Array<T,3>& bz = data.get<T,3>();
        for (int i=0; i<(int)data.getShape()[0]; ++i)
          for (int j=0; j<(int)data.getShape()[1]; ++j)
            for (int k=0; k<(int)data.getShape()[2]; ++k) {
              T val = bz(i,j,k);
              s.write((const char*)&val, core::array::getElementSize<T>());
            }
        break;
      }
    case 4:
      {
        const blitz::Array<T,4>& bz = data.get<T,4>();
        for (int i=0; i<(int)data.getShape()[0]; ++i)
          for (int j=0; j<(int)data.getShape()[1]; ++j)
            for (int k=0; k<(int)data.getShape()[2]; ++k)
              for (int l=0; l<(int)data.getShape()[3]; ++l) {
                T val = bz(i,j,k,l);
                s.write((const char*)&val, core::array::getElementSize<T>());
              }
        break;
      }
    default:
      throw io::DimensionError(data.getNDim(), Torch::core::array::N_MAX_DIMENSIONS_ARRAY);
  }
}

void io::BinFile::write(const io::detail::InlinedArrayImpl& data) {

  /**
   * @warning: Please convert your files to HDF5, this format is
   * deprecated starting on 16.04.2011 - AA
   */
  throw core::DeprecationError("BinFile format deprecated on 16.04.2011, use HDF5");

  if(!m_header_init) {
    //initializes the header
    initHeader(data.getElementType(), data.getNDim(), data.getShape());
  }
  else {
      //checks compatibility with previously written stuff
      if (data.getNDim() != m_header.getNDim()) throw DimensionError(data.getNDim(), m_header.getNDim());
      const size_t* p_shape = data.getShape();
      const size_t* h_shape = m_header.getShape();
      for (size_t i=0; i<data.getNDim(); ++i)
        if(p_shape[i] != h_shape[i]) throw DimensionError(p_shape[i], h_shape[i]);
  }

  // copy the data into the output stream
  switch(data.getElementType()) {
    case Torch::core::array::t_bool: write_inlined<bool>(data, m_stream); break;
    case Torch::core::array::t_int8: write_inlined<int8_t>(data, m_stream); break;
    case Torch::core::array::t_int16: write_inlined<int16_t>(data, m_stream); break;
    case Torch::core::array::t_int32: write_inlined<int32_t>(data, m_stream); break;
    case Torch::core::array::t_int64: write_inlined<int64_t>(data, m_stream); break;
    case Torch::core::array::t_uint8: write_inlined<uint8_t>(data, m_stream); break;
    case Torch::core::array::t_uint16: write_inlined<uint16_t>(data, m_stream); break;
    case Torch::core::array::t_uint32: write_inlined<uint32_t>(data, m_stream); break;
    case Torch::core::array::t_uint64: write_inlined<uint64_t>(data, m_stream); break;
    case Torch::core::array::t_float32: write_inlined<float>(data, m_stream); break;
    case Torch::core::array::t_float64: write_inlined<double>(data, m_stream); break;
    case Torch::core::array::t_float128: write_inlined<long double>(data, m_stream); break;
    case Torch::core::array::t_complex64: write_inlined<std::complex<float> >(data, m_stream); break;
    case Torch::core::array::t_complex128: write_inlined<std::complex<double> >(data, m_stream); break;
    case Torch::core::array::t_complex256: write_inlined<std::complex<long double> >(data, m_stream); break;
    default: throw TypeError(data.getElementType(), Torch::core::array::t_unknown);
  }

  // increment m_n_arrays_written and m_current_array
  ++m_current_array;
  if (m_current_array>m_n_arrays_written) ++m_n_arrays_written;
}

template <typename T>
io::detail::InlinedArrayImpl read_inlined(size_t ndim, const size_t* shape,
    std::istream& s) {
  switch(ndim) {
    case 1:
      {
        blitz::Array<T,1> bz(shape[0]);
        s.read(reinterpret_cast<char*>(bz.data()), shape[0]*sizeof(T));
        return bz;
      }
    case 2:
      {
        //arrays are always stored in C-style ordering
        blitz::Array<T,2> bz(shape[0], shape[1]);
        s.read(reinterpret_cast<char*>(bz.data()), shape[0]*shape[1]*sizeof(T));
        return bz;
      }
    case 3:
      {
        //arrays are always stored in C-style ordering
        blitz::Array<T,3> bz(shape[0], shape[1], shape[2]);
        s.read(reinterpret_cast<char*>(bz.data()), shape[0]*shape[1]*shape[2]*sizeof(T));
        return bz;
      }
    case 4:
      {
        //arrays are always stored in C-style ordering
        blitz::Array<T,4> bz(shape[0], shape[1], shape[2], shape[3]);
        s.read(reinterpret_cast<char*>(bz.data()), shape[0]*shape[1]*shape[2]*shape[3]*sizeof(T));
        return bz;
      }
    default:
      throw io::DimensionError(ndim, Torch::core::array::N_MAX_DIMENSIONS_ARRAY);
  }
}

io::detail::InlinedArrayImpl io::BinFile::read() {
  if(!m_header_init) throw Uninitialized();
  switch(getElementType()) {
    case Torch::core::array::t_bool: return read_inlined<bool>(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_int8: return read_inlined<int8_t>(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_int16: return read_inlined<int16_t>(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_int32: return read_inlined<int32_t>(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_int64: return read_inlined<int64_t>(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_uint8: return read_inlined<uint8_t>(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_uint16: return read_inlined<uint16_t>(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_uint32: return read_inlined<uint32_t>(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_uint64: return read_inlined<uint64_t>(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_float32: return read_inlined<float>(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_float64: return read_inlined<double>(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_float128: return read_inlined<long double>(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_complex64: return read_inlined<std::complex<float> >(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_complex128: return read_inlined<std::complex<double> >(m_header.getNDim(), m_header.getShape(), m_stream);
    case Torch::core::array::t_complex256: return read_inlined<std::complex<long double> >(m_header.getNDim(), m_header.getShape(), m_stream);
    default: throw Torch::io::TypeError(getElementType(), Torch::core::array::t_unknown);
  }
  ++m_current_array;
}

io::detail::InlinedArrayImpl io::BinFile::read (size_t index) {
  // Check that we are reaching an existing array
  if( index > m_header.m_n_samples ) {
    core::error << "Trying to reach a non-existing array." << std::endl;
    throw IndexError(index);
  }

  // Set the stream pointer at the correct position
  m_stream.seekg( m_header.getArrayIndex(index) );
  m_current_array = index;

  // Put the content of the stream in the blitz array.
  return read();
}