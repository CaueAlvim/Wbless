# Wbless

#### Building from source

```bash
$ git clone https://github.com/Alexays/Waybar
$ cd Wbless
$ meson setup build
$ ninja -C build
$ ./build/waybar
# If you want to install it
$ ninja -C build install
$ waybar
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
