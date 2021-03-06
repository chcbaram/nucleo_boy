/*
 * qbuffer.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: HanCheol Cho
 */
#include "hw.h"
#include "cmdif.h"
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>





//-- Internal Variables
//
cmdif_cmd_node_t cmdif_cmd_node[_HW_DEF_CMDIF_LIST_MAX];
cmdif_cmd_t      cmdif_cmd;


static const char *delim = " \f\n\r\t\v";



//-- External Variables
//


//-- Internal Functions
//
void    cmdifResetBuffer(void);
int     cmdifGetCmdString(char *s, int *count);
uint8_t cmdifReadByte(char *p_data);

int parseCommandArgs(char *cmdline, char **argv);
void upperStr( char *Str );

int cmdifCmdExit(int argc, char **argv);
int cmdifCmdShowList(int argc, char **argv);
int cmdifCmdMemoryDump(int argc, char **argv);
int cmdifCmdMemoryWrite32(int argc, char **argv);



//-- External Functions
//





bool cmdifInit(void)
{
  uint32_t i;


  cmdif_cmd.index    = 0;
  cmdif_cmd.err_code = 0;
  cmdif_cmd.init     = true;
  cmdif_cmd.node     = &cmdif_cmd_node[0];


  for(i=0; i<_HW_DEF_CMDIF_LIST_MAX; i++)
  {
    cmdif_cmd.node[i].cmd_str[0] = 0;
    cmdif_cmd.node[i].cmd_func   = NULL;
  }

  cmdifResetBuffer();

  cmdif_cmd.his_count =  0;
  cmdif_cmd.his_index = -1;
  for(i=0; i<CMDIF_CMD_HIS_MAX; i++)
  {
    memset(cmdif_cmd.his_buff, 0x00, CMDIF_CMD_BUF_LENGTH);
  }


  cmdifAdd("help", cmdifCmdShowList);
  cmdifAdd("exit", cmdifCmdExit);

  cmdifAdd("md",   cmdifCmdMemoryDump);
  cmdifAdd("mw32", cmdifCmdMemoryWrite32);


  return true;
}

bool cmdifIsInit(void)
{
  return cmdif_cmd.init;
}

void cmdifBegin(uint8_t ch, uint32_t baud)
{
  cmdif_cmd.ch   = ch;
  cmdif_cmd.baud = baud;

  uartOpen(ch, baud);
}

void cmdifLoop(void)
{
  cmdif_cmd.exit = false;

  while(1)
  {
    cmdifMain();

    if (cmdif_cmd.exit == true)
    {
      break;
    }
  }
}

void cmdifMain(void)
{
  int   cmdlp, argc, ret, count;
  uint32_t cmdchk = 0;
  char *argv[128];

  count = (int)cmdif_cmd.p_read_buffer - (int)cmdif_cmd.read_buffer;

  ret = cmdifGetCmdString(cmdif_cmd.p_read_buffer, &count);

  if(count !=((int)cmdif_cmd.p_read_buffer-(int)cmdif_cmd.read_buffer))
  {
    cmdif_cmd.p_read_buffer = cmdif_cmd.read_buffer+count;
  }

  if(ret ==0)
  {
    cmdifPrintf("\n");
    argc = parseCommandArgs( cmdif_cmd.read_buffer, argv );

    if (argc)
    {
      upperStr( argv[0] );
      cmdlp = 0;
      cmdchk=0;

      while( cmdif_cmd.node[cmdlp].cmd_str[0] )
      {
        if( strcmp( argv[0], cmdif_cmd.node[cmdlp].cmd_str ) == 0 )
        {
          cmdchk++;
          cmdif_cmd.node[cmdlp].cmd_func( argc, argv );
          cmdifPrintf( "\n");
          break;
        }
        cmdlp++;
      }
      if(cmdchk == 0)
      {
        cmdifPrintf("wrong command..\n");
      }
    }
    cmdifResetBuffer();
  }
}

void cmdifAdd(char *cmd_str, int (*p_func)(int argc, char **argv))
{
  uint16_t index;


  if (cmdif_cmd.index >= CMDIF_CMD_STR_MAX || cmdif_cmd.init != true || cmdif_cmd.node == NULL)
  {
    cmdif_cmd.err_code = 1;
    return;
  }

  index = cmdif_cmd.index;


  strcpy(cmdif_cmd.node[index].cmd_str, cmd_str);
  cmdif_cmd.node[index].cmd_func = p_func;

  upperStr(cmdif_cmd.node[index].cmd_str);

  cmdif_cmd.index++;
}

int cmdifCmdExit(int argc, char **argv)
{
  cmdif_cmd.exit = true;

  cmdifPrintf("exit...\n");
  return 0;
}

int cmdifCmdShowList(int argc, char **argv)
{
  int cmdlp = 0;

  cmdifPrintf("\n---------- cmd list ---------\n");
  while( cmdif_cmd.node[cmdlp].cmd_str[0] )
  {
    cmdifPrintf(cmdif_cmd.node[cmdlp].cmd_str);
    cmdifPrintf("\n");
    cmdlp++;
  }
  cmdifPrintf("\n-----------------------------\n");
  return 0;
}

int cmdifCmdMemoryDump(int argc, char **argv)
{
  int idx, size = 16;
  unsigned int *addr;
  int idx1, i;
  unsigned int *ascptr;
  unsigned char asc[4];

  if(argc <2)
  {
    cmdifPrintf(">> md addr [size] \n");
    return -1;
  }

  if(argc>2)
  {
    size = (int) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
  }
  addr   = (unsigned int *)strtoul((const char * ) argv[1], (char **)NULL, (int) 0);
  ascptr = (unsigned int *)addr;

  cmdifPrintf ("\n   ");
  for (idx = 0; idx<size; idx++)
  {
    if((idx%4) == 0)
    {
      cmdifPrintf (" 0x%08X: ", (unsigned int)addr);
    }
    cmdifPrintf (" 0x%08X", *(addr));
    if ((idx%4) == 3)
    {
      cmdifPrintf ("  |");
      for (idx1= 0; idx1< 4; idx1++)
      {
        memcpy((char *)asc, (char *)ascptr, 4);
        for (i=0;i<4;i++)
        {
          if (asc[i] > 0x1f && asc[i] < 0x7f)
          {
            cmdifPrintf ("%c", asc[i]);
          }
          else
          {
            cmdifPrintf (".");
          }
        }
        ascptr+=1;
      }
      cmdifPrintf ("|\n   ");
    }
    addr++;
  }
  cmdifPrintf ("\n");

  return 0;
}

int cmdifCmdMemoryWrite32(int argc, char **argv)
{
  unsigned long *ptrTo;
  int            lp;

  if( argc < 3 )
  {
    cmdifPrintf( "mw32 addr value [value] ...\n");
    return -1;
  }

  ptrTo = (unsigned long *)strtoul( (const char * ) argv[1], (char **)NULL, 0);

  for( lp = 2; lp < argc; lp++ )
  {
    ptrTo[ lp - 2 ] = strtoul( (const char * ) argv[lp], NULL, 0);
  }

  return 0;
}



void cmdifResetBuffer(void)
{
  memset(cmdif_cmd.read_buffer, 0x00, CMDIF_CMD_BUF_LENGTH);
  cmdif_cmd.p_read_buffer = (char *)cmdif_cmd.read_buffer;

  cmdifPrintf("cmdif>> ");
}


void cmdifPrint(char *str)
{
  uartPrint(cmdif_cmd.ch, (uint8_t *)str);
}

void cmdifPrintf(const char *fmt, ...)
{
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[128];
  char print_buffer_out[128];
  uint32_t i;
  uint32_t index;

  len = vsnprintf(print_buffer, 128, fmt, arg);
  va_end (arg);

  index = 0;
  for (i=0; i<len; i++)
  {
    if (print_buffer[i] == '\n')
    {
      print_buffer_out[index++] = '\r';
    }
    print_buffer_out[index++] = print_buffer[i];
  }

  uartWrite(cmdif_cmd.ch, (uint8_t *)print_buffer_out, index);
}

void cmdifPutch(char data)
{
  uartPutch(cmdif_cmd.ch, data);
}

uint8_t cmdifGetch(void)
{
  return uartGetch(cmdif_cmd.ch);
}

uint32_t cmdifRxAvailable(void)
{
  return uartAvailable(cmdif_cmd.ch);
}

uint8_t cmdifReadByte(char *p_data)
{
  uint8_t ret = 0;

  if (uartAvailable(cmdif_cmd.ch) > 0)
  {
    *p_data = uartRead(cmdif_cmd.ch);
    ret = 1;
  }

  return ret;
}




int hisAppend(char *s)
{
  int loop;


  for( loop = 0; loop < CMDIF_CMD_HIS_MAX; loop++ )
  {
    if( strcmp( s, cmdif_cmd.his_buff[loop] ) == 0 )
    {
      cmdif_cmd.his_index = 0;
      if( cmdif_cmd.his_count )
      {
        cmdif_cmd.his_index = cmdif_cmd.his_count - 1;
      }

      return cmdif_cmd.his_count;
    }
  }


  if( cmdif_cmd.his_count < CMDIF_CMD_HIS_MAX )
  {
    strcpy( cmdif_cmd.his_buff[cmdif_cmd.his_count], s );
    cmdif_cmd.his_count++;
  }
  else
  {
    for( loop = 1; loop < CMDIF_CMD_HIS_MAX ; loop++ )
    {
      strcpy( cmdif_cmd.his_buff[loop-1], cmdif_cmd.his_buff[loop] );
    }
    strcpy( cmdif_cmd.his_buff[CMDIF_CMD_HIS_MAX-1], s );
    cmdif_cmd.his_index = cmdif_cmd.his_count - 1;
  }

  if( cmdif_cmd.his_count )
  {
    cmdif_cmd.his_index = cmdif_cmd.his_count-1;
  }
  else
  {
    cmdif_cmd.his_index = 0;
  }

  return cmdif_cmd.his_count;
}

int hisSet(char *s, int index )
{
  int loop;
  int len;

  len = strlen( s );

  for( loop = 0; loop < len; loop++ )
  {
    cmdifPrintf("\b \b");
  }

  strcpy( s, cmdif_cmd.his_buff[index] );
  cmdifPrintf(s);

  return index;
}

int parseCommandArgs(char *cmdline, char **argv)
{
  char *tok;
  int argc = 0;

  argv[argc] = NULL;

  for (tok = strtok(cmdline, delim); tok; tok = strtok(NULL, delim))
  {
    argv[argc++] = tok;
  }

  return argc;
}


int cmdifGetCmdString(char *s, int *count)
{
  int ret = -1;
  char  *fp;
  char  c;
  int cnt = *count;

  *count = 0;

  //fp = s;
  fp = cmdif_cmd.read_buffer;

  while(cmdifReadByte(&c))
  {
    if( c == 0xd /* CR */ )
    {
      *s = 0;
      if( strlen( fp ) ) hisAppend( fp );
      ret = 0;
      break;
    }

    switch( c )
    {
      case 0x1a  : // ^Z
        if( cmdif_cmd.his_index >= 0 )
        {
          hisSet( fp, cmdif_cmd.his_index );
          if( cmdif_cmd.his_index )
          {
            cmdif_cmd.his_index--;
          }
          if( cmdif_cmd.his_index >= cmdif_cmd.his_count )
          {
            cmdif_cmd.his_index = cmdif_cmd.his_count -1;
          }
          cnt = strlen( fp );
          s = fp + cnt;
        }
        break;

      case 0x18  : // ^X
        if(( cmdif_cmd.his_index >= 0 )&&( cmdif_cmd.his_index < cmdif_cmd.his_count ))
        {
          hisSet( fp, cmdif_cmd.his_index );
          cmdif_cmd.his_index++;

          if (cmdif_cmd.his_index >= cmdif_cmd.his_count)
          {
            cmdif_cmd.his_index=cmdif_cmd.his_count-1;
          }
          cnt = strlen( fp );
          s = fp + cnt;
        }
        break;

      case 0x08 : // BS
        if (cnt > 0)
        {
          cnt--; *s-- = ' ';
          cmdifPrintf("\b \b");
        }
        break;

    default:
      cnt++;
      *s++ = c;
      cmdifPutch(c);
      break;
    }
  }

  *count = cnt;
  return ret;
}

void upperStr( char *Str )
{
   while( *Str ){ *Str = toupper( *Str ); Str++; }
}




