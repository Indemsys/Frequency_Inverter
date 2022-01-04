/**HEADER********************************************************************
*
* Copyright (c) 2012 Freescale Semiconductor;
* All Rights Reserved

*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: sh_ppp.c$
* $Version : 3.8.10.0$
* $Date    : Oct-2-2012$
*
* Comments:
*
*   This file contains the RTCS shell command related to PPP connection.
*
*END************************************************************************/
#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include "shell.h"
#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include "sh_rtcs.h"
#include <ppp.h>
#include <fio.h>


SHELL_PPP_LINK default_ppp;

static boolean   PPP_link = FALSE;
static void PPP_linkup (pointer lwsem)
{
    PPP_link = TRUE;
    _lwsem_post(lwsem);
    printf( " PPP link On \n");
}
static void PPP_linkdown (pointer lwsem)
{
    PPP_link = FALSE;
    _lwsem_post(lwsem);
    printf( " PPP link Off \n");
}
extern PPP_SECRET_PTR _PPP_PAP_LSECRET;
extern PPP_SECRET_PTR _PPP_PAP_RSECRETS;
extern char_ptr       _PPP_CHAP_LNAME;
extern PPP_SECRET_PTR _PPP_CHAP_LSECRETS;
extern PPP_SECRET_PTR _PPP_CHAP_RSECRETS;

void   shell_ppp_start(SHELL_PPP_LINK_PTR ppp_current);
int_32 shell_ppp_stop(SHELL_PPP_LINK_PTR ppp_current);

/*FUNCTION*------------------------------------------------
*
* Function Name: Shell_ppp
* Comments     : Shell command "ppp" is starting or stopping PPP connection.
*
*END*-----------------------------------------------------*/
int_32 Shell_ppp(int_32 argc, char_ptr argv[] )
{
    uint_32           error;
    uint_32           i=1;
    boolean           print_usage, shorthelp = FALSE;

   default_ppp.LOCAL_NAME     = NULL;
   default_ppp.LOCAL_PASSWORD = NULL;

   /* Check for help. */
    print_usage = Shell_check_help_request(argc, argv, &shorthelp );
    if (!print_usage)
    {
        if (argc >= 2)
        {  /* If we have more args, we need parce them. */
            while(i<argc)
            {
                switch (i)
                {
                    case (1):
                        /* If "stop" start shell_ppp_stop. */
                        if( strcmp(argv[i],"stop")==0)
                        {
                            return(shell_ppp_stop(&default_ppp));
                        }
                        else if(strcmp(argv[i],"client")==0)
                        {
                            default_ppp.SERVER_FLAG=0;
                        }
                        else if(strcmp(argv[i],"server")==0)
                        {
                            default_ppp.SERVER_FLAG=1;
                            /* In case of "server" we need have min 7 arg : ppp server ittyX: rlogin rpassword boardip clientip */
                            if(argc <=6)
                            {
                                print_usage = TRUE;
                            }
                        }
                        else
                        {
                            print_usage = TRUE;
                        }
                        break;
                    case (2):
                        if( strstr(argv[i],"itty") !=0)
                        {
                            default_ppp.PPP_CONNECTION_DEVICE = argv[i];
                        }
                         else
                        {
                            printf("Incorrect ittyX: device name.\n");
                            print_usage = TRUE;
                        }
                        break;
                    case (3):
                        if(default_ppp.SERVER_FLAG==0)
                        {
                            default_ppp.LOCAL_NAME = argv[i];
                        }
                        else
                        {
                            default_ppp.REMOTE_NAME = argv[i];
                        }
                        break;
                    case (4):
                        if(default_ppp.SERVER_FLAG==0)
                        {
                            default_ppp.LOCAL_PASSWORD = argv[i];
                        }
                        else
                        {
                            default_ppp.REMOTE_PASSWORD = argv[i];
                        }
                        break;
                    case (5):
                        if(default_ppp.SERVER_FLAG==1)
                        {  /* It is valid only for server mode.*/
                          if( inet_pton(AF_INET, argv[i],&default_ppp.PPP_LOCAL_ADDRESS,sizeof(default_ppp.PPP_LOCAL_ADDRESS)) == (uint_32)RTCS_ERROR)
                            {
                                print_usage = TRUE;
                            }
                        }
                        else
                        {
                            print_usage = TRUE;
                        }
                        break;
                    case (6):
                        if(default_ppp.SERVER_FLAG==1)
                        {  /* It is valid only for server mode.*/
                            if( inet_pton(AF_INET, argv[i],&default_ppp.PPP_REMOTE_ADDRESS,sizeof(default_ppp.PPP_REMOTE_ADDRESS)) ==(uint_32)RTCS_ERROR)
                            {
                                print_usage = TRUE;
                            }
                        }
                        else
                        {
                            print_usage = TRUE;
                        }
                        break;
                    default:
                        error = 0;
                        break;
                }
                i++;
            }
        }
        else
        {
            print_usage = TRUE;
        }
    }

    if (print_usage)
    {
        if (shorthelp)
        {
            printf(" %s - Start PPP connection. \n", argv[0]);
            return RTCS_OK;
        }
        else
        {
            printf(" Usage: %s\n", argv[0]);
            printf(" [param] [device name] [login] [password] [localIPaddress ] [remoteIPaddress]\n");
            printf(" [param]           - is \"server\", \"client\" \"stop\"\n");
            printf(" [device name]     - is system name of interrupting device,\n");
            printf("                     suitable for your board, like  ittyX: \n");
            printf(" [login] and\n");
            printf(" [password]        - is \"login\" and \"password\" for PPP PAP.\n");
            printf("                     For \"server\" login and pass should be defined. \n");
            printf("                     For \"server\" and \"client\" the device name is requered.\n");
            printf(" [localIPaddress]  - is IP addres of your board(server mode).\n");
            printf(" [remoteIPaddress] - is IP addres for  client of your board(server mode).\n");
            printf("\n Examples:\n");
            printf("    To start PPP client using \"ittyd:\" you need type:\n");
            printf("        shell> ppp client ittyd: \n");
            printf("    or using PPP PAP client:\n");
            printf("        shell> ppp client ittyd: yourlogin yourpassword \n");
            printf("\n    To start PPP PAP server using \"ittyd:\":\n");
            printf("        shell> ppp server ittyd: yrlog yrpass brd_ip_addr cl_ip_addr\n");
            printf("        shell> ppp server ittyd: guest anon 192.168.0.1 192.168.0.217\n");
            printf("\n    To stop PPP connection you need type \"ppp stop\":\n");
            printf("        shell> ppp stop\n\n");
            return RTCS_OK;
        }
    }
   /* Check if PPP already started. */
    if(default_ppp.PPP_HANDLE)
    {
        printf("\n PPP already started. Stop it first.\n");
        return RTCS_OK;
    }
   /* Start connection:   */
   /* Open serial device. */
    default_ppp.PPP_CON_DEV_HANDLE = fopen(default_ppp.PPP_CONNECTION_DEVICE, NULL);

   /* Open suitable ppp device. */
    if(default_ppp.SERVER_FLAG==0)
    {
        /* Open PPP like client. */
        default_ppp.PPP_DEV_HANDLE = fopen("dcn:", (char_ptr)default_ppp.PPP_CON_DEV_HANDLE);
        /* Checking, does it open ? */
        if(default_ppp.PPP_DEV_HANDLE == NULL)
        {
            if(default_ppp.PPP_CON_DEV_HANDLE)
            {
                /* Close PPP_DEVICE. */
                fclose(default_ppp.PPP_CON_DEV_HANDLE);
            }
            printf("\n ERROR: Can not open \"dcn\" PPP driver.\n");
            return RTCS_ERROR;
        }
    }
    else
    {
        /* Open PPP like server. */
        default_ppp.PPP_DEV_HANDLE = fopen("dun:", (char_ptr)default_ppp.PPP_CON_DEV_HANDLE);
        /* Checking, does it open ? */
        if(default_ppp.PPP_DEV_HANDLE == NULL)
        {
            /* Checking, was driver installed or not. */
            if(default_ppp.PPP_CON_DEV_HANDLE)
            {
                /* Close Close PPP_DEVICE. */
                fclose(default_ppp.PPP_CON_DEV_HANDLE);
            }
            printf("\n ERROR: Can not open \"dun\" PPP driver.\n");
            return RTCS_ERROR;
        }
    }

    if(default_ppp.SERVER_FLAG==0)
    {
       /* Send invitation string to Windows PPP server. */
        _io_fputs("CLIENTCLIENT", default_ppp.PPP_DEV_HANDLE);
        fflush(default_ppp.PPP_DEV_HANDLE);
       /* Start connection procedure. */
        default_ppp.PPP_GATE_ADDR = IPADDR(192,168,0,1);
    }
    else
    {
        default_ppp.PPP_GATE_ADDR = default_ppp.PPP_LOCAL_ADDRESS;
        /* Fill  rsecrets[] array here. */
         default_ppp.rsecrets[0].PPP_ID_LENGTH = strlen(default_ppp.REMOTE_NAME);
         default_ppp.rsecrets[0].PPP_PW_LENGTH = strlen(default_ppp.REMOTE_PASSWORD);
         default_ppp.rsecrets[0].PPP_ID_PTR    = default_ppp.REMOTE_NAME;
         default_ppp.rsecrets[0].PPP_PW_PTR    = default_ppp.REMOTE_PASSWORD;
         default_ppp.rsecrets[1].PPP_ID_LENGTH = 0;
         default_ppp.rsecrets[1].PPP_PW_LENGTH = 0;
         default_ppp.rsecrets[1].PPP_ID_PTR    = NULL;
         default_ppp.rsecrets[1].PPP_PW_PTR    = NULL;
    }

    shell_ppp_start(&default_ppp);
    /* shell_ppp_start() must get PPP_HANDLE */
    if(default_ppp.PPP_HANDLE)
    {
        return RTCS_OK;
    }
    else
    {  /* Remove all created elements. */
        printf("\n Error: The connection attemp failed because the modem (or other connecting device) on the remote computer is out of order \n");
        /* Destroy semafor. */
        error = _lwsem_destroy(&default_ppp.PPP_SEM);
        if(default_ppp.PPP_IF_HANDLE)
        {
           /* Unregister PPP interface. */
            error = RTCS_if_remove(default_ppp.PPP_IF_HANDLE);
        }
        if(default_ppp.PPP_IO_DRIVER_HANDLE)
        {
           /* Close PPP driver. */
            error = _iopcb_close(default_ppp.PPP_IO_DRIVER_HANDLE);
        }
        if(default_ppp.PPP_DEV_HANDLE)
        {
            /* Close PPP driver. */
            error = fclose(default_ppp.PPP_DEV_HANDLE);
        }
        if(default_ppp.PPP_CON_DEV_HANDLE)
        {
            /* Close Close PPP_DEVICE. */
            error = fclose(default_ppp.PPP_CON_DEV_HANDLE);
        }
        if(error)
        {
          printf("\nSome error happend during closing PPP connection.\n");
        }
        else
        {
          printf("\nPPP connection closed.\n");
        }
        return RTCS_ERROR;
    }
}

/*FUNCTION*------------------------------------------------
*
* Function Name: shell_ppp_stop
* Comments     : Shell command "ppp stop" is calling this function to stop PPP connection.
*
*END*-----------------------------------------------------*/
int_32 shell_ppp_stop(SHELL_PPP_LINK_PTR local_ppp)
{
    uint_32           error;

   /* Check if PPP is alive. */
    if(!local_ppp->PPP_HANDLE)
    {
      printf("Can not stop PPP.It is not started\n");
    }
   /* Cleanup PPP connection. */

   /* Unbind interface. */
    error = RTCS_if_unbind(local_ppp->PPP_IF_HANDLE, IPCP_get_local_addr(local_ppp->PPP_IF_HANDLE));
    if(default_ppp.PPP_HANDLE)
    {
       /* Send connection terminate request to remote server and close PPP connection. */
        error = PPP_close(local_ppp->PPP_HANDLE);

       /* Clean up all PPP structure. */
        error = PPP_shutdown(local_ppp->PPP_HANDLE);
        local_ppp->PPP_HANDLE =NULL;
    }
   /* Destroing the semafor. */
    error = _lwsem_destroy(&local_ppp->PPP_SEM);
    if(local_ppp->PPP_IF_HANDLE)
    {
       /* Unregister PPP interface. */
        error = RTCS_if_remove(local_ppp->PPP_IF_HANDLE);
        local_ppp->PPP_IF_HANDLE=0;
    }
    if(local_ppp->PPP_IO_DRIVER_HANDLE)
    {
       /* Closing PPP driver. */
        error = _iopcb_close(local_ppp->PPP_IO_DRIVER_HANDLE);
    }
    if(local_ppp->PPP_DEV_HANDLE)
    {
        /* Close PPP driver. */
         error = fclose(default_ppp.PPP_DEV_HANDLE);
    }
    if(local_ppp->PPP_CON_DEV_HANDLE)
    {
       /* Close Close PPP_DEVICE. */
        error = fclose(default_ppp.PPP_CON_DEV_HANDLE);
    }
    local_ppp->PPP_HANDLE = NULL;
   /* We need remove route. */
    RTCS_gate_remove(local_ppp->PPP_GATE_ADDR, INADDR_ANY, INADDR_ANY);
    if(error)
    {
      printf("\nSome error happend during closing PPP connection.\n");
      return RTCS_ERROR;
    }
    else
    {
      printf("PPP connection closed\n");
      return RTCS_OK;
    }
}

/*FUNCTION*------------------------------------------------
*
* Function Name: shell_ppp_start()
* Comments     :
*   This function start PPP communication and try to establish PPP connection.
*END*-----------------------------------------------------*/
void shell_ppp_start(SHELL_PPP_LINK_PTR ppp_current)
{
     uint_32           error;
     _ip_address       local_address;
     _ip_address       peer_address=0;
     IPCP_DATA_STRUCT  ipcp_data;

    /* This is configuration for PAP. */
     char* localname     = ppp_current->LOCAL_NAME;
     char* localsecret   = ppp_current->LOCAL_PASSWORD;
     PPP_SECRET lsecret;
    if(default_ppp.SERVER_FLAG==0)
    {   /* Setup  PAP for client mode.*/
         lsecret.PPP_ID_LENGTH = strlen(localname);
         lsecret.PPP_PW_LENGTH = strlen(localsecret);
         lsecret.PPP_ID_PTR    = localname;
         lsecret.PPP_PW_PTR    = localsecret;
        _PPP_PAP_LSECRET   = &lsecret; /* lsecrets  */
        _PPP_PAP_RSECRETS  =  NULL;    /* rsecrets  */
        _PPP_CHAP_LNAME    =  NULL;    /* localname */
        _PPP_CHAP_LSECRETS =  NULL;    /* lsecrets  */
        _PPP_CHAP_RSECRETS =  NULL;    /* rsecrets  */
    }
    else if(default_ppp.SERVER_FLAG==1)
    {   /* Setup  PAP for server mode.*/
        _PPP_PAP_LSECRET   =  NULL;                  /* lsecrets  */
        _PPP_PAP_RSECRETS  =  ppp_current->rsecrets; /* rsecrets  */
        _PPP_CHAP_LNAME    =  NULL;                  /* localname */
        _PPP_CHAP_LSECRETS =  NULL;                  /* lsecrets  */
        _PPP_CHAP_RSECRETS =  NULL;                  /* rsecrets  */
    }
    else
    {
         ppp_current->PPP_HANDLE = NULL;
         return;
    }

   /* Install a route for a default gateway */
    RTCS_gate_add(ppp_current->PPP_GATE_ADDR, INADDR_ANY, INADDR_ANY);

    ppp_current->PPP_IO_DRIVER_HANDLE = _iopcb_ppphdlc_init(ppp_current->PPP_DEV_HANDLE);
    _PPP_ACCM = 0;
    error = PPP_initialize(ppp_current->PPP_IO_DRIVER_HANDLE, &ppp_current->PPP_HANDLE);
    if (error)
    {
       printf("\n PPP initialize: %lx", error);
       _task_block();
    }
    _iopcb_open(ppp_current->PPP_IO_DRIVER_HANDLE, PPP_lowerup, PPP_lowerdown,ppp_current->PPP_HANDLE);
    error = RTCS_if_add(ppp_current->PPP_HANDLE, RTCS_IF_PPP, &ppp_current->PPP_IF_HANDLE);
    if (error)
    {
       printf("\n IF add failed, error = %lx", error);
       _task_block();
    }
    _lwsem_create(&ppp_current->PPP_SEM, 0);
    _mem_zero(&ipcp_data, sizeof(ipcp_data));
    ipcp_data.IP_UP              = PPP_linkup;
    ipcp_data.IP_DOWN            = PPP_linkdown;
    ipcp_data.IP_PARAM           = &ppp_current->PPP_SEM;

    if(default_ppp.SERVER_FLAG==0)
    {
        ipcp_data.ACCEPT_LOCAL_ADDR  = TRUE;
        ipcp_data.ACCEPT_REMOTE_ADDR = TRUE;
    }
    else
    {
        ipcp_data.ACCEPT_LOCAL_ADDR  = FALSE;
        ipcp_data.ACCEPT_REMOTE_ADDR = FALSE;
    }
    ipcp_data.LOCAL_ADDR         = ppp_current->PPP_LOCAL_ADDRESS;
    ipcp_data.REMOTE_ADDR        = ppp_current->PPP_REMOTE_ADDRESS;
    ipcp_data.DEFAULT_NETMASK    = TRUE;
    ipcp_data.DEFAULT_ROUTE      = TRUE;

    error = RTCS_if_bind_IPCP(ppp_current->PPP_IF_HANDLE, &ipcp_data);

    if (error)
    {
       printf("\n IF bind failed, error = %lx", error);
       _task_block();
    }

    /*In case "client" we need wait for connection to server*/
    if(default_ppp.SERVER_FLAG==0)
    {
        printf("\n Please wait initiate PPP connection.  Waiting...\n");
       /*Handshake with RAS server */
       /* 10 attempts to connect */
        error = 0;
        while (error < 10)
        {
            if (PPP_link == FALSE)
            {
                _time_delay(1000);
            }
            else
            {
                printf("\n PPP_link = %d, time = %d\n",PPP_link,error);
                break;
            }
            error ++;
        }

        if (PPP_link == TRUE)
        {
            local_address = IPCP_get_local_addr(ppp_current->PPP_IF_HANDLE);
            peer_address = IPCP_get_peer_addr(ppp_current->PPP_IF_HANDLE);
            printf("\n PPP device on %s is bound on.\n", ppp_current->PPP_CONNECTION_DEVICE );
            printf(" PPP local  address is : %d.%d.%d.%d\n", IPBYTES(local_address));
            printf(" PPP remote address is : %d.%d.%d.%d\n", IPBYTES(peer_address));
            printf(" Now PPP connection is established on %d.%d.%d.%d\n", IPBYTES(peer_address));
        }
        else
        {
            if(ppp_current->PPP_HANDLE)
            {
               /* clean up all PPP structure */
                error = PPP_shutdown(ppp_current->PPP_HANDLE);
            }
            ppp_current->PPP_HANDLE = NULL;
           /* We shoul remove route to clear part block. */
            RTCS_gate_remove(ppp_current->PPP_GATE_ADDR, INADDR_ANY, INADDR_ANY);
        }

    }
    else
    {
       /* In case of "server" we do not need wait at all. */
        printf("\n PPP server ready and waiting for connection.\n");
    }
}
#endif /* SHELLCFG_USES_RTCS */
