#include "modules/temperature.hpp"

#include <filesystem>
#include <string>

waybar::modules::Temperature::Temperature(const std::string& id, const Json::Value& config)
    : ALabel(config, "temperature", id, "{temperatureC}°C", 10) {
      
  auto traverseAsArray = [](const Json::Value& value, auto&& check_set_path) {
    if (value.isString())
      check_set_path(value.asString());
    else if (value.isArray())
      for (const auto& item : value)
        if (check_set_path(item.asString())) break;
  };

  // if hwmon_path is an array, loop to find first valid item
  traverseAsArray(config_["hwmon-path"], [this](const std::string& path) {
    if (!std::filesystem::exists(path)) return false;
    file_path_ = path;
    return true;
  });

  if (file_path_.empty() && config_["input-filename"].isString()) {
    // fallback to hwmon_paths-abs
    traverseAsArray(config_["hwmon-path-abs"], [this](const std::string& path) {
      if (!std::filesystem::is_directory(path)) return false;
      return std::ranges::any_of(
          std::filesystem::directory_iterator(path), [this](const auto& hwmon) {
            if (!hwmon.path().filename().string().starts_with("hwmon")) return false;
            file_path_ = hwmon.path().string() + "/" + config_["input-filename"].asString();
            return true;
          });
    });
  }

  if (file_path_.empty()) {
    auto zone = config_["thermal-zone"].isInt() ? config_["thermal-zone"].asInt() : 0;
    file_path_ = fmt::format("/sys/class/thermal/thermal_zone{}/temp", zone);
  }

  // check if file_path_ can be used to retrieve the temperature
  std::ifstream temp(file_path_);
  if (!temp.is_open()) {
    throw std::runtime_error("Can't open " + file_path_);
  }
  if (!temp.good()) {
    temp.close();
    throw std::runtime_error("Can't read from " + file_path_);
  }
  temp.close();

  thread_ = [this] {
    dp.emit();
    thread_.sleep_for(interval_);
  };
}

auto waybar::modules::Temperature::update() -> void {
  auto temperature = getTemperature();
  uint16_t temperature_c = std::round(temperature);
  uint16_t temperature_f = std::round(temperature * 1.8 + 32);
  uint16_t temperature_k = std::round(temperature + 273.15);
  auto critical = isCritical(temperature_c);
  auto warning = isWarning(temperature_c);
  auto format = format_;
  if (critical) {
    format = config_["format-critical"].isString() ? config_["format-critical"].asString() : format;
    label_.get_style_context()->add_class("critical");
  } else if (warning) {
    format = config_["format-warning"].isString() ? config_["format-warning"].asString() : format;
    label_.get_style_context()->add_class("warning");
  } else {
    label_.get_style_context()->remove_class("critical");
    label_.get_style_context()->remove_class("warning");
  }

  if (format.empty()) {
    event_box_.hide();
    return;
  }

  event_box_.show();

  auto max_temp = config_["critical-threshold"].isInt() ? config_["critical-threshold"].asInt() : 0;
  label_.set_markup(fmt::format(fmt::runtime(format), fmt::arg("temperatureC", temperature_c),
                                fmt::arg("temperatureF", temperature_f),
                                fmt::arg("temperatureK", temperature_k),
                                fmt::arg("icon", getIcon(temperature_c, "", max_temp))));
  if (tooltipEnabled()) {
    std::string tooltip_format = "{temperatureC}°C";
    if (config_["tooltip-format"].isString()) {
      tooltip_format = config_["tooltip-format"].asString();
    }
    label_.set_tooltip_text(fmt::format(
        fmt::runtime(tooltip_format), fmt::arg("temperatureC", temperature_c),
        fmt::arg("temperatureF", temperature_f), fmt::arg("temperatureK", temperature_k)));
  }
  // Call parent update
  ALabel::update();
}

float waybar::modules::Temperature::getTemperature() {
  std::ifstream temp(file_path_);
  if (!temp.is_open()) {
    throw std::runtime_error("Can't open " + file_path_);
  }
  std::string line;
  if (temp.good()) {
    getline(temp, line);
  } else {
    temp.close();
    throw std::runtime_error("Can't read from " + file_path_);
  }
  temp.close();
  auto temperature_c = std::strtol(line.c_str(), nullptr, 10) / 1000.0;
  return temperature_c;
}

bool waybar::modules::Temperature::isWarning(uint16_t temperature_c) {
  return config_["warning-threshold"].isInt() &&
         temperature_c >= config_["warning-threshold"].asInt();
}

bool waybar::modules::Temperature::isCritical(uint16_t temperature_c) {
  return config_["critical-threshold"].isInt() &&
         temperature_c >= config_["critical-threshold"].asInt();
}
