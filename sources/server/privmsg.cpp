#include "../includes/server/Server.hpp"
#include "../includes/channel/Channel.hpp"

bool check_params(Client &client, const IRCMessage &msg);

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
bool sendMsgToChannel();
bool Server::sendMsgToChannel(Client &client, const IRCMessage &msg)
{
    Channel *channel;
    std::string dest = dest;

    if(dest[0] == '#' && channels.find(dest) != channels.end()) {  
        channel = channels[dest];
        if (!channel->isMember(&client)) {
            //colocar o erro correto
            return;
        }
            ///ver se é necessario fazer a validação de onlyinvited

        std::string response = RPL_PRIVMSG(client.getNickname(), dest, msg.params[1]);
        channel->broadcast(response, &client);
    }
    else{
        client.sendReply(ERR_NOSUCHCHANNEL(dest));
        return false;
    }

    return true;
}
bool sendMagToClient();
bool sendMagToClient();

void Server::handlePrivmsg(Client &client, const IRCMessage &msg){ 

    Channel *channel;

    if(!check_params(client, msg))
        return;
    if(sendMsgToChannel(client, msg))
        return;
    
    
    return;
}