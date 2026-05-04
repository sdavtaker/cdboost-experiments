#pragma once

#include <any>
#include <cdboost/convenience.hpp>
#include <cdboost/pdevs/basic_models/generator.hpp>

namespace vdw14 {

    // Factory: tick_gen fires at the given period and outputs std::any{int{1}}.
    // G_1/10: period = 1/10 s (caller supplies the TIME-typed period value).
    template <class TIME> auto make_tick_gen(TIME period) {
        return cdboost::make_atomic_ptr<cdboost::pdevs::basic_models::generator<TIME, std::any>,
                                        TIME, std::any>(period, std::any{int{1}});
    }

} // namespace vdw14
