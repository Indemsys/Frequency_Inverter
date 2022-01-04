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
copy %MQXROOTDIR%\usb\device\source\classes\include\usb_audio.h %OUTPUTDIR%\usb_audio.h /Y
copy %MQXROOTDIR%\usb\device\source\classes\include\usb_cdc.h %OUTPUTDIR%\usb_cdc.h /Y
copy %MQXROOTDIR%\usb\device\source\classes\include\usb_cdc_pstn.h %OUTPUTDIR%\usb_cdc_pstn.h /Y
copy %MQXROOTDIR%\usb\device\source\classes\include\usb_class.h %OUTPUTDIR%\usb_class.h /Y
copy %MQXROOTDIR%\usb\device\source\classes\include\usb_framework.h %OUTPUTDIR%\usb_framework.h /Y
copy %MQXROOTDIR%\usb\device\source\classes\include\usb_hid.h %OUTPUTDIR%\usb_hid.h /Y
copy %MQXROOTDIR%\usb\device\source\classes\include\usb_msc.h %OUTPUTDIR%\usb_msc.h /Y
copy %MQXROOTDIR%\usb\device\source\classes\include\usb_msc_scsi.h %OUTPUTDIR%\usb_msc_scsi.h /Y
copy %MQXROOTDIR%\usb\device\source\classes\include\usb_phdc.h %OUTPUTDIR%\usb_phdc.h /Y
copy %MQXROOTDIR%\usb\device\source\classes\include\usb_stack_config.h %OUTPUTDIR%\usb_stack_config.h /Y
copy %MQXROOTDIR%\usb\device\source\include\dev_cnfg.h %OUTPUTDIR%\dev_cnfg.h /Y
copy %MQXROOTDIR%\usb\device\source\include\dev_main.h %OUTPUTDIR%\dev_main.h /Y
copy %MQXROOTDIR%\usb\device\source\include\devapi.h %OUTPUTDIR%\devapi.h /Y
copy %MQXROOTDIR%\usb\device\source\include\ehci\ehci_dev_main.h %OUTPUTDIR%\ehci_dev_main.h /Y
copy %MQXROOTDIR%\usb\device\source\include\khci\khci_dev_main.h %OUTPUTDIR%\khci_dev_main.h /Y
copy %MQXROOTDIR%\usb\device\source\include\rtos\mqx_dev.h %OUTPUTDIR%\mqx_dev.h /Y
copy %MQXROOTDIR%\usb\host\source\host\khci\khci_prv.h %OUTPUTDIR%\khci_prv.h /Y


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

