# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: D:\project\Final_Project\SHJ\VITIS\cnn_a_system\_ide\scripts\systemdebugger_cnn_a_system_standalone.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source D:\project\Final_Project\SHJ\VITIS\cnn_a_system\_ide\scripts\systemdebugger_cnn_a_system_standalone.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Zybo Z7 210351BE7C7CA" && level==0 && jtag_device_ctx=="jsn-Zybo Z7-210351BE7C7CA-23727093-0"}
fpga -file D:/project/Final_Project/SHJ/VITIS/cnn_a/_ide/bitstream/CNN_CAM_A.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw D:/project/Final_Project/SHJ/VITIS/CNN_CAM_A/export/CNN_CAM_A/hw/CNN_CAM_A.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source D:/project/Final_Project/SHJ/VITIS/cnn_a/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow D:/project/Final_Project/SHJ/VITIS/cnn_a/Debug/cnn_a.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
