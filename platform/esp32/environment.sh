# Source the normal esp-idf export.sh to have all the common tools on $PATH.
source /root/.espressif/esp-idf/export.sh
# Set the port where the esp32 is available. The real port of the Windows host
# is bridged to a socket on localhost. Obtain the ip of the host (as we are
# inside a container) and use this as rfc2217 telnet port. All esp-idf tools
# look for the following environment variable.
# https://stackoverflow.com/questions/22944631/how-to-get-the-ip-address-of-the-docker-host-from-inside-a-docker-container
export ESPPORT=rfc2217://host.docker.internal:21214
