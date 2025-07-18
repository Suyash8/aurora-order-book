#include "core/Configuration.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>

using aurora::core::Configuration;

TEST_CASE("Configuration singleton instance", "[configuration]") {
  Configuration &config1 = Configuration::instance();
  Configuration &config2 = Configuration::instance();

  REQUIRE(&config1 == &config2);
}

TEST_CASE("Configuration default values", "[configuration]") {
  Configuration &config = Configuration::instance();

  REQUIRE(config.get_port() == 8080);
  REQUIRE(config.get_io_threads() == 4);
  REQUIRE(config.get_worker_threads() >= 0);
  REQUIRE_FALSE(config.is_persistence_enabled());
  REQUIRE(config.get_instruments().empty());
}

TEST_CASE("Configuration JSON loading", "[configuration]") {
  std::string test_config_path = "test_config.json";
  {
    std::ofstream config_file(test_config_path);
    config_file << R"({
        "network": {
            "port": 9090,
            "io_threads": 8
        },
        "processing": {
            "worker_threads": 16
        },
        "persistence": {
            "enabled": true
        },
        "instruments": ["AAPL", "GOOGL", "MSFT"]
    })";
  }

  Configuration &config = Configuration::instance();

  REQUIRE_NOTHROW(config.load_from_file(test_config_path));

  REQUIRE(config.get_port() == 9090);
  REQUIRE(config.get_io_threads() == 8);
  REQUIRE(config.get_worker_threads() == 16);
  REQUIRE(config.is_persistence_enabled());

  auto instruments = config.get_instruments();
  REQUIRE(instruments.size() == 3);
  REQUIRE(instruments[0] == "AAPL");
  REQUIRE(instruments[1] == "GOOGL");
  REQUIRE(instruments[2] == "MSFT");

  std::filesystem::remove(test_config_path);
}

TEST_CASE("Configuration YAML loading", "[configuration]") {
  std::string test_config_path = "test_config.yaml";
  {
    std::ofstream config_file(test_config_path);
    config_file << R"(
network:
  port: 7070
  io_threads: 2
processing:
  worker_threads: 12
persistence:
  enabled: false
instruments:
  - BTC
  - ETH
        )";
  }

  Configuration &config = Configuration::instance();

  REQUIRE_NOTHROW(config.load_from_file(test_config_path));

  REQUIRE(config.get_port() == 7070);
  REQUIRE(config.get_io_threads() == 2);
  REQUIRE(config.get_worker_threads() == 12);
  REQUIRE_FALSE(config.is_persistence_enabled());

  auto instruments = config.get_instruments();
  REQUIRE(instruments.size() == 2);
  REQUIRE(instruments[0] == "BTC");
  REQUIRE(instruments[1] == "ETH");

  std::filesystem::remove(test_config_path);
}

TEST_CASE("Configuration command line arguments", "[configuration]") {
  Configuration &config = Configuration::instance();

  const char *argv[] = {"aurora_order_book",
                        "--port",
                        "5050",
                        "--io-threads",
                        "6",
                        "--worker-threads",
                        "10",
                        "--enable-persistence",
                        "--instruments",
                        "TSLA,AMZN"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  REQUIRE_NOTHROW(config.load_from_args(argc, const_cast<char **>(argv)));

  REQUIRE(config.get_port() == 5050);
  REQUIRE(config.get_io_threads() == 6);
  REQUIRE(config.get_worker_threads() == 10);
  REQUIRE(config.is_persistence_enabled());

  auto instruments = config.get_instruments();
  REQUIRE(instruments.size() == 2);
  REQUIRE(instruments[0] == "TSLA");
  REQUIRE(instruments[1] == "AMZN");
}

TEST_CASE("Configuration file not found", "[configuration]") {
  Configuration &config = Configuration::instance();

  REQUIRE_THROWS(config.load_from_file("non_existent_file.json"));
}

TEST_CASE("Configuration invalid JSON", "[configuration]") {
  std::string test_config_path = "invalid_config.json";
  {
    std::ofstream config_file(test_config_path);
    config_file << R"({
            "network": {
                "port": 9090,
                "io_threads": 8
            },
            "processing": {
                "worker_threads": 16
            },
            "persistence": {
                "enabled": true
            },
            "instruments": ["AAPL", "MSFT", "GOOGL"
        })"; // Missing closing bracket
  }

  Configuration &config = Configuration::instance();

  REQUIRE_THROWS(config.load_from_file(test_config_path));

  std::filesystem::remove(test_config_path);
}

TEST_CASE("Configuration command line priority over file", "[configuration]") {
  std::string test_config_path = "priority_test_config.json";
  {
    std::ofstream config_file(test_config_path);
    config_file << R"({
            "network": {
                "port": 9090,
                "io_threads": 8
            },
            "processing": {
                "worker_threads": 16
            },
            "persistence": {
                "enabled": false
            },
            "instruments": ["AAPL", "MSFT", "GOOGL"]
        })";
  }

  Configuration &config = Configuration::instance();

  REQUIRE_NOTHROW(config.load_from_file(test_config_path));

  const char *argv[] = {"aurora_order_book", "--port", "6060",
                        "--enable-persistence"};

  int argc = sizeof(argv) / sizeof(argv[0]);

  REQUIRE_NOTHROW(config.load_from_args(argc, const_cast<char **>(argv)));

  REQUIRE(config.get_port() == 6060);
  REQUIRE(config.is_persistence_enabled());

  REQUIRE(config.get_io_threads() == 8);
  REQUIRE(config.get_worker_threads() == 16);

  auto instruments = config.get_instruments();
  REQUIRE(instruments.size() == 3);

  std::filesystem::remove(test_config_path);
}