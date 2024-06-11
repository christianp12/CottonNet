#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "../../cJSON/cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../global_variables.h"

/* Log configuration */
#include "coap-log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

// External variables to store the current light state and the attuator state
extern int light_attuatore;
extern int co2;
extern int fase;

// Function declarations for CoAP request handlers
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

// Event resource definition for the observation resource
EVENT_RESOURCE(res_observation,
               "title=\"Observation\";rt=\"\";obs",
               res_get_handler,
               NULL,
               NULL,
               NULL,
               res_event_handler);

// GET handler function to return the current observation state in JSON format
static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

    // Print a message to indicate that the observation is being sent
    printf("observation sending\n");

    // Create a JSON object to store the observation data
    cJSON *row = cJSON_CreateObject();
    if (row == NULL)
    {
        printf("Failed to create JSON object\n");
        return;
    }

    // Add the CO2, light, and phase values to the JSON object
    cJSON_AddNumberToObject(row, "c", co2);
    cJSON_AddNumberToObject(row, "l", light_attuatore);
    cJSON_AddNumberToObject(row, "p", fase);

    // Convert the JSON object to a string
    char *payload = cJSON_PrintUnformatted(row);
    // Print the payload and its length
   

    // Set the content format of the response to JSON
    coap_set_header_content_format(response, APPLICATION_JSON);

    // Set the payload of the response
    coap_set_payload(response, (uint8_t *)payload, strlen(payload));

}

// Event handler function to notify observers about the observation state
static void res_event_handler(void)
{
    // Print a message to indicate that a notification is being sent
    printf("Sending notification\n");

    // Notify all the observers about the observation state
    coap_notify_observers(&res_observation);

    // Print a message to indicate that the notification has been sent
    printf(" notification sent\n");
}