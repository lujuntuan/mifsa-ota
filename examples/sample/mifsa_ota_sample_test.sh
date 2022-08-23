#!/bin/sh
#*********************************************************************************
#  *Copyright(C): Juntuan.Lu 2021
#  *Author:  Juntuan.Lu
#  *Version: 1.0
#  *Date:  2022/04/01
#  *Phone: 15397182986
#  *Description:
#  *Others:
#  *Function List:
#  *History:
#**********************************************************************************

CURRENT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]:-$0}")" && pwd)"

MIFSA_OTA_PART_LIBDIR=$CURRENT_DIR/../lib
MIFSA_OTA_PART_BINDIR=$CURRENT_DIR
MIFSA_OTA_NAME_PREFIX="domain"
MIFSA_OTA_TEST_COUNT=10

usage_func() {
    echo "Usage:"
    echo -e "mifsa_ota_sample_test.sh \n \
    [-l MIFSA_OTA_PART_LIBDIR] \n \
    [-b MIFSA_OTA_PART_BINDIR] \n \
    [-n MIFSA_OTA_NAME_PREFIX] \n \
    [-c MIFSA_OTA_TEST_COUNT]"
    exit -1
}

while getopts 'l:b:n:c:h' sd_setup_flag
do
    case $sd_setup_flag in
        l) MIFSA_OTA_PART_LIBDIR="$OPTARG";;
        b) MIFSA_OTA_PART_BINDIR="$OPTARG";;
        n) MIFSA_OTA_NAME_PREFIX="$OPTARG";;
        c) MIFSA_OTA_TEST_COUNT="$OPTARG";;
        h) usage_func;;
        ?) usage_func;;
    esac
done

echo "Run DCus test..."
export LD_LIBRARY_PATH=$MIFSA_OTA_PART_LIBDIR:$LD_LIBRARY_PATH
for i in $(seq 1 $MIFSA_OTA_TEST_COUNT)
do
    echo "Start ${MIFSA_OTA_NAME_PREFIX}_${i}..."
    ${MIFSA_OTA_PART_BINDIR}/mifsa_ota_client_simple --name=${MIFSA_OTA_NAME_PREFIX}_${i} &
    usleep 100000
done

read -p "Kill all?  [y/n]" USER_KILL

if [ "${USER_KILL}" = "n" -o "${USER_KILL}" = "N" ]; then
    echo "Exit."
    exit 0
fi

killall -9 mifsa_ota_client_simple

echo "Done."

exit 0
