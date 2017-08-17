#include <CoreMIDI/CoreMIDI.h>
#include <unistd.h> 
#include <stdio.h> 

char *OSStatus_to_str(OSStatus stat)
{
    static char buf[sizeof(OSStatus)+1];
    buf[sizeof(OSStatus)] = '\0';
    *(OSStatus*)buf = stat;
    return buf;
}

#define STR_BUF_LEN 100 
static char buffer[STR_BUF_LEN];

void list_devs_ents_refs (void)
{
    puts("devices, entities, destinations");
    ItemCount i,j,k;
    i = MIDIGetNumberOfDevices();
    while (i--) {
        MIDIDeviceRef devref = MIDIGetDevice(i);
        j = MIDIDeviceGetNumberOfEntities(devref);
        printf("%lu ",i);
        CFStringRef name;
        MIDIObjectGetStringProperty(devref,kMIDIPropertyName,&name);
        CFStringGetCString(name,buffer,STR_BUF_LEN,kCFStringEncodingUTF8);
        printf("device: %s, ",buffer);
        while (j--) {
            MIDIEntityRef entref = MIDIDeviceGetEntity(devref,j);
            k = MIDIEntityGetNumberOfDestinations(k);
            printf("%lu ",j);
            CFStringRef name;
            MIDIObjectGetStringProperty(entref,kMIDIPropertyName,&name);
            CFStringGetCString(name,buffer,STR_BUF_LEN,kCFStringEncodingUTF8);
            printf("entity: %s, ",buffer);
            while (k--) {
                MIDIEndpointRef endref = MIDIEntityGetDestination(entref,k);
                printf("%lu ",k);
                CFStringRef name;
                MIDIObjectGetStringProperty(endref,kMIDIPropertyName,&name);
                CFStringGetCString(name,buffer,STR_BUF_LEN,kCFStringEncodingUTF8);
                printf("destination: %s, ",buffer);
            }
        }
        printf("\n");
    }
}

void list_dests (void)
{
    puts("destinations:");
    ItemCount i;
    i = MIDIGetNumberOfDestinations();
    while (i--) {
        MIDIEndpointRef destref = MIDIGetDestination(i);
        printf("%lu ",i);
        CFStringRef name;
        MIDIObjectGetStringProperty(destref,kMIDIPropertyName,&name);
        CFStringGetCString(name,buffer,STR_BUF_LEN,kCFStringEncodingUTF8);
        printf("destination: %s, ",buffer);
        printf("\n");
    }
}

void send_test_on_IAC_driver (char *destname)
{
    puts("sending note on on channel 0 of IAC Bus 1\n"
         "make sure you have such a bus my checking Audio MIDI Setup and adding the bus \"IAC Bus 1\" to IAC Driver\n");
    ItemCount i;
    i = MIDIGetNumberOfDestinations();
    while (i--) {
        MIDIEndpointRef destref = MIDIGetDestination(i);
        CFStringRef name;
        MIDIObjectGetStringProperty(destref,kMIDIPropertyName,&name);
        CFStringGetCString(name,buffer,STR_BUF_LEN,kCFStringEncodingUTF8);
        if (strcmp(buffer,destname) == 0) {
            MIDIClientRef client;
            MIDIPortRef port;
            CFStringRef clientname = CFSTR("myclient");
            CFStringRef portname = CFSTR("MYSTR");
            OSStatus oss;
            if ((oss = MIDIClientCreate(clientname,NULL,NULL,&client))) {
                fprintf(stderr,"Error %d creating client\n",oss);
                return;
            }
            if ((oss = MIDIOutputPortCreate(client, 
                    portname,
                    &port))) {
                fprintf(stderr,"Error %d creating port\n",oss);
                MIDIClientDispose(client);
                return;
            }
            MIDIPacketList pktlist = {
                .numPackets = 1,
                .packet = (MIDIPacket) 
                    {
                        .timeStamp = 0,
                        .length = 3,
                        .data = {0x90,60,100}
                    }
                
            };
            if ((oss = MIDISend(port, destref, &pktlist))) {
                fprintf(stderr,"Error %d sending MIDI\n",oss);
            }
            sleep(1);
            pktlist = (MIDIPacketList) {
                .numPackets = 1,
                .packet = (MIDIPacket) 
                    {
                        .timeStamp = 0,
                        .length = 3,
                        .data = {0x80,60,0}
                    }
                
            };
            if ((oss = MIDISend(port, destref, &pktlist))) {
                fprintf(stderr,"Error %d sending MIDI\n",oss);
            }
            MIDIPortDispose(port);
            MIDIClientDispose(client);
        }
    }
}

int
main (int argc, char **argv)
{
    char *destname;
    if (argc >= 2) {
        destname = argv[1];
    } else {
        fprintf(stderr,"No destination name specified as argument, using \"IAC Bus 1\"");
        destname = "IAC Bus 1";
    }
    list_devs_ents_refs();
    list_dests();
    send_test_on_IAC_driver(destname);
    return 0;
}
