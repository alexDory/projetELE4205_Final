#!/bin/bash
readonly TARGET_IP="$1"
readonly TARGET_PORT="$2"
readonly PROGRAM="$3"
readonly PROGRAMPATH="$4"
readonly TARGET_DIR="/home/root"

# Must match startsPattern in tasks.json
echo "Deploying to target"

# Build
make -C build


# kill gdbserver on target and delete old binary
ssh root@${TARGET_IP} "sh -c '/usr/bin/killall -q gdbserver; rm -rf ${TARGET_DIR}/${PROGRAM}  exit 0'"
echo 228 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio228/direction
cat /sys/class/gpio/export
cat /sys/class/gpio/gpio228/direction
# send the program to the target
scp ${PROGRAMPATH}${PROGRAM} root@${TARGET_IP}:${TARGET_DIR}

# Must match endsPattern in tasks.json
echo "Starting GDB Server on Target"

# start gdbserver on target
ssh -t root@${TARGET_IP} "sh -c 'cd ${TARGET_DIR}; gdbserver localhost:${TARGET_PORT} ${PROGRAM}'"

