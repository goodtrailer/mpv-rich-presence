# mpv-rich-presence

## Installation

Folder structure, assuming built & installed correctly:
* mpv config folder (e.g. `%APPDATA%/mpv`)
    * `scripts`
        * `rich-presence-conf.lua`
        * `rich-presence.dll/so/dylib`
    * `script-opts`
        * `rich-presence.conf`
* mpv install folder (e.g. `%LOCALAPPDATA%/Programs/mpv`)
    * `discord_partner_sdk.dll/so/dylib`

Note that Discord Social SDK needs to be downloaded separately from Discord officially (it is not OSS), and the shared library needs to be placed somewhere in the operating system's DLL/SO/DYLIB search path (e.g. next to `mpv.exe` on Windows). Also, after downloading the SDK, there is a CMake cache entry `DISCORD_SOCIAL_SDK_DIR` that needs to be configured for building.

A Discord application needs to be created using the [Discord developer portal](https://discord.com/developers/applications) for this to work (Discord Social SDK requires an application ID for Rich Presence). The only field that is used by this plugin (indirectly) is the application icon. Put the application ID in `script-opts/rich-presence.conf`.

## Configuration

### Options (`script-opts/rich-presence.conf`)
* `application_id=<id>`: the ID of the Discord application from the [Discord developer portal](https://discord.com/developers/applications)
* `on=<yes/no>`: initial setting of Rich Presence on/off

### Commands (`script-message-to rich_presence [cmd] [args]`)
* `toggle`: toggle Rich Presence on/off
* `on`: set Rich Presence on
* `off`: set Rich Presence off
* `application_id <id>`: set the ID of the Discord application from the [Discord developer portal](https://discord.com/developers/applications)
