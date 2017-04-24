#ifndef CONFIG_H
#define CONFIG_H
#include <string>

class Config {
    public:
        void update(std::string config);
        int port;

        Config(std::string json);


};


extern Config config;


#endif
