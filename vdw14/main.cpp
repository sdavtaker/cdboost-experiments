#include <boost/rational.hpp>

#include "k_counter.hpp"
#include "reset_gen.hpp"
#include "tick_gen.hpp"
#include <any>
#include <cdboost/pdevs/coordinator.hpp>
#include <cdboost/pdevs/coupled.hpp>
#include <cdboost/rational_time.hpp>
#include <iostream>
#include <map>
#include <vector>

using namespace cdboost;
using namespace cdboost::pdevs;

// ── Simulation driver ─────────────────────────────────────────────────────────

template <class TIME>
std::vector<int> run_experiment(TIME tick_period, TIME reset_period, int max_resets) {
    using model_ptr = std::shared_ptr<model<TIME>>;

    auto tick_gen  = vdw14::make_tick_gen(tick_period);
    auto reset_gen = vdw14::make_reset_gen(reset_period);
    auto counter   = std::make_shared<vdw14::k_counter<TIME>>();

    auto top = std::make_shared<coupled<TIME, std::any>>(
        std::initializer_list<model_ptr>{tick_gen, reset_gen, counter},
        std::initializer_list<model_ptr>{},
        std::initializer_list<std::pair<model_ptr, model_ptr>>{{tick_gen, counter},
                                                               {reset_gen, counter}},
        std::initializer_list<model_ptr>{counter});

    coordinator<TIME, std::any, nullqueue> coord(top);
    TIME t = coord.init(TIME{0});

    std::vector<int> outputs;
    outputs.reserve(max_resets);

    while (static_cast<int>(outputs.size()) < max_resets) {
        auto msgs = coord.collectOutputs(t);
        for (const auto &msg : msgs)
            outputs.push_back(std::any_cast<int>(msg));
        coord.advanceSimulation(t);
        t = coord.next();
    }
    return outputs;
}

// ── Statistics ────────────────────────────────────────────────────────────────

static void print_stats(const char *label, const std::vector<int> &outputs) {
    if (outputs.empty()) {
        std::cout << label << ": no outputs\n";
        return;
    }

    long long errors = 0;
    std::map<int, long long> hist;
    for (int v : outputs) {
        ++hist[v];
        if (v != 10)
            ++errors;
    }

    std::cout << label << ":\n"
              << "  resets:  " << outputs.size() << "\n"
              << "  errors:  " << errors << " (" << (100.0 * errors / (double)outputs.size())
              << " %)\n"
              << "  range:   [" << hist.begin()->first << ", " << hist.rbegin()->first << "]\n"
              << "  histogram:";
    for (const auto &[val, cnt] : hist)
        std::cout << "  " << val << "x" << cnt;
    std::cout << "\n\n";
}

// ── main ──────────────────────────────────────────────────────────────────────

int main() {
    const int max_resets = 10000;

    std::cout << "VDW14 Tick-Counter Experiment (CDBoost PDEVS)\n"
              << "G_1/10 period=1/10  G_1 period=1  expected counter output=10\n"
              << "max_resets=" << max_resets << "\n\n";

    auto double_outputs = run_experiment(double{0.1}, double{1.0}, max_resets);
    print_stats("double", double_outputs);

    using rat        = boost::rational<int>;
    auto rat_outputs = run_experiment(rat{1, 10}, rat{1, 1}, max_resets);
    print_stats("rational", rat_outputs);

    return 0;
}
