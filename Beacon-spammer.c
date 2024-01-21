#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int isProgramInstalled(const char *program) {
    char command[128];
    snprintf(command, sizeof(command), "which %s > /dev/null 2>&1", program);
    return system(command) == 0;
}

void installProgram(const char *program) {
    char command[128];
    snprintf(command, sizeof(command), "sudo apt-get install -y %s", program);
    system(command);
}

int main() {
    char interface[50], filePath[100], command[300];

    // Check if 'airmon-ng' and 'mdk3' are installed
    if (!isProgramInstalled("airmon-ng")) {
        printf("Installing 'airmon-ng'...\n");
        installProgram("airmon-ng");
    }
    if (!isProgramInstalled("mdk3")) {
        printf("Installing 'mdk3'...\n");
        installProgram("mdk3");
    }

    printf("Enter network interface name: ");
    scanf("%49s", interface);

    printf("Enter path to the list of names: ");
    scanf("%99s", filePath);

    // Start the monitor mode on the specified interface
    snprintf(command, sizeof(command), "sudo airmon-ng start %s", interface);
    system(command);

    // Run mdk3 with the specified interface and file path
    snprintf(command, sizeof(command), "sudo mdk3 %smon b -f %s", interface, filePath);
    system(command);

    // Stop the monitor mode on the specified interface
    snprintf(command, sizeof(command), "sudo airmon-ng stop %smon", interface);
    system(command);

    printf("Monitor mode stopped on interface %s.\n", interface);

    return 0;
}
