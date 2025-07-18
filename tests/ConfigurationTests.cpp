#include "core/Configuration.h"
#include "core/IConfigurationObserver.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_streaming_base.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <thread>

using aurora::core::Configuration;
using aurora::core::IConfigurationObserver;

// Mock observer for testing the observer pattern
class MockConfigObserver : public IConfigurationObserver {
public:
  bool was_notified = false;
  void on_configuration_reload() override { was_notified = true; }
  ~MockConfigObserver() override = default;
};

class TempConfigFile {
public:
  TempConfigFile(const std::string &filename, const std::string &content)
      : filename_(filename) {
    std::ofstream file(filename);
    file << content;
    file.close();
  }

  ~TempConfigFile() { std::filesystem::remove(filename_); }

  std::string filename_;
};

// RAII class to redirect stdout and stderr
class OutputRedirect {
public:
  OutputRedirect() : old_cout(std::cout.rdbuf()), old_cerr(std::cerr.rdbuf()) {
    std::cout.rdbuf(null_stream.rdbuf());
    std::cerr.rdbuf(null_stream.rdbuf());
  }

  ~OutputRedirect() {
    std::cout.rdbuf(old_cout);
    std::cerr.rdbuf(old_cerr);
  }

private:
  std::stringstream null_stream;
  std::streambuf *old_cout;
  std::streambuf *old_cerr;
};

TEST_CASE("Configuration setup - redirect output", "[configuration]") {
  static OutputRedirect redirector;
}

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
  TempConfigFile config_file("test_config.json", R"({
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
  })");

  Configuration &config = Configuration::instance();

  REQUIRE_NOTHROW(config.load_from_file(config_file.filename_));

  REQUIRE(config.get_port() == 9090);
  REQUIRE(config.get_io_threads() == 8);
  REQUIRE(config.get_worker_threads() == 16);
  REQUIRE(config.is_persistence_enabled());

  auto instruments = config.get_instruments();
  REQUIRE(instruments.size() == 3);
  REQUIRE(instruments[0] == "AAPL");
  REQUIRE(instruments[1] == "GOOGL");
  REQUIRE(instruments[2] == "MSFT");
}

TEST_CASE("Configuration YAML loading", "[configuration]") {
  TempConfigFile config_file("test_config.yaml", R"(
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
  )");

  Configuration &config = Configuration::instance();

  REQUIRE_NOTHROW(config.load_from_file(config_file.filename_));

  REQUIRE(config.get_port() == 7070);
  REQUIRE(config.get_io_threads() == 2);
  REQUIRE(config.get_worker_threads() == 12);
  REQUIRE_FALSE(config.is_persistence_enabled());

  auto instruments = config.get_instruments();
  REQUIRE(instruments.size() == 2);
  REQUIRE(instruments[0] == "BTC");
  REQUIRE(instruments[1] == "ETH");
}

TEST_CASE("Configuration missing file handling", "[configuration]") {
  Configuration &config = Configuration::instance();

  REQUIRE_THROWS(config.load_from_file("non_existent_file.json"));
}

TEST_CASE("Configuration invalid JSON", "[configuration]") {
  TempConfigFile config_file("invalid_config.json", R"({
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
  })"); // Missing closing bracket

  Configuration &config = Configuration::instance();

  REQUIRE_THROWS(config.load_from_file(config_file.filename_));
}

TEST_CASE("Configuration file with unsupported extension", "[configuration]") {
  TempConfigFile config_file("config.txt", R"(
This is not a supported format
  )");

  Configuration &config = Configuration::instance();

  REQUIRE_THROWS(config.load_from_file(config_file.filename_));
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

TEST_CASE("Configuration command line help", "[configuration]") {
  Configuration &config = Configuration::instance();

  const char *argv[] = {"aurora_order_book", "--help"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  REQUIRE_NOTHROW(config.load_from_args(argc, const_cast<char **>(argv)));
}

TEST_CASE("Configuration command line with invalid option", "[configuration]") {
  Configuration &config = Configuration::instance();

  const char *argv[] = {"aurora_order_book", "--non-existent-option", "value"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  REQUIRE_THROWS(config.load_from_args(argc, const_cast<char **>(argv)));
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

TEST_CASE("Configuration command line with invalid value", "[configuration]") {
  Configuration &config = Configuration::instance();

  const char *argv[] = {"aurora_order_book", "--port", "not_a_number"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  REQUIRE_THROWS(config.load_from_args(argc, const_cast<char **>(argv)));
}

TEST_CASE("Configuration command line with config file", "[configuration]") {
  TempConfigFile config_file("cmd_config.json", R"({
    "network": {
      "port": 9090,
      "io_threads": 8
    },
    "instruments": ["AAPL", "MSFT", "GOOGL"]
  })");

  Configuration &config = Configuration::instance();

  const char *argv[] = {
      "aurora_order_book", "--config", "cmd_config.json", "--port",
      "5050" // Should override file setting
  };
  int argc = sizeof(argv) / sizeof(argv[0]);

  REQUIRE_NOTHROW(config.load_from_args(argc, const_cast<char **>(argv)));

  REQUIRE(config.get_port() == 5050);    // Command line value
  REQUIRE(config.get_io_threads() == 8); // File value

  auto instruments = config.get_instruments();
  REQUIRE(instruments.size() == 3);
}

TEST_CASE("Configuration validation with invalid port", "[configuration]") {
  Configuration &config = Configuration::instance();

  const char *argv[] = {
      "aurora_order_book", "--port", "80" // Invalid (privileged port)
  };
  int argc = sizeof(argv) / sizeof(argv[0]);

  REQUIRE_THROWS(config.load_from_args(argc, const_cast<char **>(argv)));
}

TEST_CASE("Configuration validation with extremely high thread count",
          "[configuration]") {
  Configuration &config = Configuration::instance();

  const char *argv[] = {
      "aurora_order_book", "--io-threads", "1000" // Unreasonably high
  };
  int argc = sizeof(argv) / sizeof(argv[0]);

  REQUIRE_THROWS(config.load_from_args(argc, const_cast<char **>(argv)));
}

TEST_CASE("Configuration thread safety for readers", "[configuration]") {
  // Load a configuration with known values
  TempConfigFile config_file("thread_test.json", R"({
    "network": {
      "port": 9090
    }
  })");

  Configuration &config = Configuration::instance();
  config.load_from_file(config_file.filename_);

  // Create threads that read the configuration concurrently
  constexpr int num_threads = 10;
  std::vector<std::thread> threads;
  std::vector<uint16_t> results(num_threads);

  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(
        [&config, &results, i]() { results[i] = config.get_port(); });
  }

  for (auto &t : threads) {
    t.join();
  }

  // Verify all threads read the correct value
  for (int i = 0; i < num_threads; ++i) {
    REQUIRE(results[i] == 9090);
  }
}

TEST_CASE("Configuration observer registration and notification",
          "[configuration]") {
  // Create temporary configuration file
  TempConfigFile config_file("observer_test.json", R"({
    "network": {
      "port": 9090
    }
  })");

  Configuration &config = Configuration::instance();
  config.load_from_file(config_file.filename_);

  // Register observers
  MockConfigObserver observer1;
  MockConfigObserver observer2;

  config.register_observer(&observer1);
  config.register_observer(&observer2);

  // Update the file and reload
  {
    std::ofstream file(config_file.filename_);
    file << R"({
      "network": {
        "port": 9091
      }
    })";
    file.close();
  }

  config.reload();

  // Verify observers were notified
  REQUIRE(observer1.was_notified);
  REQUIRE(observer2.was_notified);
  REQUIRE(config.get_port() == 9091);
}

TEST_CASE("Configuration reload with invalid configuration",
          "[configuration]") {
  // Create temporary configuration file
  TempConfigFile config_file("reload_test.json", R"({
    "network": {
      "port": 9090
    }
  })");

  Configuration &config = Configuration::instance();
  config.load_from_file(config_file.filename_);

  // Register an observer
  MockConfigObserver observer;
  config.register_observer(&observer);

  // Update the file with invalid JSON
  {
    std::ofstream file(config_file.filename_);
    file << R"({
      "network": {
        "port": 9091
      }
  )"; // Missing closing brace
    file.close();
  }

  // Reload should handle the error gracefully
  config.reload();

  // Observer should not be notified since reload failed
  REQUIRE_FALSE(observer.was_notified);

  // Configuration should remain unchanged
  REQUIRE(config.get_port() == 9090);
}

TEST_CASE("Configuration reload with missing file", "[configuration]") {
  // Create temporary configuration file
  TempConfigFile config_file("missing_test.json", R"({
    "network": {
      "port": 9090
    }
  })");

  Configuration &config = Configuration::instance();
  config.load_from_file(config_file.filename_);

  // Delete the file
  std::filesystem::remove(config_file.filename_);

  // Reload should handle the error gracefully
  config.reload();

  // Configuration should remain unchanged
  REQUIRE(config.get_port() == 9090);
}

TEST_CASE("Configuration register null observer", "[configuration]") {
  Configuration &config = Configuration::instance();

  // Should not crash when registering a null observer
  REQUIRE_NOTHROW(config.register_observer(nullptr));
}

TEST_CASE("Configuration register duplicate observer", "[configuration]") {
  Configuration &config = Configuration::instance();

  // Create a mock observer on the stack (safer than heap)
  MockConfigObserver observer;

  // Register the same observer twice
  config.register_observer(&observer);
  config.register_observer(&observer);

  // Load a simple valid configuration
  TempConfigFile config_file("simple_test.json",
                             R"({"instruments": ["TEST"]})");
  REQUIRE_NOTHROW(config.load_from_file(config_file.filename_));

  // Success if we get here without crashing
}

TEST_CASE("Configuration reload functionality", "[configuration][.]") {
  // Create a minimal valid configuration file
  TempConfigFile config_file("minimal_test.json", R"({
    "instruments": ["TEST"]
  })");

  // Load the file
  Configuration &config = Configuration::instance();
  REQUIRE_NOTHROW(config.load_from_file(config_file.filename_));

  // Try to reload without observers
  REQUIRE_NOTHROW(config.reload());
}

TEST_CASE("Configuration empty instruments", "[configuration]") {
  TempConfigFile config_file("empty_instruments.json", R"({
    "network": {
      "port": 9090
    },
    "instruments": []
  })");

  Configuration &config = Configuration::instance();
  config.load_from_file(config_file.filename_);

  auto instruments = config.get_instruments();
  REQUIRE(instruments.empty());
}