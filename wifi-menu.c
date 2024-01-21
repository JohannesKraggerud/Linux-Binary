#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void listNetworkInterfaces() {
    printf("Available network interfaces:\n");
    system("ip link show | grep '^[0-9]' | awk '{print $2}' | sed 's/://'");
}

void listWifiNetworks() {
    printf("Scanning for Wi-Fi networks...\n");
    system("nmcli dev wifi");
}

int main() {
    int choice;
    char command[256];
    char interface[50], networkName[50], password[50];

    printf("1. Enable network device (type 1 to go into this menu)\n");
    printf("2. Connect to network (type 2 to go into this menu)\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            listNetworkInterfaces();
            printf("Enter network interface name: ");
            scanf("%s", interface);
            sprintf(command, "sudo ip link set %s up", interface);
            system(command);
            break;

        case 2:
            listWifiNetworks();
            listNetworkInterfaces();
            printf("Enter network name: ");
            scanf("%s", networkName);
            printf("Enter password: ");
            scanf("%s", password);
            printf("Enter network interface: ");
            scanf("%s", interface);
            sprintf(command, "sudo nmcli dev wifi connect '%s' password '%s' ifname '%s'", networkName, password, interface);
            system(command);
            break;

        default:
            printf("Invalid choice.\n");
    }

    return 0;
}
