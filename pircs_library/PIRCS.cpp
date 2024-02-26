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

Note: This is a C file but is named .cpp to be loadable as an
Arduino libary! However, it is compiled (server side, for example)
as a C file.

 *
 *       Filename:  PIRCS.cpp
 *
 *    Description: Basic self-contained utilities for the PIRCS standard
 *
 *        Version:  1.1
 *        Created:  05/07/2020 10:35:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Robert L. Read
 *   Organization:  Public Invention
 *        License:  MIT
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include "PIRCS.h"


#ifndef htons

#define htons(x) ( (((x)<<8)&0xFF00) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)

#endif // !defined(htons)


// From: https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
// These are oddly named to avoid name collision without having to add
// name spaces here
char *trimwhitespace_PIRCS(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}
// Assume double quoted, simply remove the first ant last character.
char *trimquotes_PIRCS(char *str)
{
  // Trim first quote...
  str++;

  // Trim trailing space
  str[strlen(str) - 1] = '\0';
  return str;
}



/* Fill the byte buffer with a PIRCS-standard bytes from the
   SetCommand Object */
uint16_t fill_byte_buffer_set_command(SetCommand* s,uint8_t* buff,uint16_t blim) {
  buff[0] = s->command;
  buff[1] = s->parameter;
  buff[2] = s->interpretation;
  buff[3] = s->modifier;
  *((int32_t *) &buff[4]) = htonl(s->val);
  return 0;
}

SetCommand get_set_command_from_buffer(uint8_t* buff,uint16_t blim) {
  SetCommand m;
  m.command = buff[0];
  m.parameter = buff[1];
  m.interpretation = buff[2];
  m.modifier = buff[3];
  m.val = ntohl(*((int32_t *) &buff[4])) ;
  return m;
}

uint16_t fill_JSON_buffer_set_command(SetCommand* s,char* buff,uint16_t blim) {
  uint16_t rval = sprintf(buff,
  "{\"com\":\"%c\",\"par\":\"%c\",\"int\":\"%c\",\"mod\":\"%c\",\"val\":%ld}",
                     s->command,
                     s->parameter,
                     s->interpretation,
                     s->modifier,
                     s->val);
  return rval;
}



int assign_value_set_command(SetCommand *s,char* k, char*v) {
  // First strip the key...
  char *stripped_key = trimwhitespace_PIRCS(k);
  char *stripped_value = trimwhitespace_PIRCS(v);
  if (0 == strcmp(stripped_key,"\"com\"")) {
    s->command = stripped_value[1];
  } else
  if (0 == strcmp(stripped_key,"\"par\"")) {
    s->parameter = stripped_value[1];
  } else
  if (0 == strcmp(stripped_key,"\"int\"")) {
    s->interpretation = stripped_value[1];
  } else
  if (0 == strcmp(stripped_key,"\"mod\"")) {
    s->modifier = stripped_value[1];
  } else
  if (0 == strcmp(stripped_key,"\"val\"")) {
    s->val = atoi(stripped_value);
  } else {
    fprintf(stderr,"Key Error: %s",stripped_key);
    return 0;
  }
  return 1;
}

int assign_value_ack(Acknowledgement *a,char* k, char*v) {
  // First strip the key...
  char *stripped_key = trimwhitespace_PIRCS(k);
  char *stripped_value = trimwhitespace_PIRCS(v);
  if (0 == strcmp(stripped_key,"\"ack\"")) {
    a->command = stripped_value[1];
  } else
  if (0 == strcmp(stripped_key,"\"err\"")) {
    a->command = atoi(stripped_value);
  } else
  if (0 == strcmp(stripped_key,"\"com\"")) {
    a->command = stripped_value[1];
  } else
  if (0 == strcmp(stripped_key,"\"par\"")) {
    a->parameter = stripped_value[1];
  } else
  if (0 == strcmp(stripped_key,"\"int\"")) {
    a->interpretation = stripped_value[1];
  } else
  if (0 == strcmp(stripped_key,"\"mod\"")) {
    a->modifier = stripped_value[1];
  } else
  if (0 == strcmp(stripped_key,"\"val\"")) {
    a->val = atoi(stripped_value);
  } else {
    fprintf(stderr,"Key Error: %s",stripped_key);
    return 0;
  }
  return 1;
}


// Note: This is a VERY weak and specialized JSON parser
// This will fail if a string contains a colon!!!
SetCommand get_set_command_from_JSON(char* buff,uint16_t blim) {
  int i = 0;
  SetCommand m;
  //  char *scratch = strtok(buff, "{,:}");

  // eat the first brace...
  //  scratch = strtok(NULL, "{,:}");
  //  fprintf(stderr,"scratch %s\n",scratch);
  //  fprintf(stderr,"buff %s\n",buff);
  for(i = 0; i < 4; i++) {
    char *k = strtok((i == 0) ? buff : NULL, "{,:}");
    char *v = strtok(NULL, "{,:}");
    assign_value_set_command(&m,k,v);
  }
  char *k = strtok(NULL,"{,:}");
  char *v = strtok(NULL,"{,:}");
  assign_value_set_command(&m,k,v);
  return m;
}


Acknowledgement get_raw_ack_from_command(SetCommand *m) {
  Acknowledgement ack;
  ack.command = m->command;
  ack.parameter = m->parameter;
  ack.interpretation = m->interpretation;
  ack.modifier = m->modifier;
  ack.val = m->val;
  return ack;
}

// TODO: Change this to a pointer; we
// don't want to pass structures!
Acknowledgement get_success_ack_from_command(SetCommand *m) {
  Acknowledgement ack = get_raw_ack_from_command(m);
  ack.ack = 'S';
  ack.err = 0;
  return ack;
}
Acknowledgement get_ack_from_JSON_buffer(char *buff,uint16_t size) {
  int i = 0;
  Acknowledgement ack;
  for(i = 0; i < 6; i++) {
    char *k = strtok((i == 0) ? buff : NULL, "{,:}");
    char *v = strtok(NULL, "{,:}");
    assign_value_ack(&ack,k,v);
  }
  char *k = strtok(NULL,"{,:}");
  char *v = strtok(NULL,"{,:}");
  assign_value_ack(&ack,k,v);
  return ack;
}

// TODO: Change this to a pointer; we
// don't want to pass structures!
Acknowledgement get_error_ack_from_command(SetCommand *c,
                                           char e, uint32_t err_no) {
  Acknowledgement ack = get_raw_ack_from_command(c);
  ack.ack = e;
  ack.err = err_no;
  return ack;
}

uint16_t fill_JSON_buffer_with_ack(Acknowledgement *ack,
                                    char *buff,
                                    uint16_t size) {
  uint16_t rval = sprintf(buff,
                          "{\"ack\":\"%c\",\"err\":\"%d\",\"com\":\"%c\",\"par\":\"%c\",\"int\":\"%c\",\"mod\":\"%c\",\"val\":%ld}",
                          ack->ack,
                          ack->err,
                     ack->command,
                     ack->parameter,
                     ack->interpretation,
                     ack->modifier,
                     ack->val);
  return rval;
}
uint8_t match_PIRCS_com_and_ack(SetCommand *com,Acknowledgement *ack) {

  return
    (ack->command == com->command) &&
    (ack->parameter == com->parameter) &&
    (ack->interpretation == com->interpretation) &&
    (ack->modifier == com->modifier) &&
    (ack->val == com->val);

}

uint8_t match_successful_PIRCS_com_and_ack(SetCommand *com,Acknowledgement *ack) {
  return
    ack->ack == 'S' &&
    ack->err == 0 &&
    match_PIRCS_com_and_ack(com,ack);
}
