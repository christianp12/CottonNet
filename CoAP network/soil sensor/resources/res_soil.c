#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../cJSON/cJSON.h"
/* Log configuration */
#include "coap-log.h"
#define LOG_MODULE "App"

#define LOG_LEVEL LOG_LEVEL_APP
static int moisture = 0;
static int temperature = 0;
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_soil,
               "title=\"Moisture\";rt=\"Temperature\";",
               res_get_handler,
               NULL,
               NULL,
               NULL,
               res_event_handler);

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    cJSON *row = cJSON_CreateObject();
    cJSON_AddNumberToObject(row, "m", moisture);
    cJSON_AddNumberToObject(row, "t", temperature);
    char *payload = cJSON_PrintUnformatted(row);
   
    coap_set_header_content_format(response, APPLICATION_JSON);
    coap_set_payload(response, (uint8_t *)payload, strlen(payload));
}

// Event handler function
static void res_event_handler(void)
{
    moisture = rand() % 1018 + 4;   // Valore di umidità casuale
    temperature = rand() % 35 + 10; // Valore di temperatura casuale
    printf("sending notify to the server\n");
    coap_notify_observers(&res_soil);
    printf(" notification sent\n");
}