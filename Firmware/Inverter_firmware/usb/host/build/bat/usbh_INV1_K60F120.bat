@echo off

rem convert path to backslash format
set MQXROOTDIR=%1
set MQXROOTDIR=%MQXROOTDIR:/=\%
set OUTPUTDIR=%2
set OUTPUTDIR=%OUTPUTDIR:/=\%
set TOOL=%3


rem copy common files
IF NOT EXIST %OUTPUTDIR% mkdir %OUTPUTDIR%
copy %MQXROOTDIR%\usb\common\include\usb.h %OUTPUTDIR%\usb.h /Y
copy %MQXROOTDIR%\usb\common\include\usb_debug.h %OUTPUTDIR%\usb_debug.h /Y
copy %MQXROOTDIR%\usb\common\include\usb_desc.h %OUTPUTDIR%\usb_desc.h /Y
copy %MQXROOTDIR%\usb\common\include\usb_error.h %OUTPUTDIR%\usb_error.h /Y
copy %MQXROOTDIR%\usb\common\include\usb_misc.h %OUTPUTDIR%\usb_misc.h /Y
copy %MQXROOTDIR%\usb\common\include\usb_misc_prv.h %OUTPUTDIR%\usb_misc_prv.h /Y
copy %MQXROOTDIR%\usb\common\include\usb_prv.h %OUTPUTDIR%\usb_prv.h /Y
copy %MQXROOTDIR%\usb\common\include\usb_types.h %OUTPUTDIR%\usb_types.h /Y
copy %MQXROOTDIR%\usb\host\source\classes\audio\usb_host_audio.h %OUTPUTDIR%\usb_host_audio.h /Y
copy %MQXROOTDIR%\usb\host\source\classes\cdc\usb_host_cdc.h %OUTPUTDIR%\usb_host_cdc.h /Y
copy %MQXROOTDIR%\usb\host\source\classes\hid\usb_host_hid.h %OUTPUTDIR%\usb_host_hid.h /Y
copy %MQXROOTDIR%\usb\host\source\classes\hub\usb_host_hub.h %OUTPUTDIR%\usb_host_hub.h /Y
copy %MQXROOTDIR%\usb\host\source\classes\hub\usb_host_hub_sm.h %OUTPUTDIR%\usb_host_hub_sm.h /Y
copy %MQXROOTDIR%\usb\host\source\classes\msd\mfs\usbmfs.h %OUTPUTDIR%\usbmfs.h /Y
copy %MQXROOTDIR%\usb\host\source\classes\msd\mfs\usbmfspr.h %OUTPUTDIR%\usbmfspr.h /Y
copy %MQXROOTDIR%\usb\host\source\classes\msd\usb_host_msd_bo.h %OUTPUTDIR%\usb_host_msd_bo.h /Y
copy %MQXROOTDIR%\usb\host\source\classes\msd\usb_host_msd_ufi.h %OUTPUTDIR%\usb_host_msd_ufi.h /Y
copy %MQXROOTDIR%\usb\host\source\classes\phdc\usb_host_phdc.h %OUTPUTDIR%\usb_host_phdc.h /Y
copy %MQXROOTDIR%\usb\host\source\classes\printer\usb_host_printer.h %OUTPUTDIR%\usb_host_printer.h /Y
copy %MQXROOTDIR%\usb\host\source\host\ehci\ehci.h %OUTPUTDIR%\ehci.h /Y
copy %MQXROOTDIR%\usb\host\source\host\ehci\ehci_bw.h %OUTPUTDIR%\ehci_bw.h /Y
copy %MQXROOTDIR%\usb\host\source\host\ehci\ehci_cache.h %OUTPUTDIR%\ehci_cache.h /Y
copy %MQXROOTDIR%\usb\host\source\host\ehci\ehci_cncl.h %OUTPUTDIR%\ehci_cncl.h /Y
copy %MQXROOTDIR%\usb\host\source\host\ehci\ehci_intr.h %OUTPUTDIR%\ehci_intr.h /Y
copy %MQXROOTDIR%\usb\host\source\host\ehci\ehci_iso.h %OUTPUTDIR%\ehci_iso.h /Y
copy %MQXROOTDIR%\usb\host\source\host\ehci\ehci_main.h %OUTPUTDIR%\ehci_main.h /Y
copy %MQXROOTDIR%\usb\host\source\host\ehci\ehci_shut.h %OUTPUTDIR%\ehci_shut.h /Y
copy %MQXROOTDIR%\usb\host\source\host\ehci\ehci_utl.h %OUTPUTDIR%\ehci_utl.h /Y
copy %MQXROOTDIR%\usb\host\source\host\khci\khci.h %OUTPUTDIR%\khci.h /Y
copy %MQXROOTDIR%\usb\host\source\include\host_ch9.h %OUTPUTDIR%\host_ch9.h /Y
copy %MQXROOTDIR%\usb\host\source\include\host_close.h %OUTPUTDIR%\host_close.h /Y
copy %MQXROOTDIR%\usb\host\source\include\host_cnfg.h %OUTPUTDIR%\host_cnfg.h /Y
copy %MQXROOTDIR%\usb\host\source\include\host_cnl.h %OUTPUTDIR%\host_cnl.h /Y
copy %MQXROOTDIR%\usb\host\source\include\host_common.h %OUTPUTDIR%\host_common.h /Y
copy %MQXROOTDIR%\usb\host\source\include\host_dev_list.h %OUTPUTDIR%\host_dev_list.h /Y
copy %MQXROOTDIR%\usb\host\source\include\host_main.h %OUTPUTDIR%\host_main.h /Y
copy %MQXROOTDIR%\usb\host\source\include\host_rcv.h %OUTPUTDIR%\host_rcv.h /Y
copy %MQXROOTDIR%\usb\host\source\include\host_shut.h %OUTPUTDIR%\host_shut.h /Y
copy %MQXROOTDIR%\usb\host\source\include\host_snd.h %OUTPUTDIR%\host_snd.h /Y
copy %MQXROOTDIR%\usb\host\source\include\hostapi.h %OUTPUTDIR%\hostapi.h /Y
copy %MQXROOTDIR%\usb\host\source\rtos\mqx\mqx_dll.h %OUTPUTDIR%\mqx_dll.h /Y
copy %MQXROOTDIR%\usb\host\source\rtos\mqx\mqx_host.h %OUTPUTDIR%\mqx_host.h /Y
copy %MQXROOTDIR%\usb\host\source\rtos\mqx\usb_mqx.h %OUTPUTDIR%\usb_mqx.h /Y


goto tool_%TOOL%

rem cw10 files
:tool_cw10
goto copy_end

rem iar files
:tool_iar
goto copy_end

rem cw10gcc files
:tool_cw10gcc
goto copy_end

rem uv4 files
:tool_uv4
goto copy_end

:copy_end

