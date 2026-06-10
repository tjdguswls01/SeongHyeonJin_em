# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: D:\project\Final_Project\SHJ\VITIS\Zybo-Z7-20-pcam-5c_system\_ide\scripts\debugger_zybo-z7-20-pcam-5c-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source D:\project\Final_Project\SHJ\VITIS\Zybo-Z7-20-pcam-5c_system\_ide\scripts\debugger_zybo-z7-20-pcam-5c-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw D:/project/Final_Project/SHJ/VITIS/system_wrapper/export/system_wrapper/hw/system_wrapper.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
stop
source D:/project/Final_Project/SHJ/VITIS/Zybo-Z7-20-pcam-5c/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
rst -processor
targets -set -nocase -filter {name =~ "*A9*#0"}
dow D:/project/Final_Project/SHJ/VITIS/Zybo-Z7-20-pcam-5c/Debug/Zybo-Z7-20-pcam-5c.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
