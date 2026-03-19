#include <sys/types.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define SOUTH 0
#define NORTH 1
#define RED 2

#define NorthQueueArrival 1
#define NorthBridgeArrival 2
#define SouthQueueArrival 4
#define SouthBridgeArrival 8

#define NorthGreen 9 // North Green + South Red = 0001 + 1000 = 1001 (9)
#define SouthGreen 6 // South Green + North Red = 0010 + 0100 = 0110 (6)
#define RedRed 10 // North Red + South Red = 0010 + 1000 = 1010 (10) 


int PORT;
int qN= 0, qS=0;
int carsOnBridge = 0;
int lightStatus = RED;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
struct termios old_terminal_settings; // To restore terminal on exit



// initialize the serial port
void initPorts() {
    PORT = open("/dev/cu.usbserial-FT6SCY68", O_RDWR | O_NOCTTY);
    if (PORT < 0) {
        printf("Port not there lol\n");
        exit(1);
    }
    struct termios t;
    tcgetattr(PORT, &t);
    cfsetospeed(&t, B9600);
    cfsetispeed(&t, B9600);
    t.c_cflag = CS8 | CLOCAL | CREAD;
    t.c_iflag = 0;
    t.c_oflag = 0;
    t.c_lflag = 0;
    tcsetattr(PORT, TCSANOW, &t);
}
    
// No enter needed
void setupTerminal() {
    tcgetattr(STDIN_FILENO, &old_terminal_settings);
    struct termios newt = old_terminal_settings;
    newt.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}
// Print to the terminal
void printInfo() {
    char *lightStr;
    if (lightStatus == NORTH) {
        lightStr = "\033[42m G \033[0m (N) | \033[41m R \033[0m (S)";
    } else if (lightStatus == SOUTH) {
        lightStr = "\033[41m R \033[0m (N) | \033[42m G \033[0m (S)";
    } else {
        lightStr = "\033[41m R \033[0m (N) | \033[41m R \033[0m (S)";
    }

    printf("\rLights: %s | North Q: %d | South Q: %d | Cars: %d    ", 
           lightStr, qN, qS, carsOnBridge);
    fflush(stdout); 
}

void *carThread(void* arg) {
    pthread_mutex_lock(&lock);
    carsOnBridge++;
    pthread_mutex_unlock(&lock);
    printInfo();

    sleep(5); // Cross bridge

    pthread_mutex_lock(&lock);
    carsOnBridge--;
    pthread_mutex_unlock(&lock);
    printInfo();
    return NULL;
}

void* inputThread(void* arg) {
    char input;
    while(read(STDIN_FILENO, &input, 1) > 0) {
        pthread_mutex_lock(&lock);
        if (input == 'n') {
            qN++;
            char out = NorthQueueArrival;
            write(PORT, &out, 1);
        } else if (input == 's') {
            qS++;
            char out = SouthQueueArrival;
            write(PORT, &out, 1);
        } else if (input == 'e') {
            printf("\nBaj baj\n");
            // Important: Restore terminal before exiting!
            tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal_settings);
            exit(0);
    }
    pthread_mutex_unlock(&lock);
    printInfo();
    }
    return NULL;
}

void* serialReadThread(void* arg) {
    char bit;
    while(read(PORT, &bit, 1) > 0) {
        pthread_mutex_lock(&lock);
        if (bit == NorthGreen) lightStatus = NORTH;
        else if (bit == SouthGreen) lightStatus = SOUTH;
        else if (bit == RedRed) lightStatus = RED;
        pthread_mutex_unlock(&lock);
        printInfo();
    }
    return NULL;
}

int main() {
    initPorts();
    setupTerminal();
    printInfo();

    pthread_t t1, t2;
    pthread_create(&t1, NULL, inputThread, NULL);
    pthread_create(&t2, NULL, serialReadThread, NULL);

    while(1) {
        int carsDispatched = 0;
        pthread_mutex_lock(&lock);
        if (lightStatus == NORTH && qN > 0) {
            qN--;
            char out = NorthBridgeArrival;
            write(PORT, &out, 1);
            carsDispatched = 1;

        } else if (lightStatus == SOUTH && qS > 0) {
            qS--;
            char out = SouthBridgeArrival;
            write(PORT, &out, 1);
            carsDispatched = 1;
        }
        pthread_mutex_unlock(&lock);

        if (carsDispatched) {
            pthread_t t3;
            pthread_create(&t3, NULL, carThread, NULL);
            pthread_detach(t3); 
            sleep(1); // Wait for next
        } else {
            usleep(100000); // Sleep for 100ms to avoid busy waiting
        }
    }
}