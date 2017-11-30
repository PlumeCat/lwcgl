#ifndef _CUBE_CONFIG_H
#define _CUBE_CONFIG_H

class Config
{
    map<string, string> settings;
    friend class Game;
public:
    Config();
    ~Config();
};

#endif