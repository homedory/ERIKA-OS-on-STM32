#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int initCANSocket(const char* name)
{
    int socketCANDescriptor;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
        
    if (socketCANDescriptor = socket(PF_CAN, SOCK_RAW, CAN_RAW) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    strcpy(ifr.ifr_name, name); 
    ioctl(socketCANDescriptor, SIOCGIFINDEX, &ifr);
    memset(&addr, 0, sizeof(addr));
    
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    
    if (bind(socketCANDescriptor, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    return socketCANDescriptor;
}

int sendResponseCAN(int socketCANDescriptor, int return_value)
{
    struct can_frame frame;

    frame.can_id = 0x555;
    
    // integer size
    frame.can_dlc = 4;
    memcpy(frame.data, return_value, 4);

    if (write(socketCANDescriptor, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Write failed");
        return -1;
    }          
    
    return 0;
}