/*
 * ospf.h  Header file for OSPF module 
 * AUTHOR: Hanif Jetha
 */

#ifndef __OSPF_H_
#define __OSPF_H_

#include <sys/types.h>
#include "grouter.h"
#include "message.h"

#define OSPF_HELLO               1    /* OSPF "hello" packet */
#define OSPF_LINKSTATEUPDATE     4    /* OSPF "link state update" packet */


typedef struct _ospfhdr_t
{
  // as defined in assignment specs
  uint8_t version = 2;              /* OSPF Version 2 */
  uint8_t type;                   /* Type of OSPF Packet */
  uint16_t length;                /* Length of OSPF Packet (incl. header) */
  uchar ip_src[4];                /* Source Router IP */
  int32_t areaID = 0;               /* Area ID */
  uint16_t checksum;              
  uint16_t auth_type = 0;           
  int32_t auth = NULL;                 /* TODO: null bytes? */

} ospfhdr_t;


// fix this, probably won't work...
typedef struct _ospfhello_t
{
  ospfhdr_t header;
  uchar netmask[4] = {255,255,255,0};
  uint16_t hellointerval = 10;
  uint8_t options = 0;
  uint8_t priority = 0;
  int32_t routerdeadinterval = 40;
  int32_t designatedrouterIP = 0;
  int32_t backuprouterIP = 0;
  uchar *neighborIParray[4];

}

//  function prototypes
void OSPFProcessHelloPacket(gpacket_t *in_pkt);

#endif
