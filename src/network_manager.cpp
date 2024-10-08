#include "network_manager.h"

void NetworkManager::close_server (CURL * curl) {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

size_t NetworkManager::write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::vector<char> *response = reinterpret_cast<std::vector<char> *>(userdata);
    response->insert(response->end(), ptr, ptr + nmemb);
    return nmemb;
}

std::vector<char> NetworkManager::request(CURL *curl, const std::string &url, const std::string &payload) {
    std::vector<char> response;

    // Set the URL for the request
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Follow redirects and resolve via IPv4
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

    // Set the HTTP method to POST and attach the payload
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    // Set callback function for writing the response
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NetworkManager::write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    const auto start = std::chrono::steady_clock::now();
    CURLcode res = curl_easy_perform(curl);
    const auto end = std::chrono::steady_clock::now();

    // Check for errors
    if (res != CURLE_OK)
    {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << '\n';
    }

    // Log the time taken
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Request took: " << elapsed.count() << " s\n";

    return response;
}
