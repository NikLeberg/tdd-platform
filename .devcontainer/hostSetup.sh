#!/usr/bin/env bash

# WSL can't access COM ports of the host Windows 10. To circumvent that, start a
# RFC2217 telnet server which then can be accessed.
# 
# Based on the gist of DraTeots:
# https://gist.github.com/DraTeots/e0c669608466470baa6c
# 
# Real COM port on Windows 10 can be accessed:
# In WSL: Host's IP (cat /etc/resolv.conf | grep nameserver | cut -d ' ' -f 2)
# In Docker: host.docker.internal
# always on port 2217

# Similarly in Linux for an available tty port in /dev a server will also be
# started. One could also use '--device' flag for the 'docker run' command to
# map a device from the host into the container. But with starting a server the
# container does not have to differentiate between WSL or real Linux. It can
# access the ports allways at 'host.docker.internal' port '2217'.

# This script needs to run in the background so that it can detect later added
# ports and does not delay the container startup.
if [[ "$1" != "background" ]]; then
    pkill -f dtach
    dtach -n /tmp/tdd-platform-tty $0 "background"
    exit 0
fi

# We need access to Windows information, conveniently everything that is run
# with *.exe is automatically in the Windows environment
# (see https://docs.microsoft.com/en-us/windows/wsl/interop).
psCommand=""
psOutput=""
runPowershellCommand () {
    psOutput=`powershell.exe -Command "${psCommand}"`
}
runPowershellCommandSilent () {
    powershell.exe -Command "${psCommand} | Out-Null" > /dev/null 2>&1
    psOutput=$?
}

# Get available port, waits if none is available
comPort=""
getComPortWSL () {
    # Get available COM ports
    psCommand="[System.IO.Ports.SerialPort]::GetPortNames()"
    runPowershellCommand
    comPort=$psOutput
    comPort=`echo "${comPort}" | head -1` # only first port
    if [[ $comPort =~ "COM" ]]; then
        echo "Detected $comPort."
    else
        echo "No COM port detected."
        sleep 5
        getComPortWSL
    fi
}
getComPortLinux () {
    # https://unix.stackexchange.com/questions/144029/command-to-determine-ports-of-a-device-like-dev-ttyusb0
    for sysdevpath in $(find /sys/bus/usb/devices/usb*/ -name dev); do
        syspath="${sysdevpath%/dev}"
        devname="$(udevadm info -q name -p $syspath)"
        if [[ "$devname" == "bus/"* ]]; then
            contine
        fi
        if [[ "$devname" != "tty"* ]]; then
            contine # only look for tty*
        fi
        eval "$(udevadm info -q property --export -p $syspath)"
        if [[ -z "$ID_SERIAL" ]]; then
            contine
        fi
        # We have found a seemingly valid tty over USB.
        comPort="/dev/$devname"
        break
    done
    if [[ $comPort =~ "/dev/tty" ]]; then
        echo "Detected $comPort."
    else
        echo "No COM port detected."
        sleep 5
        getComPortLinux
    fi
}

startServerWSL () {
    # First terminate previous server.
    psCommand="Stop-Process -Name 'hub4com'"
    runPowershellCommandSilent || true
    # Start new one with this stack of parameters... it is copy-paste from the
    # arguments that com2tcp-rcf2217.bat script generates.
    local arguments="--reconnect=2000
--share-mode=on
--create-filter=escparse,com,parse
--create-filter=purge,com,purge
--create-filter=pinmap,com,pinmap:--rts=cts --dtr=dsr --break=break
--create-filter=linectl,com,lc:--br=remote --lc=remote
--add-filters=0:com
--create-filter=telnet,tcp,telnet:--comport=server --suppress-echo=yes
--create-filter=lsrmap,tcp,lsrmap
--create-filter=pinmap,tcp,pinmap:--cts=cts --dsr=dsr --dcd=dcd --ring=ring
--create-filter=linectl,tcp,lc:--br=local --lc=local
--add-filters=1:tcp
--octs=off
\\\\.\\$comPort
--use-driver=tcp
*2217"
    printf "%s" "$arguments" > args.txt
    psCommand="Start-Process -FilePath .devcontainer/hub4com.exe -ArgumentList '--load=args.txt'" #  -WindowStyle Hidden
    runPowershellCommand
    sleep 5 # wait a bit, hub4com needs a second to read the config
    rm args.txt
}
startServerLinux () {
    ser2net -C 2217:telnet:0:$comPort
}

connected=0
testServerWSL() {
    psCommand="Test-NetConnection 127.0.0.1 -Port 2217 -InformationLevel Quiet"
    runPowershellCommand
    if [[ "$psOutput" =~ "failed" ]]; then
        connected=0
    else
        connected=1
    fi
}
testServerLinux() {
    nc -zv -w5 host.docker.internal 2217
    if [[ $? -eq 0 ]]; then
        connected=1
    else
        connected=0
    fi
}

# Setup Host, i.e. detect port and start server.
setupWSL () {
    while true; do
        testServerWSL
        if [[ $connected -eq 0 ]]; then
            # Get available COM ports
            getComPortWSL
            # Start hub4com
            startServerWSL
        fi
        sleep 10
    done
}
setupLinux () {
    while true; do
        testServerLinux
        if [[ $connected -eq 0 ]]; then
            # Get available tty
            getComPortLinux
            # Start ser2net
            startServerLinux
        fi
        sleep 10
    done
}


# Main
sleep 1
if [[ -z "$WSL_DISTRO_NAME" ]]; then
    # We are a real Linux
    setupLinux
else
    # We are running in WSL
    setupWSL
fi

exit 0
