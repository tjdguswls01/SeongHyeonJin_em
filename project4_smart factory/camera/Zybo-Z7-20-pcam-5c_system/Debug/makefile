#
# This file was created by Xilinx Vitis tool.
#


#
# tools
#

RM = rm -f
RMDIR = rm -rf



#
# variables
#

SDCARD_GEN = sdcard_gen
GENERATE_BIF = ::scw::generate_bif

XPFM_PATH = D:/project/Final_Project/SHJ/VITIS/system_wrapper/export/system_wrapper/system_wrapper.xpfm
SYS_CONFIG = system_wrapper
DOMAINS = domain_ps7_cortexa9_0
BIF_PATH = D:/project/Final_Project/SHJ/VITIS/Zybo-Z7-20-pcam-5c_system/Debug/system.bif

BITSTREAM_PATH = D:/project/Final_Project/SHJ/VITIS/Zybo-Z7-20-pcam-5c/_ide/bitstream/system_wrapper.bit
SD_FILES = --elf D:/project/Final_Project/SHJ/VITIS/Zybo-Z7-20-pcam-5c/Debug/Zybo-Z7-20-pcam-5c.elf,ps7_cortexa9_0

#
# primary build targets
#

all: package

package:
	-@echo Generating bif file for the system project
	generate_system_bif.bat 50351 ${XPFM_PATH} ${DOMAINS} ${BIF_PATH}
	${SDCARD_GEN} --xpfm ${XPFM_PATH} --sys_config ${SYS_CONFIG} --bif ${BIF_PATH} --bitstream ${BITSTREAM_PATH} ${SD_FILES}

clean:
	$(RMDIR) ./_sds
	${RMDIR} package
	${RM} system.bif

.PHONY: all package clean


