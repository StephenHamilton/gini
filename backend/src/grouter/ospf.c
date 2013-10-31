/*
 * ospf.c  COMP 535 - Programming Assignment 1 (OSPF Implementation)
 * AUTHOR: Hanif Jetha 
 * DATE:
 *
 */

#include "ospf.h"
#include "protocols.h"
#include "ip.h"
#include "message.h"
#include "grouter.h"
#include <slack/err.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>

void OSPFProcessPacket(gpacket_t *in_pkt){

  ip_packet_t *ip_pkt = (ip_packet_t *)in_pkt->data.data;
  int iphdrlen = ip_pkt->ip_hdr_len *4;

  // finish this shit










}
