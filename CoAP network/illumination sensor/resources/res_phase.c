#include "contiki.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coap-engine.h"

// Function declarations for CoAP request handlers
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

// Global variables to store the current phase and fase values
int phase = 0;
int fase = 0;

// Event resource definition for the phase resource
EVENT_RESOURCE(res_phase,
               "title=\"Phase\";rt=\"\";obs",
               res_get_handler,
               NULL,
               NULL,
               NULL,
               res_event_handler);

// Event handler function to generate a random phase value and notify observers
static void
res_event_handler(void)
{
    // Generate a random phase value: 0 if first phase, 1 if second phase
    int new_phase = rand() % 2;
    fase = new_phase;

    if (phase != new_phase)
    {
        // Notify all the observers about the phase change
        coap_notify_observers(&res_phase);
        phase = new_phase;
    }
}

// GET handler function to return the current phase value in JSON format
static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    // Set the content format of the response to JSON
    coap_set_header_content_format(response, APPLICATION_JSON);
    // Create a JSON payload with the current phase value
    snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "{\"phase\": %d}", phase);
    // Set the payload of the response
    coap_set_payload(response, buffer, strlen((char *)buffer));
}
