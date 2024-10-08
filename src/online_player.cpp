#include "online_player.h"
#include "network_manager.h"
#include "input_handler.h"

OnlinePlayer::OnlinePlayer(char white_plays)
        : Player(white_plays) {}

bool OnlinePlayer::get_promotion(char &piece) {
    std::string in;
    std::cout << "To what piece u want to promote? (q, b, n, r)" << std::endl;
    // Get positions
    std::cin >> in;
    // TODO: add online input
    if (in.length() > 1) {
        std::cout << "Enter just 1 word!" << std::endl;
        return false;
    }
    Input_handler input_handler;
    if (!input_handler.coordinates_to_int(in[0])) {
        std::cout << "Wrong piece!" << std::endl;
        return false;
    }
    if (tolower(in[0]) == 'k' || tolower(in[0]) == 'p') {
        std::cout << "Wrong piece!" << std::endl;
        return false;
    }
    piece = in[0];
    return true;
}

bool OnlinePlayer::get_move(Board &board) {
    std::string start, end;

    NetworkManager server;
    std::vector<char> response;

    // TODO: change later
    std::string url = "http://127.0.0.1:8080";
    std::string payload = "BLACK plays!\nWhat is your move?.";
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();

    
    const auto start_timer = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed;
    
    // Get positions
    while (true) {
        response = server.request(curl, url, payload);

        const auto now = std::chrono::steady_clock::now();
        elapsed = now - start_timer;

        if (!response.empty() || elapsed.count() >= 10.0) {
            server.close_server(curl);
            break;
        }
    }
    std::string online_input {response.begin(), response.end()}; 
    std::cout << "Converted input:\n" << online_input << std::endl;

    // TODO: split online_input into 'start' and 'end'
    std::string str {response.begin(), response.end()};
    Input_handler input_handler;
    input_handler.start_parse(str, start, end);

    if (start == "save") {
        ask_draw_ = save;
        return true;
    }

    if (start == "resign") {
        ask_draw_ = resign;
        return true;
    }

    if (start == "draw") {
        ask_draw_ = draw_asked;
        return true;
    }

    if (start == "yes") {
        ask_draw_ = draw;
        return true;
    }

    if (start == "no") {
        ask_draw_ = no_draw;
        return true;
    }

    if (!input_handler.extract_input(start, start_)) {
        return false;
    }

    if (!input_handler.extract_input(end, end_)) {
        return false;
    }

    return true;
}

