# Arduino
Arduino experiments using PlatformIO and VisualStudio Code

## PlatformIO

I use [PlatformIO](https://platformio.org/platformio-ide) and [VS code](https://code.visualstudio.com/) with the Arduino framework under Windows 10. 
All examples are part of a single PlatformIO workspace. 
It should also be possible to open the individual examples/folders directly in the Arduino IDE.

To make Intellisense work, I explicitely include "Arduino.h".

### ini file

My platformio.ini files support the Arduino-style folder structure. Here is an example from the Blink example:

```
[platformio]
src_dir = .

[env:uno]
platform = atmelavr
board = uno
framework = arduino
monitor_speed = 115200
```

### ENV variables

I use some environment variables for global folders, instead of defining them in every ini file:

| ENV variable | path |
| --- | --- |
| PLATFORMIO_LIB_EXTRA_DIRS | e.g. c:\pio\extra_libs |
| PLATFORMIO_BUILD_DIR | e.g. c:\temp\pio_build |
| PLATFORMIO_LIBDEPS_DIR | e.g. c:\temp\pio_libs|

Alternatively, one could set those directories directly in the platformio.ini file:
```
lib_extra_dirs = c:\pio\extra_libs
build_dir      = c:\temp\pio_build
libdeps_dir    = c:\temp\pio_libs
```
