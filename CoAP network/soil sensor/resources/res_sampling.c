#include "contiki.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coap-engine.h"
#include "../global_variables.h"

#define MAX_AGE 60
int sampling = 10; 

static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_sampling,
         "title=\"Moisture\";rt=\"Temperature\";obs",
         NULL,
         res_post_handler,
         NULL,
         NULL);

static void
res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    // non va
    const uint8_t *payload = NULL;
    if (response == NULL)
    {
        printf("Error while changing sampling time\n");
        return;
    }
    int len = coap_get_payload(request, &payload);
    char string[len + 1];
    // Copy the payload into a temporary buffer
    memcpy(string, payload, len);
    string[len] = '\0';

    // Convert the payload to float
    sampling = atoi(string);

    // Log the new threshold
    printf("Sampling set as: %i\n", sampling);

    // Construct the response payload
    len = snprintf((char *)buffer, preferred_size, "Sampling time set as: %i seconds", sampling);

    coap_set_header_content_format(response, TEXT_PLAIN);
    coap_set_header_etag(response, (uint8_t *)&len, 1);
    coap_set_payload(response, (uint8_t *)buffer, len);
}
