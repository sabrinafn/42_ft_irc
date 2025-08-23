#include "../includes/server/Server.hpp"
#include "../includes/channel/Channel.hpp"
#include "../includes/standardReplies/StdReplies.hpp"

bool check_params(Client &client, const IRCMessage &msg);

std::string Server::buildMessageFromParams(const std::vector<std::string>& params) {
    std::string message;

    for (size_t i = 1; i < params.size(); ++i) {
        message += params[i];
        if (i != params.size() - 1)
            message += " ";
    }
    if (!message.empty() && message[0] == ':')
        message = message.substr(1);

    return message;
}

bool check_params(Client &client, const IRCMessage &msg){
        //colocar if de autenticação
    if (msg.params.empty() || msg.params.size() < 2) { 
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return false;
    }
    	if (msg.params[1][0] != ':') {
		client.sendReply(ERR_NOTEXTTOSEND(msg.command));
		return false;
	}
    return true;
}

bool Server::sendMsgToChannel(Client &client, const IRCMessage &msg)
{
    Channel *channel;
    std::string dest = msg.params[0];

    if(dest[0] == '#' && channels.find(dest) != channels.end()) {  
        channel = channels[dest];
        if (!channel->isMember(&client)) {
        client.sendReply(ERR_NOTONCHANNEL(dest));
            return false;
        }
            ///ver se é necessario fazer a validação de onlyinvited
        std::string message =  buildMessageFromParams(msg.params);
        std::string response = RPL_PRIVMSG(client.getNickname(), dest, message);
        channel->broadcast(response, &client);
    }
    else{
        client.sendReply(ERR_NOSUCHCHANNEL(dest));
        return false;
    }

    return true;
}

bool  Server::sendMsgToClient(Client &client, const IRCMessage &msg) {

    std::string dest = msg.params[0];
    std::string message =  buildMessageFromParams(msg.params);

    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->getNickname() == dest) {
            std::string fullMessage = RPL_PRIVMSG(client.getNickname(), dest, message);
            clients[i]->sendReply(fullMessage); 
            return true; // encontrou, não precisa continuar
        }
    }
    // se não encontrar o cliente:
    client.sendReply(ERR_NOSUCHNICK(dest));
    //verificar se o erro esta correto
    return false;
}

void Server::handlePrivmsg(Client &client, const IRCMessage &msg){ 


    if(!check_params(client, msg))
        return;
    if(sendMsgToChannel(client, msg))
        return;
    if(sendMsgToClient(client, msg))
        return;
    
    return;
}
