SET PATH=D:\hardware\ARM_V5\ARM\ARMCC\Bin;C:\Program Files\OPT\OPT Camera Demo\Runtime\x64\;C:\Program Files\OPT\OPT Camera Demo\Runtime\x64\GenICam_v2_4\bin\Win64_x64\;C:\Program Files\OPT\OPT Camera Demo\Runtime\Win32\;C:\Program Files\OPT\OPT Camera Demo\Runtime\Win32\GenICam_v2_4\bin\Win32_i86\;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;C:\Program Files\VanDyke Software\Clients\;D:\software\Python36\Scripts\;D:\software\Python36\;C:\Program Files\Microsoft MPI\Bin\;D:\PC-SOFT\python36\;D:\PC-SOFT\python36\Scripts;C:\MentorGraphics\9.5PADS\SDD_HOME\common\win32\bin;C:\MentorGraphics\9.5PADS\SDD_HOME\common\win32\lib;C:\MentorGraphics\9.5PADS\MGC_HOME.ixn\bin;C:\MentorGraphics\9.5PADS\MGC_HOME.ixn\lib;C:\ProgramData\Oracle\Java\javapath;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Program Files (x86)\Intel\OpenCL SDK\2.0\bin\x86;C:\Program Files (x86)\Intel\OpenCL SDK\2.0\bin\x64;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Program Files\TortoiseSVN\bin;C:\Program Files\Keysight\IO Libraries Suite\bin;C:\Program Files\IVI Foundation\VISA\Win64\ktvisa;C:\Program Files\IVI Foundation\VISA\Win64\bin;C:\Program Files (x86)\Keysight\IO Libraries Suite\bin;C:\Program Files (x86)\IVI Foundation\VISA\WinNT\ktvisa;C:\Program Files (x86)\IVI Foundation\VISA\WinNT\bin;C:\Program Files (x86)\IVI Foundation\IVI\bin;C:\Program Files\IVI Foundation\IVI\bin;C:\Program Files (x86)\IVI Foundation\VISA\WinNT\Bin\;C:\Program Files\IVI Foundation\VISA\Win64\Bin\;C:\Program Files (x86)\IVI Foundation\VISA\winnt\agvisa;C:\WINDOWS\System32\OpenSSH\;C:\Program Files\dotnet\;D:\software\Git\cmd;C:\Users\Canping.Xie.INCUBECN\AppData\Local\Microsoft\WindowsApps;
SET CPU_TYPE=STM32F429ZITx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=rt_thread_stm32f429
SET CPU_CLOCK=0x00B71B00
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\clock.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\components.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\device.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\idle.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\ipc.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\irq.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\kservice.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\mem.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\memheap.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\mempool.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\object.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\scheduler.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\signal.__i"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\ArmLink" --Via ".\Objects\template.lnp"
"D:\hardware\ARM_V5\ARM\ARMCC\Bin\fromelf.exe" ".\Objects\template.axf" --i32combined --output ".\Objects\template.hex"
--bin -o  ./Objects/template.bin  ./Objects/template.axf
