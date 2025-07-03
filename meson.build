project(
    'wbless', 'cpp', 'c',
    version: '0.13.0',
    license: 'MIT',
    meson_version: '>= 0.59.0',
    default_options : [
        'cpp_std=c++20',
        'buildtype=release',
        'default_library=static'
    ],
)

compiler = meson.get_compiler('cpp')

cpp_args = []
cpp_link_args = []

if get_option('libcxx')
    cpp_args += ['-stdlib=libc++']
    cpp_link_args += ['-stdlib=libc++', '-lc++abi']
endif

if compiler.has_link_argument('-lc++fs')
    cpp_link_args += ['-lc++fs']
elif compiler.has_link_argument('-lstdc++fs')
    cpp_link_args += ['-lstdc++fs']
endif

git = find_program('git', native: true, required: false)

if not git.found()
    add_project_arguments('-DVERSION="@0@"'.format(meson.project_version()), language: 'cpp')
else
    git_path = run_command(git, 'rev-parse', '--show-toplevel', check: false).stdout().strip()
    if meson.project_source_root() == git_path
        git_commit_hash = run_command(git, 'describe', '--always', '--tags', check: false).stdout().strip()
        git_branch = run_command(git, 'rev-parse', '--abbrev-ref', 'HEAD', check: false).stdout().strip()
        version = '"@0@ (branch \'@1@\')"'.format(git_commit_hash, git_branch)
        add_project_arguments('-DVERSION=@0@'.format(version), language: 'cpp')
    else
        add_project_arguments('-DVERSION="@0@"'.format(meson.project_version()), language: 'cpp')
    endif
endif

code = '''
#include <langinfo.h>
#include <locale.h>
int main(int argc, char** argv) {
    locale_t locale = newlocale(LC_ALL, "en_US.UTF-8", nullptr);
    char* str;
    str = nl_langinfo_l(_NL_TIME_WEEK_1STDAY, locale);
    str = nl_langinfo_l(_NL_TIME_FIRST_WEEKDAY, locale);
    freelocale(locale);
    return 0;
}
'''
if compiler.links(code, name : 'nl_langinfo with _NL_TIME_WEEK_1STDAY, _NL_TIME_FIRST_WEEKDAY')
    add_project_arguments('-DHAVE_LANGINFO_1STDAY', language: 'cpp')
endif

add_global_arguments(cpp_args, language : 'cpp')
add_global_link_arguments(cpp_link_args, language : 'cpp')

is_linux = host_machine.system() == 'linux'

thread_dep = dependency('threads')
fmt = dependency('fmt', version : ['>=8.1.1'], fallback : ['fmt', 'fmt_dep'])
spdlog = dependency('spdlog', version : ['>=1.10.0'], fallback : ['spdlog', 'spdlog_dep'], default_options : ['external_fmt=enabled', 'std_format=disabled', 'tests=disabled'])
wayland_client = dependency('wayland-client')
wayland_cursor = dependency('wayland-cursor')
wayland_protos = dependency('wayland-protocols')
gtkmm = dependency('gtkmm-3.0', version : ['>=3.22.0'])
dbusmenu_gtk = dependency('dbusmenu-gtk3-0.4', required: get_option('dbusmenu-gtk'))
giounix = dependency('gio-unix-2.0')
jsoncpp = dependency('jsoncpp', version : ['>=1.9.2'], fallback : ['jsoncpp', 'jsoncpp_dep'])
sigcpp = dependency('sigc++-2.0')
libinput = dependency('libinput', required: get_option('libinput'))
libnl = dependency('libnl-3.0', required: get_option('libnl'))
libnlgen = dependency('libnl-genl-3.0', required: get_option('libnl'))
pipewire = dependency('libpipewire-0.3', required: get_option('pipewire'))
libudev = dependency('libudev', required: get_option('libudev'))
libevdev = dependency('libevdev', required: get_option('libevdev'))
xkbregistry = dependency('xkbregistry')
libwireplumber = dependency('wireplumber-0.5', required: get_option('wireplumber'))

gtk_layer_shell = dependency('gtk-layer-shell-0', version: ['>=0.9.0'],
        default_options: ['introspection=false', 'vapi=false'],
        fallback: ['gtk-layer-shell', 'gtk_layer_shell'])

cpp_lib_chrono = compiler.compute_int('__cpp_lib_chrono', prefix : '#include <chrono>')
have_chrono_timezones = cpp_lib_chrono >= 201611

if have_chrono_timezones
   code = '''
#include <chrono>
using namespace std::chrono;
int main(int argc, char** argv) {
   const time_zone* tz;
   return 0;
}
'''
   if not compiler.links(code)
      have_chrono_timezones = false
   endif
endif

if have_chrono_timezones
  tz_dep = declare_dependency()
else
  tz_dep = dependency('date',
      required: false,
      default_options : [ 'use_system_tzdb=true' ],
      modules : [ 'date::date', 'date::date-tz' ],
      fallback: [ 'date', 'tz_dep' ])
endif

prefix = get_option('prefix')
sysconfdir = get_option('sysconfdir')
conf_data = configuration_data()
conf_data.set('prefix', prefix)

add_project_arguments('-DSYSCONFDIR="@0@"'.format(prefix / sysconfdir), language : 'cpp')

src_files = files(
    'src/factory.cpp',
    'src/AModule.cpp',
    'src/ALabel.cpp',
    'src/AIconLabel.cpp',
    'src/modules/custom.cpp',
    'src/modules/disk.cpp',
    'src/modules/load.cpp',
    'src/modules/temperature.cpp',
    'src/modules/user.cpp',
    'src/main.cpp',
    'src/bar.cpp',
    'src/client.cpp',
    'src/config.cpp',
    'src/group.cpp',
    'src/util/portal.cpp',
    'src/util/enum.cpp',
    'src/util/prepare_for_sleep.cpp',
    'src/util/ustring_clen.cpp',
    'src/util/gtk_icon.cpp',
    'src/util/regex_collection.cpp',
    'src/util/css_reload_helper.cpp'
)

inc_dirs = ['include']

if is_linux
    add_project_arguments('-DHAVE_CPU_LINUX', language: 'cpp')
    add_project_arguments('-DHAVE_MEMORY_LINUX', language: 'cpp')
    src_files += files(
        'src/modules/cpu.cpp',
        'src/modules/cpu_frequency/common.cpp',
        'src/modules/cpu_frequency/linux.cpp',
        'src/modules/cpu_usage/common.cpp',
        'src/modules/cpu_usage/linux.cpp',
        'src/modules/memory/common.cpp',
        'src/modules/memory/linux.cpp',
    )
endif

if true
    add_project_arguments('-DHAVE_HYPRLAND', language: 'cpp')
    src_files += files(
        'src/modules/hyprland/backend.cpp',
        'src/modules/hyprland/workspace.cpp',
        'src/modules/hyprland/workspaces.cpp',
        'src/modules/hyprland/windowcreationpayload.cpp',
    )
endif

if get_option('niri')
    add_project_arguments('-DHAVE_NIRI', language: 'cpp')
    src_files += files(
        'src/modules/niri/backend.cpp',
        'src/modules/niri/workspaces.cpp',
    )
endif

if libnl.found() and libnlgen.found()
    add_project_arguments('-DHAVE_LIBNL', language: 'cpp')
    src_files += files('src/modules/network.cpp')
endif

if pipewire.found()
    add_project_arguments('-DHAVE_PIPEWIRE', language: 'cpp')
    src_files += files(
        'src/modules/privacy/privacy.cpp',
        'src/modules/privacy/privacy_item.cpp',
        'src/util/pipewire/pipewire_backend.cpp',
        'src/util/pipewire/privacy_node_info.cpp',
    )
endif

if libwireplumber.found()
    add_project_arguments('-DHAVE_LIBWIREPLUMBER', language: 'cpp')
    src_files += files('src/modules/wireplumber.cpp')
endif

if dbusmenu_gtk.found()
    add_project_arguments('-DHAVE_DBUSMENU', language: 'cpp')
    src_files += files(
        'src/modules/sni/tray.cpp',
        'src/modules/sni/watcher.cpp',
        'src/modules/sni/host.cpp',
        'src/modules/sni/item.cpp'
    )
endif

if libevdev.found() and is_linux and libinput.found()
    add_project_arguments('-DHAVE_LIBEVDEV', language: 'cpp')
    add_project_arguments('-DHAVE_LIBINPUT', language: 'cpp')
endif

if have_chrono_timezones
    add_project_arguments('-DHAVE_CHRONO_TIMEZONES', language: 'cpp')
    src_files += files('src/modules/clock.cpp')
elif tz_dep.found()
    add_project_arguments('-DHAVE_LIBDATE', language: 'cpp')
    src_files += files('src/modules/clock.cpp')
else
    src_files += files('src/modules/simpleclock.cpp')
endif

subdir('protocol')

app_resources = []

executable(
    'waybar',
    [src_files, app_resources],
    dependencies: [
        thread_dep,
        client_protos,
        wayland_client,
        fmt,
        spdlog,
        sigcpp,
        jsoncpp,
        wayland_cursor,
        gtkmm,
        dbusmenu_gtk,
        giounix,
        libinput,
        libnl,
        libnlgen,
        pipewire,
        libwireplumber,
        libudev,
        libevdev,
        gtk_layer_shell,
        tz_dep,
		xkbregistry,
    ],
    include_directories: inc_dirs,
    install: true,
)
