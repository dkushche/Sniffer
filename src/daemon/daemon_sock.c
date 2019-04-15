
static void             promiscious_off( int sock_raw, int status_fd );
static void             make_promiscious( int sock_raw, int status_fd);
static int		        bind_to_device(t_sniffer *this, const char *device);

static void         promiscious_off( int sock_raw, int status_fd )
{
    struct ifreq    interface;

    if (ioctl(sock_raw, SIOCGIFFLAGS, &interface) < 0)
        err_log("Unload interface struct(off)", status_fd, 1);
    interface.ifr_flags &= ~IFF_PROMISC;
    if (ioctl(sock_raw, SIOCSIFFLAGS, &interface) < 0)
        err_log("Load interface struct(off)", status_fd, 1);
}

static void         make_promiscious( int sock_raw, int status_fd )
{
    struct ifreq    interface;

    if (ioctl(sock_raw, SIOCGIFFLAGS, &interface) < 0)
        err_log("Unload interface struct", status_fd, 1);
    interface.ifr_flags |= IFF_PROMISC;
    if (ioctl(sock_raw, SIOCSIFFLAGS, &interface) < 0)
        err_log("Load interface struct", status_fd, 1);
}

static int		    bind_to_device(t_sniffer *this, const char *device)
{
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
    memcpy(this->now_device, device, strlen(device));
    this->now_device[strlen(device)] = '\0';
    if (this->data.array)
        restart(&this->data);
    else
        create_vector(&this->data);
    create_log_file(this);
	return 0;
}
