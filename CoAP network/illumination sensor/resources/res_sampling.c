#include "contiki.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coap-engine.h"
#include "../global_variables.h"

// Define the maximum age of a resource
#define MAX_AGE 60

// Initialize the sampling time to 10
int sampling = 10;

// Define a static function to handle POST requests to the resource
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

// Define a CoAP resource with the name "res_sampling"
RESOURCE(res_sampling,
         "title=\"Moisture\";rt=\"Temperature\";obs",
         NULL,
         res_post_handler,
         NULL,
         NULL);

// Define the res_post_handler function to handle POST requests to the resource
static void
res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    // Check if the response is NULL
    if (response == NULL)
    {
        // Print an error message if the response is NULL
        printf("Error while changing sampling time\n");
        return;
    }

    // Get the payload from the request
    const uint8_t *payload = NULL;
    int len = coap_get_payload(request, &payload);

    // Create a temporary buffer to store the payload as a string
    char string[len + 1];

    // Copy the payload into the temporary buffer
    memcpy(string, payload, len);
    string[len] = '\0';

    // Convert the payload to an integer and store it in the sampling variable
    sampling = atoi(string);

    // Log the new sampling time
    printf("Sampling set as: %i\n", sampling);

    // Construct the response payload
    len = snprintf((char *)buffer, preferred_size, "Sampling time set as: %i seconds", sampling);

    // Set the content format of the response to text/plain
    coap_set_header_content_format(response, TEXT_PLAIN);

    // Set the ETag of the response to the length of the payload
    coap_set_header_etag(response, (uint8_t *)&len, 1);

    // Set the payload of the response
    coap_set_payload(response, (uint8_t *)buffer, len);
}