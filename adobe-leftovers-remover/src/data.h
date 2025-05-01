#pragma once
#include <QStringList>

inline const QStringList paths = {
    "C:/Program Files/Adobe",
    "C:/Program Files/Common Files/Adobe",
    "C:/Program Files (x86)/Adobe",
    "C:/Program Files (x86)/Common Files/Adobe",
    "C:/ProgramData/Adobe",
    "%LOCALAPPDATA%/Adobe",
    "%LOCALAPPDATA%/Low/Adobe",
    "%APPDATA%/LocalLow/Adobe",
    "%APPDATA%/Adobe",
    "%TEMP%/Adobe",
    "%USERPROFILE%/Documents/Adobe",
    "%ALLUSERSPROFILE%/Adobe",
    "%ProgramData%/Adobe/SLStore",
    "%ProgramData%/Adobe/SLCache",
    "%LOCALAPPDATA%/Adobe/Creative Cloud Libraries",
    "%LOCALAPPDATA%/Adobe/Creative Cloud Desktop",
    "%APPDATA%/Adobe/Creative Cloud Libraries",
    "%APPDATA%/Adobe/CoreSync",
    "%APPDATA%/Adobe/CCX Welcome",
    "%APPDATA%/Adobe/OOBE"
};

inline const QStringList registryKeys = {
    "HKEY_CURRENT_USER\\Software\\Adobe",
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Adobe",
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Adobe",
    "HKEY_CLASSES_ROOT\\Adobe",
    "HKEY_CURRENT_USER\\Software\\Adobe\\Photoshop",
    "HKEY_CURRENT_USER\\Software\\Adobe\\Acrobat Reader",
    "HKEY_CURRENT_USER\\Software\\Adobe\\After Effects",
    "HKEY_CURRENT_USER\\Software\\Adobe\\Common",
    "HKEY_CURRENT_USER\\Software\\Adobe\\Premiere Pro",
    "HKEY_CURRENT_USER\\Software\\Adobe\\Creative Cloud",
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Adobe\\Adobe Acrobat",
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Adobe\\Adobe Creative Cloud",
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Adobe\\CSXS.*",
    "HKEY_LOCAL_MACHINE\\Software\\MozillaPlugins\\Adobe Acrobat"
};
