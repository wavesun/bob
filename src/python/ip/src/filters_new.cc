/**
 * @file src/python/ip/src/filters_new.cc 
 * @author <a href="mailto:Laurent.El-Shafey@idiap.ch">Laurent El Shafey</a> 
 *
 * @brief Binds new filters implementation to python 
 */

#include <boost/python.hpp>

#include "core/logging.h"
#include "ip/crop.h"
#include "ip/flipflop.h"
#include "ip/gammaCorrection.h"
#include "ip/scale.h"
#include "ip/shear.h"
#include "ip/shift.h"
#include "ip/generateWithCenter.h"
#include "ip/zigzag.h"

using namespace boost::python;

static const char* CROP2D_DOC = "Crop a 2D blitz array/image.";
static const char* CROP2D_MASK_DOC = "Crop a 2D blitz array/image, taking mask into account.";
static const char* CROP3D_DOC = "Crop a 3D blitz array/image.";
static const char* CROP3D_MASK_DOC = "Crop a 3D blitz array/image, taking mask into account.";
static const char* FLIP2D_DOC = "Flip a 2D blitz array/image upside-down.";
static const char* FLIP3D_DOC = "Flip a 3D blitz array/image upside-down.";
static const char* FLOP2D_DOC = "Flop a 2D blitz array/image left-right.";
static const char* FLOP3D_DOC = "Flop a 3D blitz array/image left-right.";
static const char* GENERATEWITHCENTER2D_DOC = "Extend a 2D blitz array/image, putting a given point in the center.";
static const char* GENERATEWITHCENTER2D_MASK_DOC = "Extend a 2D blitz array/image, putting a given point in the center, taking mask into account.";
static const char* GET_GENERATEWITHCENTER_SHAPE2D_DOC = "Return the shape of the output 2D blitz array/image, when calling generateWithCenter which puts a given point of an image in the center.";
static const char* GET_GENERATEWITHCENTER_OFFSET2D_DOC = "Return the offset of the output 2D blitz array/image, when calling generateWithCenter which puts a given point of an image in the center.";
static const char* RESCALE2D_DOC = "Rescale a 2D blitz array/image with the given dimensions.";
static const char* SHEARX2D_DOC = "Shear a 2D blitz array/image with the given shear parameter along the X-dimension.";
static const char* SHEARY2D_DOC = "Shear a 2D blitz array/image with the given shear parameter along the Y-dimension.";
static const char* SHIFT2D_DOC = "Shift a 2D blitz array/image.";
static const char* SHIFT2D_MASK_DOC = "Shift a 2D blitz array/image, taking mask into account.";
static const char* SHIFT3D_DOC = "Shift a 3D blitz array/image.";
static const char* SHIFT3D_MASK_DOC = "Shift a 3D blitz array/image, taking mask into account.";
static const char* GAMMACORRECTION2D_DOC = "Perform a power-law gamma correction on a 2D blitz array/image.";
static const char* ZIGZAG2D_DOC = "Extract a 1D blitz array using a zigzag pattern from a 2D blitz array/image.";
static const char* SCALE_DOC = "Gives back a scaled version of the original blitz array (image)";

#define FILTER_DECL(T,N) \
  BOOST_PYTHON_FUNCTION_OVERLOADS(crop_overloads_ ## N, Torch::ip::crop<T>, 6, 8) \
  BOOST_PYTHON_FUNCTION_OVERLOADS(crop_mask_overloads_ ## N, Torch::ip::crop<T>, 8, 10) \
  BOOST_PYTHON_FUNCTION_OVERLOADS(rescale_overloads_ ## N, Torch::ip::scale<T>, 2, 3) \
  BOOST_PYTHON_FUNCTION_OVERLOADS(shearX_overloads_ ## N, Torch::ip::shearX<T>, 3, 4) \
  BOOST_PYTHON_FUNCTION_OVERLOADS(shearY_overloads_ ## N, Torch::ip::shearY<T>, 3, 4) \
  BOOST_PYTHON_FUNCTION_OVERLOADS(shift_overloads_ ## N, Torch::ip::shift<T>, 4, 6) \
  BOOST_PYTHON_FUNCTION_OVERLOADS(shift_mask_overloads_ ## N, Torch::ip::shift<T>, 6, 8) \
  BOOST_PYTHON_FUNCTION_OVERLOADS(zigzag_overloads_ ## N, Torch::ip::zigzag<T>, 2, 4)


#define FILTER_DEF(T,N) \
  def("crop", (void (*)(const blitz::Array<T,2>&, blitz::Array<T,2>&, const int, const int, const int, const int, const bool, const bool))&Torch::ip::crop<T>, crop_overloads_ ## N ((arg("src"), arg("dst"), arg("crop_y"), arg("crop_x"), arg("crop_h"), arg("crop_w"), arg("allow_out")="False", arg("zero_out")="False"), CROP2D_DOC)); \
  def("crop", (void (*)(const blitz::Array<T,2>&, const blitz::Array<bool,2>&, blitz::Array<T,2>&, blitz::Array<bool,2>&, const int, const int, const int, const int, const bool, const bool))&Torch::ip::crop<T>, crop_mask_overloads_ ## N ((arg("src"), arg("src_mask"), arg("dst"), arg("dst_mask"), arg("crop_y"), arg("crop_x"), arg("crop_h"), arg("crop_w"), arg("allow_out")="False", arg("zero_out")="False"), CROP2D_MASK_DOC)); \
  def("crop", (void (*)(const blitz::Array<T,3>&, blitz::Array<T,3>&, const int, const int, const int, const int, const bool, const bool))&Torch::ip::crop<T>, crop_overloads_ ## N ((arg("src"), arg("dst"), arg("crop_y"), arg("crop_x"), arg("crop_h"), arg("crop_w"), arg("allow_out")="False", arg("zero_out")="False"), CROP3D_DOC)); \
  def("crop", (void (*)(const blitz::Array<T,3>&, const blitz::Array<bool,3>&, blitz::Array<T,3>&, blitz::Array<bool,3>&, const int, const int, const int, const int, const bool, const bool))&Torch::ip::crop<T>, crop_mask_overloads_ ## N ((arg("src"), arg("src_mask"), arg("dst"), arg("dst_mask"), arg("crop_y"), arg("crop_x"), arg("crop_h"), arg("crop_w"), arg("allow_out")="False", arg("zero_out")="False"), CROP3D_MASK_DOC)); \
  def("flip", (void (*)(const blitz::Array<T,2>&, blitz::Array<T,2>&))&Torch::ip::flip<T>, (arg("src"), arg("dst")), FLIP2D_DOC); \
  def("flip", (void (*)(const blitz::Array<T,3>&, blitz::Array<T,3>&))&Torch::ip::flip<T>, (arg("src"), arg("dst")), FLIP3D_DOC); \
  def("flop", (void (*)(const blitz::Array<T,2>&, blitz::Array<T,2>&))&Torch::ip::flop<T>, (arg("src"), arg("dst")), FLOP2D_DOC); \
  def("flop", (void (*)(const blitz::Array<T,3>&, blitz::Array<T,3>&))&Torch::ip::flop<T>, (arg("src"), arg("dst")), FLOP3D_DOC); \
  def("getGenerateWithCenterShape", (const blitz::TinyVector<int,2> (*)(const blitz::Array<T,2>&, const int, const int))&Torch::ip::getGenerateWithCenterShape<T>, (arg("src"), arg("center_y"), arg("center_x")), GET_GENERATEWITHCENTER_SHAPE2D_DOC); \
  def("getGenerateWithCenterOffset", (const blitz::TinyVector<int,2> (*)(const blitz::Array<T,2>&, const int, const int))&Torch::ip::getGenerateWithCenterOffset<T>, (arg("src"), arg("center_y"), arg("center_x")), GET_GENERATEWITHCENTER_OFFSET2D_DOC); \
  def("generateWithCenter", (void (*)(const blitz::Array<T,2>&, blitz::Array<T,2>&, const int, const int))&Torch::ip::generateWithCenter<T>, (arg("src"), arg("dst"), arg("center_y"), arg("center_x")), GENERATEWITHCENTER2D_DOC); \
  def("generateWithCenter", (void (*)(const blitz::Array<T,2>&, const blitz::Array<bool,2>&, blitz::Array<T,2>&, blitz::Array<bool,2>&, const int, const int))&Torch::ip::generateWithCenter<T>, (arg("src"), arg("src_mask"), arg("dst"), arg("dst_mask"), arg("center_y"), arg("center_x")), GENERATEWITHCENTER2D_MASK_DOC); \
  def("scale", (void (*)(const blitz::Array<T,2>&, blitz::Array<T,2>&, const enum Torch::ip::Rescale::Algorithm))&Torch::ip::scale<T>, rescale_overloads_ ## N ((arg("src"), arg("dst"), arg("algorithm")="BilinearInterp"), RESCALE2D_DOC)); \
  def("shearX", (void (*)(const blitz::Array<T,2>&, blitz::Array<double,2>&, const double, const bool))&Torch::ip::shearX<T>, shearX_overloads_ ## N ((arg("src"), arg("dst"), arg("angle"), arg("antialias")="True"), SHEARX2D_DOC)); \
  def("shearY", (void (*)(const blitz::Array<T,2>&, blitz::Array<double,2>&, const double, const bool))&Torch::ip::shearY<T>, shearY_overloads_ ## N ((arg("src"), arg("dst"), arg("angle"), arg("antialias")="True"), SHEARY2D_DOC)); \
  def("shift", (void (*)(const blitz::Array<T,2>&, blitz::Array<T,2>&, const int, const int, const bool, const bool))&Torch::ip::shift<T>, shift_overloads_ ## N ((arg("src"), arg("dst"), arg("shift_y"), arg("shift_x"), arg("allow_out")="False", arg("zero_out")="False"), SHIFT2D_DOC)); \
  def("shift", (void (*)(const blitz::Array<T,2>&, const blitz::Array<bool,2>&, blitz::Array<T,2>&, blitz::Array<bool,2>&, const int, const int, const bool, const bool))&Torch::ip::shift<T>, shift_mask_overloads_ ## N ((arg("src"), arg("src_mask"), arg("dst"), arg("dst_mask"), arg("shift_y"), arg("shift_x"), arg("allow_out")="False", arg("zero_out")="False"), SHIFT2D_MASK_DOC)); \
  def("shift", (void (*)(const blitz::Array<T,3>&, blitz::Array<T,3>&, const int, const int, const bool, const bool))&Torch::ip::shift<T>, shift_overloads_ ## N ((arg("src"), arg("dst"), arg("shift_y"), arg("shift_x"), arg("allow_out")="False", arg("zero_out")="False"), SHIFT3D_DOC)); \
  def("shift", (void (*)(const blitz::Array<T,3>&, const blitz::Array<bool,3>&, blitz::Array<T,3>&, blitz::Array<bool,3>&, const int, const int, const bool, const bool))&Torch::ip::shift<T>, shift_mask_overloads_ ## N ((arg("src"), arg("src_mask"), arg("dst"), arg("dst_mask"), arg("shift_y"), arg("shift_x"), arg("allow_out")="False", arg("zero_out")="False"), SHIFT3D_MASK_DOC)); \
  def("gammaCorrection", (void (*)(const blitz::Array<T,2>&, blitz::Array<double,2>&, const double))&Torch::ip::gammaCorrection<T>, (arg("src"), arg("dst"), arg("gamma")), GAMMACORRECTION2D_DOC); \
  def("zigzag", (void (*)(const blitz::Array<T,2>&, blitz::Array<T,1>&, int, const bool))&Torch::ip::zigzag<T>, zigzag_overloads_ ## N ((arg("src"), arg("dst"), arg("n_coef")="0", arg("right_first")="False"), ZIGZAG2D_DOC)); 


#define SCALE_AS(T) \
	def("scaleAs", (blitz::Array<T,2> (*)(const blitz::Array<T,2>&, const double))&Torch::ip::scaleAs<T>, (arg("original"), arg("scale_factor")), SCALE_DOC); \
	def("scaleAs", (blitz::Array<T,3> (*)(const blitz::Array<T,3>&, const double))&Torch::ip::scaleAs<T>, (arg("original"), arg("scale_factor")), SCALE_DOC); 

/*
FILTER_DECL(bool,bool)
FILTER_DECL(int8_t,int8)
FILTER_DECL(int16_t,int16)
FILTER_DECL(int32_t,int32)
FILTER_DECL(int64_t,int64)
*/
FILTER_DECL(uint8_t,uint8)
FILTER_DECL(uint16_t,uint16)
/*
FILTER_DECL(uint32_t,uint32)
FILTER_DECL(uint64_t,uint64)
FILTER_DECL(float,float32)
*/
FILTER_DECL(double,float64)
/*
FILTER_DECL(std::complex<float>,complex64)
FILTER_DECL(std::complex<double>,complex128)
*/


void bind_ip_filters_new()
{
  enum_<Torch::ip::Rescale::Algorithm>("RescaleAlgorithm")
    .value("NearesetNeighbour", Torch::ip::Rescale::NearestNeighbour)
    .value("BilinearInterp", Torch::ip::Rescale::BilinearInterp)
    ;
 
/*
  FILTER_DEF(bool,bool)
  FILTER_DEF(int8_t,int8)
  FILTER_DEF(int16_t,int16)
  FILTER_DEF(int32_t,int32)
  FILTER_DEF(int64_t,int64)
*/
  FILTER_DEF(uint8_t,uint8)
  FILTER_DEF(uint16_t,uint16)
/*
  FILTER_DEF(uint32_t,uint32)
  FILTER_DEF(uint64_t,uint64)
  FILTER_DEF(float,float32)
*/
  FILTER_DEF(double,float64)
/*
  FILTER_DEF(std::complex<float>,complex64)
  FILTER_DEF(std::complex<double>,complex128)
*/

  SCALE_AS(uint8_t)
  SCALE_AS(uint16_t)
  SCALE_AS(double)
}
