#ifndef __ftpd_msg_h__
#define __ftpd_msg_h__
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
* $FileName: ftpd_msg.h$
* $Version : 3.8.6.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/
extern const char ftpmsg_sending[];   
extern const char ftpmsg_typeok[];    
extern const char ftpmsg_portok[];    
extern const char ftpmsg_banner[];    
extern const char ftpmsg_bye[];       
extern const char ftpmsg_rxok[];      
extern const char ftpmsg_txok[];      
extern const char ftpmsg_logged[];    
extern const char ftpmsg_pass[];      
extern const char ftpmsg_noconn[];    
extern const char ftpmsg_writefail[]; 
extern const char ftpmsg_writeeof[];  
extern const char ftpmsg_badcmd[];    
extern const char ftpmsg_badtype[];   
extern const char ftpmsg_badport[];   
extern const char ftpmsg_badsyntax[]; 
extern const char ftpmsg_unimp[];     
extern const char ftpmsg_cantopen[];  

extern const char ftpmsg_size[];            
extern const char ftpmsg_syntax_error[];    
extern const char ftpmsg_need_password[];   
extern const char ftpmsg_not_logged[];      
extern const char ftpmsg_goodbye[];         
extern const char ftpmsg_no_fs[];           
extern const char ftpmsg_pasv_mode[];       
extern const char ftpmsg_site_info[];       
extern const char ftpmsg_bad_option[];      
extern const char ftpmsg_syst[];            
extern const char ftpmsg_help_start[];      
extern const char ftpmsg_help_end[];        
extern const char ftpmsg_pwd_error[];       
extern const char ftpmsg_cd_error[];        
extern const char ftpmsg_rmdir_error[];     
extern const char ftpmsg_delete_error[];    
extern const char ftpmsg_mkdir_error[];     
extern const char ftpmsg_ok[];              
extern const char ftpmsg_type_ascii[];      
extern const char ftpmsg_type_image[];      
extern const char ftpmsg_type_unknown[];    
extern const char ftpmsg_no_file[];         
extern const char ftpmsg_opening_datacon[]; 
extern const char ftpmsg_tx_complete[];     
extern const char ftpmsg_no_datacon[];      
extern const char ftpmsg_no_memory[];

#endif
/*EOF*/
