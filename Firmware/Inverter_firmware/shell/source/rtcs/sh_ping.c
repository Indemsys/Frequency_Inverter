/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: sh_ping.c$
* $Version : 3.8.13.0$
* $Date    : Sep-11-2012$
*
* Comments:
*
*   This file contains the RTCS shell ping commnad.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <mqx.h>
#include "shell.h"

#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include "sh_rtcs.h"
#include <enet.h>

typedef struct shell_ping_parameter
{
	const char_ptr  parameter;
   	int_32          value;
} SHELL_PING_PARAMETER, _PTR_ SHELL_PING_PARAMETER_PTR; 


static int_32 shell_ping_take_value(int_32_ptr value, char_ptr param,char_ptr message )	;

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_ping
*  Returned Value:  none
*  Comments  :  SHELL utility to ping a host.
*
*END*-----------------------------------------------------------------*/
int_32  Shell_ping(int_32 argc, char_ptr argv[] )
{
   	uint_32         i;
    uint_32         x;
   	uint_16         pingid = 0;
   	boolean         print_usage;
   	boolean         shorthelp = FALSE;
   	int_32          return_code = SHELL_EXIT_SUCCESS;
	uint_32         error_flag=0;
    int_32          time;
    int_32          error=0;
    char            addr_str[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"];
    char            *ping_send_buffer=NULL;
    struct addrinfo hints;          /* Used for getaddrinfo()*/
    struct addrinfo *addrinfo_res;  /* Used for getaddrinfo()*/

    /* ping default parameters. */
    SHELL_PING_PARAMETER ping_params[] =
    {
        { "-n", 0	},  /* Resolve or not numeric IP addresses.*/ /* Ignored.*/
        { "-c", 4 	},  /* Amount of ping.*/
        { "-h", 64	},  /* Max hops for IPv6 , or TTL for IPv4.*/
        { "-i", 1000},  /* Wait interval seconds between sending each packet.*/
        { "-p", 0xff }, /* Byte pattern for data of paket.*/
        { "-s", 56	}   /* number of data bytes to be sent.*/
    };
    #define SHELL_PING_PARAMETERS_NUMBER (sizeof(ping_params)/sizeof(SHELL_PING_PARAMETER))


    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)
    {
        if (argc >= 2)
        {
            if (argc > 2)
            {
                i=1;
                while(i < argc)
                {/* Checking all input parameters.*/
                    x=0;
                    error_flag=0;   /*=0 - no error for unknown parameters.*/
                    while(x < SHELL_PING_PARAMETERS_NUMBER)
                    {/* Looking for valid parameters.*/
                        if((strcmp(argv[i],ping_params[x].parameter))==0)
                        {/* If we found something.*/
                            switch (x)
                            {
                                case (0): 
                                    ping_params[x].value = 1;
                                    printf("num output only\n");
                                    error_flag = 0;
                                    break; /* Ignored.*/
                                case (1):
                                    i++;
                                    error_flag = shell_ping_take_value(&(ping_params[x].value), argv[i], "count");       /* Amount of ping.*/
                                    break;
                                case (2):
                                    i++;
                                    error_flag = shell_ping_take_value(&(ping_params[x].value), argv[i], "hoplimit");    /* Something like TTL(time to live)*/
                                    break;
                                case (3):
                                    i++;
                                    error_flag = shell_ping_take_value(&(ping_params[x].value), argv[i], "p_wait");      /* Time interval between two packets.*/
                                    ping_params[x].value = ping_params[x].value * 1000;
                                    break;
                                case (4):
                                    i++;
                                    error_flag = shell_ping_take_value(&(ping_params[x].value), argv[i], "pattern");     /* What digit we will use for packets data.*/
                                    break;
                                case (5):
                                    i++;
                                    error_flag = shell_ping_take_value(&(ping_params[x].value), argv[i], "packet size"); /* Size of packet.*/
                                    break;
                                default:
                                    error_flag=0;
                            }/* switch (x) */
                        }/* if((strcmp(argv[i],ping6_param[x].parameter))==0) */
                        x++;
                    }/* while(x < SHELL_PING_PARAMETERS_NUMBER) */
                    i++;
                }/* while(i<argc) */
            } /* if (argc > 2) */
           
            /* TBD: handle error_flag */

            /* Extract IP address and detect family, here we will get scope_id too. */
            memset(&hints,0,sizeof(hints));
            hints.ai_family 	= AF_UNSPEC;
            hints.ai_socktype 	= SOCK_DGRAM;
            hints.ai_flags 		= AI_CANONNAME;
            if (getaddrinfo(argv[argc-1], NULL, &hints, &addrinfo_res) != 0)
            {
                printf("GETADDRINFO error\n");
                return(SHELL_EXIT_ERROR); /* We can return right here and do not need free freeaddrinfo(addrinfo_res)*/
            }

            /* Prepare ping */
            if(inet_ntop(addrinfo_res->ai_family,&((struct sockaddr_in6 *)((*addrinfo_res).ai_addr))->sin6_addr, addr_str, sizeof(addr_str)))
			{
                /* Allocate data buffer for ping. */
                ping_send_buffer = RTCS_mem_alloc(ping_params[5].value); /* ping6_param[5].value - value of bytes in packet */
                if(ping_send_buffer)
                {
                    /* Set value of bytes of data buffer in according with patterns
                     * ping6_param[4].value - patterns.
                     * ping6_param[5].value - size of buffer.
                     */
                    memset(ping_send_buffer, ping_params[4].value, ping_params[5].value);
                }
                else
                {
                    printf("Ping error - can not allocate ping buffer. Size is too big.\n");
                    return_code = SHELL_EXIT_ERROR;
                    goto EXIT_1;
                }
			}
			else
			{
		  	    printf("Address parameter is wrong.\n");
                return_code = SHELL_EXIT_ERROR;
                goto EXIT_1;
			}
            
            printf("Pinging %s:\n", addr_str);
            printf("Press [q] to cancel.\n", addr_str);

            /* Start of sending of packets. */
            for (i=0; i< ping_params[1].value; i++) /* Amount of how much packets we will send.*/
            {

                if (fstatus(stdin))
                { 
                    uchar c = fgetc(stdin); 
                    if (tolower(c)=='q')
                    { 
                        printf("\n"); 
                        goto EXIT_1; 
                    } 
                } 

                time = ping_params[3].value; /* Maximum waiting time for answer */

            #if RTCSCFG_ENABLE_IP4
                if(addrinfo_res->ai_family==AF_INET)
                {   /* IPv4 Ping. */
                     error = RTCS_ping( ((sockaddr_in *)(addrinfo_res->ai_addr))->sin_addr.s_addr, 
                                        (uint_32_ptr)&time,
                                        ++pingid,
                                        (uint_8)ping_params[2].value,
                                        ping_send_buffer,
                                        ping_params[5].value );
                }
                else 
            #endif
            #if RTCSCFG_ENABLE_IP6
                if(addrinfo_res->ai_family==AF_INET6)
                {   /* IPv6 Ping. */
                    error = RTCS6_ping( addrinfo_res->ai_addr,
                                        (uint_32_ptr)&time,
                                        ++pingid,
                                        (uint_8)ping_params[2].value,
                                        ping_send_buffer,
                                        ping_params[5].value );
                }
                else
            #endif
                {   /* Exit if inet family is unknown. */
                    printf("Ping error - %d is not supported inet family\n",addrinfo_res->ai_family);
                    return_code = SHELL_EXIT_ERROR;
                    goto EXIT_1;
                }


                if (error)
                {
                    if (error == RTCSERR_ICMP_ECHO_TIMEOUT)
                    {
                        printf("Request timed out\n");
                    }
                    else
                    {
                    #if BSP_ENET_DEVICE_COUNT > 0
                        printf("Error 0x%04lX - %s\n", error,  ENET_strerror(error));
                    #else
                        printf("Error 0x%04lX \n", error);
                    #endif
                    }
                }
                else
                {
                    if (time < 1)
                    {
                        printf("Reply from [%s]: time<1ms\n", addr_str);
                    }
                    else
                    {
                        printf("Reply from [%s]: time=%ldms\n", addr_str, time);
                    }
                }
                
                /* Possible timeout*/
                if ((time < ping_params[3].value) && (i<(ping_params[1].value-1)))
                {
                    RTCS_time_delay(ping_params[3].value - time); /* ping6_param[3].value - time interval between two packets*/
                }
            } 

            error = _mem_free(ping_send_buffer);
            if( error != MQX_OK)
            {
                printf("Ping buffer mem free error - %d \n", error);
                return_code = SHELL_EXIT_ERROR;
                goto EXIT_1;
            }
        }/* if (argc >= 2) */
        else
        {
            printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
            print_usage = TRUE;
        }
    }

    if (print_usage)
    {
        if (shorthelp)
        {
            printf("%s [-c count] [-h hoplimit] [-i interval] [-p pattern] [-s size] <host>\n", argv[0]);
        }
        else
        {
            printf("Usage: %s [-c count][-h hoplimit][-i wait][-p pattern][-s size] <host>\n", argv[0]);
        #if 0 /* TBD */
            printf("   -n numeric output only. (IPv6) \n");
        #endif
            printf("   -c Stop after sending count packets.\n");
            printf("   -h Set IPv6 hoplimit, or IPv4 TTL.\n");
            printf("   -i Wait interval seconds between sending each packet.\n");
            printf("   -p Byte pattern for data of paket.\n");
            printf("   -s Number of data bytes to be sent.\n");
        }
        goto EXIT_2;
    }

EXIT_1:
    freeaddrinfo(addrinfo_res);
EXIT_2:
    return return_code;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  shell_ping_take_value
*  Returned Value:  Error.
*  Comments  :  Convert sring to paraneter value.
*
*END*-----------------------------------------------------------------*/
static int_32 shell_ping_take_value(int_32_ptr value, char_ptr param, char_ptr message )	
{
    if((strcmp(param,"0"))==0)
    {
        *value = 0;
        printf("num. of %s= %d\n", message, (*value));

        return(0);
    }
    else
    {
        *value = _io_atoi(param);

        if(!(*value))
        {/* it can not to be 0 */
            printf("%s is not valid number of %s\n",param,message);
            return(1);
        }	
        else
        {
            printf("num. of %s= %d\n",message,(*value));
            return(0);
        }
    }
}




#endif /* SHELLCFG_USES_RTCS */

/* EOF */
