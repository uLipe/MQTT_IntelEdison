CC = gcc
CFLAGS = -O2 -Wall -c -I MQTTClient-C
LD = gcc

#ld uses the newlib nano
LDFLAGS = -lmraa

#add mqtt files stuff:
SOURCES  = MQTTClient-C/MQTTClient.c \
		   MQTTClient-C/MQTTConnectClient.c \
		   MQTTClient-C/MQTTConnectServer.c \
		   MQTTClient-C/MQTTFormat.c \
		   MQTTClient-C/MQTTLinux.c \
		   MQTTClient-C/MQTTPacket.c \
		   MQTTClient-C/MQTTSerializePublish.c \
		   MQTTClient-C/MQTTDeserializePublish.c \
		   MQTTClient-C/MQTTSubscribeClient.c \
		   MQTTClient-C/MQTTSubscribeServer.c \
		   MQTTClient-C/MQTTUnsubscribeClient.c \
		   MQTTClient-C/MQTTUnsubscribeServer.c \
 	   	   main.c


#link object recursion magic:
#(dont touch here!)
OBJS = $(SOURCES:.c=.o)


#define default targets
.PHONY: all
all: sensoriot.out
	chmod +x sensoriot.out


.PHONY: clean
clean:

	rm -rf MQTTClient-C/*.o
	rm -rf *.o
	rm -rf *.out


sensoriot.out: $(OBJS)
		@echo "[LD]linking files \n\r"
		$(LD) $(LDFLAGS) $(OBJS) -o $@

.c.o:
		@echo "[CC] compiling $< file \n\r"
		$(CC) $(CFLAGS) -o $@  $<
