# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct D:\project\Final_Project\SHJ\VITIS\CNN_CMA_B\platform.tcl
# 
# OR launch xsct and run below command.
# source D:\project\Final_Project\SHJ\VITIS\CNN_CMA_B\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {CNN_CMA_B}\
-hw {D:\project\Final_Project\SHJ\VIVADO\CNN_CAM_B\CNN_CAM_B.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -fsbl-target {psu_cortexa53_0} -out {D:/project/Final_Project/SHJ/VITIS}

platform write
platform generate -domains 
platform active {CNN_CMA_B}
platform generate
platform generate -domains 
platform clean
platform generate
platform active {CNN_CMA_B}
platform generate
platform clean
platform generate
platform clean
platform generate
