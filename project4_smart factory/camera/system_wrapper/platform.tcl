platform generate
platform active {system_wrapper}
domain active {zynq_fsbl}
bsp reload
domain active {domain_ps7_cortexa9_0}
bsp reload
platform active {system_wrapper}
platform generate -domains 
platform generate
platform generate
