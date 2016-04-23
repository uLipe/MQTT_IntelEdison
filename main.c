/*
 * @file main.c
 * @brief main application entry point
 *
 */

#include <stdbool.h>
#include <string.h>
#include "MQTTClient.h"
#include <time.h>
#include <mraa.h>
#include <stdio.h>

#define ABS(x) (x < 0 ? -x : x)

/* Defines the number os messages to deposit */
#define NOOF_PUBLISH 128

/** Defines the tasks time slice */
#define ACC_SLICE       2
#define MQTT_SLICE      5

/* Define the network transfer */
static Network sensornw;
static Client  sensorClient;
static MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
static MessageData publish_message;

/** Forward references to thread funcs */
static void mqttTask(void *args);
static bool mqttInit(void);



/* global timer for tasks timings */
__timer_t t;

/* Simple hello world message */
char message[] = {"Hello MQTT!\n\r"};


/* Topic name */
static const char topic[] = {"MQTT-IntelEdison"};

/* BUffer for messaging */
static unsigned char msgBuff[256];
static unsigned char messageToTransmit[256];



/* timers for tasks */
unsigned int nowMQTT = 0;
static bool mqttSend = false;



/*
 * mqttTask()
 */
static void mqttTask(void *args)
{
    int rc = -1;

    printf("[MQTT] Running MQTT Task \n\r");

    /* Mount message frame */
    publish_message.message->qos = QOS2;
    publish_message.message->retained = true;
    publish_message.message->dup = false;
    publish_message.message->payload = (void *)&message;
    publish_message.message->payloadlen = sizeof(message);


    if(mqttSend != false ) {
            rc = MQTTPublish(&sensorClient, (const char *)&topic[0] ,publish_message.message);
            if(rc != 0) {
                printf("Message transmission failed \n\r");
            }
            else {
                printf("Message transmission passed \n\r");
            }
            mqttSend = false;
    }

}

/*
 * mqttTask()
 */
 static bool mqttInit(void)
 {
     bool ret = false;
     int rc = -1;

     memset(&messageToTransmit,0, sizeof(messageToTransmit));
     memset(&publish_message,0, sizeof(publish_message));

     /* Allocate the fields for message population */
     publish_message.message = (MessageData *)malloc(sizeof(MQTTMessage));
     if(publish_message.message == NULL) {
         printf("[MQTTTRACE] ponteiro nulo \n\r");
         while(1);
     }

     publish_message.topicName = (MQTTString *)malloc(32);

     /* Prefixes the topic name */
     strncpy(publish_message.topicName, &topic[0], sizeof(topic));

     /** Alloc the net interface */
     NewNetwork(&sensornw);
     rc = ConnectNetwork(&sensornw, "iot.eclipse.org", 1883);

     /* Check for errors */
     if(rc != 0){
         printf("Error, cannot connect to network \n\r");
     }
     else {

         printf("Connection accepted! \n\r");

         /* So if net were open, prepare mqtt client */
         MQTTClient(&sensorClient, &sensornw, 1000, messageToTransmit,
                     256, msgBuff, 256);

         /* Initialized tries to connect via mqtt */
         data.willFlag = 0;
         data.MQTTVersion = 3;
         data.clientID.cstring = "      ";
         data.username.cstring = "      ";
         data.password.cstring = "      ";
         data.keepAliveInterval = 60;
         data.cleansession = 1;
         rc = MQTTConnect(&sensorClient, &data);
         if(rc != 0 ){
             printf ("MQTT connection refused! \n\r");
             sensornw.disconnect(&sensornw);

         }
         else {
             int i = 0;
             /* Connection fully accepted: */
             printf ("Horray! MQTT connected! \n\r");
             ret = true;
         }
     }
     return(ret);
 }


/*
 * main()
 */
int main (int argc, char *argv[])
{
    memset(&sensorData,0, sizeof(sensorData));

    /* try to up the mqtt client */
    if(mqttInit()!=true) {
        printf("[MQTT] Mqtt failed to connect \n\r");
    }
    else {
        printf("[MQTT] Mqtt connected and is ready for use \n\r");
    }
    /*
     * Create and fire the threads:
     */
     while(1) {
         /* check for time to send packet */
         if(time(&t) - nowMQTT >= (MQTT_SLICE)){
             mqttSend = true;
             nowMQTT = time(&t);
             mqttTask(0);
         }

     }
    return 0;
}
