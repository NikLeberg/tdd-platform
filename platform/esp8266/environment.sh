# Source the normal RTOS_SDK export.sh to have all the common tools on $PATH.
source /root/.espressif/ESP8266_RTOS_SDK/export.sh
# Set the port where the esp8266 is available. The real port of the Windows host
# is bridged to a socket on localhost. Obtain the ip of the host (as we are
# inside a container) and use this as rfc2217 telnet port. All RTOS_SDK tools
# look for the following environment variable.
# https://stackoverflow.com/questions/22944631/how-to-get-the-ip-address-of-the-docker-host-from-inside-a-docker-container
export ESPPORT=rfc2217://host.docker.internal:21214
