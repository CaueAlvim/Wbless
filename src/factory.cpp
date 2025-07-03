#include "factory.hpp"

#include "bar.hpp"

#if defined(HAVE_CHRONO_TIMEZONES) || defined(HAVE_LIBDATE)
#include "modules/clock.hpp"
#else
#include "modules/simpleclock.hpp"
#endif
#ifdef HAVE_HYPRLAND
#include "modules/hyprland/workspaces.hpp"
#endif
#ifdef HAVE_NIRI
#include "modules/niri/workspaces.hpp"
#endif
#if defined(HAVE_CPU_LINUX)
#include "modules/cpu.hpp"
#include "modules/cpu_frequency.hpp"
#include "modules/cpu_usage.hpp"
#include "modules/load.hpp"
#endif
#if defined(HAVE_MEMORY_LINUX)
#include "modules/memory.hpp"
#endif
#include "modules/disk.hpp"
#ifdef HAVE_DBUSMENU
#include "modules/sni/tray.hpp"
#endif
#ifdef HAVE_LIBNL
#include "modules/network.hpp"
#endif
#ifdef HAVE_PIPEWIRE
#include "modules/privacy/privacy.hpp"
#endif
#ifdef HAVE_LIBWIREPLUMBER
#include "modules/wireplumber.hpp"
#endif
#include "modules/custom.hpp"
#include "modules/temperature.hpp"
#include "modules/user.hpp"

waybar::Factory::Factory(const Bar& bar, const Json::Value& config) : bar_(bar), config_(config) {}

waybar::AModule* waybar::Factory::makeModule(const std::string& name,
                                             const std::string& pos) const {
  try {
    auto hash_pos = name.find('#');
    auto ref = name.substr(0, hash_pos);
    auto id = hash_pos != std::string::npos ? name.substr(hash_pos + 1) : "";
#ifdef HAVE_PIPEWIRE
    if (ref == "privacy") {
      return new waybar::modules::privacy::Privacy(id, config_[name], bar_.orientation, pos);
    }
#endif
#ifdef HAVE_HYPRLAND
    if (ref == "hyprland/workspaces") {
      return new waybar::modules::hyprland::Workspaces(id, bar_, config_[name]);
    }
#endif
#ifdef HAVE_NIRI
    if (ref == "niri/workspaces") {
      return new waybar::modules::niri::Workspaces(id, bar_, config_[name]);
    }
#endif
#if defined(HAVE_MEMORY_LINUX)
    if (ref == "memory") {
      return new waybar::modules::Memory(id, config_[name]);
    }
#endif
#if defined(HAVE_CPU_LINUX)
    if (ref == "cpu") {
      return new waybar::modules::Cpu(id, config_[name]);
    }
    if (ref == "cpu_frequency") {
      return new waybar::modules::CpuFrequency(id, config_[name]);
    }
    if (ref == "cpu_usage") {
      return new waybar::modules::CpuUsage(id, config_[name]);
    }
    if (ref == "load") {
      return new waybar::modules::Load(id, config_[name]);
    }
#endif
    if (ref == "clock") {
      return new waybar::modules::Clock(id, config_[name]);
    }
    if (ref == "user") {
      return new waybar::modules::User(id, config_[name]);
    }
    if (ref == "disk") {
      return new waybar::modules::Disk(id, config_[name]);
    }
#ifdef HAVE_DBUSMENU
    if (ref == "tray") {
      return new waybar::modules::SNI::Tray(id, bar_, config_[name]);
    }
#endif
#ifdef HAVE_LIBNL
    if (ref == "network") {
      return new waybar::modules::Network(id, config_[name]);
    }
#endif
#ifdef HAVE_LIBWIREPLUMBER
    if (ref == "wireplumber") {
      return new waybar::modules::Wireplumber(id, config_[name]);
    }
#endif
    if (ref == "temperature") {
      return new waybar::modules::Temperature(id, config_[name]);
    }
    if (ref.compare(0, 7, "custom/") == 0 && ref.size() > 7) {
      return new waybar::modules::Custom(ref.substr(7), id, config_[name], bar_.output->name);
    }
  } catch (const std::exception& e) {
    auto err = fmt::format("Disabling module \"{}\", {}", name, e.what());
    throw std::runtime_error(err);
  } catch (...) {
    auto err = fmt::format("Disabling module \"{}\", Unknown reason", name);
    throw std::runtime_error(err);
  }
  throw std::runtime_error("Unknown module: " + name);
}
