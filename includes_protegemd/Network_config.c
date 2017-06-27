#include <includes_protegemd/Network_config.h>
#include <stdio.h>

extern outlet Outlet_1;

//extern  UART_Handle uart;
//extern  char input;


/*
 *  ======== tcpWorker ========
 *  Task to handle TCP connection. Can be multiple Tasks running
 *  this function.
 */
Void tcpWorker(UArg arg0, UArg arg1)
{
    int clientfd = (int) arg0;
//    int  bytesRcvd;
//    int  bytesSent;
    char buffer[TCPPACKETSIZE];
    //char helloTM4C[] = "\n*-----------------------*\n   Hello from TM4C   \n ";

    System_printf("tcpWorker: start clientfd = 0x%x\n", clientfd);

    recv(clientfd, buffer, TCPPACKETSIZE, 0);

    //
    // Firmware Update Request -> 'U' + MAC address (U=0x55 in ascii)
    // eg.:  0x55 0x00 0x1a 0xb6 0x03 0x06 0x1a
    //
    if ((buffer[0] == 'U') && (buffer[1] == 0x00) && (buffer[2] == 0x1a)
            && (buffer[3] == 0xb6) && (buffer[4] == 0x03) && (buffer[5] == 0x06)
            && (buffer[6] == 0x1a))
    {
        const char UpdateRequest[] = "Firmware Update Request. Reseting...";
        send(clientfd, UpdateRequest, sizeof(UpdateRequest), 0);

        //
        // Disable all processor interrupts.  Instead of disabling them
        // one at a time (and possibly missing an interrupt if new sources
        // are added), a direct write to NVIC is done to disable all
        // peripheral interrupts.
        //
        HWREG(NVIC_DIS0) = 0xffffffff;
        HWREG(NVIC_DIS1) = 0xffffffff;
        HWREG(NVIC_DIS2) = 0xffffffff;
        HWREG(NVIC_DIS3) = 0xffffffff;
        HWREG(NVIC_DIS4) = 0xffffffff;

        //
        // Also disable the SysTick interrupt.
        //
        SysTickIntDisable();
        SysTickDisable();

        //
        // Return control to the boot loader.  This is a call to the SVC
        // handler in the flashed-based boot loader, or to the ROM if configured.
        //
#if ((defined ROM_UpdateEMAC) && !(defined USE_FLASH_BOOT_LOADER))
        ROM_UpdateEMAC(ui32SysClock);
#else
        (*((void (*)(void)) (*(uint32_t *) 0x2c)))();
#endif
    }
    else if (buffer[0] == 'R')
    {
        //char t=0; //numero de tentativas

       // for(t=0;t<3;t++)
        {
        GPIO_write(RFIDPinTXCT, 0); //wake pulse
        SysCtlDelay(2000); //  =~50us
        GPIO_write(RFIDPinTXCT, 1);
        SysCtlDelay(80000); // =~2ms // wait
        GPIO_write(RFIDPinTXCT, 0); // charge and diag.

        SysCtlDelay(2000000); // =~50ms
        GPIO_write(RFIDPinTXCT, 1); //start comm
        SysCtlDelay(2000000);

        }
        //TODO: Put a semaphore here!
        char id[8];
        sprintf(id,"%X%X%X%X%X%X%X%X",Outlet_1.id[7],Outlet_1.id[6],Outlet_1.id[5],Outlet_1.id[4],Outlet_1.id[3],Outlet_1.id[2],Outlet_1.id[1],Outlet_1.id[0]);
        send(clientfd, id, sizeof(id), 0);

    }
    else
    {
        send(clientfd, "Unknown Command", 19, 0);
    }

//    while ((bytesRcvd = recv(clientfd, buffer, TCPPACKETSIZE, 0)) > 0) {
//        bytesSent = send(clientfd, buffer, bytesRcvd, 0);
//        if (bytesSent < 0 || bytesSent != bytesRcvd) {
//            System_printf("Error: send failed.\n");
//            break;
//        }
//    }
    System_printf("tcpWorker stop clientfd = 0x%x\n", clientfd);

    close(clientfd);
}

/*
 *  ======== tcpHandler ========
 *  Creates new Task to handle new TCP connections.
 */
Void tcpHandler(UArg arg0, UArg arg1)
{
    int status;
    int clientfd;
    int server;
    struct sockaddr_in localAddr;
    struct sockaddr_in clientAddr;
    int optval;
    int optlen = sizeof(optval);
    socklen_t addrlen = sizeof(clientAddr);
    Task_Handle taskHandle;
    Task_Params taskParams;
    Error_Block eb;

    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1)
    {
        System_printf("Error: socket not created.\n");
        goto shutdown;
    }

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(arg0);

    status = bind(server, (struct sockaddr *) &localAddr, sizeof(localAddr));
    if (status == -1)
    {
        System_printf("Error: bind failed.\n");
        goto shutdown;
    }

    status = listen(server, NUMTCPWORKERS);
    if (status == -1)
    {
        System_printf("Error: listen failed.\n");
        goto shutdown;
    }

    optval = 1;
    if (setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0)
    {
        System_printf("Error: setsockopt failed\n");
        goto shutdown;
    }

    while ((clientfd = accept(server, (struct sockaddr *) &clientAddr, &addrlen))
            != -1)
    {

        Semaphore_pend(fft_end_Sem, BIOS_WAIT_FOREVER);

        System_printf("tcpHandler: Creating thread clientfd = %d\n", clientfd);

        /* Init the Error_Block */
        Error_init(&eb);

        /* Initialize the defaults and set the parameters. */
        Task_Params_init(&taskParams);
        taskParams.arg0 = (UArg) clientfd;
        taskParams.stackSize = 1280;
        taskHandle = Task_create((Task_FuncPtr) tcpWorker, &taskParams, &eb);
        if (taskHandle == NULL)
        {
            System_printf("Error: Failed to create new Task\n");
            close(clientfd);
        }

        /* addrlen is a value-result param, must reset for next accept call */
        addrlen = sizeof(clientAddr);
    }

    System_printf("Error: accept failed.\n");

    shutdown: if (server > 0)
    {
        close(server);
    }
}

/*
 *  ======== netOpenHook ========
 *  NDK network open hook used to initialize IPv6
 */
void netOpenHook()
{
    Task_Handle taskHandle;
    Task_Params taskParams;
    Error_Block eb;

    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    /*
     *  Create the Task that farms out incoming TCP connections.
     *  arg0 will be the port that this task listens to.
     */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TCPHANDLERSTACK;
    taskParams.priority = 1;
    taskParams.arg0 = TCPPORT;
    taskHandle = Task_create((Task_FuncPtr) tcpHandler, &taskParams, &eb);
    if (taskHandle == NULL)
    {
        System_printf("netOpenHook: Failed to create tcpHandler Task\n");
    }

    System_flush();
}
