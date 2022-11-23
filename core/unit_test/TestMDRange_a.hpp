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

#include <TestMDRange.hpp>

namespace Test {

TEST(TEST_CATEGORY, mdrange_5d) {
// FIXME_OPENMPTARGET requires MDRange parallel_reduce
#ifndef KOKKOS_ENABLE_OPENMPTARGET
  TestMDRange_5D<TEST_EXECSPACE>::test_reduce5(100, 10, 10, 10, 5);
#endif
  TestMDRange_5D<TEST_EXECSPACE>::test_for5(100, 10, 10, 10, 5);
}

}  // namespace Test
