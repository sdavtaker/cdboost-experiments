// SPDX-License-Identifier: BSD-2-Clause
#pragma once

#include <cdboost/log.hpp>
#include <cdcommons/time/decimal.hpp>
#include <cdcommons/time/mbfp.hpp>
#include <cdcommons/time/rational.hpp>
#include <cdcommons/time/rsfp.hpp>
#include <cstdint>

namespace cdboost::log {

    template <>
    inline double to_sim_double<cdcommons::time::rational<std::int32_t>>(
        const cdcommons::time::rational<std::int32_t> &t) noexcept {
        return static_cast<double>(t.numerator()) / static_cast<double>(t.denominator());
    }

    // rsfp<1,10>: value = magnitude × (1/10)
    template <>
    inline double
    to_sim_double<cdcommons::time::rsfp<1, 10>>(const cdcommons::time::rsfp<1, 10> &t) noexcept {
        return static_cast<double>(t.magnitude()) * 0.1;
    }

    // mbfp<10,-1>: value = magnitude × 10^-1
    template <>
    inline double
    to_sim_double<cdcommons::time::mbfp<10, -1>>(const cdcommons::time::mbfp<10, -1> &t) noexcept {
        return static_cast<double>(t.magnitude()) * 0.1;
    }

    // decimal<3>: value = raw / 10^3
    template <>
    inline double
    to_sim_double<cdcommons::time::decimal<3>>(const cdcommons::time::decimal<3> &t) noexcept {
        return static_cast<double>(t.raw_value()) / 1000.0;
    }

} // namespace cdboost::log
