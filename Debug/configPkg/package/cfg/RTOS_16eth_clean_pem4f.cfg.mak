# invoke SourceDir generated makefile for RTOS_16eth_clean.pem4f
RTOS_16eth_clean.pem4f: .libraries,RTOS_16eth_clean.pem4f
.libraries,RTOS_16eth_clean.pem4f: package/cfg/RTOS_16eth_clean_pem4f.xdl
	$(MAKE) -f C:\Users\skelter\Protegemed\Protegemed/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\skelter\Protegemed\Protegemed/src/makefile.libs clean

