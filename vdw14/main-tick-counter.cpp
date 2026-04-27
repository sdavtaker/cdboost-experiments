/**
 * VDW14 Experiment: Tick Counter Float Reordering
 *
 * Demonstrates the causality error from VDW14 (Vicino, Dalle, Wainer 2014):
 * a generator firing at period 1/10 for 100 ticks should accumulate exactly
 * time 10. With boost::rational<int> arithmetic is exact and the 100th tick
 * lands at the boundary, excluded by runUntil's strict-less-than semantics.
 * With double, IEEE 754 underaccumulation places the 100th tick just below
 * end_time, so it fires — producing a different tick count for the same model.
 *
 * Reference: "A Data Type for Discretized Time Representation in DEVS",
 * SIMUTOOLS 2014, Lisbon. HAL: hal-01055555
 */

#include <boost/rational.hpp>
#include <iostream>

#include <cdboost/cdboost.hpp>
#include <cdboost/pdevs/basic_models/generator.hpp>
#include <cdboost/rational_time.hpp>

using namespace cdboost;
using namespace cdboost::pdevs;
using namespace cdboost::pdevs::basic_models;

template <class TIME>
long count_ticks(TIME period, TIME end_time) {
    using model_ptr = std::shared_ptr<model<TIME>>;
    auto gen        = make_atomic_ptr<generator<TIME, int>, TIME, int>(period, int{1});
    auto root       = std::make_shared<coupled<TIME, int>>(
        std::initializer_list<model_ptr>{gen},
        std::initializer_list<model_ptr>{},
        std::initializer_list<std::pair<model_ptr, model_ptr>>{},
        std::initializer_list<model_ptr>{gen});
    coordinator<TIME, int, nullqueue> coord(root);
    TIME t     = coord.init(TIME{0});
    long ticks = 0;
    while (t < end_time) {
        coord.advanceSimulation(t);
        t = coord.next();
        ++ticks;
    }
    return ticks;
}

int main() {
    const long expected = 100;

    long d_ticks = count_ticks(double{0.1}, double{10.0});
    long r_ticks = count_ticks(boost::rational<int>{1, 10}, boost::rational<int>{10, 1});

    std::cout << "VDW14 Tick Counter Experiment\n";
    std::cout << "period=1/10  end_time=10  expected=" << expected << " ticks\n\n";
    std::cout << "double:   " << d_ticks << " ticks\n";
    std::cout << "rational: " << r_ticks << " ticks\n\n";

    if (d_ticks == r_ticks) {
        std::cout << "RESULT: no discrepancy observed on this platform\n";
    } else {
        std::cout << "RESULT: discrepancy of " << (d_ticks - r_ticks)
                  << " tick(s) — float arithmetic changes simulation behavior\n";
    }
    return 0;
}
