#include "contiki.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coap-engine.h"

// Function declarations for CoAP request handlers
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

// Global variables to store the current light state and the attuator state
int light = 0;
int light_attuatore = 0;

// Event resource definition for the light resource
EVENT_RESOURCE(res_light,
               "title=\"Light\";rt=\"\";obs",
               res_get_handler,
               res_post_handler,
               NULL,
               NULL,
               res_event_handler);

// Event handler function to generate a random light state and notify observers
static void
res_event_handler(void)
{
    // Generates a random value: 1 if there is light, 0 if not
    int new_light = rand() % 2;
    light_attuatore = new_light;
    if (light != new_light)
    {
        // Notify all the observers about the light state change
        coap_notify_observers(&res_light);
        light = new_light;
    }
}

// GET handler function to return the current light state in JSON format
static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    // Set the content format of the response to JSON
    coap_set_header_content_format(response, APPLICATION_JSON);
    // Create a JSON payload with the current light state
    snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "{\"light\": %d}", light);
    // Set the payload of the response
    coap_set_payload(response, buffer, strlen((char *)buffer));
}

// POST handler function to toggle the light state and notify observers
static void
res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    // Alternate the light value (toggle)
    light = (light == 0) ? 1 : 0;

    // Notify all the observers about the light state change
    coap_notify_observers(&res_light);
}