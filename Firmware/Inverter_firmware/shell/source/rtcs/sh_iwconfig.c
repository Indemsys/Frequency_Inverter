/**HEADER********************************************************************
* 
* Copyright (c) 2009 Freescale Semiconductor;
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
* $FileName: sh_iwconfig.c$
* $Version : 3.7.8.0$
* $Date    : Feb-24-2011$
*
* Comments:
*
*   Example using RTCS Library.
*
*END************************************************************************/


#include <mqx.h>
#include "shell.h"
#if SHELLCFG_USES_RTCS
#include <bsp.h>
#include <rtcs.h>
#include <enet.h>
#include <ipcfg.h>
#include <iwcfg.h>
#include <enet_wifi.h>
#include <ctype.h>
#include "sh_rtcs.h"
#include "sh_enet.h"
#include "string.h"

#ifdef BSP_ENET_DEVICE_COUNT
#if  (BSP_ENET_DEVICE_COUNT > 0) 

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : Shell_iwconfig_set_ssid
* Returned Value : ERROR code on error else success
* Comments       :
*
*END------------------------------------------------------------------*/
static int_32 Shell_iwconfig_set_ssid (uint_32 enet_device,char_ptr ssid)
{
    uint_32 error;

    error = iwcfg_set_essid (enet_device,ssid);
    if (error != 0)
    {
        printf ("Error during setting of ssid %s error=%08x!\n",ssid, error);
        return SHELL_EXIT_ERROR;
    }
    
    printf ("SSID %s set successful.\n",ssid);
    return SHELL_EXIT_SUCCESS;
}
/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : Shell_iwconfig_set_mode
* Returned Value : ERROR code on error else success
* Comments       :
*
*END------------------------------------------------------------------*/
static int_32 Shell_iwconfig_set_mode (uint_32 enet_device,char_ptr mode)
{
    uint_32 error;

    error = iwcfg_set_mode (enet_device,mode);
    if (error != 0)
    {
        printf ("Error during setting of mode %08x!\n", error);
        return SHELL_EXIT_ERROR;
    }
    
    printf ("mode %s set successful.\n",mode);
    return SHELL_EXIT_SUCCESS;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : Shell_iwconfig_set_mode
* Returned Value : ERROR code on error else success
* Comments       :
*
*END------------------------------------------------------------------*/
static int_32 Shell_iwconfig_commit (uint_32 enet_device)
{
    uint_32 error;

    error = iwcfg_commit (enet_device);
    if (error != 0)
    {
        printf ("Error commiting \n");
        return SHELL_EXIT_ERROR;
    }
    
    printf ("commit successful.\n");
    return SHELL_EXIT_SUCCESS;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : Shell_iwconfig_set_mode
* Returned Value : ERROR code on error else success
* Comments       :
*
*END------------------------------------------------------------------*/
static int_32 Shell_iwconfig_set_wep_key (uint_32 enet_device,int_32 index,int_32 argc,char_ptr key[])
{
    uint_32 error;
    uint_32 i = 0;
    uint_32 flags = 0;
    char_ptr wep_key = NULL;
    uint_32 key_index = 0;
    uint_32 key_len=0;
        
    for (i=index+1;i<argc;i++) 
    {
        if (strcmp(key[i],"none") == 0)
        {
            wep_key = NULL;
            key_len = 0;
            break;
        }
        else if (strncmp(key[i],"[",1) == 0)
        {
          char_ptr ptr = key[i];
          if (!isdigit(*(++ptr)))
          {
            printf("%s key index passed is incorrect\n",key[i]);
            return SHELL_EXIT_ERROR;
          }
          key_index = key_index*10 + (*ptr - '0');
          continue;     
          
        }
        else if (strncmp(key[i],"s:",2) == 0)
        {
            char_ptr data = key[i];
            wep_key = data+2;
            key_len = strlen(data)-2;
            continue;
        }
        else if (strcmp(key[i],"restricted") == 0)
        {
           continue;
        }
        else if (strcmp(key[i],"open") == 0)
        {
            continue;
        }
        else
        {
            //Key is in hexadecimal format.
            printf("WEP Key in hexadecimal format not supported. Pass it as string using s:[password]\n");
            return SHELL_EXIT_ERROR;
        }
        
    }
    error = iwcfg_set_wep_key (enet_device,wep_key,key_len,key_index);
    if (error != 0)
    {
        printf ("Error during setting of key %08x!\n", error);
        return SHELL_EXIT_ERROR;
    }
    
    printf ("key set successful.\n");
    return SHELL_EXIT_SUCCESS;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : Shell_iwconfig_set_sec_type
* Returned Value : ERROR code on error else success
* Comments       :
*
*END------------------------------------------------------------------*/
static int_32 Shell_iwconfig_set_sec_type (uint_32 enet_device,char_ptr sec_type)
{
    uint_32 error;

    error = iwcfg_set_sec_type (enet_device,sec_type);
    if (error != 0)
    {
        printf ("Error during setting of sectype %08x!\n", error);
        return SHELL_EXIT_ERROR;
    }
    
    printf ("sectype %s set successful.\n",sec_type);
    return SHELL_EXIT_SUCCESS;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : Shell_iwconfig_set_passphrase
* Returned Value : ERROR code on error else success
* Comments       :
*
*END------------------------------------------------------------------*/
static int_32 Shell_iwconfig_set_passphrase (uint_32 enet_device,char_ptr passphrase)
{
    uint_32 error;
    char_ptr p_phrase;
   
    
    if (passphrase[0] == '"')
    {
       uint_32 tmp_len = 0;
       p_phrase = passphrase+1;
       tmp_len = strlen(p_phrase);
       p_phrase[tmp_len] = '\0';    
    }
    else 
    {
        p_phrase = passphrase;
    }
        
    error = iwcfg_set_passphrase (enet_device,p_phrase);
    if (error != 0)
    {
        printf ("Error during setting of passphrase %08x!\n", error);
        return SHELL_EXIT_ERROR;
    }
    
    printf ("passphrase %s set successful.\n",p_phrase);
    return SHELL_EXIT_SUCCESS;
}
/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : Shell_iwconfig_set_power
* Returned Value : ERROR code on error else success
* Comments       :
*
*END------------------------------------------------------------------*/
static int_32 Shell_iwconfig_set_power (uint_32 enet_device,int_32 index,int_32 argc,char_ptr argv[])
{
    uint_32 error;
    uint_32 i = 0;
    uint_32 flags = 0;
    uint_32 pow_val=0;
        
    for (i=index+1;i<argc;i++) 
    {
        if (strcmp(argv[i],"off") == 0)
        {
            flags = TRUE;
            break;
        }
        else if (strcmp(argv[i],"on") == 0)
        {
           flags = FALSE;
           break;
        }
        else if (strcmp(argv[i],"period") == 0)
        {
            char_ptr data = argv[i+1];
            uint_32 str_len = strlen(data);
            uint_32 sec_flag;
            if (data[str_len-1] == 's')
            {
              sec_flag = 1;
            }
            else if (data[str_len-1] == 'm')
            {
                sec_flag = 0;
            }
            else
            {
                printf("period supported only in seconds\n");
                return SHELL_EXIT_ERROR;
            }
            
            data[--str_len] = '\0';
  
            while (*data != '\0') 
            {
               if (!isdigit(*data))
               {
                   printf("%s  period value passed is incorrect\n",argv[i+1]);
                   return SHELL_EXIT_ERROR;
               }
          
               pow_val = pow_val*10 + (*data - '0');
               data++;
            }
            i++;
            flags = FALSE;
            if (sec_flag)
               pow_val = pow_val*1000;
            
            break;
        }
        else
        {
            //Key is in hexadecimal format.
            printf("parameter not supported with power command\n");
            return SHELL_EXIT_ERROR;
        }
        
    }
    error = iwcfg_set_power (enet_device,pow_val,flags);
    if (error != 0)
    {
        printf ("Error during setting of key %08x!\n", error);
        return SHELL_EXIT_ERROR;
    }
    
    printf ("power %s set successful.\n",argv[index+1]);
    return SHELL_EXIT_SUCCESS;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : Shell_iwconfig_set_power
* Returned Value : ERROR code on error else success
* Comments       :
*
*END------------------------------------------------------------------*/
static int_32 Shell_iwconfig_set_scan (uint_32 enet_device,int_32 index,int_32 argc,char_ptr argv[])
{
    uint_32 error;
    char_ptr ssid = NULL;
    if (argc > 2)
    {
        ssid = argv[index+1];
    }

    error = iwcfg_set_scan (enet_device, ssid);
    if (error != 0)
    {
        printf ("Error during scan %08x!\n", error);
        return SHELL_EXIT_ERROR;
    }
    
    if (ssid)
      printf ("scan %s done.\n",ssid);
    else
      printf("scan done.\n");
    
    return SHELL_EXIT_SUCCESS;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : Shell_iwconfig
* Returned Value : ERROR code on error else success
* Comments       :
*
*END------------------------------------------------------------------*/

int_32 Shell_iwconfig(int_32 argc, char_ptr argv[] )
{ /* Body */
    boolean                 print_usage, shorthelp = FALSE;
    int_32                  return_code = SHELL_EXIT_SUCCESS;
    uint_32                 enet_device = BSP_DEFAULT_ENET_DEVICE, index = 1;
  
    print_usage = Shell_check_help_request (argc, argv, &shorthelp);

    if (!print_usage)  
    {
        if (argc > index)  
        {
            if (Shell_parse_number (argv[index], &enet_device))
            {
                index++;
            }
        }

        if (enet_device >= BSP_ENET_DEVICE_COUNT)
        {
            printf ("Wrong number of ethernet device (%d)!\n", enet_device);
            return_code = SHELL_EXIT_ERROR;
        }
        else
        {
            if (argc > index)
            {
                if (strcmp (argv[index], "ssid") == 0)
                {
                    return_code = Shell_iwconfig_set_ssid(enet_device,argv[index+1]);
                }
                else if (strcmp (argv[index], "mode") == 0)
                {
                    return_code = Shell_iwconfig_set_mode(enet_device,argv[index+1]);
                }
                else if (strcmp (argv[index], "commit") == 0)
                {
                    return_code = Shell_iwconfig_commit(enet_device);   
                }
                else if (strcmp (argv[index], "key") == 0)
                {
                    return_code = Shell_iwconfig_set_wep_key(enet_device,index,argc,argv);
                }
                else if (strcmp (argv[index], "sectype") == 0)
                {
                    return_code = Shell_iwconfig_set_sec_type(enet_device,argv[index+1]);
                }
                else if (strcmp (argv[index], "passphrase") == 0)
                {
                    return_code = Shell_iwconfig_set_passphrase(enet_device,argv[index+1]);
                }
                else if (strcmp (argv[index], "power") == 0)
                {
                    return_code = Shell_iwconfig_set_power(enet_device,index,argc,argv);
                }
                else if (strcmp (argv[index], "scan") == 0)
                {
                    return_code = Shell_iwconfig_set_scan(enet_device,index,argc,argv);
                }

                else if (strcmp (argv[index], "help") == 0)
                {
                    return_code = SHELL_EXIT_ERROR; 
                }
                else
                {
                    printf ("Unknown iwconfig command!\n");
                    return_code = SHELL_EXIT_ERROR;
                }
            }
            else if (argc == 1)
            {
                char data[32];
                uint_32 error;
                error = iwcfg_get_essid (enet_device,(char_ptr)data);
                if (error != 0)
                   return_code = SHELL_EXIT_ERROR;
                
                printf("ssid=%s\n",data);
                iwcfg_get_mode (enet_device,(char_ptr)data);
                printf("mode=%s\n",data);
                iwcfg_get_sectype (enet_device,(char_ptr)data);
                printf("security type=%s\n",data);
                if (strcmp(data,"wep") == 0)
                {
                    uint_32 def_key_index;
                    iwcfg_get_wep_key(enet_device,(char_ptr)data,&def_key_index);
                    printf ("wep key=%s\n",data);
                    printf ("default key index=%d\n",def_key_index);
                }
                if (strcmp(data,"wpa") == 0)
                {
                    uchar p_phrase[65];
                    iwcfg_get_passphrase(enet_device,(char_ptr)p_phrase);
                    printf ("   passphrase=%s\n",p_phrase);
                }   
                if (strcmp(data,"wpa2") == 0)
                {
                    uchar p_phrase[65];
                    iwcfg_get_passphrase(enet_device,(char_ptr)p_phrase);
                    printf ("   passphrase=%s\n",p_phrase);
                }                           
            }
        }
    }
    else
    {
        return_code = SHELL_EXIT_ERROR;
    }

    if ((print_usage) || (return_code != SHELL_EXIT_SUCCESS))
    {
        if (shorthelp)
        {
            printf ("%s [<device>] [<command>]\n", argv[0]);
        } 
        else  
        {
            printf ("Use iwconfig commands if Wifi device is initialized\n");
            printf ("Usage: %s [<command>]\n", argv[0]);
            printf ("  Commands:\n");
            printf ("    ssid   [<network id>]   = Set SSID on device to associate with AP\n");
            printf ("    mode   [<network mode>] = Set network mode on device\n");          
            printf ("    commit                  = Forces the card to apply all pending changes.\n");                       
            printf ("    key [<WEP Key>],[<key index>]  = Sets WEP Keys and enables WEP security\n");
            printf ("    passphrase [<passphrase string>] = Enables WPA2 security and sets the passphrase\n");
            printf ("    sectype [<security type>]= Sets the Security for WiFi Device    \n");
            printf ("    power   [<on>] [<off>] [period] [<value>] = Enables or disables the power mode and sets the sleep duration.\n");
            printf ("    scan                   = Scan for Access Points and display there infrmation\n");
            printf ("    help                   = Display commands list and parameters.\n");
            printf ("  Parameters:\n");
            printf ("    <network id>   = name of the wireless network\n");
            printf ("    <network mode> = type of Wireless network. Possible values: managed or adhoc\n");
            printf ("    <WEP Key>      = WEP Security Key. If set to none, WEP security is disabled.\n");
            printf ("    <key index>    = Four keys can be set as WEP keys.Set index as [1],[2],[3],[4]\n");
            printf ("                   = To change which key is the currently active key, just enter [index]\n");
            printf ("                   = (without entering any key value).\n");
            printf ("    <security type>= wep,wpa,wpa2 or none\n");
            printf ("    <passphrase string> = pass phrase string\n");
            printf ("    <on>                = enables power mode\n");
            printf ("    <off>               = disables power mode\n");
            printf ("    <value>             = sleep duration\n");
        }
    }

    return return_code;
} /* Endbody */

#else /* (BSP_ENET_DEVICE_COUNT > 0) */
int_32 Shell_iwconfig(int_32 argc, char_ptr argv[] )
{
   printf ("Cannot use this command, no enet device driver available in this BSP.");
   return SHELL_EXIT_ERROR;
}
#endif /* (BSP_ENET_DEVICE_COUNT > 0) */
#else  /* BSP_ENET_DEVICE_COUNT */
int_32 Shell_iwconfig(int_32 argc, char_ptr argv[] )
{
   printf ("Cannot use this command, no enet device driver available in this BSP.");
   return SHELL_EXIT_ERROR;
}
#endif /* BSP_ENET_DEVICE_COUNT */
#endif /* SHELLCFG_USES_RTCS */
/* EOF */
