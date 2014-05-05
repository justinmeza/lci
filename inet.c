#include "inet.h"

// inet_setup (TCP:client / UDP:client)
//
// Sets up an inet_host structure.
void
inet_setup(inet_host_t *host,
		int protocol,
		const char *addr,
		unsigned short port)
{
	// Set up our inet_host structure
	host->protocol = protocol;

	// Set up our sockaddr_in structure
	host->addr.sin_family = AF_INET;
	host->addr.sin_port = htons(port);
	host->addr.sin_addr.s_addr =
		(addr == IN_ADDR_ANY ? IN_ADDR_ANY : inet_addr(addr));
	memset(host->addr.sin_zero, 0, sizeof(host->addr.sin_zero));
}

// inet_open (TCP / UDP)
//
// Prepares `host' for transmission.
//
// `protocol' specifies the protocol to use for the transmission and is either
// `IN_PROT_TCP' or `IN_PROT_UDP'.
//
// `addr' is the IP address the host session structure will use: either a string
// of the form `X.X.X.X', where each `X' is the textual representation of a
// number in the range 0 to 255, or `IN_ADDR_ANY' to specify the current host's
// IP address.
//
// `port' is the port number to bind to on the host machine. Specifying
// IN_PORT_ANY attempts to use any available port assigned by the kernel. Use
// this option when the port your host structure is bound to does not matter.
//
// Returns one of:
// 	0		Success.
// 	-EIN_SOCK	Error acquiring socket file descriptor.
// 	-EIN_BIND	Error binding socket to port.
int
inet_open(inet_host_t *host,
		int protocol,
		const char *addr,
		unsigned short port)
{
	// If we are not able to get a socket, fail with a socket error
	host->fd = socket(PF_INET, protocol, 0);
	if (host->fd < 0) {
		perror("Error acquiring socket file descriptor!\n");
		return -EIN_SOCK;
	}

	// Set up our sockaddr_in structure
	inet_setup(host, protocol, addr, port);

	// If we are not able to bind to a port, fail with a bind error
	if (bind(host->fd, (struct sockaddr *)&(host->addr),
				sizeof(host->addr)) < 0) {
		perror("Error acquiring socket file descriptor!\n");
		return -EIN_BIND;
	}

	// Set up our host protocol
	host->protocol = protocol;

	return 0;
}

// inet_accept (TCP:server)
//
// Accepts an incoming connection from the `remote' host to the `local' host.
//
// Returns one of:
// 	0		Success.
// 	-EIN_PROT	Incorrect protocol.
// 	-EIN_LSTN	Error listening.
// 	-EIN_ACPT	Error accepting connection.
int
inet_accept(inet_host_t *remote,
		inet_host_t *local)
{
	socklen_t len = sizeof(remote->addr);

	// Make sure we only accept TCP sessions
	if (local->protocol != IN_PROT_TCP) {
		fprintf(stderr, "Error accepting from non-TCP protocol!\n");
		return -EIN_PROT;
	}

	// Listen for an acceptable connection
	if (listen(local->fd, IN_BACKLOG) < 0) {
		perror("Error encountered while listening!\n");
		return -EIN_LSTN;
	}

	// Accept the connection
	remote->fd = accept(local->fd, (struct sockaddr *)&(remote->addr), &len);
	if (remote->fd < 0) {
		perror("Error accepting connection!\n");
		return -EIN_ACPT;
	}

	return 0;
}

// inet_connect (TCP:client)
//
// Connects the `local' host to the `remote' host.
//
// Returns one of:
// 	0		Success.
// 	-EIN_PROT	Incorrect protocol.
// 	-EIN_CONN	Error connecting to port.
int
inet_connect(inet_host_t *local,
		inet_host_t *remote)
{
	// Make sure we only connect TCP sessions
	if (local->protocol != IN_PROT_TCP) {
		fprintf(stderr, "Error accepting from non-TCP protocol!\n");
		return -EIN_PROT;
	}

	// Connect to remote host
	if (connect(local->fd, (struct sockaddr *)&(remote->addr), sizeof(remote->addr)) < 0) {
		perror("Error connecting to remote host!\n");
		return -EIN_CONN;
	}

	// Link `local->fd' and `remote->fd' on the client because `inet_send' always
	// sends data through `remote->fd' for TCP connections (this makes sense
	// intuitively, as one would send data *to* a remote address). However, a TCP
	// client (which would be the only host to call this function) always
	// communicates through it's `local->fd' and leaves `remote->fd' unused so we
	// may safely repurpose it for the sake of continuity.
	remote->fd = local->fd;

	return 0;
}

// inet_receive (TCP / UDP)
//
// Receives a `len' bytes sent from `remote' to `local' and stores them in
// `data'. If no message is received within `timeout' seconds, -EIN_TIME is
// returned.
//
// Setting `timeout' to 0 causes the function to return immediately if there is
// no data to receive and setting `timeout' to -1 causes the function to block
// until data is ready to be received.
//
// Returns one of:
//      Number of bytes received        Success.
//      -EIN_TIME                       Timeout.
//      -EIN_RECV                       Error receiving data.
int
inet_receive(inet_host_t *remote,
		inet_host_t *local,
		void *data,
		int len,
		int timeout)
{
	fd_set fds;
	struct timeval time;
	int size;

	// Set the timeout interval
	time.tv_sec = timeout;
	time.tv_usec = 0;

	// Initialize the file descriptor set
	FD_ZERO(&fds);

	// Depending on the protocol, receive data
	switch (local->protocol) {
		case IN_PROT_TCP:
			FD_SET(remote->fd, &fds);
			// Block for a specified amount of time
			if (timeout != -1)
				select(remote->fd + 1, &fds, NULL, NULL, &time);
			else
				select(remote->fd + 1, &fds, NULL, NULL, NULL);
			if (FD_ISSET(remote->fd, &fds)) {
				size = recv(remote->fd, data, len, 0);
				if (size < 0) {
					perror("Error receiving data!\n");
					return -EIN_RECV;
				}
			}
			else
				return -EIN_TIME;
			break;
		case IN_PROT_UDP: {
			socklen_t n = sizeof(remote->addr);
			FD_SET(local->fd, &fds);
			// Block for a specified amount of time
			if (timeout != -1)
				select(local->fd + 1, &fds, NULL, NULL, &time);
			else
				select(local->fd + 1, &fds, NULL, NULL, NULL);
			if (FD_ISSET(local->fd, &fds)) {
				size = recvfrom(local->fd, data, len, 0,
						(struct sockaddr *)&(remote->addr), &n);
				if (size < 0) {
					perror("Error receiving data!\n");
					return -EIN_RECV;
				}
			}
			else
				return -EIN_TIME;
			break;
		}
	}

	return size;
}

// inet_send (TCP / UDP)
//
// Sends `len' bytes of `data' from `local' to `remote'.
//
// Returns one of:
//      Number of bytes sent    Success.
//      -EIN_SEND               Error sending data.
int
inet_send(inet_host_t *local,
		inet_host_t *remote,
		void *data,
		int len)
{
	int size;

	// Depending on the protocol, send data
	switch (local->protocol) {
		case IN_PROT_TCP:
			size = send(remote->fd, data, len, 0);
			if (size < 0) {
				perror("Error sending data!\n");
				return -EIN_SEND;
			}
			break;
		case IN_PROT_UDP: {
			size = sendto(local->fd, data, len, 0,
					(struct sockaddr *)&(remote->addr), sizeof(remote->addr));
			if (size < 0) {
				perror("Error sending data!\n");
				return -EIN_SEND;
			}
			break;
		}
	}

	return size;
}

// inet_close (TCP / UDP)
//
// Closes an inet connection.
//
// Returns one of:
//      0       Success.
//      -1      Invalid `host' pointer.
int
inet_close(inet_host_t *host)
{
	// Sanity check
	if (!host) return -1;

	// Close the file descriptor
	if (host->fd) close(host->fd);

	return 0;
}

// inet_lookup
//
// Looks up information by resolving a DNS name and returns a pointer to the
// resolved IP address.
//
// Returns one of:
//      0                                       Unable to resolve name.
//      Pointer to resolved IP address          Must be freed by caller.
char *
inet_lookup(const char *name)
{
	char *hostname;
	char *temp;
	struct hostent *ent;

	// If unable to get host by name, quit with lookup error
	if (!(ent = gethostbyname(name))) {
		herror("gethostbyname");
		return 0;
	}

	// Copy the looked up name
	temp = inet_ntoa(*((struct in_addr *)ent->h_addr));
	hostname = malloc(sizeof(char) * (strlen(temp) + 1));
	strcpy(hostname, temp);

	return hostname;
}
