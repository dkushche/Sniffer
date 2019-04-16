
static void             promiscious_off( int sock_raw, int status_fd, char *now_device );
static void             make_promiscious( int sock_raw, int status_fd, char *now_device);
static int		        bind_to_device(t_sniffer *this, const char *device);

static void             sock_close( t_sniffer *this )
{
     if (this->sock_raw != -1) {
        promiscious_off(this->sock_raw, this->status, this->now_device);
        close(this->sock_raw);
    }
}

static void         promiscious_off( int sock_raw, int status_fd, char *now_device )
{
    struct ifreq    interface;

    strcpy(interface.ifr_name, now_device);
    if (ioctl(sock_raw, SIOCGIFFLAGS, &interface) < 0)
        err_log("Unload interface struct(off)", status_fd, 1);
    interface.ifr_flags &= ~IFF_PROMISC;
    if (ioctl(sock_raw, SIOCSIFFLAGS, &interface) < 0)
        err_log("Load interface struct(off)", status_fd, 1);
}

static void         make_promiscious( int sock_raw, int status_fd, char *now_device )
{
    struct ifreq    interface;

    strcpy(interface.ifr_name, now_device);
    if (ioctl(sock_raw, SIOCGIFFLAGS, &interface) < 0)
        err_log("Unload interface struct", status_fd, 1);
    interface.ifr_flags |= IFF_PROMISC;
    if (ioctl(sock_raw, SIOCSIFFLAGS, &interface) < 0)
        err_log("Load interface struct", status_fd, 1);
}

static int		    bind_to_device(t_sniffer *this, const char *device)
{
    struct sockaddr_ll  iface;

    memset(&iface, 0, sizeof(struct sockaddr_ll));
    iface.sll_family = AF_PACKET;
    iface.sll_protocol = htons(ETH_P_ALL);
    iface.sll_ifindex = if_nametoindex(device);
    if (!iface.sll_ifindex) {
        err_log("No such device", this->status, 0);
		return -1;
    }
    sock_close(this);
    this->sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL) );
	if (this->sock_raw < 0)
		err_log("Trouble with creating socket", this->status, 1);
    if (bind(this->sock_raw, (struct sockaddr*)&iface, sizeof(iface)) < 0)     
		err_log("Bind to device", this->status, 1);
    int flags = fcntl(this->sock_raw, F_GETFL, 0);
    fcntl(this->sock_raw, F_SETFL, flags | O_NONBLOCK);
    memcpy(this->now_device, device, strlen(device));
    this->now_device[strlen(device)] = '\0';
    if (this->data.array)
        restart(&this->data);
    else
        create_vector(&this->data);
    create_log_file(this);
    make_promiscious(this->sock_raw, this->status, this->now_device);
	return 0;
}

/*
struct ifreq	ifr;

memset(&ifr, 0, sizeof(ifr));
    memcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if (setsockopt(this->sock_raw, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) != 0)
	{
		if (errno == 19) {
			err_log("No such device", this->status, 0);
			return -1;
		}
		else
			err_log("Bind to device", this->status, 1);
	}
*/