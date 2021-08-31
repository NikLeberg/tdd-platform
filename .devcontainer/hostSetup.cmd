REM Command to run on host at start of container. As host is WSL use *.exe to
REM run command in Windows. https://docs.microsoft.com/en-us/windows/wsl/interop
powershell.exe -File .devcontainer/hostSetup.ps1
