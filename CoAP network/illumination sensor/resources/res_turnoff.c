#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "global_variables.h"
int turnoff = 0;
/* Handler function for the GET request */
static void res_turnoff_get(coap_message_t *request, coap_message_t *response,
                                     uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

// Resource definition for turning off the device
RESOURCE(res_turnoff,
         "title=\" spegnimento device\";rt=\"\"",
         res_turnoff_get,
         NULL,
         NULL,
         NULL);
         
static void res_turnoff_get(coap_message_t *request, coap_message_t *response,
                                     uint8_t *buffer, uint16_t preferred_size, int32_t *offset)

{ 
    //imposto variabile ad 1 per far uscire il nodo dal ciclo
     turnoff=1; 
    coap_set_payload(response, "Bye!", 19);
    printf("Plant Sensor Offline\n");
}
