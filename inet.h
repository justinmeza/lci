// inet.h
// A library for simplifying networked communication.
// 2008-4-27
//
// MAINTAINER
//
//     Justin J. Meza <justin dot meza at gmail dot com>
//
// LICENSE
//
//     You are free to use, modify, and re-distribute this software provided
//     that you make any modifications publicly available. Additionally, this
//     software is provided without any warranty; use of this software is at the
//     user's own risk. The author disclaims any liability for malfunctions as a
//     result of this software or any derivative works produced with it.
//
//     Please report and bugs, fixes, and improvements to the maintainer.
//
// This library allows two `host's (specifically, two processes) to connect and
// exchange data in a session using either TCP or UDP.
//
// The following two diagrams list the general order of function calls required
// to handle a session for TCP and UDP. The order of function calls in `SETUP'
// must be strictly followed; `TRANSMISSION' and `TEARDOWN' function calls may
// be done in any order and some examples are listed.
//
// Functions are labeled in the form ([TCP]:[client|server] /
// [UDP]:[client|server]) to denote whether the function is meant to be used by
// TCP, UDP, or either connection type and furthermore whether only clients,
// servers, or either are meant to call the function.
//
// +===========================================+
// |                    TCP                    |
// +=====================+=====================+
// |       SERVER        |        CLIENT       |
// +---------------------+---------------------+
// | inet_open           | inet_open           |\               \
// | inet_accept         | inet_setup          | | SETUP         |
// |                     | inet_connect        |/                |
// |                     |                     |                 |
// | inet_receive        | inet_send           |\  TRANSMISSION   > SESSION
// | inet_send           | inet_receive        |/                |
// |                     |                     |                 |
// | inet_close(local)   | inet_close(local)   |\  TEARDOWN      |
// | inet_close(remote)  |                     |/               /
// +---------------------+---------------------+
//
// +===========================================+
// |                    UDP                    |
// +=====================+=====================+
// |       SERVER        |       CLIENT        |
// +---------------------+---------------------+
// | inet_open           | inet_open           |\  SETUP        \
// |                     | inet_setup          |/                |
// |                     |                     |                 |
// | inet_receive        | inet_send           |\  TRANSMISSION   > SESSION
// | inet_send           | inet_receive        |/                |
// |                     |                     |                 |
// | inet_close          | inet_close          |>  TEARDOWN     /
// +---------------------+---------------------+

#ifndef __INET_H__
#define __INET_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>

#define IN_PORT_ANY 0
#define IN_ADDR_ANY INADDR_ANY

#define IN_PROT_TCP SOCK_STREAM
#define IN_PROT_UDP SOCK_DGRAM

#define IN_BACKLOG 10 // Number of simultaneous incoming connections to allow
#define IN_HOSTLEN 64 //

// Set up our error codes
typedef enum err_code {
	EIN_SOCK = 2,	// Error acquiring socket file descriptor
	EIN_BIND,	// Error binding socket to port
	EIN_CONN,	// Error connecting to port
	EIN_PROT,	// Incorrect protocol
	EIN_LSTN,	// Error listening
	EIN_ACPT,	// Error accepting connection
	EIN_SEND,	// Error sending data
	EIN_RECV,	// Error receiving data
	EIN_TIME,	// Timeout
} err_code_t;

// Structure representing an internet host
typedef struct inet_host {
	int fd;
	int protocol;
	struct sockaddr_in addr;
} inet_host_t;

void inet_setup(inet_host_t *, int, const char *, unsigned short);
int inet_open(inet_host_t *, int, const char *, unsigned short);
int inet_accept(inet_host_t *, inet_host_t *);
int inet_connect(inet_host_t *, inet_host_t *);
int inet_receive(inet_host_t *, inet_host_t *, void *, int, int);
int inet_send(inet_host_t *, inet_host_t *, void *, int);
int inet_close(inet_host_t *);
char *inet_lookup(const char *);

#endif // __INET_H__
