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

#include <TestDynViewAPI.hpp>
namespace Test {
TEST(TEST_CATEGORY, dyn_rank_view_api_generic) {
  TestDynViewAPI<double, TEST_EXECSPACE>::run_tests();
}
}  // namespace Test
