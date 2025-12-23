#!/bin/bash
set -e

#-----------------------------------------------------------------

# Name of the produced .nro file (Overrides TARGET in the MakeFile)
PLUGIN_NAME="nx_plugin"

# Set your path to the Ryujinx main folder
RYUJINX_PATH=""

# Set your path to the nxtool executable
NXTOOL_PATH=""

#-----------------------------------------------------------------

# Make it available to Makefile
export PLUGIN_NAME

# Create the plugin
make
MAKE_EXIT_CODE=$?

# Function to patch and copy for a given game
patch_and_copy()
{
    local GAME_NAME="$1"
    local GAME_ID="$2"
    local FILE_PATH="./switch/${GAME_NAME}/launcher_main.nrr"

    if [ ! -f "$FILE_PATH" ]; then
        echo "File not found: $FILE_PATH"
        exit 1
    fi

    echo "------------------------------------"
    echo "Patching ${FILE_PATH}..."

    # Read the hex content from the file and remove spaces and newlines
    FILE_HEX=$(xxd -p "$FILE_PATH" | tr -d '\n' | tr -d ' ')

    # Increment byte at offset 0x344 (836 decimal)
    offset=836
    current_byte=${FILE_HEX:$((offset * 2)):2}
    new_value=$((16#$current_byte + 1))
    new_byte=$(printf "%02x" $((new_value % 256)))

    # Update the hex string
    FILE_HEX="${FILE_HEX:0:$((offset * 2))}$new_byte${FILE_HEX:$(((offset + 1) * 2))}"

    EXTRA_HASH=$("${NXTOOL_PATH}" "${PLUGIN_NAME}.nro" | grep Hash | tr -s ' ' | cut -d ' ' -f 2)

    # Find the first non-zero byte from the end
    len=${#FILE_HEX}
    zero_offset=""
    for ((i=$len-2; i>=0; i-=2)); do
        if [ "${FILE_HEX:$i:2}" != "00" ]; then
            zero_offset=$((i + 2))
            break
        fi
    done

    # If zero sequence found, insert EXTRA_HASH
    if [ -n "$zero_offset" ]; then
        FILE_HEX="${FILE_HEX:0:$zero_offset}${EXTRA_HASH}${FILE_HEX:$((zero_offset + ${#EXTRA_HASH}))}"
    else
        echo "No sequence of zeros found."
        exit 1
    fi

    # Write the updated file
    echo "$FILE_HEX" | xxd -r -p > launcher_main.nrr

    # Copy it to a destination
    echo "Copying to $GAME_NAME destination..."
    DEST_PATH="${RYUJINX_PATH}/mods/contents/${GAME_ID}/romfs"
    mv -v launcher_main.nrr "${DEST_PATH}/.nrr"
    cp -v "${PLUGIN_NAME}.nro" "${DEST_PATH}/nro"
}

# Only copy if the build was successful
if [ $MAKE_EXIT_CODE -eq 0 ] && ! make -q; then
    patch_and_copy "portal"  "01007bb017812000"
    patch_and_copy "portal2" "0100abd01785c000"
fi
