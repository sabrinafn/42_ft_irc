#include "../includes/server/Server.hpp"

// bool Server::isValidChannelName(const std::string& name) {

//     if (name[0] != '#' && name[0] != '&')
//         return false;

//     for (size_t i = 1; i < name.size(); ++i) {
//         unsigned char c = name[i];
//         if (std::isspace(c))         // espaço
//             return false;
//         if (c == ':')                // caractere especial proibido
//             return false;
//         if (std::iscntrl(c))         // caracteres de controle
//             return false;
//     }
// 	return true;
// }

// bool Server::isValidkey(std::string key){

// 	   for (size_t i = 1; i < key.size(); ++i) {
//         unsigned char c = key[i];
//         if (std::isspace(c))         // espaço
//             return false;
//         if (c == ':')                // caractere especial proibido
//             return false;
//         if (std::iscntrl(c))         // caracteres de controle
//             return false;
// 	   }
//        return true;
// }

// 	void Server::addChannel(Channel* new_channel) {
// 		channels[new_channel->getName()] = new_channel;
// 	}


std::vector<std::string> Server::split(const std::string& str, char limit) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, limit)) {
        if (!item.empty())
            result.push_back(item);
    }
    return result;
}


Client* Server::serverGetClientByNick(const std::string& nick) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->getNickname() == nick)
            return clients[i];
    }
    return nullptr; // cliente não encontrado
}