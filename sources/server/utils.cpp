#include "../includes/server/Server.hpp"

bool Server::isValidChannelName(const std::string& name) {

    if (name[0] != '#' && name[0] != '&')
        return false;

    for (size_t i = 1; i < name.size(); ++i) {
        unsigned char c = name[i];
        if (std::isspace(c))         // espaço
            return false;
        if (c == ':')                // caractere especial proibido
            return false;
        if (std::iscntrl(c))         // caracteres de controle
            return false;
    }
	return true;
}

bool Server::isValidkey(std::string key){

	   for (size_t i = 1; i < key.size(); ++i) {
        unsigned char c = key[i];
        if (std::isspace(c))         // espaço
            return false;
        if (c == ':')                // caractere especial proibido
            return false;
        if (std::iscntrl(c))         // caracteres de controle
            return false;
	   }
       return true;
}

	void Server::addChannel(Channel* new_channel) {
		channels[new_channel->getName()] = new_channel;
	}

