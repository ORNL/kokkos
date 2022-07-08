/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 3.0
//       Copyright (2020) National Technology & Engineering
//               Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY NTESS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NTESS OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

// FIXME_OPENACC - macro for workaround implementation in UniqueToken
// constructor. undef'ed at the end
#define KOKKOS_IMPL_OPENACC_WORKAROUND

#include <Kokkos_OpenACC.hpp>
#include <OpenACC/Kokkos_OpenACC_UniqueToken.hpp>
#include <OpenACC/Kokkos_OpenACC_Instance.hpp>

#include <sstream>

namespace Kokkos {
namespace Experimental {
namespace Impl {
uint32_t OpenACCInternal::impl_get_instance_id() const noexcept {
  return m_instance_id;
}

void OpenACCInternal::fence(openacc_fence_is_static is_static) {
  fence(
      "Kokkos::Experimental::Impl::OpenACCInternal::fence: Unnammed Internal "
      "Fence",
      is_static);
}
void OpenACCInternal::fence(const std::string& name,
                            openacc_fence_is_static is_static) {
  if (is_static == openacc_fence_is_static::no) {
    Kokkos::Tools::Experimental::Impl::profile_fence_event<
        Kokkos::Experimental::OpenACC>(
        name,
        Kokkos::Tools::Experimental::Impl::DirectFenceIDHandle{
            impl_get_instance_id()},
        [&]() {
          //[DEBUG] disabled due to synchronous behaviors of the current
          // parallel construct implementations. acc_wait_all();
        });
  } else {
    Kokkos::Tools::Experimental::Impl::profile_fence_event<
        Kokkos::Experimental::OpenACC>(
        name,
        Kokkos::Tools::Experimental::SpecialSynchronizationCases::
            GlobalDeviceSynchronization,
        [&]() {  //[TODO]: correct device ID
          acc_wait_all();
        });
  }
}
int OpenACCInternal::concurrency() { return 128000; }
const char* OpenACCInternal::name() { return "OpenACC"; }
void OpenACCInternal::print_configuration(std::ostream& os, const bool) {
  // FIXME_OPENACC
  os << "Using OpenACC\n";
}

void OpenACCInternal::impl_finalize() {
  m_is_initialized = false;
  Kokkos::Impl::OpenACCExec space;
  if (space.m_lock_array != nullptr) space.clear_lock_array();

  if (space.m_uniquetoken_ptr != nullptr)
    Kokkos::kokkos_free<Kokkos::Experimental::OpenACCSpace>(
        space.m_uniquetoken_ptr);
}
void OpenACCInternal::impl_initialize() { m_is_initialized = true; }
int OpenACCInternal::impl_is_initialized() { return m_is_initialized ? 1 : 0; }

OpenACCInternal* OpenACCInternal::impl_singleton() {
  static OpenACCInternal self;
  return &self;
}

}  // Namespace Impl

OpenACC::OpenACC()
    : m_space_instance(Impl::OpenACCInternal::impl_singleton()) {}

const char* OpenACC::name() {
  return Impl::OpenACCInternal::impl_singleton()->name();
}
void OpenACC::print_configuration(std::ostream& stream, const bool detail) {
  // m_space_instance->print_configuration(stream, detail);
  Impl::OpenACCInternal::impl_singleton()->print_configuration(stream, detail);
}

uint32_t OpenACC::impl_instance_id() const noexcept {
  return m_space_instance->impl_get_instance_id();
}

int OpenACC::concurrency() {
  return Impl::OpenACCInternal::impl_singleton()->concurrency();
}
void OpenACC::fence() {
  Impl::OpenACCInternal::impl_singleton()->fence(
      "Kokkos::OpenACC::fence: Unnamed Instance Fence");
}
void OpenACC::fence(const std::string& name) {
  Impl::OpenACCInternal::impl_singleton()->fence(name);
}
void OpenACC::impl_static_fence() {
  Impl::OpenACCInternal::impl_singleton()->fence(
      "Kokkos::OpenACC::fence: Unnamed Instance Fence",
      Kokkos::Experimental::Impl::openacc_fence_is_static::yes);
}
void OpenACC::impl_static_fence(const std::string& name) {
  Impl::OpenACCInternal::impl_singleton()->fence(
      name, Kokkos::Experimental::Impl::openacc_fence_is_static::yes);
}

void OpenACC::impl_initialize() { m_space_instance->impl_initialize(); }
void OpenACC::impl_finalize() { m_space_instance->impl_finalize(); }
int OpenACC::impl_is_initialized() {
  return Impl::OpenACCInternal::impl_singleton()->impl_is_initialized();
}
}  // Namespace Experimental

namespace Impl {
int g_openacc_space_factory_initialized =
    Kokkos::Impl::initialize_space_factory<OpenACCSpaceInitializer>(
        "170_OpenACC");

void OpenACCSpaceInitializer::initialize(const InitializationSettings& settings) {
  // Prevent "unused variable" warning for 'setting' input struct.  If
  // Serial::initialize() ever needs to take arguments from the input
  // struct, you may remove this line of code.
  (void)settings;

  if (std::is_same<Kokkos::Experimental::OpenACC,
                   Kokkos::DefaultExecutionSpace>::value) {
    Kokkos::Experimental::OpenACC().impl_initialize();
  }
}

void OpenACCSpaceInitializer::finalize(const bool all_spaces) {
  if (std::is_same<Kokkos::Experimental::OpenACC,
                   Kokkos::DefaultExecutionSpace>::value ||
      all_spaces) {
    if (Kokkos::Experimental::OpenACC().impl_is_initialized())
      Kokkos::Experimental::OpenACC().impl_finalize();
  }
}

void OpenACCSpaceInitializer::fence(const std::string& name) {
  Kokkos::Experimental::OpenACC::impl_static_fence(name);
}

void OpenACCSpaceInitializer::print_configuration(std::ostream& msg,
                                                  const bool detail) {
  msg << "OpenACC Execution Space:" << std::endl;
  msg << "  KOKKOS_ENABLE_OPENACC: ";
  msg << "yes" << std::endl;

  msg << "\nOpenACC Runtime Configuration:" << std::endl;
  Kokkos::Experimental::OpenACC().print_configuration(msg, detail);
}

}  // namespace Impl
}  // Namespace Kokkos

namespace Kokkos {
namespace Experimental {

UniqueToken<Kokkos::Experimental::OpenACC,
            Kokkos::Experimental::UniqueTokenScope::Global>::
    UniqueToken(Kokkos::Experimental::OpenACC const&) {
#ifdef KOKKOS_IMPL_OPENACC_WORKAROUND
  uint32_t* ptr = Kokkos::Impl::OpenACCExec::m_uniquetoken_ptr;
  int count     = Kokkos::Experimental::OpenACC().concurrency();
  if (ptr == nullptr) {
    int size = count * sizeof(uint32_t);
    ptr      = static_cast<uint32_t*>(
        Kokkos::kokkos_malloc<Kokkos::Experimental::OpenACCSpace>(
            "Kokkos::OpenACC::m_uniquetoken_ptr", size));
    std::vector<uint32_t> h_buf(count, 0);
    acc_memcpy_to_device(ptr, h_buf.data(), size);

    Kokkos::Impl::OpenACCExec::m_uniquetoken_ptr = ptr;
  }
#else
// FIXME_OPENACC - 2 versions of non-working implementations to fill `ptr`
// with 0's
// Version 1 - Creating a target region and filling the
// pointer Error - CUDA error: named symbol not found
#pragma acc parallel loop independent deviceptr(ptr)
  for (int i = 0; i < count; ++i) ptr[i] = 0;

  // Version 2 : Allocating a view on the device and filling it with a scalar
  // value of 0.
  Kokkos::View<uint32_t*, Kokkos::Experimental::OpenACCSpace> ptr_view(ptr,
                                                                       count);
  Kokkos::deep_copy(ptr_view, 0);
#endif
  m_buffer = ptr;
  m_count  = count;
}
}  // namespace Experimental
}  // namespace Kokkos

#undef KOKKOS_IMPL_OPENACC_WORKAROUND
