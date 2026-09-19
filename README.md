# timesync-dsi

NTP Time synchronization tool for Nintendo DSi

## Build Requirement

- devkitARM
  - Get started by reading [official guide](https://devkitpro.org/wiki/Getting_Started)

## Usage

1. Get the NDS file from [Release page](https://github.com/inucat/timesync-dsi/releases).
2. Put a config file `timesync-dsi.conf` in the root of your SD card.
3. Write values for your location in it (See example below).

## Example config

```
server=time.google.com
port=123
offset_minutes=0
```

- `server` is NTP server address. Max 255 length.
- `port` is NTP server port.
- `offset_minutes` is UTC offset for your timezone as signed integer.

Omit any lines and the default values will be used.

## Memorandum

How to set up VSCode C/C++ Extension IntelliSense:

1. Run `bear -- make` or `compiledb make`
2. Create `.vscode/c_cpp_properties.json`
3. Write something like _JSON below_
   - The `"compileCommands"` and `"compilerPath"` are important
4. Will be OK

```json
{
  "configurations": [
    {
      "name": "Linux",
      "includePath": ["${workspaceFolder}/**"],
      "compileCommands": ["${workspaceFolder}/compile_commands.json"],
      "compilerPath": "/opt/devkitpro/devkitARM/bin/arm-none-eabi-gcc"
    }
  ],
  "version": 4
}
```
