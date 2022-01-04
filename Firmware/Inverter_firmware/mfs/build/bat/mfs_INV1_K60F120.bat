@echo off

rem convert path to backslash format
set MQXROOTDIR=%1
set MQXROOTDIR=%MQXROOTDIR:/=\%
set OUTPUTDIR=%2
set OUTPUTDIR=%OUTPUTDIR:/=\%
set TOOL=%3


rem copy common files
IF NOT EXIST %OUTPUTDIR% mkdir %OUTPUTDIR%
copy %MQXROOTDIR%\mfs\source\include\mfs.h %OUTPUTDIR%\mfs.h /Y
copy %MQXROOTDIR%\mfs\source\include\mfs_cnfg.h %OUTPUTDIR%\mfs_cnfg.h /Y
copy %MQXROOTDIR%\mfs\source\include\mfs_rev.h %OUTPUTDIR%\mfs_rev.h /Y
copy %MQXROOTDIR%\mfs\source\include\part_mgr.h %OUTPUTDIR%\part_mgr.h /Y


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

