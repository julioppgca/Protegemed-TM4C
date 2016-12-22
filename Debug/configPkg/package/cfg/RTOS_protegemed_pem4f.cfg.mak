# invoke SourceDir generated makefile for RTOS_protegemed.pem4f
RTOS_protegemed.pem4f: .libraries,RTOS_protegemed.pem4f
.libraries,RTOS_protegemed.pem4f: package/cfg/RTOS_protegemed_pem4f.xdl
	$(MAKE) -f C:\Users\skelter\Workspace_PTGMD\Protegemed/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\skelter\Workspace_PTGMD\Protegemed/src/makefile.libs clean

