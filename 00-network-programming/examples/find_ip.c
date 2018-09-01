#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/types.h>

void init_hints(struct addrinfo *hints);
void traverse_addrinfo(struct addrinfo *ptr);
void print_addrinfo(struct addrinfo *ptr);

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        return 1;
    }

    struct addrinfo hints; 
    init_hints(&hints);

    /**
     * #include <sys/types.h>
     * #include <sys.socket.h>
     * #include <netdb.h>
     *
     * int getaddrinfo(const char *node,
     *                 const char *service,
     *                 const struct addrinfo *hints,
     *                 struct addrinfo **res);
     *
     * Populates socket address structures. Performs DNS lookup (by node, a dot
     * formatted ip or hostname) or struct sockaddr_in/sockaddr_in6 preparation
     * for binding (by service, a string service name or port, or node and
     * service).
     *
     * If ai_flags contains AI_PASSIVE and node is NULL, getaddrinfo() returns
     * the wildcard address INADDR_ANY/IN6ADDR_ANY_INIT, which can be bound to.
     *
     * If AI_PASSIVE is not set, the returned socket address is suitable for
     * use with connect() or sendto(), with a NULL node causing the loopback
     * address INADDR_LOOPBACK/IN6ADDR_LOOPBACK_INIT to be returned.
     *
     * A NULL service causes the port in the returned socket address structure
     * to remain uninitialized.
     *
     * Either node (when binding) or service (when doing DNS lookup) may be
     * NULL, but not both. 
     *
     * Populates makes res the head of a linked list of results which must be
     * freed later using freeadrinfo(). Returns 0 on success or an error code
     * which can be read using gai_strerror().
     */
    int ret;
    struct addrinfo *res;
    if((ret = getaddrinfo(argv[1], NULL, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return 1;
    }

    traverse_addrinfo(res);
    freeaddrinfo(res);
}

void init_hints(struct addrinfo *hints)
{
    /**
     * Hints may contain ai_flags, ai_family, ai_socktype and ai_protocol
     * restrictions. Other fields must be zeroed.
     */ 
    memset(hints, 0, sizeof(struct addrinfo));

    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_protocol = IPPROTO_TCP;
}

void traverse_addrinfo(struct addrinfo *ptr)
{
    while(ptr != NULL)
    {
        print_addrinfo(ptr);
        ptr = ptr->ai_next;
    }
}

void print_addrinfo(struct addrinfo *ptr)
{
    char *version;
    void *addr;

    if(ptr->ai_family == AF_INET)
    {
        version = "ipv4";
        addr = &((struct sockaddr_in *) ptr->ai_addr)->sin_addr; 
    }
    else
    {
        version = "ipv6";
        addr = &((struct sockaddr_in6 *) ptr->ai_addr)->sin6_addr;
    }

    /**
     * Sometimes inet_ntop may fail, but since the socket structures are 
     * generated from getaddrinfo, we may assume that it doesn't in this case.
     */
    char ip[INET6_ADDRSTRLEN];
    inet_ntop(ptr->ai_family, addr, ip, sizeof ip);

    printf("%s: %s\n", version, ip);    
}











