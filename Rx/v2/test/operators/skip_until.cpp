#include "rxcpp/rx.hpp"
namespace rxu=rxcpp::util;
namespace rxsc=rxcpp::schedulers;

#include "rxcpp/rx-test.hpp"
#include "catch.hpp"

SCENARIO("skip_until, some data next", "[skip_until][skip][operators]"){
    GIVEN("2 sources"){
        auto sc = rxsc::make_test();
        auto w = sc.create_worker();
        const rxsc::test::messages<int> on;

        auto l = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_next(210, 2),
            on.on_next(220, 3),
            on.on_next(230, 4),
            on.on_next(240, 5),
            on.on_completed(250)
        });

        auto r = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_next(225, 99),
            on.on_completed(230)
        });

        WHEN("one is taken until the other emits a marble"){

            auto res = w.start(
                [&]() {
                    return l
                        .skip_until(r)
                        // forget type to workaround lambda deduction bug on msvc 2013
                        .as_dynamic();
                }
            );

            THEN("the output only contains items sent while subscribed"){
                auto required = rxu::to_vector({
                    on.on_next(230, 4),
                    on.on_next(240, 5),
                    on.on_completed(250)
                });
                auto actual = res.get_observer().messages();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the source"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 250)
                });
                auto actual = l.subscriptions();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the trigger"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 225)
                });
                auto actual = r.subscriptions();
                REQUIRE(required == actual);
            }
        }
    }
}

SCENARIO("skip_until, some data error", "[skip_until][skip][operators]"){
    GIVEN("2 sources"){
        auto sc = rxsc::make_test();
        auto w = sc.create_worker();
        const rxsc::test::messages<int> on;

        std::runtime_error ex("skip_until on_error from source");

        auto l = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_next(210, 2),
            on.on_next(220, 3),
            on.on_next(230, 4),
            on.on_next(240, 5),
            on.on_completed(250)
        });

        auto r = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_error(225, ex)
        });

        WHEN("one is taken until the other emits a marble"){

            auto res = w.start(
                [&]() {
                    return l
                        .skip_until(r)
                        // forget type to workaround lambda deduction bug on msvc 2013
                        .as_dynamic();
                }
            );

            THEN("the output only contains error message"){
                auto required = rxu::to_vector({
                    on.on_error(225, ex)
                });
                auto actual = res.get_observer().messages();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the source"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 225)
                });
                auto actual = l.subscriptions();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the trigger"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 225)
                });
                auto actual = r.subscriptions();
                REQUIRE(required == actual);
            }
        }
    }
}

SCENARIO("skip_until, error some data", "[skip_until][skip][operators]"){
    GIVEN("2 sources"){
        auto sc = rxsc::make_test();
        auto w = sc.create_worker();
        const rxsc::test::messages<int> on;

        std::runtime_error ex("skip_until on_error from source");

        auto l = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_next(210, 2),
            on.on_error(220, ex)
        });

        auto r = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_next(230, 3),
            on.on_completed(250)
        });

        WHEN("one is taken until the other emits a marble"){

            auto res = w.start(
                [&]() {
                    return l
                        .skip_until(r)
                        // forget type to workaround lambda deduction bug on msvc 2013
                        .as_dynamic();
                }
            );

            THEN("the output only contains error message"){
                auto required = rxu::to_vector({
                    on.on_error(220, ex)
                });
                auto actual = res.get_observer().messages();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the source"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 220)
                });
                auto actual = l.subscriptions();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the trigger"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 220)
                });
                auto actual = r.subscriptions();
                REQUIRE(required == actual);
            }
        }
    }
}

SCENARIO("skip_until, some data empty", "[skip_until][skip][operators]"){
    GIVEN("2 sources"){
        auto sc = rxsc::make_test();
        auto w = sc.create_worker();
        const rxsc::test::messages<int> on;

        auto l = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_next(210, 2),
            on.on_next(220, 3),
            on.on_next(230, 4),
            on.on_next(240, 5),
            on.on_completed(250)
        });

        auto r = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_completed(225)
        });

        WHEN("one is taken until the other emits a marble"){

            auto res = w.start(
                [&]() {
                    return l
                        .skip_until(r)
                        // forget type to workaround lambda deduction bug on msvc 2013
                        .as_dynamic();
                }
            );

            THEN("the output is empty"){
                auto required = std::vector<rxsc::test::messages<int>::recorded_type>();
                auto actual = res.get_observer().messages();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the source"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 250)
                });
                auto actual = l.subscriptions();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the trigger"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 225)
                });
                auto actual = r.subscriptions();
                REQUIRE(required == actual);
            }
        }
    }
}

SCENARIO("skip_until, never next", "[skip_until][skip][operators]"){
    GIVEN("2 sources"){
        auto sc = rxsc::make_test();
        auto w = sc.create_worker();
        const rxsc::test::messages<int> on;

        auto l = sc.make_hot_observable({
            on.on_next(150, 1)
        });

        auto r = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_next(225, 2),
            on.on_completed(250)
        });

        WHEN("one is taken until the other emits a marble"){

            auto res = w.start(
                [&]() {
                    return l
                        .skip_until(r)
                        // forget type to workaround lambda deduction bug on msvc 2013
                        .as_dynamic();
                }
            );

            THEN("the output is empty"){
                auto required = std::vector<rxsc::test::messages<int>::recorded_type>();
                auto actual = res.get_observer().messages();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the source"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 1000)
                });
                auto actual = l.subscriptions();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the trigger"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 225)
                });
                auto actual = r.subscriptions();
                REQUIRE(required == actual);
            }
        }
    }
}

SCENARIO("skip_until, never error", "[skip_until][skip][operators]"){
    GIVEN("2 sources"){
        auto sc = rxsc::make_test();
        auto w = sc.create_worker();
        const rxsc::test::messages<int> on;

        std::runtime_error ex("skip_until on_error from source");

        auto l = sc.make_hot_observable({
            on.on_next(150, 1)
        });

        auto r = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_error(225, ex)
        });

        WHEN("one is taken until the other emits a marble"){

            auto res = w.start(
                [&]() {
                return l
                    .skip_until(r)
                    // forget type to workaround lambda deduction bug on msvc 2013
                    .as_dynamic();
            }
            );

            THEN("the output only contains error message"){
                auto required = rxu::to_vector({
                    on.on_error(225, ex)
                });
                auto actual = res.get_observer().messages();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the source"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 225)
                });
                auto actual = l.subscriptions();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the trigger"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 225)
                });
                auto actual = r.subscriptions();
                REQUIRE(required == actual);
            }
        }
    }
}

SCENARIO("skip_until, some data error after completed", "[skip_until][skip][operators]"){
    GIVEN("2 sources"){
        auto sc = rxsc::make_test();
        auto w = sc.create_worker();
        const rxsc::test::messages<int> on;

        std::runtime_error ex("skip_until on_error from source");

        auto l = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_next(210, 2),
            on.on_next(220, 3),
            on.on_next(230, 4),
            on.on_next(240, 5),
            on.on_completed(250)
        });

        auto r = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_error(300, ex)
        });

        WHEN("one is taken until the other emits a marble"){

            auto res = w.start(
                [&]() {
                    return l
                        .skip_until(r)
                        // forget type to workaround lambda deduction bug on msvc 2013
                        .as_dynamic();
                }
            );

            THEN("the output only contains error message"){
                auto required = rxu::to_vector({
                    on.on_error(300, ex)
                });
                auto actual = res.get_observer().messages();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the source"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 250)
                });
                auto actual = l.subscriptions();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the trigger"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 300)
                });
                auto actual = r.subscriptions();
                REQUIRE(required == actual);
            }
        }
    }
}

SCENARIO("skip_until, some data never", "[skip_until][skip][operators]"){
    GIVEN("2 sources"){
        auto sc = rxsc::make_test();
        auto w = sc.create_worker();
        const rxsc::test::messages<int> on;

        auto l = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_next(210, 2),
            on.on_next(220, 3),
            on.on_next(230, 4),
            on.on_next(240, 5),
            on.on_completed(250)
        });

        auto r = sc.make_hot_observable({
            on.on_next(150, 1)
        });

        WHEN("one is taken until the other emits a marble"){

            auto res = w.start(
                [&]() {
                    return l
                        .skip_until(r)
                        // forget type to workaround lambda deduction bug on msvc 2013
                        .as_dynamic();
                }
            );

            THEN("the output is empty"){
                auto required = std::vector<rxsc::test::messages<int>::recorded_type>();
                auto actual = res.get_observer().messages();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the source"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 250)
                });
                auto actual = l.subscriptions();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the trigger"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 1000)
                });
                auto actual = r.subscriptions();
                REQUIRE(required == actual);
            }
        }
    }
}

SCENARIO("skip_until, never empty", "[skip_until][skip][operators]"){
    GIVEN("2 sources"){
        auto sc = rxsc::make_test();
        auto w = sc.create_worker();
        const rxsc::test::messages<int> on;

        auto l = sc.make_hot_observable({
            on.on_next(150, 1)
        });

        auto r = sc.make_hot_observable({
            on.on_next(150, 1),
            on.on_completed(225)
        });

        WHEN("one is taken until the other emits a marble"){

            auto res = w.start(
                [&]() {
                    return l
                        .skip_until(r)
                        // forget type to workaround lambda deduction bug on msvc 2013
                        .as_dynamic();
                }
            );

            THEN("the output is empty"){
                auto required = std::vector<rxsc::test::messages<int>::recorded_type>();
                auto actual = res.get_observer().messages();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the source"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 1000)
                });
                auto actual = l.subscriptions();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the trigger"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 225)
                });
                auto actual = r.subscriptions();
                REQUIRE(required == actual);
            }
        }
    }
}

SCENARIO("skip_until, never never", "[skip_until][skip][operators]"){
    GIVEN("2 sources"){
        auto sc = rxsc::make_test();
        auto w = sc.create_worker();
        const rxsc::test::messages<int> on;

        auto l = sc.make_hot_observable({
            on.on_next(150, 1)
        });

        auto r = sc.make_hot_observable({
            on.on_next(150, 1)
        });

        WHEN("one is taken until the other emits a marble"){

            auto res = w.start(
                [&]() {
                    return l
                        .skip_until(r)
                        // forget type to workaround lambda deduction bug on msvc 2013
                        .as_dynamic();
                }
            );

            THEN("the output is empty"){
                auto required = std::vector<rxsc::test::messages<int>::recorded_type>();
                auto actual = res.get_observer().messages();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the source"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 1000)
                });
                auto actual = l.subscriptions();
                REQUIRE(required == actual);
            }

            THEN("there was 1 subscription/unsubscription to the trigger"){
                auto required = rxu::to_vector({
                    on.subscribe(200, 1000)
                });
                auto actual = r.subscriptions();
                REQUIRE(required == actual);
            }
        }
    }
}
