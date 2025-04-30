# Adobe Leftovers Remover

This repository contains a set of tools to help remove leftover Adobe software components, directories, and registry entries from your system after uninstallation.

> **COMMUNITY HELP NEEDED**: We're looking for contributions! If you know of any paths or registry keys created by Adobe software that aren't already covered by this tool, please submit them through GitHub issues or pull requests.

## How to contribute with paths or registry keys created by Adobe software

1. Open a new issue on the GitHub repository
2. Provide the paths or registry keys you've discovered
3. Include information about which Adobe software created them

## Project Structure

This repository contains multiple projects, each with its own specific purpose:

- **adobe-leftovers-remover**: Main application for removing Adobe leftovers from your system
- **generate-header**: Utility tool that generates C++ header files from configuration data
- **packager**: .NET-based utility that creates release packages for distribution

## Documentation

Each project has its own README file with detailed information:

- For information about the main application, see [adobe-leftovers-remover/README.md](adobe-leftovers-remover/README.md)
- For information about the header generator tool, see [generate-header/README.md](generate-header/README.md)

## Packaging Releases

The packager utility is used to create ZIP distribution packages of the application. It works by:

1. Taking a version number as input
2. Copying files from the Qt release directory (`x64/Release`)
3. Creating a ZIP file named `adobe-leftovers-remover-v<version>.zip`

Usage:
```
packager.exe <version>
```

For example, to create a version 1.2.3 release package:
```
packager.exe 1.2.3
```

## License

This project is licensed under the MIT License - see the LICENSE file in each project directory for details.