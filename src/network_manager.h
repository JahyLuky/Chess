#pragma once

#include <vector>
#include <chrono>
#include <iostream>
#include <curl/curl.h>
#include <string.h>

class NetworkManager {
public:
    void close_server(CURL * curl);

    // way of getting and saving the response
    static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

    // sends request and returns the response
    std::vector<char> request(CURL *curl, const std::string &url, const std::string &payload);

protected:
    CURL *curl_;
};