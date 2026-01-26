# mpv-rich-presence

## Installation

Simply merge the contents of the distribution into your mpv config folder (e.g. `%APPDATA%/mpv` on Windows). Of course, the licenses/notices can (should) be omitted. Configure the plugin using the available options (see [§Configuration](#configuration)). By default, Rich Presence is initially off, so a `script-message-to rich_presence toggle` must be sent through the mpv console to toggle on Rich Presence.

## Configuration

### Options (`script-opts/rich-presence.conf`)
* `application_id=<id>` - the ID of the Discord application from the [Discord developer portal](https://discord.com/developers/applications)
* `on=<yes/no>` - initial setting of Rich Presence on/off

### Commands (`script-message-to rich_presence [cmd] [args]`)
* `toggle` - toggle Rich Presence on/off
* `on` - set Rich Presence on
* `off` - set Rich Presence off
* `application_id <id>` - set the ID of the Discord application from the [Discord developer portal](https://discord.com/developers/applications)

> [!TIP]
> I recommend adding in `input.conf` a binding like `D script-message-to rich_presence toggle` to easily toggle Rich Presence. Then, leave `on=no` in `rich-presence.conf`. This is so that Rich Presence is opt-in, and not everything you watch will be immediately shared.

## Building

> [!WARNING]
> Ideally this plugin should be cross-platform, but I haven't tested it on anything other than MSVC. At worst, only small changes should be required to get this building on other compilers/platforms.

> [!NOTE]
> Make sure to set the CMake variable `VCPKG_TARGET_TRIPLET` to the *static* version of your platform's triplet. For example on 64-bit Windows, it should be `VCPKG_TARGET_TRIPLET=x64-windows-static-md`. Otherwise, the build may depend on Boost's shared libraries, which is less portable for installation as an mpv plugin.

First, download the [Discord Social SDK](https://discord.com/developers/social-sdk). Unpack its contents into `thirdparty/discord_social_sdk`, such that `bin` is placed as `thirdparty/discord_social_sdk/bin`. Afterwards,
```bash
cmake --preset default
# Update VCPKG_TARGET_TRIPLET here, e.g.
# cmake -D VCPKG_TARGET_TRIPLET:STRING=x64-windows-static-md build
cmake --build build
cmake --build build --target INSTALL
```
If everything worked correctly, the complete binary distribution should be located in `install/mpv-rich-presence`.
