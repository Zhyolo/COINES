# Generating static (or) shared COINES library

## PC

### Static libary with GCC
  - Make sure you have installed GCC toolchain (TDM/MinGW/Cygwin/Linux)
  - Use `mingw32-make TARGET=PC`
  - The resulting library `libcoines-pc.a` and `coines.h` can be separately used in a project.

To link `application.c` with COINES library, use the below command

```
$ gcc application.c -I . -L . -lcoines-pc -lsetupapi
```
For Linux/macOS, replace `-lsetupapi` by `-lusb-1.0`

-------------------------------------------------------------------------------

### Static/shared library with GCC/Clang/MSVC etc.,
- Windows - Get cmake from https://cmake.org/
- Ubuntu

    ```
    $ sudo apt install cmake
    ```
- macOS

    ```
    brew install cmake
    ```

```
$ cd coines_api/pc
$ mkdir build
$ cd build
$ cmake ..
```

(Execute `cmake` in *Developer Prompt for VS 201x* in case of MSVC)

#### Visual Studio

```
$ msbuild coines_api.sln
```

Find `coines-pc.lib` and `coines.dll` in coines_api/pc folder

#### MinGW Makefiles

```
$ mingw32-make
```

Find `libcoines-pc.a` and `libcoines.dll` in coines_api/pc folder

#### Linux/macOS

```
$ make
```

Find `libcoines-pc.a` and `libcoines.so`/`libcoines.dylib` (macOS) in `coines_api/pc` folder

-------------------------------------------------------------------------------

## APP2.0/APP3.0 MCU:

- Make sure you have installed the [GNU ARM Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
- Use `mingw32-make TARGET=MCU_APP20` (or) `mingw32-make TARGET=MCU_APP30`
- The resulting library `libcoines-mcu_app20.a` (or) `libcoines-mcu_app30.a` 
  and `coines.h` can be separately used in a project.

#### NOTE 
- `libcoines-mcu_app20.a` and `libcoines-mcu_app30.a` have `printf` integration with USB CDC ACM. 
- File handling functions (`fopen`,`fclose`,`fprintf`,etc.,) which can be used on the APP3.0 flash memory are also integrated with `libcoines-mcu_app30.a`

-------------------------------------------------------------------------------

Cygwin/Linux users can use `make` instead of `mingw32-make`
