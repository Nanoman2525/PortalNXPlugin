#!/bin/bash
set -e

# Needed so that we force a recompilation if we switch target games
make clean

#-----------------------------------------------------------------

# Set your target game here:
# GAME_TYPE="-DPORTAL"  : Compiles for Portal
# GAME_TYPE="-DPORTAL2" : Compiles for Portal 2
GAME_TYPE="-DPORTAL2"

# Name of the produced .nro file (Overrides TARGET in the MakeFile)
PLUGIN_NAME="nx_plugin"

# Set your path to the Ryujinx main folder
RYUJINX_PATH=""

# Set your path to the nxtool executable
NXTOOL_PATH=""

#-----------------------------------------------------------------

# Ensure that PORTAL is set before proceeding, or else exit
if [ -z "$GAME_TYPE" ]; then
    echo "Error: PORTAL or PORTAL2 must be set for compilation!"
    exit 1
fi

# Make it available to Makefile
export PLUGIN_NAME

# Create the plugin
make GAME_TYPE="$GAME_TYPE"

# Set the relative path to the file
if [ "$GAME_TYPE" == "-DPORTAL2" ]; then
    FILE_PATH="./switch/portal2/launcher_main.nrr"
elif [ "$GAME_TYPE" == "-DPORTAL" ]; then
    FILE_PATH="./switch/portal/launcher_main.nrr"
fi

# Check if the file exists
if [ ! -f "$FILE_PATH" ]; then
    echo "File not found: $FILE_PATH"
    exit 1
fi

# Read the hex content from the file and remove spaces and newlines
FILE_HEX=$(xxd -p "$FILE_PATH" | tr -d '\n' | tr -d ' ')

# Increment byte at offset 0x344 (836 decimal)
offset=836
current_byte=${FILE_HEX:$((offset * 2)):2}
new_value=$((16#$current_byte + 1))
new_byte=$(printf "%02x" $((new_value % 256)))

# Update the hex string
FILE_HEX="${FILE_HEX:0:$((offset * 2))}$new_byte${FILE_HEX:$(((offset + 1) * 2))}"

# Add new bin's hash
len=${#FILE_HEX}
zero_offset=""

# Find the first non-zero byte from the end
for ((i=$len-2; i>=0; i-=2)); do
    if [ "${FILE_HEX:$i:2}" != "00" ]; then
        zero_offset=$((i + 2))
        break
    fi
done

# If zero sequence found, insert EXTRA_HASH
EXTRA_HASH=$("${NXTOOL_PATH}" "${PLUGIN_NAME}.nro" | grep Hash | tr -s ' ' | cut -d ' ' -f 2)

if [ -n "$zero_offset" ]; then
    FILE_HEX="${FILE_HEX:0:$zero_offset}${EXTRA_HASH}${FILE_HEX:$((zero_offset + ${#EXTRA_HASH}))}"
else
    echo "No sequence of zeros found."
    exit 1
fi

# Write the updated file
echo "$FILE_HEX" | xxd -r -p > launcher_main.nrr

# Copy it to a destination
if [[ "$GAME_TYPE" == "-DPORTAL" ]]; then
    echo "Copying to PORTAL destination..."
    mv -v launcher_main.nrr "${RYUJINX_PATH}/mods/contents/01007bb017812000/romfs/.nrr"
    cp -v "${PLUGIN_NAME}.nro" "${RYUJINX_PATH}/mods/contents/01007bb017812000/romfs/nro"
elif [[ "$GAME_TYPE" == "-DPORTAL2" ]]; then
    echo "Copying to PORTAL2 destination..."
    mv -v launcher_main.nrr "${RYUJINX_PATH}/mods/contents/0100abd01785c000/romfs/.nrr"
    cp -v "${PLUGIN_NAME}.nro" "${RYUJINX_PATH}/mods/contents/0100abd01785c000/romfs/nro"
else
    echo "No copy action taken."
fi
