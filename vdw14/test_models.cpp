#include "k_counter.hpp"
#include "reset_gen.hpp"
#include "tick_gen.hpp"
#include <any>
#include <catch2/catch_test_macros.hpp>

// ── tick_gen ──────────────────────────────────────────────────────────────────

TEST_CASE("tick_gen period", "[tick_gen]") {
    auto g = vdw14::make_tick_gen(0.1);
    REQUIRE(g->advance() == 0.1);
}

TEST_CASE("tick_gen output is 1", "[tick_gen]") {
    auto g          = vdw14::make_tick_gen(0.1);
    const auto msgs = g->out();
    REQUIRE(msgs.size() == 1);
    REQUIRE(std::any_cast<int>(msgs[0]) == 1);
}

// ── reset_gen ─────────────────────────────────────────────────────────────────

TEST_CASE("reset_gen period", "[reset_gen]") {
    auto g = vdw14::make_reset_gen(1.0);
    REQUIRE(g->advance() == 1.0);
}

TEST_CASE("reset_gen output is 0", "[reset_gen]") {
    auto g          = vdw14::make_reset_gen(1.0);
    const auto msgs = g->out();
    REQUIRE(msgs.size() == 1);
    REQUIRE(std::any_cast<int>(msgs[0]) == 0);
}

// ── k_counter ─────────────────────────────────────────────────────────────────

TEST_CASE("k_counter initial advance is infinity", "[k_counter]") {
    vdw14::k_counter<double> k;
    REQUIRE(k.advance() == std::numeric_limits<double>::infinity());
}

TEST_CASE("k_counter tick increments count", "[k_counter]") {
    vdw14::k_counter<double> k;
    k.external({std::any{int{1}}, std::any{int{1}}}, 0.5);
    // still passive (no zero received)
    REQUIRE(k.advance() == std::numeric_limits<double>::infinity());
}

TEST_CASE("k_counter zero input arms output", "[k_counter]") {
    vdw14::k_counter<double> k;
    k.external({std::any{int{1}}}, 0.1);
    k.external({std::any{int{0}}}, 1.0);
    REQUIRE(k.advance() == 0.0);
}

TEST_CASE("k_counter output equals accumulated count", "[k_counter]") {
    vdw14::k_counter<double> k;
    for (int i = 0; i < 3; ++i)
        k.external({std::any{int{1}}}, static_cast<double>(i) * 0.1 + 0.1);
    k.external({std::any{int{0}}}, 1.0);
    const auto out = k.out();
    REQUIRE(out.size() == 1);
    REQUIRE(std::any_cast<int>(out[0]) == 3);
}

TEST_CASE("k_counter internal resets to passive", "[k_counter]") {
    vdw14::k_counter<double> k;
    k.external({std::any{int{1}}}, 0.1);
    k.external({std::any{int{0}}}, 1.0);
    REQUIRE(k.advance() == 0.0);
    k.internal();
    REQUIRE(k.advance() == std::numeric_limits<double>::infinity());
}
