# FocalEngineAppExample

![build](https://github.com/Azzinoth/FocalEngineAppExample/actions/workflows/Build.yml/badge.svg?branch=master)

This repository provides an example of a simple application that integrates the [Focal Engine](https://github.com/Azzinoth/FocalEngine/).
## Building the Project for Visual Studio (Windows)
```bash
# Initialize a new Git repository
git init

# Add the remote repository
git remote add origin https://github.com/Azzinoth/FocalEngineAppExample

# Pull the contents of the remote repository
git pull origin master

# Initialize and update submodules
git submodule update --init --recursive

# Generate the build files using CMake
# Will work in Windows PowerShell
cmake CMakeLists.txt
```
