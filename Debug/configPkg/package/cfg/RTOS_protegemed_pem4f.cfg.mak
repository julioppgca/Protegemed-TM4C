# invoke SourceDir generated makefile for RTOS_protegemed.pem4f
RTOS_protegemed.pem4f: .libraries,RTOS_protegemed.pem4f
.libraries,RTOS_protegemed.pem4f: package/cfg/RTOS_protegemed_pem4f.xdl
	$(MAKE) -f /Users/Julio/Documents/NeonWorkspace/Protegemed-TM4C/src/makefile.libs

clean::
	$(MAKE) -f /Users/Julio/Documents/NeonWorkspace/Protegemed-TM4C/src/makefile.libs clean

