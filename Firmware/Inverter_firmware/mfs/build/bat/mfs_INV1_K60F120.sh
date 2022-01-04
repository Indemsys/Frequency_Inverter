#!/bin/sh

# expect forward slash paths
MQXROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
cp -f "${MQXROOTDIR}/mfs/source/include/mfs.h" "${OUTPUTDIR}/mfs.h"
cp -f "${MQXROOTDIR}/mfs/source/include/mfs_cnfg.h" "${OUTPUTDIR}/mfs_cnfg.h"
cp -f "${MQXROOTDIR}/mfs/source/include/mfs_rev.h" "${OUTPUTDIR}/mfs_rev.h"
cp -f "${MQXROOTDIR}/mfs/source/include/part_mgr.h" "${OUTPUTDIR}/part_mgr.h"


# cw10 files
if [ "${TOOL}" = "cw10" ]; then
:
fi
# iar files
if [ "${TOOL}" = "iar" ]; then
:
fi
# cw10gcc files
if [ "${TOOL}" = "cw10gcc" ]; then
:
fi
# uv4 files
if [ "${TOOL}" = "uv4" ]; then
:
fi

