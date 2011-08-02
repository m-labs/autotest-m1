#!/bin/bash

BOOT_CRC_BIN="boot.crc.bin"

CRC_LEN_TMP=`mktemp`
BOOT_BIN_TMP=`mktemp`

FILENAMES=\
standby.fpg,\
soc-rescue.fpg,\
bios-rescue-without-CRC.bin,\
splash-rescue.raw,\
flickernoise.fbi,\
soc.fpg,\
bios.bin,\
splash.raw,\
flickernoise.fbi

IFS=','  FILES=( ${FILENAMES} )

create_crc_len_file() {
    CRC=`mkmmimg $1 | awk '{print $3}'`
    LEN=`ls -l   $1 | awk '{printf "%08x\n",$5}'`

    if [ `basename "$1"` == "bios-rescue-without-CRC.bin" ]; then
	CRC="00000000"
    fi
    if [ `basename "$1"` == "bios.bin" ]; then
	CRC="00000000"
	LEN=`ls -l   $1 | awk '{printf "%08x\n",$5-4}'`
    fi
    if [ `basename "$1"` == "flickernoise.fbi" ]; then
	CRC="00000000"
	LEN="00000000"
    fi
    # 0 mean length/crc included by themself 

    CRC1=`echo $CRC | cut -b1-2`
    CRC2=`echo $CRC | cut -b3-4`
    CRC3=`echo $CRC | cut -b5-6`
    CRC4=`echo $CRC | cut -b7-8`

    LEN1=`echo $LEN | cut -b1-2`
    LEN2=`echo $LEN | cut -b3-4`
    LEN3=`echo $LEN | cut -b5-6`
    LEN4=`echo $LEN | cut -b7-8`

    printf "\\x$(printf "%x" 0x${CRC1})" >  ${CRC_LEN_TMP}
    printf "\\x$(printf "%x" 0x${CRC2})" >> ${CRC_LEN_TMP}
    printf "\\x$(printf "%x" 0x${CRC3})" >> ${CRC_LEN_TMP}
    printf "\\x$(printf "%x" 0x${CRC4})" >> ${CRC_LEN_TMP}

    printf "\\x$(printf "%x" 0x${LEN1})" >> ${CRC_LEN_TMP}
    printf "\\x$(printf "%x" 0x${LEN2})" >> ${CRC_LEN_TMP}
    printf "\\x$(printf "%x" 0x${LEN3})" >> ${CRC_LEN_TMP}
    printf "\\x$(printf "%x" 0x${LEN4})" >> ${CRC_LEN_TMP}
}

cp boot.bin ${BOOT_CRC_BIN}

for (( i=0; i<9; i++ ))
do
    if [ -e ${IMAGES_DIR}/${FILES[i]} ]; then
	 create_crc_len_file ${IMAGES_DIR}/${FILES[i]}
	 cp -f ${BOOT_CRC_BIN} ${BOOT_BIN_TMP}
	 cat ${BOOT_BIN_TMP} ${CRC_LEN_TMP} > ${BOOT_CRC_BIN}
    else
	echo "${IMAGES_DIR}/${FILES[i]} not exist, check IMAGES_DIR value"
	exit 1
    fi
done

echo "boot.crc.bin created"
exit 0
