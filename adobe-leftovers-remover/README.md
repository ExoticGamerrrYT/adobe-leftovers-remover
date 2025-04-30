# Adobe Leftovers Remover

> **COMMUNITY HELP NEEDED**: We're looking for contributions! If you know of any paths or registry keys created by Adobe software that aren't already covered by this tool, please submit them through GitHub issues or pull requests.

A utility for removing leftover Adobe software components, directories, and registry entries from your system after uninstallation.

## Overview

Adobe products often leave behind numerous files, folders, and registry entries even after uninstallation. This tool helps you identify and remove these leftover components to completely clean your system of Adobe software remnants.

## Features

- Clean identification and listing of Adobe-related directories and registry keys
- User-friendly graphical interface with checkboxes for selective cleanup
- Batch selection/deselection options for convenience
- Removal process
- Configurable through external data files

## Screenshots

[Screenshots will be added in a future update]

## Requirements

- Windows 10/11

## Installation

### Pre-built Binaries

1. ~~Download the latest release from the [Releases](https://github.com/yourusername/adobe-leftovers-remover/releases) page~~
2. ~~Extract the ZIP file to a location of your choice~~
3. ~~Run `adobe-leftovers-remover.exe`~~ *NOT YET*

### Building from Source

> **NOTE: You must install Qt 6.9 (and Qt VS Tools extension)**.

This project uses Qt and can be built with Visual Studio:

1. Clone the repository
2. Open `adobe-leftovers-remover.sln` in Visual Studio
3. Select the desired build configuration (Debug/Release)
4. Build the solution

#### windeployqt command
```
windeployqt --release --no-translations --no-system-d3d-compiler --no-system-dxc-compiler --no-opengl-sw --skip-plugin-types generic,tls,imageformats,iconengines <.exe file>
```

## Usage

1. Launch the application
2. Review the listed paths and registry keys
3. Select the items you want to remove (use "Select All" buttons for convenience)
4. Click "Delete Selected" to remove the selected items
5. Restart your computer to complete the cleanup process

## Configuration

The application uses a configuration file (`data.txt`) to define the directories and registry keys to target. This file is processed by the included `generate-header` tool to create the C++ header used by the main application.

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

[RegistryLeys]
"HKEY_CURRENT_USER\\Software\\Adobe"
"HKEY_LOCAL_MACHINE\\SOFTWARE\\Adobe"
```

## Project Structure

- `adobe-leftovers-remover/`: Main application project
  - `src/`: Source code files
    - `main.cpp`: Main application code
    - `data.h`: Generated header with paths and registry keys

## Warning

Use this tool with caution. Always back up your registry before making changes. This tool modifies your system and removes files and registry entries. While designed to be safe, there is always a risk when removing system components.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## To-do

- [ ] Remove shortcuts (Desktop/Start Menu).
- [ ] Remove open with shortcuts.
- [x] Add restart PC message after removal.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [Qt Framework](https://www.qt.io/) for the GUI implementation
- All contributors who have helped to improve this tool