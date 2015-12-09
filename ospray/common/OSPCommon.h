// ======================================================================== //
// Copyright 2009-2015 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

/*! \file OSPCommon.h Defines common types and classes that _every_
  ospray file should know about */

// include cmake config first - we need to know the config even before
// the first "#include <mpi>"
#include "OSPConfig.h"

// mpi, if we need it
#ifdef OSPRAY_MPI
# include <mpi.h>
#endif

#ifdef _WIN32
  typedef unsigned long long id_t;
#endif

#if defined(__WIN32__) || defined(_WIN32)
// ----------- windows only -----------
# define _USE_MATH_DEFINES 1
# include <cmath>
# include <math.h>
# ifdef _M_X64
typedef long long ssize_t;
# else
typedef int ssize_t;
# endif
#else
// ----------- NOT windows -----------
# include "unistd.h"
#endif

// embree
#include "common/math/vec2.h"
#include "common/math/vec3.h"
#include "common/math/vec4.h"
#include "common/math/bbox.h"
#include "common/math/affinespace.h" // includes "common/math/linearspace[23].h"
#include "common/sys/ref.h"
//#include "common/sys/taskscheduler.h"
#ifdef __NEW_EMBREE__
# include "common/sys/atomic.h"
# include "common/sys/condition.h"
# include <unistd.h>
#else
# include "common/sys/sync/atomic.h"
# include "common/sys/sync/condition.h"
#endif

// ospray
#include "ospray/common/OSPDataType.h"

// std
#include <stdint.h> // for int64_t etc
#include <sstream>

#ifdef _WIN32
#  ifdef ospray_EXPORTS
#    define OSPRAY_INTERFACE __declspec(dllexport)
#  else
#    define OSPRAY_INTERFACE __declspec(dllimport)
#  endif
#else
#  define OSPRAY_INTERFACE
#endif


// for MIC, disable the 'variable declared bbut never referenced'
// warning, else the ISPC-generated code produces _far_ too many such
// outputs
#if defined(__INTEL_COMPILER) && defined(__MIC__)
#pragma warning(disable:177 ) // variable declared but was never referenced
#endif

#ifdef OSPRAY_TARGET_MIC
inline void* operator new(size_t size) throw(std::bad_alloc) { return embree::alignedMalloc(size); }       
inline void operator delete(void* ptr) throw() { embree::alignedFree(ptr); }      
inline void* operator new[](size_t size) throw(std::bad_alloc) { return embree::alignedMalloc(size); }  
inline void operator delete[](void* ptr) throw() { embree::alignedFree(ptr); }    
#endif

//! main namespace for all things ospray (for internal code)
namespace ospray {

  using embree::one;
  using embree::empty;
  using embree::zero;
  using embree::inf;
  using embree::deg2rad;
  using embree::rad2deg;
  using embree::sign;
  using embree::clamp;

  /*! basic types */
  typedef ::int64_t int64;
  typedef ::uint64_t uint64;

  typedef ::int32_t int32;
  typedef ::uint32_t uint32;

  typedef ::int16_t int16;
  typedef ::uint16_t uint16;

  typedef ::int8_t int8;
  typedef ::uint8_t uint8;

  typedef ::int64_t index_t;

  /*! OSPRay's two-int vector class */
  typedef embree::Vec2i    vec2i;
  /*! OSPRay's three-unsigned char vector class */
  typedef embree::Vec3<uint8> vec3uc;
  /*! OSPRay's 4x unsigned char vector class */
  typedef embree::Vec4<uint8> vec4uc;
  /*! OSPRay's 2x uint32 vector class */
  typedef embree::Vec2<uint32> vec2ui;
  /*! OSPRay's 3x uint32 vector class */
  typedef embree::Vec3<uint32> vec3ui;
  /*! OSPRay's 4x uint32 vector class */
  typedef embree::Vec4<uint32> vec4ui;
  /*! OSPRay's 3x int32 vector class */
  typedef embree::Vec3<int32>  vec3i;
  /*! OSPRay's four-int vector class */
  typedef embree::Vec4i    vec4i;
  /*! OSPRay's two-float vector class */
  typedef embree::Vec2f    vec2f;
  /*! OSPRay's three-float vector class */
  typedef embree::Vec3f    vec3f;
  /*! OSPRay's three-float vector class (aligned to 16b-boundaries) */
  typedef embree::Vec3fa   vec3fa;
  /*! OSPRay's four-float vector class */
  typedef embree::Vec4f    vec4f;

  typedef embree::BBox<vec2ui>   box2ui;
  typedef embree::BBox<vec2i>    region2i;
  typedef embree::BBox<vec2ui>   region2ui;

  typedef embree::BBox<vec3i>    box3i;
  typedef embree::BBox<vec3ui>   box3ui;
  
  typedef embree::BBox3f         box3f;
  typedef embree::BBox3fa        box3fa;
  typedef embree::BBox<vec3uc>   box3uc;
  typedef embree::BBox<vec4f>    box4f;
  typedef embree::BBox3fa        box3fa;
  
  /*! affice space transformation */
  typedef embree::AffineSpace2f  affine2f;
  typedef embree::AffineSpace3f  affine3f;
  typedef embree::AffineSpace3fa affine3fa;
  typedef embree::AffineSpace3f  AffineSpace3f;
  typedef embree::AffineSpace3fa AffineSpace3fa;

  typedef embree::LinearSpace2f  linear2f;
  typedef embree::LinearSpace3f  linear3f;
  typedef embree::LinearSpace3fa linear3fa;
  typedef embree::LinearSpace3f  LinearSpace3f;
  typedef embree::LinearSpace3fa LinearSpace3fa;

  using   embree::Ref;
  using   embree::RefCount;

  /*! return system time in seconds */
  OSPRAY_INTERFACE double getSysTime();

  void init(int *ac, const char ***av);

  /*! remove specified num arguments from an ac/av arglist */
  OSPRAY_INTERFACE void removeArgs(int &ac, char **&av, int where, int howMany);
  /*! for debugging. compute a checksum for given area range... */
  OSPRAY_INTERFACE void *computeCheckSum(const void *ptr, size_t numBytes);

  OSPRAY_INTERFACE void doAssertion(const char *file, int line, const char *expr, const char *expl);
#ifdef NDEBUG
# define Assert(expr) /* nothing */
# define Assert2(expr,expl) /* nothing */
# define AssertError(errMsg) /* nothing */
#else
# define Assert(expr)                                                   \
  ((void)((expr) ? 0 : ((void)ospray::doAssertion(__FILE__, __LINE__, #expr, NULL), 0)))
# define Assert2(expr,expl)                                             \
  ((void)((expr) ? 0 : ((void)ospray::doAssertion(__FILE__, __LINE__, #expr, expl), 0)))
# define AssertError(errMsg)                            \
  doAssertion(__FILE__,__LINE__, (errMsg), NULL)
#endif

  inline size_t rdtsc() { return embree::rdtsc(); }

  /*! logging level (cmdline: --osp:loglevel \<n\>) */
  extern uint32 logLevel;
  /*! whether we're running in debug mode (cmdline: --osp:debug) */
  extern bool debugMode;
  /*! number of Embree threads to use, 0 for the default
      number. (cmdline: --osp:numthreads \<n\>) */
  extern int32 numThreads;

  /*! size of OSPDataType */
  size_t sizeOf(OSPDataType type);

  /*! Convert a type string to an OSPDataType. */
  OSPDataType typeForString(const char *string);

  struct WarnOnce {
    WarnOnce(const std::string &s);
  private:
    const std::string s;
  };

  /*! added pretty-print function for large numbers, printing 10000000 as "10M" instead */
  inline std::string prettyNumber(const size_t s) {
    double val = s;
    char result[100];
    if (val >= 1e12f) {
      sprintf(result,"%.1fT",val/1e12f);
    } else if (val >= 1e9f) {
      sprintf(result,"%.1fG",val/1e9f);
    } else if (val >= 1e6f) {
      sprintf(result,"%.1fM",val/1e6f);
    } else if (val >= 1e3f) {
      sprintf(result,"%.1fK",val/1e3f);
    } else {
      sprintf(result,"%lu",s);
    }
    return result;
  }

#if __EXTERNAL_EMBREE__
  struct Condition : public embree::ConditionSys 
  {
  inline void broadcast() { notify_all(); }
};
#else
  typedef embree::ConditionSys Condition;
#endif


} // ::ospray

#ifdef _WIN32
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
#define NOTIMPLEMENTED    throw std::runtime_error(std::string(__PRETTY_FUNCTION__)+": not implemented...");

// #define divRoundUp(X,Y) (((X)+(Y)-1)/(Y))

template <typename T>
inline T divRoundUp(const T&a, const T&b) { return (a+(b-T(1)))/b; }

  

