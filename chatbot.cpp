#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Chatbot {
private:
    std::string api_url;
    std::string api_key;

    // Function to make HTTP POST requests
    static std::string makeHttpRequest(const std::string& url, const std::string& data) {
        CURL* curl;
        CURLcode res;
        std::string response_string;
        
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl) {
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");

            // Replace with actual API key and headers
            headers = curl_slist_append(headers, ("Authorization: Bearer " + api_key).c_str());

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                std::cerr << "Curl request failed: " << curl_easy_strerror(res) << std::endl;
            }

            curl_easy_cleanup(curl);
            curl_global_cleanup();
        }

        return response_string;
    }

    // Callback function to collect data from the HTTP response
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t total_size = size * nmemb;
        output->append((char*)contents, total_size);
        return total_size;
    }

public:
    Chatbot(const std::string& url, const std::string& key) : api_url(url), api_key(key) {}

    // Function to interact with the NLP API (e.g., Dialogflow, Watson, etc.)
    std::string processUserMessage(const std::string& user_message) {
        json request_body;
        request_body["query"] = user_message; // For example, Dialogflow requires "query" field

        std::string response = makeHttpRequest(api_url, request_body.dump());

        // Parse the response and handle accordingly
        json response_json = json::parse(response);
        std::string bot_reply = response_json["reply"].get<std::string>();

        return bot_reply;
    }

    // Function for sentiment analysis (could be another API call to a service like IBM Watson)
    std::string analyzeSentiment(const std::string& user_message) {
        // Call sentiment analysis API (e.g., IBM Watson API for sentiment analysis)
        json request_body;
        request_body["text"] = user_message;

        std::string response = makeHttpRequest("https://api.ibm.com/sentiment", request_body.dump());

        json response_json = json::parse(response);
        std::string sentiment = response_json["sentiment"].get<std::string>();

        return sentiment;
    }
};

int main() {
    // Initialize the chatbot with the appropriate API URL and key
    Chatbot chatbot("https://api.dialogflow.com/v1/query", "YOUR_DIALOGFLOW_API_KEY");

    std::string user_input;
    while (true) {
        std::cout << "User: ";
        std::getline(std::cin, user_input);

        if (user_input == "exit") break;

        // Analyze sentiment
        std::string sentiment = chatbot.analyzeSentiment(user_input);
        std::cout << "Sentiment: " << sentiment << std::endl;

        // Process the message and get the chatbot's response
        std::string bot_response = chatbot.processUserMessage(user_input);
        std::cout << "Bot: " << bot_response << std::endl;
    }

    return 0;
}

