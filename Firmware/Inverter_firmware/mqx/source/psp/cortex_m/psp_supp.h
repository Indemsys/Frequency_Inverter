/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor
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
* $FileName: psp_supp.h$
* $Version : 3.8.7.0$
* $Date    : Jun-7-2012$
*
* Comments:
*   This file contains the support functions for byte/word and others 
*   manipulations.
*
*END************************************************************************/
#ifndef __PSP_SUPP_H__
#define __PSP_SUPP_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ASM__

#if defined(__CODEWARRIOR__)

uint_16 _psp_swap2byte(uint_16 n);
uint_32 _psp_swap4byte(uint_32 n);
void __set_BASEPRI(uint_32 basePri);

#define _PSP_SWAP2BYTE(n)   _psp_swap2byte(n)
#define _PSP_SWAP4BYTE(n)   _psp_swap4byte(n)

#elif defined(__ICCARM__)

#define _PSP_SWAP2BYTE(n)   (uint_16)__REVSH(n)
#define _PSP_SWAP4BYTE(n)   __REV(n)

#else

uint_16 _psp_swap2byte(uint_16 n);
uint_32 _psp_swap4byte(uint_32 n);

#define _PSP_SWAP2BYTE(n)   _psp_swap2byte(n)
#define _PSP_SWAP4BYTE(n)   _psp_swap4byte(n)

#endif //__CODEWARRIOR__, __ICCARM__

#endif // __ASM__

#ifdef __cplusplus
}
#endif

#endif
