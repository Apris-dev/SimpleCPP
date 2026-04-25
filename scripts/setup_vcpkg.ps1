# Unlike the .cmake script, this installs at ~/.vcpkg via microsofts vcpkg-init script
# Also adds to path to be able to call vcpkg anywhere

# The Directory to install/find vcpkg
$VCPKG_DIR = Join-Path $HOME ".vcpkg"

# Finds VCPKG_ROOT, if exists do not bother installing
$CurrentRoot = [System.Environment]::GetEnvironmentVariable("VCPKG_ROOT", "User")
if ($CurrentRoot -and (Test-Path "$CurrentRoot/.vcpkg-root")) {
	Write-Host "VCPKG_ROOT is already defined at $CurrentRoot, attempting to add to path."
} else {
	# Installs vcpkg to ~/.vcpkg
	iex (iwr -useb https://aka.ms/vcpkg-init.ps1)

	# Set VCPKG_ROOT for the User
	if (!$CurrentRoot -or $CurrentRoot -ne $VCPKG_DIR) {
		[System.Environment]::SetEnvironmentVariable("VCPKG_ROOT", $VCPKG_DIR, "User")
		$env:VCPKG_ROOT = $VCPKG_DIR # Set for current session too
		Write-Host "Set VCPKG_ROOT to $VCPKG_DIR" -ForegroundColor Green
	}
}

$UserPath = [System.Environment]::GetEnvironmentVariable("Path", "User")

# Add vcpkg to path
if ($UserPath -notlike "*$env:VCPKG_ROOT*") {
    $NewPath = $UserPath.TrimEnd(';') + ";" + $env:VCPKG_ROOT
    [System.Environment]::SetEnvironmentVariable("Path", $NewPath, "User")
    
    # Update current session path
    $env:Path += ";$VCPKG_DIR"
    
    Write-Host "Added %VCPKG_ROOT% to User Path environment variable." -ForegroundColor Green
} else {
    Write-Host "VCPKG_ROOT is already in the User Path." -ForegroundColor Gray
}