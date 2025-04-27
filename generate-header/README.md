# Generate-Header Tool

A utility for the Adobe Leftovers Remover project that generates C++ header files from configuration data.

## Overview

This tool processes a configuration file containing directories and registry keys related to Adobe products and generates a C++ header file (`data.h`) with QStringList literals. This header can then be included in the main Adobe Leftovers Remover application.

## Features

- Parses configuration files with directories and registry keys in a simple format
- Automatically escapes backslashes and quotes for C++ string literals
- Generates a ready-to-use C++ header file with Qt compatible QStringList definitions
- Simple command-line interface with help options

## Usage

```bash
generate-header [data_file]
```

### Parameters

- `data_file`: Path to the configuration file (optional, defaults to `data.txt` in the executable directory)

### Options

- `-h`, `-help`: Display help message and exit

## Configuration File Format

The configuration file should follow this format:

```
[Directories]
"C:/Program Files/Adobe"
"C:/Program Files/Common Files/Adobe"
"C:/Program Files (x86)/Adobe"
"C:/Program Files (x86)/Common Files/Adobe"
"C:/ProgramData/Adobe"
"%LOCALAPPDATA%/Adobe"
"%TEMP%/Adobe"
"%LOCALAPPDATA%/Low/Adobe"
"%APPDATA%/Adobe"
"..."

[RegistryLeys]
"HKEY_CURRENT_USER\\Software\\Adobe"
"HKEY_LOCAL_MACHINE\\SOFTWARE\\Adobe"
"..."
```

### Sections

- `[Directories]`: List of file system paths to be removed
- `[RegistryLeys]`: List of registry keys to be removed

Each entry must be enclosed in double quotes.

## Output

The tool generates a `data.h` file with the following structure:

```cpp
#pragma once

#include <QStringList>

// List of paths (directories)
inline const QStringList paths = {
    "C:\\Program Files\\Adobe\\Some Directory",
    "C:\\Users\\Username\\AppData\\Local\\Adobe",
};

// List of registry keys
inline const QStringList registryKeys = {
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Adobe",
    "HKEY_CURRENT_USER\\Software\\Adobe",
};
```

## Building

This project is built using Visual Studio and requires a C++11 compatible compiler.

## Integration

This tool is part of the Adobe Leftovers Remover project. The generated `data.h` file should be included in the main project to provide the lists of directories and registry keys to clean up.

## License

[MIT](LICENSE).