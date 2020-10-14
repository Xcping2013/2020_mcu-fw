::rd /Q /S project\MDK-ARM(uV4)\Flash
::rd /Q /S project\MDK-ARM(uV4)\CpuRAM
::rd /Q /S project\MDK-ARM(uV4)\ExtSRAM
::del /Q project\MDK-ARM(uV4)\*.bak
::del /Q project\MDK-ARM(uV4)\*.dep
::del /Q project\MDK-ARM(uV4)\JLink*
::del /Q project\MDK-ARM(uV4)\project.uvgui.*

::del /Q project\EWARMv6\Project.dep
::del /Q project\EWARMv6\Flash
::del /Q project\EWARMv6\CpuRAM
::del /Q project\EWARMv6\settings
::rd  /Q /S project\EWARMv6\Flash
::rd /Q /S project\EWARMv6\CpuRAM
::rd /Q /S project\EWARMv6\settings

del *.bak /s
del *.ddk /s
del *.edk /s
del *.lst /s
del *.lnp /s
del *.mpf /s
del *.mpj /s
del *.obj /s
del *.omf /s
::del *.opt /s  ::²»ÔÊÐíÉ¾³ýJLINKµÄÉèÖÃ
del *.plg /s
del *.rpt /s
del *.tmp /s
del *.__i /s
del *.crf /s
del *.o /s
del *.d /s
del *.axf /s
del *.tra /s
del *.dep /s           
del JLinkLog.txt /s

del *.iex /s
del *.htm /s
::del *.sct /s
del *.map /s
exit
