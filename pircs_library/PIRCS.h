/* =====================================================================================
MIT License

Copyright (c) 2020 Public Invention

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 *
 *       Filename:  PIRCS.h
 *
 *    Description:
 *
 *        Version:  1.1
 *        Created:  04/07/2020 10:35:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Robert L. Read read.robert@gmail.com
 *   Organization:  Public Invention
 *        License:  MIT
 *
 * =====================================================================================
 */

#ifndef PIRCS_H
#define PIRCS_H

#include <inttypes.h>



// struct containing SetCoommandValues (fixed size 7 bytes)
typedef struct SetCommand
{
  char     command; // This will be a C
  char     parameter; // which param to set
  char     interpretation; // the interprestation of the param (typically mimn, max, or target)
  char     modifier;
  int32_t  val;
} SetCommand;



typedef struct Acknowledgement
{
  char     ack; // "S" means success
  char padding0; // we want the 32 bit values to end on word boundaries!
  char padding1;
  char padding2;
  uint32_t  err;
  char     command;
  char     parameter;
  char     interpretation;
  char     modifier;
  int32_t  val;
} Acknowledgement;


/* Fill the byte buffer with a PIRCS-standard bytes from the
   SetCommand Object */
uint16_t fill_byte_buffer_set_command(SetCommand* s,uint8_t* buff,uint16_t blim);

SetCommand get_set_command_from_buffer(uint8_t* buff,uint16_t blim);

uint16_t fill_JSON_buffer_set_command(SetCommand* s,char* buff,uint16_t blim);

SetCommand get_set_command_from_JSON(char* buff,uint16_t blim);

Acknowledgement get_success_ack_from_command(SetCommand *c);
Acknowledgement get_error_ack_from_command(SetCommand *c, char e, uint32_t err_no);

Acknowledgement get_ack_from_JSON_buffer(char *buff,uint16_t size);
uint16_t fill_JSON_buffer_with_ack(Acknowledgement *ack,
                                    char *buff,
                                    uint16_t size);

// Return true (1) if the command matches the acknowledgement,
// whether or not it is a succes!!!
uint8_t match_PIRCS_com_and_ack(SetCommand *com,Acknowledgement *ack);

// Return true if the command matches AND the acknowledgment is a
// success.
uint8_t match_successful_PIRCS_com_and_ack(SetCommand *com,Acknowledgement *ack);


#endif
