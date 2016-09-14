#include <iostream>
#include <stdio.h>
#include <coap/coap.h>

#define WITH_POSIX

using namespace std;

//Temperature GET handler
static void tempHandler(coap_context_t *ctx, struct coap_resource_t *resource,
                        const coap_endpoint_t *localInterface, coap_address_t *peer,
                        coap_pdu_t *request, str *token, coap_pdu_t *response)
{
    unsigned char buf[3];
    const char* response_data  = "72.4";
    response->hdr->code = COAP_RESPONSE_CODE(205);
    coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
    coap_add_data(response, strlen(response_data), (unsigned  char*)response_data);
    cout << "  Response sent!" << endl;
}

#define IPV6

//TODO: make observable
int main()
{
    coap_context_t *ctx;
    coap_address_t svr_addr;
    coap_resource_t *tempResource;
    fd_set readfds;

    cout << "Prep server socket..." << endl;
    coap_address_init(&svr_addr);

    #ifdef IPV6
    svr_addr.addr.sin.sin_family = AF_INET6;
    svr_addr.addr.sin.sin_port = htons(5683);
    svr_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;
    //*** The above works with example client: "coap-client -m get coap://[::1]/Temp" ***
    //svr_addr.addr.sin6.sin6_family = AF_INET6;
    //svr_addr.addr.sin6.sin6_addr = IN6ADDR_ANY_INIT;
    //svr_addr.addr.sin6.sin6_port = htons(5683);
    #endif // IPV6

    #ifdef IPV4
    svr_addr.addr.sin.sin_family = AF_INET;
    svr_addr.addr.sin.sin_addr.s_addr =  INADDR_ANY;
    svr_addr.addr.sin.sin_port = htons(5683);
    #endif // IPV4

    ctx = coap_new_context(&svr_addr);
    if(!ctx) exit(EXIT_FAILURE);

    cout << "Init Temp resource..." << endl;
    tempResource = coap_resource_init((const unsigned char *)"Temp", 4, 0);
    coap_register_handler(tempResource, COAP_REQUEST_GET, tempHandler);
    coap_add_resource(ctx, tempResource);

    cout << "Listening for connections..." << endl;

    while(1)
    {
        FD_ZERO(&readfds);
        FD_SET(ctx->sockfd, &readfds);
        int result = select(FD_SETSIZE, &readfds, 0,0, NULL);
        if(result < 0) //socket error
        {
            exit(EXIT_FAILURE);
        }
        else if(result > 0 && FD_ISSET(ctx->sockfd, &readfds)) //socket read
        {

            int x = coap_read(ctx);
            cout << "Socket READ" << endl;
        }
    }
    return 0;
}
