/*
 * ospf.h  Header file for OSPF module 
 * AUTHOR: Hanif Jetha
 */

#ifndef __OSPF_H_
#define __OSPF_H_


//TODO: figure out what's actually necessary here....
#include "grouter.h"
#include "message.h"
#include <sys/types.h>
#include <stdint.h>
#include <endian.h>
#include <sys/time.h>
#include <stdbool.h>
#include <slack/map.h>

// because of ARP
#define MAX_NEIGHBORS            20

// OSPF packet types
#define OSPF_HELLO               1                 /* OSPF "hello" packet */
#define OSPF_LINKSTATEUPDATE     4                 /* OSPF "link state update" packet */

// OSPF header (as defined in assignment specs)
typedef struct _ospfhdr_t
{
  uint8_t version;                             /* OSPF Version 2 */
  uint8_t type;                                    /* Type of OSPF Packet */
  uint16_t length;                                 /* Length of OSPF Packet (incl. header) */
  uchar ip_src[4];                                 /* Source Router IP */
  int32_t areaID;                              /* Area ID */
  uint16_t checksum;
  uint16_t auth_type;
} ospfhdr_t;

typedef struct _hellohdr_t
{
  uchar netmask[4];
  uint16_t hellointerval;
  uint8_t options;
  uint8_t priority;
  int32_t routerdeadinterval;
  int32_t designatedrouterIP;
  int32_t backuprouterIP;
} hellohdr_t;

// maybe have some kind of is_up variable, 
// for now, just do a diff time check
typedef struct _neighbor_entry_t
{
  bool is_up;
  time_t last_seen;
  uchar neighbor_ip[4];
} neighbor_entry_t;

// might need this externally?
Map *neighbor_table;

//  function prototypes
void OSPFProcessPacket(gpacket_t *in_pkt);
void OSPFProcessHelloPacket(gpacket_t *in_pkt);
void OSPFInit();
void OSPFBroadcastHelloPacket();
gpacket_t *CreateHelloPacket();

#endif
