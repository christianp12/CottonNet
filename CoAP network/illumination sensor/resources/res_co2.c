#include "contiki.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coap-engine.h"
#include "contiki-net.h"
#include "coap-blocking-api.h"
#include "os/dev/leds.h"

// Function declarations for handling CoAP requests and events
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

// Function to initialize the CO2 level
void initialize_co2_level(void);

// Global variables to store the CO2 level and related flags
int co2_level = 0;
int co2 = 0;
int too_high = 1500;
int too_low = 400;
bool switched = 0;
int counter = 0;

// Event resource definition for CoAP
EVENT_RESOURCE(res_co2,
               "title=\"CO2\";rt=\"\";obs",
               res_get_handler,
               res_post_handler,
               NULL,
               NULL,
               res_event_handler);

// Initialize the CO2 level with a random value between 200 and 2000
void initialize_co2_level(void)
{
    co2_level = (rand() % 1800) + 200;
    co2 = co2_level;
}

// Event handler function to notify observers and update the CO2 level
static void res_event_handler(void)
{
    if (co2_level == 0 || counter == 2 || switched)
    {
        // Re-initialize the CO2 level and reset flags
        initialize_co2_level();
        counter = 0;
        switched = false;
        // Notify all observers
        coap_notify_observers(&res_co2);
    }
    else if (co2_level < too_low || co2_level > too_high) // Check if CO2 level is out of range
    {
        switched = true;
        // Notify all observers
        coap_notify_observers(&res_co2);
    }
    counter++;
}

// GET handler function to respond to CoAP GET requests
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    // Set the response content format to JSON
    coap_set_header_content_format(response, APPLICATION_JSON);
    // Create a JSON payload with the current CO2 level
    snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "{\"co2_level\": %d}", co2_level);
    // Set the response payload
    coap_set_payload(response, buffer, strlen((char *)buffer));
}

static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    // Get the payload from the CoAP request
    const uint8_t *payload = NULL;
    char new_co2_level[32];
    coap_get_payload(request, &payload);
    // Parse the payload to extract the new CO2 level
    sscanf((const char *)payload, "%s", new_co2_level);
    // Update the CO2 level
    co2_level = atoi(new_co2_level);
    // Set the response content format to JSON
    coap_set_header_content_format(response, APPLICATION_JSON);
    // Create a JSON payload with the updated CO2 level
    snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "{\"co2_level\": %d}", co2_level);
    // Set the response payload
    coap_set_payload(response, buffer, strlen((char *)buffer));
}