/*
 * Modified version, originally from Samuli Laine's and Tero Karras' CudaRaster.
 * (http://code.google.com/p/cudaraster/)
 * 
 * 04-2012 - Thibault Coppex
 * 
 * ---------------------------------------------------------------------------
 * 
 *  Copyright 2009-2010 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
 
#include "Buffer.hpp"

#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
#include <cudaGL.h>

#include "GLContext.hpp" //

using namespace FW;


//------------------------------------------------------------------------

#define FW_IO_BUFFER_SIZE 65536

//------------------------------------------------------------------------

void Buffer::wrapCPU(void* cpuPtr, S64 size)
{
  FW_ASSERT(cpuPtr || !size);
  FW_ASSERT(size >= 0);

  m_cpuPtr = (U8*)cpuPtr;
  wrap(CPU, size);
}

//------------------------------------------------------------------------

void Buffer::wrapGL(GLuint glBuffer)
{
  FW_ASSERT(glBuffer != 0);

  GLint size;
  {
    GLint oldBuffer;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, glBuffer);

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
    GLContext::checkErrors();
  }

  m_glBuffer = glBuffer;
  wrap(GL, size);
}

//------------------------------------------------------------------------

void Buffer::wrapCuda(CUdeviceptr cudaPtr, S64 size)
{
  FW_ASSERT(cudaPtr || !size);

  m_cudaPtr = cudaPtr;
  wrap(Cuda, size);
}

//------------------------------------------------------------------------

void Buffer::free(Module module)
{
  if ((m_exists & module) == 0 || m_exists == (U32)module || m_original == module)
    return;

  setOwner(module, false);

  if (m_owner == module)
    for (int i = 1; i < (int)Module_All; i <<= 1)
      if (module != i && (m_exists & i) != 0 && (m_dirty & i) == 0)
      {
        setOwner((Module)i, false);
        break;
      }

  if (m_owner == module)
    for (int i = 1; i < (int)Module_All; i <<= 1)
      if (module != i && (m_exists & i) != 0)
      {
        setOwner((Module)i, false);
        break;
      }

  switch (module)
  {
    case CPU:   
      cpuFree(m_cpuPtr, m_cpuBase, m_hints); 
    break;
    
    case GL:    
      glFree(m_glBuffer, m_cudaGLReg); 
    break;
    
    case Cuda:  
      cudaFree(m_cudaPtr, m_cudaBase, m_glBuffer, m_hints); 
    break;
  }
  m_exists &= ~module;
}

//------------------------------------------------------------------------

void Buffer::setRange(S64 dstOfs, const void* src, S64 size, bool async, CUstream cudaStream)
{
  FW_ASSERT(dstOfs >= 0 && dstOfs <= m_size - size);
  FW_ASSERT(src || !size);
  FW_ASSERT(size >= 0);

  if (!size)
    return;

  switch (m_owner)
  {
    case GL:
    {
      GLint oldBuffer;
      glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, getMutableGLBuffer());
      glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)dstOfs, (GLsizeiptr)size, src);
      glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
      GLContext::checkErrors();
    }
    break;

    case Cuda:
      memcpyHtoD(getMutableCudaPtr(dstOfs), src, (U32)size, async, cudaStream);
    break;

    default:
      memcpy(getMutablePtr(dstOfs), src, (size_t)size);
    break;
  }
}

//------------------------------------------------------------------------

void Buffer::setRange(S64 dstOfs, Buffer& src, S64 srcOfs, S64 size, bool async, CUstream cudaStream)
{
  FW_ASSERT(size >= 0);
  FW_ASSERT(dstOfs >= 0 && dstOfs <= m_size - size);
  FW_ASSERT(srcOfs >= 0 && srcOfs <= src.m_size - size);

  if (!size)
    return;

  if ((src.m_exists & Cuda) != 0 && 
      (src.m_dirty & Cuda) == 0 && 
      (m_owner == Cuda || m_owner == Module_None))
  {
    memcpyDtoD(getMutableCudaPtr(dstOfs), src.getCudaPtr(srcOfs), (U32)size);
  } else if ((src.m_exists & CPU) != 0 && (src.m_dirty & CPU) == 0) {
    setRange(dstOfs, src.getPtr(srcOfs), size, async, cudaStream);
  } else {
    src.getRange(getMutablePtr(dstOfs), srcOfs, size, async, cudaStream);
  }
}

//------------------------------------------------------------------------

void Buffer::clearRange(S64 dstOfs, int value, S64 size, bool async, CUstream cudaStream)
{
  FW_ASSERT(size >= 0);
  FW_ASSERT(dstOfs >= 0 && dstOfs <= m_size - size);
  FW_UNREF(async);      // unsupported
  FW_UNREF(cudaStream); // unsupported

  if (!size) {
    return;
  }

  if (m_owner == Cuda) {
    CUresult res = cuMemsetD8(getMutableCudaPtr(dstOfs), (U8)value, (U32)size);
    CudaModule::checkError("cuMemsetD8", res);
  } else {
    memset(getMutablePtr(dstOfs), value, (size_t)size);
  }
}

//------------------------------------------------------------------------

void Buffer::getRange(void* dst, S64 srcOfs, S64 size, bool async, CUstream cudaStream) const
{
  FW_ASSERT(dst || !size);
  FW_ASSERT(srcOfs >= 0 && srcOfs <= m_size - size);
  FW_ASSERT(size >= 0);

  if (!size) {
    return;
  }

  switch (m_owner)
  {
    case GL:
    {
      GLint oldBuffer;
      glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, m_glBuffer);
      glGetBufferSubData(GL_ARRAY_BUFFER, (GLintptr)srcOfs, (GLsizeiptr)size, dst);
      glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
      GLContext::checkErrors();
    }
    break;

    case Cuda:
      memcpyDtoH(dst, m_cudaPtr + (U32)srcOfs, (U32)size, async, cudaStream);
    break;

    default:
      memcpy(dst, m_cpuPtr + srcOfs, (size_t)size);
    break;
  }
}

//------------------------------------------------------------------------

void Buffer::setOwner(Module module, bool modify, bool async, CUstream cudaStream, S64 validSize)
{
    FW_ASSERT((module & ~Module_All) == 0);
    FW_ASSERT((module & (module - 1)) == 0);
    if (validSize == -1) {
      validSize = m_size;
    }
    
    FW_ASSERT(validSize >= 0);

    // Unmap CudaGL if necessary.
    if ((m_hints & Hint_CudaGL) != 0 && (m_exists & Cuda) != 0)
    {
      FW_ASSERT((m_dirty & Cuda) == 0);
      if ((module != Cuda && modify) || (module == GL && (m_dirty & GL) != 0))
      {
        cudaFree(m_cudaPtr, m_cudaBase, m_glBuffer, m_hints);
        m_exists &= ~Cuda;
        m_dirty &= ~GL;
      }
    }

    // Same owner => done.
    if (m_owner == module)
    {
      if (modify) {
        m_dirty = Module_All - module;
      }
      return;
    }

    // Not page-locked => not asynchronous.
    if ((m_hints & Hint_PageLock) == 0) {
      async = false;
    }

    // Validate CPU.
    if (module == CPU)
    {
      if ((m_exists & CPU) == 0)
      {
        cpuAlloc(m_cpuPtr, m_cpuBase, m_size, m_hints, m_align);
        m_exists |= CPU;
        m_dirty |= CPU;
      }
      validateCPU(async, cudaStream, validSize);
    }

    // Validate GL.
    bool needGL = (module == GL);
    if (module == Cuda && (m_hints & Hint_CudaGL) != 0) {
      needGL = true;
    }

    if (needGL && (m_exists & GL) == 0)
    {
      validateCPU(false, NULL, validSize);
      glAlloc(m_glBuffer, m_size, m_cpuPtr);
      m_exists |= GL;
      m_dirty &= ~GL;
    }
    else if (module == GL && (m_dirty & GL) != 0)
    {
      validateCPU(false, NULL, validSize);
      FW_ASSERT((m_exists & CPU) != 0);
      if (validSize)
      {
        //profilePush("glBufferSubData");
        GLint oldBuffer;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_glBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)validSize, m_cpuPtr);
        glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
        GLContext::checkErrors();
        //profilePop();
      }
      m_dirty &= ~GL;
    }

    // Validate Cuda.

    if (module == Cuda)
    {
      if ((m_exists & Cuda) == 0)
      {
        cudaAlloc(m_cudaPtr, m_cudaBase, m_cudaGLReg, m_size, m_glBuffer, m_hints, m_align);
        m_exists |= Cuda;
        m_dirty |= Cuda;
        if ((m_hints & Hint_CudaGL) != 0 && (m_dirty & GL) == 0) {
          m_dirty &= ~Cuda;
        }
      }

      if ((m_dirty & Cuda) != 0)
      {
        validateCPU(false, NULL, validSize);
        if ((m_exists & CPU) != 0 && validSize) {
          memcpyHtoD(m_cudaPtr, m_cpuPtr, (U32)validSize, async, cudaStream);
        }
        m_dirty &= ~Cuda;
      }
    }

    // Set the new owner.

    m_owner = module;
    if (modify) {
      m_dirty = Module_All - module;
    }
}

//------------------------------------------------------------------------

void Buffer::readFromStream(InputStream& s)
{
    S64 size;
    s >> size;
    resizeDiscard(size);

    std::vector<U8> tmp(FW_IO_BUFFER_SIZE, 0);
    S64 ofs = 0;
    while (ofs < size)
    {
      int num = (int)min(size - ofs, (S64)tmp.size());
      s.readFully(&tmp[0], num);
      setRange(ofs, &tmp[0], num);
      ofs += num;
    }
}

//------------------------------------------------------------------------

void Buffer::writeToStream(OutputStream& s) const
{
    s << m_size;

    std::vector<U8> tmp(FW_IO_BUFFER_SIZE, 0);
    S64 ofs = 0;
    while (ofs < m_size)
    {
        int num = (int)min(m_size - ofs, (S64)tmp.size());
        getRange(&tmp[0], ofs, num);
        s.write(&tmp[0], num);
        ofs += num;
    }
}

//------------------------------------------------------------------------

void Buffer::init(S64 size, U32 hints, int align)
{
    FW_ASSERT(size >= 0);

    m_hints     = validateHints(hints, align, Module_None);
    m_align     = align;
    m_size      = size;
    m_original  = Module_None;
    m_owner     = Module_None;
    m_exists    = Module_None;
    m_dirty     = Module_None;

    m_cpuPtr    = NULL;
    m_cpuBase   = NULL;
    m_glBuffer  = 0;
    m_cudaPtr   = NULL;
    m_cudaBase  = NULL;
    m_cudaGLReg = false;
}

//------------------------------------------------------------------------

U32 Buffer::validateHints(U32 hints, int align, Module original)
{
  FW_ASSERT((hints & ~Hint_All) == 0);
  FW_ASSERT(align > 0);

  U32 res = Hint_None;
  if ((hints & Hint_PageLock) != 0 && original != CPU) {
    res |= Hint_PageLock;
  }

  if ((hints & Hint_CudaGL) != 0 && original != Cuda && align == 1) { // && isAvailable_cuGLRegisterBufferObject())
    res |= Hint_CudaGL;
  }
  return res;
}

//------------------------------------------------------------------------

void Buffer::deinit(void)
{
  if (m_original != Cuda) {
    cudaFree(m_cudaPtr, m_cudaBase, m_glBuffer, m_hints);
  }
  
  if (m_original != GL) {
    glFree(m_glBuffer, m_cudaGLReg);
  } else if (m_cudaGLReg) {
    CudaModule::checkError("cuGLUnregisterBufferObject", 
                           cuGLUnregisterBufferObject(m_glBuffer));
    assert( "cuGLUnregisterBufferObject" && 0 );
  }


  if (m_original != CPU) {
    cpuFree(m_cpuPtr, m_cpuBase, m_hints);
  }
}

//------------------------------------------------------------------------

void Buffer::wrap(Module module, S64 size)
{
    FW_ASSERT(size >= 0);
    FW_ASSERT(m_exists == Module_None);

    m_hints     = validateHints(m_hints, m_align, module);
    m_size      = size;
    m_original  = module;
    m_owner     = module;
    m_exists    = module;
}

//------------------------------------------------------------------------

void Buffer::realloc(S64 size, U32 hints, int align)
{
  FW_ASSERT(size >= 0);
  FW_ASSERT(align > 0);

  // No change => done.
  if (m_size == size && m_hints == hints && m_align == align) {
    return;
  }

  // Wrapped buffer => free others.
  if (m_original)
  {
    switch (m_original)
    {
      case CPU:   FW_ASSERT((S64)m_cpuPtr % align == 0); break;
      case Cuda:  FW_ASSERT((S64)m_cudaPtr % align == 0); break;
      default:    break;
    }

    for (int i = 1; i < (int)Module_All; i <<= 1) {
      free((Module)i);
    }

    FW_ASSERT(m_size == size);
    m_hints = hints;
    m_align = align;
    return;
  }

  // No need to retain old data => reset.
  if (!size || !m_size || m_exists == Module_None)
  {
    reset(NULL, size, hints, align);
    return;
  }

  // CUDA buffer => device-to-device copy.
  if (m_owner == Cuda && (hints & Hint_CudaGL) == 0)
  {
    CUdeviceptr cudaPtr;
    CUdeviceptr cudaBase;
    bool cudaGLReg = false;
    cudaAlloc(cudaPtr, cudaBase, cudaGLReg, size, 0, hints, align);
    memcpyXtoX(NULL, cudaPtr, NULL, getCudaPtr(), min(size, m_size), false, NULL);

    reset(NULL, size, hints, align);
    m_exists = Cuda;
    m_cudaPtr = cudaPtr;
    m_cudaBase = cudaBase;
    return;
  }

  // Host-to-host copy.
  U8* cpuPtr;
  U8* cpuBase;
  cpuAlloc(cpuPtr, cpuBase, size, hints, align);
  memcpy(cpuPtr, getPtr(), (size_t)min(size, m_size));

  reset(NULL, size, hints, align);
  m_exists = CPU;
  m_cpuPtr = cpuPtr;
  m_cpuBase = cpuBase;
}

//------------------------------------------------------------------------

void Buffer::validateCPU(bool async, CUstream cudaStream, S64 validSize)
{
  FW_ASSERT(validSize >= 0);

  // Already valid => done.
  if ((m_exists & CPU) != 0 && (m_dirty & CPU) == 0) {
    return;
  }
  m_dirty &= ~CPU;

  // Find source for the data.
  Module source = Module_None;
  for (int i = 1; i < (int)Module_All; i <<= 1)
  {
    if (i != CPU && (m_exists & i) != 0 && (m_dirty & i) == 0)
    {
      source = (Module)i;
      break;
    }
  }

  // No source => done.
  if (source == Module_None) {
    return;
  }

  // No buffer => allocate one.
  if ((m_exists & CPU) == 0)
  {
    cpuAlloc(m_cpuPtr, m_cpuBase, m_size, m_hints, m_align);
    m_exists |= CPU;
  }

  // No valid data => no need to copy.
  if (!validSize) {
    return;
  }

  // Copy data from the source.
  if (source == GL)
  {
    //profilePush("glGetBufferSubData");
    GLint oldBuffer;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_glBuffer);
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)validSize, m_cpuPtr);
    glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
    GLContext::checkErrors();
    //profilePop();
  }
  else
  {
    FW_ASSERT(source == Cuda);
    memcpyDtoH(m_cpuPtr, m_cudaPtr, (U32)validSize, async, cudaStream);
  }
}

//------------------------------------------------------------------------

void Buffer::cpuAlloc(U8*& cpuPtr, U8*& cpuBase, S64 size, U32 hints, int align)
{
  FW_ASSERT(align > 0);
  if ((hints & Hint_PageLock) != 0)
  {
    checkSize(size, 32, "cuMemAllocHost");
    CUresult res = cuMemAllocHost((void**)&cpuBase, max(1U, (U32)(size + align - 1)));
    CudaModule::checkError("cuMemAllocHost", res);
  }
  else
  {
    checkSize(size, sizeof(U8*) * 8 - 1, "malloc");
    cpuBase = new U8[(size_t)(size + align - 1)];
  }

  cpuPtr = cpuBase + align - 1;
  cpuPtr -= (UPTR)cpuPtr % (UPTR)align;
}

//------------------------------------------------------------------------

void Buffer::cpuFree(U8*& cpuPtr, U8*& cpuBase, U32 hints)
{
  FW_ASSERT((cpuPtr == NULL) == (cpuBase == NULL));
  if (cpuPtr)
  {
    if ((hints & Hint_PageLock) != 0) {
      CudaModule::checkError("cuMemFreeHost", cuMemFreeHost(cpuBase));
    } else {
      delete[] cpuBase;
    }
    cpuPtr = NULL;
    cpuBase = NULL;
  }
}

//------------------------------------------------------------------------

void Buffer::glAlloc(GLuint& glBuffer, S64 size, const void* data)
{
  FW_ASSERT(size >= 0);
  GLContext::staticInit();

  GLint oldBuffer;
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
  glGenBuffers(1, &glBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, glBuffer);
  checkSize(size, sizeof(GLsizeiptr) * 8 - 1, "glBufferData");
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)size, data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
  GLContext::checkErrors();
}

//------------------------------------------------------------------------

void Buffer::glFree(GLuint& glBuffer, bool& cudaGLReg)
{
  if (glBuffer)
  {
    if (cudaGLReg)
    {
      assert( "cuGLUnregisterBufferObject" && 0 );
      CudaModule::checkError("cuGLUnregisterBufferObject", cuGLUnregisterBufferObject(glBuffer));
      cudaGLReg = false;
    }
    glDeleteBuffers(1, &glBuffer);
    GLContext::checkErrors();
    glBuffer = 0;
  }
}

//------------------------------------------------------------------------

void Buffer::cudaAlloc(CUdeviceptr& cudaPtr, CUdeviceptr& cudaBase, bool& cudaGLReg, S64 size, GLuint glBuffer, U32 hints, int align)
{
    CudaModule::staticInit();
    if ((hints & Hint_CudaGL) == 0)
    {
      FW_ASSERT(align > 0);
      checkSize(size, 32, "cuMemAlloc");
      CUresult res = cuMemAlloc(&cudaBase, max(1U, (U32)(size + align - 1)));
      CudaModule::checkError("cuMemAlloc", res);
      cudaPtr = cudaBase + align - 1;
      cudaPtr -= (U32)cudaPtr % (U32)align;
    }
    else
    {
      if (!cudaGLReg)
      {
        assert( "cuGLRegisterBufferObject" && 0 );
        CudaModule::checkError("cuGLRegisterBufferObject", cuGLRegisterBufferObject(glBuffer));
        cudaGLReg = true;
      }
      size_t size;
      FW_ASSERT(align == 1);
      assert( "cuGLMapBufferObject" && 0 );
      CudaModule::checkError("cuGLMapBufferObject", cuGLMapBufferObject(&cudaBase, &size, glBuffer));
      cudaPtr = cudaBase;
    }
}

//------------------------------------------------------------------------

void Buffer::cudaFree(CUdeviceptr& cudaPtr, CUdeviceptr& cudaBase, 
                      GLuint glBuffer, U32 hints)
{
  FW_ASSERT((cudaPtr == NULL) == (cudaBase == NULL));
  if (cudaPtr)
  {
    if ((hints & Hint_CudaGL) == 0) 
    {
      CudaModule::checkError("cuMemFree", cuMemFree(cudaBase));
    } 
    else 
    {
      assert( "cuGLUnmapBufferObject" && 0 );
      CudaModule::checkError("cuGLUnmapBufferObject", cuGLUnmapBufferObject(glBuffer));
    }
    cudaPtr = NULL;
    cudaBase = NULL;
  }
}

//------------------------------------------------------------------------

void Buffer::checkSize(S64 size, int bits, const std::string& funcName)
{
  FW_ASSERT(size >= 0);
  if ((U64)size > (((U64)1 << bits) - 1)) {
    fail("Buffer too large for %s()!", funcName.c_str());
  }
}

//------------------------------------------------------------------------

void Buffer::memcpyXtoX(void* dstHost, CUdeviceptr dstDevice, 
                        const void* srcHost, CUdeviceptr srcDevice, S64 size, 
                        bool async, CUstream cudaStream)
{
  CUresult res;
  
  if (size <= 0) {
    return;
  }

  // Try to copy.
  if (dstHost && srcHost)
  {
    memcpy(dstHost, srcHost, (size_t)size);
    res = CUDA_SUCCESS;
  }
  else if (srcHost)
  {
    //profilePush("cuMemcpyHtoD");
    if (async) { // && isAvailable_cuMemcpyHtoDAsync()) {
      res = cuMemcpyHtoDAsync(dstDevice, srcHost, (U32)size, cudaStream);
    } else {
      res = cuMemcpyHtoD(dstDevice, srcHost, (U32)size);
    }
    //profilePop();
  }
  else if (dstHost)
  {
    //profilePush("cuMemcpyDtoH");
    if (async) { // && isAvailable_cuMemcpyDtoHAsync()) {
      res = cuMemcpyDtoHAsync(dstHost, srcDevice, (U32)size, cudaStream);
    } else {
      res = cuMemcpyDtoH(dstHost, srcDevice, (U32)size);
    }
    //profilePop();
  }
  else
  {
    //profilePush("cuMemcpyDtoD");
#if (CUDA_VERSION >= 3000)
    if (async)// && isAvailable_cuMemcpyDtoDAsync())
      res = cuMemcpyDtoDAsync(dstDevice, srcDevice, (U32)size, cudaStream);
    else
#endif
      res = cuMemcpyDtoD(dstDevice, srcDevice, (U32)size);
    //profilePop();
  }

  // Success => done.
  if (res == CUDA_SUCCESS) {
    return;
  }
  
  // Single byte => fail.
  if (size == 1) {
    std::cerr << __FILE__ << " : l" << __LINE__ << std::endl;
    CudaModule::checkError("cuMemcpyXtoX", res);
  }

  // Otherwise => subdivide.
  // CUDA driver does not allow memcpy() to cross allocation boundaries.
  S64 mid = size >> 1;
  memcpyXtoX(dstHost, dstDevice, srcHost, srcDevice, mid, async, cudaStream);

  memcpyXtoX( (dstHost) ? (U8*)dstHost + mid : NULL,
              (dstHost) ? NULL : (CUdeviceptr)(dstDevice + mid),
              (srcHost) ? (const U8*)srcHost + mid : NULL,
              (srcHost) ? NULL : (CUdeviceptr)(srcDevice + mid),
              size - mid, async, cudaStream);
}

//------------------------------------------------------------------------
