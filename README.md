# PortalNXPlugin

This is a base repository to build server plugins on the Nintendo Switch versions of Portal (v1.0.3) and Portal 2 (v1.0.3).

# Compiling

1. Have [devkitPro](https://devkitpro.org/wiki/Getting_Started) installed for the current platform.
2. Depending on the target game, run the `MakeFile` by doing...
   - ```make GAME_TYPE="-DPORTAL"``` (for Portal)
   - ```make GAME_TYPE="-DPORTAL2"``` (for Portal 2)
3. The plugin `.nro` will appear at the root.

# How loading it works

1. Portal and Portal 2 on Switch check a whitelist of file hashes in `romfs/.nrr/launcher_main.nrr`.
2. Generate the `.nro` plugin binary and use nxtool to get its hash.
3. At offset `0x344` in `launcher_main.nrr`, the number of whitelisted binaries is stored. Increment it by one.
4. There is also a `00` byte repitition that lasts to the end of the file. _Overwrite_ it with the plugin’s hash to whitelist it.
5. If using [NXSideLoader](https://github.com/masagrator/PortalNXSideLoader), place the modified `launcher_main.nrr` in the correct location and install the plugin in `romfs/nro/(plugin_name).nro`. Otherwise, repackage `game.zip` with the plugin and modified `launcher_main.nrr`.
6. Load the plugin with the `plugin_load` console command.

# Automating the process

- This will generate a `launcher_main.nrr` file for the newly compiled plugin.

1. Have [nxtool](https://github.com/dazjo/nxtool) compiled and ready to use.
2. Place the target game's original `romfs/.nrr/launcher_main.nrr` file in `switch/(game_name)/launcher_main.nrr`.
   - A modified version can be used to support multiple plugins.
3. Configure the parameters in `build.sh`.
4. Run `build.sh`.
