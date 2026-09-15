# timesync-dsi

NTP Time synchronization tool for Nintendo DSi

## Requirement

- devkitARM
  - Get started by reading [official guide](https://devkitpro.org/wiki/Getting_Started)

## Usage

**CHANGE the offset for your timezone THEN BUILD**.

The default offset is JST.
You must modify and build if you live outside Japan.

Open `arm9/source/main.c` and locate the line like below:

```c
#define OFFSET_SECONDS 9 * 60 * 60 // JST offset in seconds
```

Then, change the value for your location.

As you might notice, the server address and port are also configurable.

## Memorandum

How to set up VSCode C/C++ Extension IntelliSense:

1. Run `bear -- make`
2. Create `.vscode/c_cpp_properties.json`
3. Write something like _JSON below_
   - The `"compileCommands"` line seems important
4. Will be OK

```json
{
  "configurations": [
    {
      "name": "Linux",
      "includePath": ["${workspaceFolder}/**"],
      "compileCommands": ["${workspaceFolder}/compile_commands.json"],
      "defines": ["__NDS__"],
      "compilerPath": "/opt/devkitpro/devkitARM/bin/arm-none-eabi-gcc"
    }
  ],
  "version": 4
}
```
