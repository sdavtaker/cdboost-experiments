// SPDX-License-Identifier: BSD-2-Clause
#include <boost/rational.hpp>
#include <cdcommons/time/decimal.hpp>
#include <cstdint>
#include <cdcommons/time/mbfp.hpp>
#include <cdcommons/time/rational.hpp>
#include <cdcommons/time/rsfp.hpp>

#include "k_counter.hpp"
#include "reset_gen.hpp"
#include "tick_gen.hpp"
#include <any>
#include <cdboost/pdevs/coupled.hpp>
#include <cdboost/pdevs/runner.hpp>
#include <cdboost/rational_time.hpp>
#include <iostream>
#include <map>
#include <string_view>

using namespace cdboost;
using namespace cdboost::pdevs;

template <class TIME>
static void run_experiment(const char *label, TIME tick_period, TIME reset_period,
                           TIME run_until_t) {
    using model_ptr = std::shared_ptr<model<TIME>>;

    auto tick_g  = vdw14::make_tick_gen(tick_period);
    auto reset_g = vdw14::make_reset_gen(reset_period);
    auto counter = std::make_shared<vdw14::k_counter<TIME>>();

    auto top = std::make_shared<coupled<TIME, std::any>>(
        std::initializer_list<model_ptr>{tick_g, reset_g, counter},
        std::initializer_list<model_ptr>{},
        std::initializer_list<std::pair<model_ptr, model_ptr>>{{tick_g, counter},
                                                               {reset_g, counter}},
        std::initializer_list<model_ptr>{counter});

    long long total = 0, errors = 0;
    std::map<int, long long> hist;

    runner<TIME, std::any> r(top, TIME{0}, [&](const std::any &msg) -> std::string {
        const int *vp = std::any_cast<int>(&msg);
        if (!vp) {
            ++errors;
            return "invalid-message-type";
        }
        ++total;
        ++hist[*vp];
        if (*vp != 10)
            ++errors;
        return std::to_string(*vp);
    });
    r.runUntil(run_until_t);

    if (hist.empty()) {
        std::cout << label << ": no outputs\n\n";
        return;
    }
    std::cout << label << ":\n"
              << "  resets:  " << total << "\n"
              << "  errors:  " << errors << " (" << (100.0 * errors / (double)total) << " %)\n"
              << "  range:   [" << hist.begin()->first << ", " << hist.rbegin()->first << "]\n"
              << "  histogram:";
    for (const auto &[val, cnt] : hist)
        std::cout << "  " << val << "x" << cnt;
    std::cout << "\n\n";
}

int main(int argc, char **argv) {
    const char *variant = (argc > 1) ? argv[1] : "all";

    std::cout << "VDW14 Tick-Counter Experiment (CDBoost PDEVS)\n"
              << "G_1/10 period=1/10  G_1 period=1  expected counter output=10\n\n";

    if (std::string_view(variant) == "float" || std::string_view(variant) == "all")
        run_experiment("float", float{0.1f}, float{1.0f}, float{10000});

    if (std::string_view(variant) == "double" || std::string_view(variant) == "all")
        run_experiment("double", double{0.1}, double{1.0}, double{10000});

    if (std::string_view(variant) == "rational" || std::string_view(variant) == "all") {
        using rat = boost::rational<int>;
        run_experiment("rational (boost)", rat{1, 10}, rat{1, 1}, rat{10000});
    }

    if (std::string_view(variant) == "decimal" || std::string_view(variant) == "all") {
        using dec3 = cdcommons::time::decimal<3>;
        run_experiment("decimal<3>", dec3::from_scaled(100), dec3::from_scaled(1000),
                       dec3::from_whole(10000));
    }

    if (std::string_view(variant) == "cdcommons-rational" || std::string_view(variant) == "all") {
        using rat32 = cdcommons::time::rational<std::int32_t>;
        run_experiment("rational<int32_t>", rat32{1, 10}, rat32{1, 1}, rat32{10000, 1});
    }

    if (std::string_view(variant) == "rsfp" || std::string_view(variant) == "all") {
        using rsfp_t = cdcommons::time::rsfp<1, 10>;
        run_experiment("rsfp<1,10>", rsfp_t{1}, rsfp_t{10}, rsfp_t{100000});
    }

    if (std::string_view(variant) == "mbfp" || std::string_view(variant) == "all") {
        using mbfp_t = cdcommons::time::mbfp<10, -1>;
        run_experiment("mbfp<10,-1>", mbfp_t{1}, mbfp_t{10}, mbfp_t{100000});
    }

    return 0;
}
