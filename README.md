# Wbless

#### Building from source

```bash
$ meson setup build
$ ninja -C build
# Put the built executable at /usr/local/bin
# ???
# Profit
```

**Dependencies**

```
gtkmm3
jsoncpp
libsigc++
fmt
wayland
chrono-date
spdlog
libgtk-3-dev [gtk-layer-shell]
gobject-introspection [gtk-layer-shell]
libgirepository1.0-dev [gtk-layer-shell]
libnl [Network module]
libappindicator-gtk3 [Tray module]
libdbusmenu-gtk3 [Tray module]
xkbregistry
```

**Build dependencies**

```
cmake
meson
wayland-protocols
```

**Current checksum**
```
9f2d6aaec63916aaeae56b18a507eb9046497dcd30d337dd398f24773ddb9b5a  waybar
```
