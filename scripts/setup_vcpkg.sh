#!/bin/bash

# Unlike the .cmake script, this installs at ~/.vcpkg via microsofts vcpkg-init script
# Also adds to ~/.bashrc (or macos equivalent) to be able to call vcpkg anywhere

# Get the correct config file to add vcpkg path to
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS: Zsh is the modern default, but .zprofile is better for IDEs than .zshrc
    if [ -f "$HOME/.zprofile" ]; then
        CONFIG="$HOME/.zprofile"
    elif [ -f "$HOME/.zshrc" ]; then
        CONFIG="$HOME/.zshrc"
    else
        CONFIG="$HOME/.bash_profile"
    fi
else
    # Linux: Check for login profile first (better for CLion/IDE inheritance)
    if [ -f "$HOME/.bash_profile" ]; then
        CONFIG="$HOME/.bash_profile"
    elif [ -f "$HOME/.profile" ]; then
        CONFIG="$HOME/.profile"
    else
        # Fallback to the standard interactive shell config
        CONFIG="$HOME/.bashrc"
    fi
fi

# The Directory to install/find vcpkg
VCPKG_DIR="$HOME/.vcpkg"

# Ensure curl is installed
if command -v curl >/dev/null 2>&1; then
	# Finds VCPKG_ROOT and ensures .vcpkg-root exists, if exists do not bother installing
	if [ -n "$VCPKG_ROOT" ] && [ -f "$VCPKG_ROOT/.vcpkg-root" ]; then
		echo "VCPKG_ROOT is already found at $VCPKG_ROOT, attempting to add to path."
	else
		# Installs vcpkg to ~/.vcpkg
		. <(curl https://aka.ms/vcpkg-init.sh -L)
		
		# Add VCPKG to config if it doesnt exist in config file, or it doesnt equal the install directory
		if ! grep -q "export VCPKG_ROOT=\"$VCPKG_DIR\"" "$CONFIG" || [ "$VCPKG_ROOT" != "$VCPKG_DIR" ]; then
			echo "export VCPKG_ROOT=\"$VCPKG_DIR\"" >> "$CONFIG"
			export VCPKG_ROOT="$VCPKG_DIR"
		fi
	fi

	# Add vcpkg to path
	if ! grep -q "export PATH=\"\$PATH:$VCPKG_ROOT\"" "$CONFIG"; then
		echo "export PATH=\"\$PATH:$VCPKG_ROOT\"" >> "$CONFIG"
		export PATH="$PATH:$VCPKG_ROOT"
		echo "Added $VCPKG_ROOT to PATH in $CONFIG"
	else
		echo "Path already exists in $CONFIG"
	fi
else
	echo "curl is not installed, please install curl before running."
fi