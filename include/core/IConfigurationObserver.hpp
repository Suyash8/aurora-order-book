#pragma once

namespace aurora {
namespace core {

class IConfigurationObserver {
public:
  virtual ~IConfigurationObserver() = default;
  virtual void on_configuration_reload() = 0;
};

} // namespace core
} // namespace aurora