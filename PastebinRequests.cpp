#include <iostream>
#include <string>
#include <curl/curl.h>
#include "cpr.h"
class PastebinCPR {
    public:
        enum class PasteStatus {
            Public,
            Unlisted,
            Private
        };

        Pastebin(const std::string& login, const std::string& password, const std::string& devKey)
                : api_dev_key(devKey) {
            LoginUser(login, password, devKey);
        }

        void sendPaste(PasteStatus pasteStatus, const std::string& pastebinFile, const std::string& pastebinText,
                       const std::string& pasteFormat = "", const std::string& expireDate = "") {
            auto response = cpr::Post(
                    cpr::Url{"https://pastebin.com/api/api_post.php"},
                    cpr::Payload{
                            {"api_dev_key", api_dev_key},
                            {"api_option", "paste"},
                            {"api_paste_name", pastebinFile},
                            {"api_paste_code", pastebinText},
                            {"api_paste_private", std::to_string(static_cast<int>(pasteStatus))},
                            {"api_paste_format", pasteFormat},
                            {"api_paste_expire_date", expireDate},
                            {"api_user_key", api_user_key}
                    }
            );
        }

        void removePaste(const std::string& pastebinUrl) {
            size_t start_pos = pastebinUrl.find_last_of('/') + 1;
            std::string pasteKey = pastebinUrl.substr(start_pos);
            auto response = cpr::Post(
                    cpr::Url{"https://pastebin.com/api/api_post.php"},
                    cpr::Payload{
                            {"api_dev_key", api_dev_key},
                            {"api_option", "delete"},
                            {"api_user_key", api_user_key},
                            {"api_paste_key", pasteKey}
                    }
            );
        }

        std::string showPaste(const std::string& pastebinUrl) {
            size_t start_pos = pastebinUrl.find_last_of('/') + 1;
            std::string pasteKey = pastebinUrl.substr(start_pos);
            auto response = cpr::Post(
                    cpr::Url{"https://pastebin.com/api/api_post.php"},
                    cpr::Payload{
                            {"api_dev_key", api_dev_key},
                            {"api_option", "show_paste"},
                            {"api_user_key", api_user_key},
                            {"api_paste_key", pasteKey}
                    }
            );
            return response.text;
        }

    private:
        void LoginUser(const std::string& login, const std::string& password, const std::string& devKey) {
            auto response = cpr::Post(
                    cpr::Url{"https://pastebin.com/api/api_login.php"},
                    cpr::Payload{
                            {"api_dev_key", api_dev_key},
                            {"api_user_name", login},
                            {"api_user_password", password}
                    }
            );
            api_user_key = (response.status_code == 200 ? response.text : "");
        }

        std::string api_dev_key;
        std::string api_user_key;
};
class PastebinCurl {
    public:
        enum class PasteStatus {
            Public,
            Unlisted,
            Private
        };

        PastebinCurl(const std::string& login, const std::string& password, const std::string& devKey)
                : api_dev_key(devKey) {
            LoginUser(login, password, devKey);
        }

        void sendPaste(PasteStatus pasteStatus, const std::string& pastebinFile, const std::string& pastebinText,
                       const std::string& pasteFormat = "", const std::string& expireDate = "") {
            CURL *curl;
            CURLcode res;
            curl = curl_easy_init();
            if (curl) {
                std::string postData = "api_dev_key=" + api_dev_key +
                                       "&api_option=paste" +
                                       "&api_paste_name=" + pastebinFile +
                                       "&api_paste_code=" + pastebinText +
                                       "&api_paste_private=" + std::to_string(static_cast<int>(pasteStatus)) +
                                       "&api_paste_format=" + pasteFormat +
                                       "&api_paste_expire_date=" + expireDate +
                                       "&api_user_key=" + api_user_key;

                curl_easy_setopt(curl, CURLOPT_URL, "https://pastebin.com/api/api_post.php");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

                res = curl_easy_perform(curl);

                curl_easy_cleanup(curl);
            }
        }

        void removePaste(const std::string& pastebinUrl) {
            size_t start_pos = pastebinUrl.find_last_of('/') + 1;
            std::string pasteKey = pastebinUrl.substr(start_pos);

            CURL *curl;
            CURLcode res;
            curl = curl_easy_init();
            if (curl) {
                std::string postData = "api_dev_key=" + api_dev_key +
                                       "&api_option=delete" +
                                       "&api_user_key=" + api_user_key +
                                       "&api_paste_key=" + pasteKey;

                curl_easy_setopt(curl, CURLOPT_URL, "https://pastebin.com/api/api_post.php");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

                res = curl_easy_perform(curl);

                curl_easy_cleanup(curl);
            }
        }

        std::string showPaste(const std::string& pastebinUrl) {
            size_t start_pos = pastebinUrl.find_last_of('/') + 1;
            std::string pasteKey = pastebinUrl.substr(start_pos);

            CURL *curl;
            CURLcode res;
            std::string readBuffer;

            curl = curl_easy_init();
            if (curl) {
                std::string postData = "api_dev_key=" + api_dev_key +
                                       "&api_option=show_paste" +
                                       "&api_user_key=" + api_user_key +
                                       "&api_paste_key=" + pasteKey;

                curl_easy_setopt(curl, CURLOPT_URL, "https://pastebin.com/api/api_post.php");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

                res = curl_easy_perform(curl);

                curl_easy_cleanup(curl);
            }

            return readBuffer;
        }

    private:
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output) {
            output->append((char *) contents, size * nmemb);
            return size * nmemb;
        }

        void LoginUser(const std::string& login, const std::string& password, const std::string& devKey) {
            CURL *curl;
            CURLcode res;
            curl = curl_easy_init();
            if (curl) {
                std::string postData = "api_dev_key=" + api_dev_key +
                                       "&api_user_name=" + login +
                                       "&api_user_password=" + password;

                curl_easy_setopt(curl, CURLOPT_URL, "https://pastebin.com/api/api_login.php");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &api_user_key);

                res = curl_easy_perform(curl);

                curl_easy_cleanup(curl);
            }
        }

        std::string api_dev_key;
        std::string api_user_key;
    };


int main() {
    //select the one you really love. I recommend to use PastebinCPR <3
    PastebinCPR pastebin("your_login", "your_password", "your_dev_key");
    PastebinCurl pastebin("your_login", "your_password", "your_dev_key");

    pastebin.sendPaste(PastebinCurl::PasteStatus::Private, "example.cpp", "int main() { return 0; }", "cpp", "N");
    pastebin.removePaste("https://pastebin.com/your_paste_url");
    std::cout << pastebin.showPaste("https://pastebin.com/your_paste_url") << std::endl;

    return 0;
}