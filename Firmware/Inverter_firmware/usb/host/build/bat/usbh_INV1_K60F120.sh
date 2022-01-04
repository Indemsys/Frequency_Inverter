#!/bin/sh

# expect forward slash paths
MQXROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
cp -f "${MQXROOTDIR}/usb/common/include/usb.h" "${OUTPUTDIR}/usb.h"
cp -f "${MQXROOTDIR}/usb/common/include/usb_debug.h" "${OUTPUTDIR}/usb_debug.h"
cp -f "${MQXROOTDIR}/usb/common/include/usb_desc.h" "${OUTPUTDIR}/usb_desc.h"
cp -f "${MQXROOTDIR}/usb/common/include/usb_error.h" "${OUTPUTDIR}/usb_error.h"
cp -f "${MQXROOTDIR}/usb/common/include/usb_misc.h" "${OUTPUTDIR}/usb_misc.h"
cp -f "${MQXROOTDIR}/usb/common/include/usb_misc_prv.h" "${OUTPUTDIR}/usb_misc_prv.h"
cp -f "${MQXROOTDIR}/usb/common/include/usb_prv.h" "${OUTPUTDIR}/usb_prv.h"
cp -f "${MQXROOTDIR}/usb/common/include/usb_types.h" "${OUTPUTDIR}/usb_types.h"
cp -f "${MQXROOTDIR}/usb/host/source/classes/audio/usb_host_audio.h" "${OUTPUTDIR}/usb_host_audio.h"
cp -f "${MQXROOTDIR}/usb/host/source/classes/cdc/usb_host_cdc.h" "${OUTPUTDIR}/usb_host_cdc.h"
cp -f "${MQXROOTDIR}/usb/host/source/classes/hid/usb_host_hid.h" "${OUTPUTDIR}/usb_host_hid.h"
cp -f "${MQXROOTDIR}/usb/host/source/classes/hub/usb_host_hub.h" "${OUTPUTDIR}/usb_host_hub.h"
cp -f "${MQXROOTDIR}/usb/host/source/classes/hub/usb_host_hub_sm.h" "${OUTPUTDIR}/usb_host_hub_sm.h"
cp -f "${MQXROOTDIR}/usb/host/source/classes/msd/mfs/usbmfs.h" "${OUTPUTDIR}/usbmfs.h"
cp -f "${MQXROOTDIR}/usb/host/source/classes/msd/mfs/usbmfspr.h" "${OUTPUTDIR}/usbmfspr.h"
cp -f "${MQXROOTDIR}/usb/host/source/classes/msd/usb_host_msd_bo.h" "${OUTPUTDIR}/usb_host_msd_bo.h"
cp -f "${MQXROOTDIR}/usb/host/source/classes/msd/usb_host_msd_ufi.h" "${OUTPUTDIR}/usb_host_msd_ufi.h"
cp -f "${MQXROOTDIR}/usb/host/source/classes/phdc/usb_host_phdc.h" "${OUTPUTDIR}/usb_host_phdc.h"
cp -f "${MQXROOTDIR}/usb/host/source/classes/printer/usb_host_printer.h" "${OUTPUTDIR}/usb_host_printer.h"
cp -f "${MQXROOTDIR}/usb/host/source/host/ehci/ehci.h" "${OUTPUTDIR}/ehci.h"
cp -f "${MQXROOTDIR}/usb/host/source/host/ehci/ehci_bw.h" "${OUTPUTDIR}/ehci_bw.h"
cp -f "${MQXROOTDIR}/usb/host/source/host/ehci/ehci_cache.h" "${OUTPUTDIR}/ehci_cache.h"
cp -f "${MQXROOTDIR}/usb/host/source/host/ehci/ehci_cncl.h" "${OUTPUTDIR}/ehci_cncl.h"
cp -f "${MQXROOTDIR}/usb/host/source/host/ehci/ehci_intr.h" "${OUTPUTDIR}/ehci_intr.h"
cp -f "${MQXROOTDIR}/usb/host/source/host/ehci/ehci_iso.h" "${OUTPUTDIR}/ehci_iso.h"
cp -f "${MQXROOTDIR}/usb/host/source/host/ehci/ehci_main.h" "${OUTPUTDIR}/ehci_main.h"
cp -f "${MQXROOTDIR}/usb/host/source/host/ehci/ehci_shut.h" "${OUTPUTDIR}/ehci_shut.h"
cp -f "${MQXROOTDIR}/usb/host/source/host/ehci/ehci_utl.h" "${OUTPUTDIR}/ehci_utl.h"
cp -f "${MQXROOTDIR}/usb/host/source/host/khci/khci.h" "${OUTPUTDIR}/khci.h"
cp -f "${MQXROOTDIR}/usb/host/source/include/host_ch9.h" "${OUTPUTDIR}/host_ch9.h"
cp -f "${MQXROOTDIR}/usb/host/source/include/host_close.h" "${OUTPUTDIR}/host_close.h"
cp -f "${MQXROOTDIR}/usb/host/source/include/host_cnfg.h" "${OUTPUTDIR}/host_cnfg.h"
cp -f "${MQXROOTDIR}/usb/host/source/include/host_cnl.h" "${OUTPUTDIR}/host_cnl.h"
cp -f "${MQXROOTDIR}/usb/host/source/include/host_common.h" "${OUTPUTDIR}/host_common.h"
cp -f "${MQXROOTDIR}/usb/host/source/include/host_dev_list.h" "${OUTPUTDIR}/host_dev_list.h"
cp -f "${MQXROOTDIR}/usb/host/source/include/host_main.h" "${OUTPUTDIR}/host_main.h"
cp -f "${MQXROOTDIR}/usb/host/source/include/host_rcv.h" "${OUTPUTDIR}/host_rcv.h"
cp -f "${MQXROOTDIR}/usb/host/source/include/host_shut.h" "${OUTPUTDIR}/host_shut.h"
cp -f "${MQXROOTDIR}/usb/host/source/include/host_snd.h" "${OUTPUTDIR}/host_snd.h"
cp -f "${MQXROOTDIR}/usb/host/source/include/hostapi.h" "${OUTPUTDIR}/hostapi.h"
cp -f "${MQXROOTDIR}/usb/host/source/rtos/mqx/mqx_dll.h" "${OUTPUTDIR}/mqx_dll.h"
cp -f "${MQXROOTDIR}/usb/host/source/rtos/mqx/mqx_host.h" "${OUTPUTDIR}/mqx_host.h"
cp -f "${MQXROOTDIR}/usb/host/source/rtos/mqx/usb_mqx.h" "${OUTPUTDIR}/usb_mqx.h"


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

