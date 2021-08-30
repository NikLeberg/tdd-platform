# As WSL2 does not allow for COM passthrough (WSL1 did), make all available
# ports accessible over TCP (rfc2217) with the help of hub4com.
# https://matevarga.github.io/esp32/m5stack/esp-idf/wsl2/2020/05/31/flashing-esp32-under-wsl2.html

# If command line argument is not "backgroundjob" then start a new process for
# this script to run in the background.
if (-NOT $args[0] -eq "backgroundjob") {
    Start-Process -FilePath powershell.exe -WindowStyle Minimized -ArgumentList `
        "-File .devcontainer/hostSetup.ps1", `
        "backgroundjob"
    exit
}

function Get-AvailableCOMNums {
    # Array of all available portnames 'COMx'
    $ports = [System.IO.Ports.SerialPort]::GetPortNames()
    # Array of all available portnumbers 'x'
    $portNums = $ports -replace "[^0-9]"
    $portNums
}

function Test-COMBridge {
    param (
        $portNum
    )

    # Test if bridge has connectivity
    Test-NetConnection 127.0.0.1 -Port 2121$portNum -InformationLevel Quiet
}

function Start-COMBridge {
    param (
        $portNum
    )

    Start-Process `
        -FilePath .devcontainer/hub4com.exe `
        -WindowStyle Hidden `
        -ArgumentList `
            "--reconnect=2000", `
            "--share-mode=on", `
            "--create-filter=escparse,com,parse", `
            "--create-filter=purge,com,purge", `
            "--create-filter=pinmap,com,pinmap:`"--rts=cts --dtr=dsr --break=break`"", `
            "--create-filter=linectl,com,lc:`"--br=remote --lc=remote`"", `
            "--add-filters=0:com", `
            "--create-filter=telnet,tcp,telnet:`"--comport=server --suppress-echo=yes`"", `
            "--create-filter=lsrmap,tcp,lsrmap", `
            "--create-filter=pinmap,tcp,pinmap:`"--cts=cts --dsr=dsr --dcd=dcd --ring=ring`"", `
            "--create-filter=linectl,tcp,lc:`"--br=local --lc=local`"", `
            "--add-filters=1:tcp", `
            "--octs=off", `
            "`"\\.\COM$portNum`"", `
            "--use-driver=tcp", `
            "`"*2121$portNum`""
}


try {
    # Get own IPv4 address
    # https://adamtheautomator.com/powershell-get-ip-address/
    $cimSession = New-CimSession
    $ip = (Get-CimInstance -CimSession $cimSession -ClassName Win32_NetworkAdapterConfiguration -Filter "IPEnabled = 'True'").IPAddress[0]
    Write-Output `
        "hostSetup.ps1 - Host COM port bridge." `
        "`n`n`n`n`n`n`n`n" `
        "This PowerShell-Script loops until gracefull termination with Ctrl+C." `
        "The available COM-ports get checked all five seconds. If a port is" `
        "newly added a bridge accessible at rfc2217://$ip`:2121x" `
        "is created where x stands for the port number (COMx)." `
        "======================================================================"
    while ($true) {
        # Array of all available port numbers
        $portNums = Get-AvailableCOMNums

        # If bridge has no connectivity, start a new bridge
        foreach ($portNum in $portNums) {
            if (-Not (Test-COMBridge -portNum $portNum)) {
                Start-COMBridge -portNum $portNum
                Write-Output "Created bridge: COM$portNum <--> rfc2217://$ip`:2121$portNum"
            }
        }

        # Wait 5 seconds until checking again
        sleep 5
    }
}
finally {
    # termination e.g. ctrl+c, stop all bridges
    Stop-Process -Name "hub4com"
}
