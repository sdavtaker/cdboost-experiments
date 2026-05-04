#pragma once

#include <any>
#include <cdboost/pdevs/basic_models/infinite_counter.hpp>

namespace vdw14 {

    // k_counter: infinite_counter parameterised with std::any messages.
    // Counts non-zero inputs; zero input triggers output of the count then reset.
    template <class TIME>
    using k_counter = cdboost::pdevs::basic_models::infinite_counter<TIME, std::any>;

} // namespace vdw14
