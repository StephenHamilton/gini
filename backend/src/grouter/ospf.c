/*
 * ospf.c  COMP 535 - Programming Assignment 1 (OSPF Implementation)
 * AUTHOR: Hanif Jetha 
 * DATE:
 *
 */

// TODO: again, figure out what's actually necessary here...
#include "ospf.h"
#include "protocols.h"
#include "ip.h"
#include "arp.h"
#include "message.h"
#include "grouter.h"
#include <slack/err.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>


// Initialize neighbor table, etc...
void OSPFInit()
{
  // init neighbor table
  if (!(neighbor_table = map_create(NULL)))
  {
    fatal("[OSPFInit]:: Could not create the neighbor table..");
    return;
  }

  verbose(2, "[OSPFInit]:: OSPF neighbor table initialized");
  return;

}


/* Process an OSPF packet */
void OSPFProcessPacket(gpacket_t *in_pkt){

  // Get the IP packet portion of a gpacket_t
  ip_packet_t *ip_pkt = (ip_packet_t *)in_pkt->data.data;

  // IP header length (in bytes), usually 20 because we don't use 
  // IP options
  int iphdrlen = ip_pkt->ip_hdr_len *4;

  // Get a pointer to the beginning of the OSPF header. Use uchars (1 byte)
  // for pointer arithmetic
  ospfhdr_t *ospfhdr = (ospfhdr_t *)((uchar *)ip_pkt + iphdrlen);

  // switch on type of OSPF packet
  switch (ospfhdr->type){

    case OSPF_HELLO:
      verbose(2, "[OSPFProcessPacket]:: Processing OSPF HELLO packet");
      OSPFProcessHelloPacket(in_pkt);
      break;

    case OSPF_LINKSTATEUPDATE:
      verbose(2, "[OSPFProcessPacket]:: Processing OSPF LINKSTATEUPDATE packet");
      // Implement this!
      break;

    default:
      verbose(2, "[OSPFProcessPacket]:: Unknown OSPF packet type");
      break;
  }

}

/* Process an OSPF Hello Packet */
void OSPFProcessHelloPacket(gpacket_t *in_pkt){

  // set current time
  time_t current_time = time(NULL);

  // get pointer to IP packet
  ip_packet_t *ip_pkt = (ip_packet_t *)in_pkt->data.data;

  // set sender IP
  uchar sender_ip[4];
  strncpy(sender_ip,ip_pkt->ip_src,12);

  int iphdrlen = ip_pkt->ip_hdr_len *4;    // 20 (no options)
  ospfhdr_t *ospfhdr = (ospfhdr_t *)((uchar *)ip_pkt + iphdrlen);

  // Get a pointer to beginning of HELLO header (OSPF header is always
  // 16 bytes)
  hellohdr_t *hellohdr = (hellohdr_t *)((uchar *)ospfhdr + 16);

  // size of array of neighbors
  int packet_neighbors_size = ospfhdr->length - 16 - 20;

  int neighbor_count = 0;

  if (packet_neighbors_size > 0)
    neighbor_count = packet_neighbors_size / sizeof(uchar[4]) ;

  // Print some of the hello packet's contents
  verbose(2, "[OSPFProcessHelloPacket]:: OSPF Header Info >>>> "
             "Type: %d - Source IP: %s", ospfhdr->type, ospfhdr->ip_src);

  verbose(2, "[OSPFProcessHelloPacket]:: Hello Header Info >>>> "
             "Hello Interval: %d - Router Dead Interval: %d "
             "Number of neighbors: %d ",
             hellohdr->hellointerval,hellohdr->routerdeadinterval,
             neighbor_count);

  // create neighbor struct
  neighbor_entry_t *neighbor;
  neighbor->is_up = TRUE;
  neighbor->last_seen = current_time;
  strncpy(neighbor->neighbor_ip, sender_ip, 12);

  // store neighbor struct in neighbor table
  map_add(neighbor_table, sender_ip, neighbor);

}

/* Broadcast an OSPF Hello Packet to all of the router's neighbors */
void OSPFBroadcastHelloPacket(){

  verbose(2, "[OSPFBroadcastHelloPacket]:: HELLO");

  gpacket_t *hello_pkt = CreateHelloPacket();
  ip_packet_t *ip_pkt = (ip_packet_t *)(hello_pkt->data.data);

  ip_pkt->ip_hdr_len = 5;

  // get pointer to start of OSPF packet
  ospfhdr_t *ospf_hdr = (ospfhdr_t *)((uchar *)ip_pkt + ip_pkt->ip_hdr_len*4);


  int i;
  // For all connected interfaces, send the OSPF HELLO packet
  // created above
  for (i = 0; i < MAX_ARP; i++){

    // there's an ARP entry at this index, so we'll send an OSPF 
    // packet here
    if (ARPtable[i].is_empty == FALSE){
      uchar dest_ip[4];
      strncpy(dest_ip,ARPtable[i].ip_addr,12);
      IPOutgoingPacket(hello_pkt, dest_ip, ospf_hdr->length,1,OSPF_PROTOCOL);
    }

  }

  printf("broadcasted hello packet");

}

/* Creates an OSPF HELLO packet for this router */
gpacket_t *CreateHelloPacket(){

  time_t curr_time = time(NULL);

  gpacket_t *out_pkt = (gpacket_t *) malloc(sizeof(gpacket_t));
  ip_packet_t *ip_pkt = (ip_packet_t *)(out_pkt->data.data);

  ip_pkt->ip_hdr_len = 5;

  // get pointer to start of OSPF packet
  ospfhdr_t *ospf_hdr = (ospfhdr_t *)((uchar *)ip_pkt + ip_pkt->ip_hdr_len*4);

  // set some of the OSPF header fields
  ospf_hdr->type = OSPF_HELLO;
  uchar bcast_ip[] = IP_BCAST_ADDR;
  strncpy(ospf_hdr->ip_src,bcast_ip,12);

  // OSPF header always 16 bytes
  hellohdr_t *hello_hdr = (hellohdr_t *)((uchar*)ospf_hdr + 16);

  // Send no neighbors for now
  int num_neighbors = 0;

  // For now, just send our IP
  /*
  // iterate over all elements in the neighbor_table map
  while (map_has_next(neighbor_table) == 1){

    // get neighbor struct
    neighbor_entry_t *neighbor = map_next(neighbor_table);

    double timediff = difftime(curr_time, neighbor->last_seen);

    // kill neighbor
    if (timediff > 40){
      neighbor->is_up = FALSE;
      continue;
    }

    // add neighbor to hello packet
    else{
      neighbor->is_up = TRUE;
    }
  } // end iteration over map
  */


  return out_pkt;

}


