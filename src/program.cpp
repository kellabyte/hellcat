#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>
#include <haywire.h>
#include "lmdb.h"
#include "common.h"
#include "storage/store.h"
#include "storage/lmdb_store.h"
#include "indexing/index_dictionary.h"
#include "indexing/index_writer.h"
#include "haywire.h"
#include "hellcat.h"

using namespace std;
using namespace chrono;
using namespace hellcat::storage;

void create_http_endpoint();
void get_root(http_request* request, hw_http_response* response, void* user_data);
void response_complete(void* user_data);

static unique_ptr<Store> store;

int main(int argc, char* argv[]) {
    store = unique_ptr<Store>(new LMDBStore());
    int rc = store->open("/tmp/hellcat_data", true);
    
    create_http_endpoint();

    return 0;
}

void create_http_endpoint()
{
    char route[] = "/";
    configuration config;
    config.http_listen_address = "0.0.0.0";
    config.http_listen_port = 8000;
    
    hw_init_with_config(&config);
    hw_http_add_route(route, get_root, NULL);
    hw_http_open(16);
}

#define CRLF "\r\n"

void response_complete(void* user_data)
{
}

void get_root(http_request* request, hw_http_response* response, void* user_data)
{
    int rc = 0;
    hw_string status_code;
    hw_string body;
    body.length = 0;
    
    if (request->method == HW_HTTP_GET)
    {
        // Process GET request.
        hcat_keypair pair;
        pair.keyspace = string_ref(hw_get_header(request, "keyspace"));
        pair.key = string_ref(hw_get_header(request, "key"));
        
        if (pair.keyspace.data() != NULL)
        {
            hcat_transaction* tx;
            store->begin_transaction(&tx, 1);
            rc = tx->get(&pair);
            tx->commit();
        
            if (rc == 0)
            {
                SETSTRING(status_code, HTTP_STATUS_200);
                body.value = (char*)pair.value;
                body.length = pair.value_length;
            }
            else if (rc != 0)
            {
                SETSTRING(status_code, HTTP_STATUS_404);
                SETSTRING(body, "hello world");
            }
            delete tx;
        }
        else
        {
            SETSTRING(status_code, HTTP_STATUS_404);
            SETSTRING(body, "FAIL");
        }
    }
    else if (request->method == HW_HTTP_PUT || request->method == HW_HTTP_POST)
    {
        // Process SET request.
        hcat_transaction* tx;
        store->begin_transaction(&tx, 0);
        hcat_keypair pair;
        pair.keyspace = string_ref(hw_get_header(request, "keyspace"));
        pair.key = string_ref(hw_get_header(request, "key"));
        string_ref val = string_ref(hw_get_header(request, "value"));
        pair.value = (void*)val.data();
        pair.value_length = val.length();
        
        if (pair.keyspace.length() != 0 && pair.key.length() != 0 && pair.value_length != 0)
        {
            rc = tx->set(&pair);
            rc = tx->commit();

            //rc = store->sync();
            SETSTRING(status_code, HTTP_STATUS_200);
            SETSTRING(body, "OK");
        }
        else
        {
            tx->abort();
            char* key = hw_get_header(request, "key");
            pair.key = string_ref(hw_get_header(request, "key"));
            SETSTRING(status_code, HTTP_STATUS_404);
            SETSTRING(body, "FAIL");
        }
        delete tx;
    }
    
    hw_string content_type_name;
    hw_string content_type_value;
    hw_string keep_alive_name;
    hw_string keep_alive_value;
    
    SETSTRING(content_type_name, "Content-Type");
    
    SETSTRING(content_type_value, "text/html");
    hw_set_response_header(response, &content_type_name, &content_type_value);
    
    hw_set_response_status_code(response, &status_code);
    hw_set_body(response, &body);
    
    if (request->keep_alive)
    {
        SETSTRING(keep_alive_name, "Connection");
        
        SETSTRING(keep_alive_value, "Keep-Alive");
        hw_set_response_header(response, &keep_alive_name, &keep_alive_value);
    }
    else
    {
        hw_set_http_version(response, 1, 0);
    }
    
    hw_http_response_send(response, NULL, response_complete);
}