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

#define NorthGreen 9
#define SouthGreen 6
#define RedRed 10

int PORT;
int queue[2] = {0, 0};
int carsOnBridge = 0;
int lightStatus = RED; 

pthread_mutex_t DataWriteMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t StateMutex = PTHREAD_MUTEX_INITIALIZER; // Protects queues and bridge count

void initPorts(){
    // IMPORTANT: Update this string to your exact Mac serial port name!
    PORT = open("/dev/cu.usbserial-FT6SCY68", O_RDWR);

    if (PORT < 0) {
        printf("Error opening serial port. Check your connection/port name!\n");
        exit(1);
    }

    struct termios termiosOptions;
    tcgetattr(PORT, &termiosOptions);
    cfsetospeed(&termiosOptions, B9600);
    cfsetispeed(&termiosOptions, B9600);

    termiosOptions.c_cflag = CS8 | CLOCAL | CREAD;
    termiosOptions.c_iflag = IGNPAR;
    termiosOptions.c_oflag = 0;
    termiosOptions.c_lflag = 0;

    tcsetattr(PORT, TCSANOW, &termiosOptions);
}

void printInfo() {
    char msg[41];
    
    // Safely grab a snapshot of the state
    pthread_mutex_lock(&StateMutex);
    int current_light = lightStatus;
    int n_q = queue[NORTH];
    int s_q = queue[SOUTH];
    int c_o_b = carsOnBridge;
    pthread_mutex_unlock(&StateMutex);

    switch (current_light) {
        case SOUTH:
            strcpy(msg, "\033[41m R \033[0m \033[42m G \033[0m"); // North Red, South Green
            break;
        case NORTH:
            strcpy(msg, "\033[42m G \033[0m \033[41m R \033[0m"); // North Green, South Red
            break;
        default:
            strcpy(msg, "\033[41m R \033[0m \033[41m R \033[0m"); // Both Red
            break;
    }

    printf("\rLight (N,S): %s | North Q: %d | Cars on Bridge: %d | South Q: %d   ", 
           msg, n_q, c_o_b, s_q);
    fflush(stdout); // Force it to print on the same line
}

void writePorts(char data) {
    pthread_mutex_lock(&DataWriteMutex);
    write(PORT, &data, 1);
    pthread_mutex_unlock(&DataWriteMutex);
}

// Fixed signature for pthread
void *carOnBridge(void *arg){
    pthread_mutex_lock(&StateMutex);
    carsOnBridge++;
    pthread_mutex_unlock(&StateMutex);
    printInfo();

    sleep(5); // Car is driving across

    pthread_mutex_lock(&StateMutex);
    carsOnBridge--;
    pthread_mutex_unlock(&StateMutex);
    printInfo();
    
    return NULL;
}

// Fixed signature for pthread
void *dispatchCar(void *arg){
    int direction = (int)(intptr_t)arg; // Retrieve direction safely

    pthread_mutex_lock(&StateMutex);
    queue[direction]--;
    pthread_mutex_unlock(&StateMutex);

    // Spawn a standalone car thread
    pthread_t car_tid;
    pthread_create(&car_tid, NULL, carOnBridge, NULL);
    pthread_detach(car_tid); // Let it clean up its own memory when done

    char outData = (direction == SOUTH) ? SouthBridgeArrival : NorthBridgeArrival;
    writePorts(outData);

    return NULL;
}

void checkToDispatchCar(int direction) {
    pthread_mutex_lock(&StateMutex);
    int q_len = queue[direction];
    int current_light = lightStatus;
    pthread_mutex_unlock(&StateMutex);

    if (q_len > 0 && current_light == direction){
        pthread_t dispatch_tid;
        // Pass direction as a casted pointer to satisfy pthread requirements
        pthread_create(&dispatch_tid, NULL, dispatchCar, (void *)(intptr_t)direction);
        pthread_detach(dispatch_tid);
        sleep(1); // 1-second pacing requirement between cars
    }
}

// Fixed signature for pthread
void *readStateOfPort(void *arg){
    char receivedData;
    while(1){
        // Read blocks until a byte arrives from the AVR
        if (read(PORT, &receivedData, 1) > 0) {
            pthread_mutex_lock(&StateMutex);
            if (receivedData == NorthGreen) {
                lightStatus = NORTH;
            } else if (receivedData == SouthGreen){
                lightStatus = SOUTH;
            } else if (receivedData == RedRed){
                lightStatus = RED;
            }
            pthread_mutex_unlock(&StateMutex);
            
            printInfo();

            checkToDispatchCar(NORTH);
            checkToDispatchCar(SOUTH);
        }
    }
    return NULL;
}

// Fixed signature for pthread
void *readUserInput(void *arg){
    char input;
    // Turn off terminal echoing so 'n' and 's' don't mess up the formatting
    system("stty -icanon -echo"); 

    while((input = getchar())){
        if (input == 's'){
            pthread_mutex_lock(&StateMutex);
            queue[SOUTH]++;
            pthread_mutex_unlock(&StateMutex);
            writePorts(SouthQueueArrival);
            printInfo();
        }
        else if (input == 'n'){
            pthread_mutex_lock(&StateMutex);
            queue[NORTH]++;
            pthread_mutex_unlock(&StateMutex);
            writePorts(NorthQueueArrival);
            printInfo();
        }
        else if (input == 'e'){
            system("stty icanon echo"); // Restore terminal state before quitting
            exit(0);
        }
    }
    return NULL;
}

int main(){
    initPorts();
    
    pthread_t readUserInput_t, readStateOfPort_t;

    printInfo();

    pthread_create(&readUserInput_t, NULL, readUserInput, NULL);
    pthread_create(&readStateOfPort_t, NULL, readStateOfPort, NULL);

    // Keep the main thread alive waiting for the input thread to finish (when 'e' is pressed)
    pthread_join(readUserInput_t, NULL); 
    return 0;
}