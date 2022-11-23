//@HEADER
// ************************************************************************
//
//                        Kokkos v. 4.0
//       Copyright (2022) National Technology & Engineering
//               Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Part of Kokkos, under the Apache License v2.0 with LLVM Exceptions.
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//@HEADER

#ifndef KOKKOS_STD_ALGORITHMS_FOR_EACH_HPP
#define KOKKOS_STD_ALGORITHMS_FOR_EACH_HPP

#include "impl/Kokkos_ForEachForEachN.hpp"
#include "Kokkos_BeginEnd.hpp"

namespace Kokkos {
namespace Experimental {

template <class ExecutionSpace, class IteratorType, class UnaryFunctorType>
UnaryFunctorType for_each(const std::string& label, const ExecutionSpace& ex,
                          IteratorType first, IteratorType last,
                          UnaryFunctorType functor) {
  return Impl::for_each_impl(label, ex, first, last, std::move(functor));
}

template <class ExecutionSpace, class IteratorType, class UnaryFunctorType>
UnaryFunctorType for_each(const ExecutionSpace& ex, IteratorType first,
                          IteratorType last, UnaryFunctorType functor) {
  return Impl::for_each_impl("Kokkos::for_each_iterator_api_default", ex, first,
                             last, std::move(functor));
}

template <class ExecutionSpace, class DataType, class... Properties,
          class UnaryFunctorType>
UnaryFunctorType for_each(const std::string& label, const ExecutionSpace& ex,
                          const ::Kokkos::View<DataType, Properties...>& v,
                          UnaryFunctorType functor) {
  Impl::static_assert_is_admissible_to_kokkos_std_algorithms(v);

  namespace KE = ::Kokkos::Experimental;
  return Impl::for_each_impl(label, ex, KE::begin(v), KE::end(v),
                             std::move(functor));
}

template <class ExecutionSpace, class DataType, class... Properties,
          class UnaryFunctorType>
UnaryFunctorType for_each(const ExecutionSpace& ex,
                          const ::Kokkos::View<DataType, Properties...>& v,
                          UnaryFunctorType functor) {
  Impl::static_assert_is_admissible_to_kokkos_std_algorithms(v);

  namespace KE = ::Kokkos::Experimental;
  return Impl::for_each_impl("Kokkos::for_each_view_api_default", ex,
                             KE::begin(v), KE::end(v), std::move(functor));
}

}  // namespace Experimental
}  // namespace Kokkos

#endif
