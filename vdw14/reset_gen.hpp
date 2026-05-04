// SPDX-License-Identifier: BSD-2-Clause
#pragma once

#include <any>
#include <cdboost/convenience.hpp>
#include <cdboost/pdevs/basic_models/generator.hpp>

namespace vdw14 {

    // Factory: reset_gen fires at the given period and outputs std::any{int{0}}.
    // G_1: period = 1 s (caller supplies the TIME-typed period value).
    // The zero value is the reset signal recognized by k_counter.
    template <class TIME> auto make_reset_gen(TIME period) {
        return cdboost::make_atomic_ptr<cdboost::pdevs::basic_models::generator<TIME, std::any>,
                                        TIME, std::any>(period, std::any{int{0}});
    }

} // namespace vdw14
