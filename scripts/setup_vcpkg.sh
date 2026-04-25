#!/bin/bash

# Get the correct config file to add vcpkg path to
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS: check for Zsh (default) then Bash
    if [[ "$SHELL" == *"zsh"* ]]; then
        CONFIG="$HOME/.zshrc"
    else
        CONFIG="$HOME/.bash_profile"
    fi
else
    # Linux standard
    CONFIG="$HOME/.bashrc"
fi

# The Directory to install/find vcpkg
VCPKG_DIR="$HOME/.vcpkg"

# Finds VCPKG_ROOT, if exists do not bother installing
if [ -n "$VCPKG_ROOT" ]; then
	echo "VCPKG_ROOT is already defined as: $VCPKG_ROOT, attempting to add to path."
else
	# Installs vcpkg to ~/.vcpkg
	. <(curl https://aka.ms/vcpkg-init.sh -L)
	
	# Add VCPKG to config
	echo "export VCPKG_ROOT=\"$VCPKG_DIR\"" >> "$CONFIG"
	export VCPKG_ROOT="$VCPKG_DIR"
fi

# Add vcpkg to path
if ! grep -q "PATH.*VCPKG_ROOT" "$CONFIG"; then
    echo "export PATH=\"\$PATH:\$VCPKG_ROOT\"" >> "$CONFIG"
	export PATH="$PATH:$VCPKG_ROOT"
    echo "Added $VCPKG_ROOT to PATH in $CONFIG"
else
    echo "Path already exists in $CONFIG"
fi