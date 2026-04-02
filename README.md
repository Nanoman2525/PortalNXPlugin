# PortalNXPlugin

Allows you to build and load [server plugins](https://developer.valvesoftware.com/wiki/Server_plugins) on the Nintendo Switch versions of **Portal (v1.0.3)** and **Portal 2 (v1.0.3)**.

> [!CAUTION]
> I have no idea if using this online would get a Switch banned. Stick with EmuMMC!

# How loading it works

- By default, Portal and Portal 2 on Switch protect against loading random `.nro` modules. (Basically `.dll`/`.so` files for Switch)
   - This is done by checking against a whitelist of **SHA-256** file hashes stored in `romfs/.nrr/launcher_main.nrr`.
- In order to bypass this, we modify that file by...
   - incrementing the number of whitelisted binaries stored at offset `0x344` by one.
   - overwriting the start of the `00` byte repetition (that lasts to the end of the file) with the plugin's hash, thereby whitelisting the plugin.
- Interestingly, both Switch games only load modules only through one directory, `romfs/nro/`, so we pack the plugin here.

# Commands added

Note that all commands are **off** by default when the plugin is loaded.

|Name|Default|Description|Supports|
|---|---|---|---|
|nx_cvar_unhide_all|cmd|Unhide all FCVAR_HIDDEN and FCVAR_DEVELOPMENTONLY convars|Both|
|nx_enable_keyboard_support|0|On Switch, enables keyboard input reading.|Both|
|nx_enable_ladders|0|Enables ladder usage for all players.|Portal 2|
|nx_enable_mouse_support|0|On Switch, enables mouse input reading.|Both|
|nx_enable_printing_in_console|0|Fixes text output in the console not appearing by default.|Portal|
|nx_enable_touchscreen_support|0|On Switch, enables touchscreen input reading.|Both|
|nx_max_players_override|0|1-33: Max player count when starting a MP server. Otherwise: Game decides.|Portal 2|
|nx_open_on_screen_keyboard|cmd|Execute commands via the on-screen keyboard.|Both|
|nx_toggle_autojump|cmd|Toggles the autojump ability for all players.|Both|
|nx_toggle_coop_loading_dots|cmd|Shows the coop loading progress on map transitions.|Portal 2|
|nx_toggle_floor_reportals|cmd|Toggles the floor reportal ability for all players.|Portal 2|
|nx_toggle_force_max_fps|cmd|Forces r_dynres_enable and nvn_swap_interval to 1. (Won't be 30 FPS in splitscreen)|Portal 2|
|nx_toggle_loading_orange_dots|cmd|Fixes the orange dots not showing when fully loaded into maps.|Portal 2|
|nx_toggle_menu_controller_button_visibility|cmd|Toggles the visibility of the menu game console helpers on screen.|Portal 2|
|nx_update_footer_buttons|cmd|Run init logic missing from the game if needed and update the menu footer.|Portal 2|

# Building

> [!NOTE]
> It's highly recommended to use [PortalNXSideLoader](https://github.com/masagrator/PortalNXSideLoader), as the build steps are optimized for it. If you're not using this, you will need to mimic the paths and manually repackage `game.zip` after each compilation.

1. Install [devkitPro](https://devkitpro.org/wiki/Getting_Started) for the current platform. (Required for Switch dev)
2. Place the target game's original `romfs/.nrr/launcher_main.nrr` file in `switch/(game_name)/launcher_main.nrr`.
   - A modified version can be used to support multiple plugins.
3. Configure the parameters at the top in `build.sh`.
4. Run `build.sh`.
   - Creates the plugin `.nro` at the root.
   - Creates the modified `launcher_main.nrr` files for both games.
   - Automatically moves these to the correct locations.

# Hooking it up to the game

This assumes you have already have a plugin and its associating `launcher_main.nrr`. (Completed the building step)

1. Create an `autoexec.cfg` file and configure it:
   - Include the `plugin_load (plugin_name)` command first.
   - Include any other [commands](https://developer.valvesoftware.com/wiki/List_of_Portal_2_console_commands_and_variables) after that you'd like to run automatically when launching the game.
   - Example contents:
      ```
      // Load the plugin
      plugin_load "nx_plugin"

      // Automatically enable some custom features
      nx_cvar_unhide_all
      nx_enable_printing_in_console "1"
      nx_enable_keyboard_support "1"
      nx_enable_mouse_support "1"
      nx_enable_touchscreen_support "1"
      ```
2. Exclusive to Portal 2, the developers left in `-nomouse` in `romfs/nxcontent/rom_boot_params.txt`
   - If you specifically want the mouse support to work for this game, dump the game's original file and remove that argument.
3. Finally, make sure that all files are placed in these relative directories to your install:
   - The plugin: `romfs/nro/(plugin_name).nro`
   - launcher_main.nrr: `romfs/.nrr/launcher_main.nrr`
   - autoexec.cfg: `romfs/portal/cfg/autoexec.cfg` for Portal, `romfs/portal2/cfg/autoexec.cfg` for Portal 2.
   - rom_boot_params.txt: `romfs/nxcontent/rom_boot_params.txt`
4. If you are not using [PortalNXSideLoader](https://github.com/masagrator/PortalNXSideLoader), repackage your archive with the new files into `game.zip`.
5. Run the game.
