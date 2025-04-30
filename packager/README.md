# Packager

A simple utility for packaging the Adobe Leftovers Remover application into distributable zip archives.

## Overview

The packager is a command-line utility written in C# that automates the process of creating release packages for the Adobe Leftovers Remover application. It works by:

1. Collecting release files from the Qt build output directory
2. Creating a versioned package folder
3. Generating a compressed zip file ready for distribution

## Requirements

- .NET 8.0 or higher
- Adobe Leftovers Remover solution with compiled binaries in the x64/Release folder

## Usage

```
packager.exe <version>
```

Where `<version>` is the version number you want to assign to the release package (e.g., 1.2.3).

### Examples

To create a version 1.0.0 release package:

```
packager.exe 1.0.0
```

This will generate a file named `adobe-leftovers-remover-v1.0.0.zip` in the solution root directory.

To display help information:

```
packager.exe -h
```
or
```
packager.exe -help
```

## How It Works

The packager performs the following steps:

1. Takes a version number as input
2. Locates the Qt release files in the `x64/Release` directory relative to the solution root
3. Creates a temporary staging folder named `adobe-leftovers-remover-v<version>`
4. Copies all files and subdirectories from the release directory to the staging folder
5. Compresses the staging folder into a zip file named `adobe-leftovers-remover-v<version>.zip`
6. Places the zip file in the solution root directory
7. Cleans up the temporary staging folder

## Building

The packager is built using standard .NET SDK commands:

```
dotnet build
```

For a release build:

```
dotnet publish -c Release -r win-x64 --self-contained
```

## License

This project is part of the Adobe Leftovers Remover solution and is licensed under the [MIT License](LICENSE).