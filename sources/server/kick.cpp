#include "../includes/server/Server.hpp"
#include "../includes/channel/Channel.hpp"
#include "../includes/standardReplies/StdReplies.hpp"

// /KICK <#canal> <nick> [motivo]

void Server::handleKick(Client &client, const IRCMessage &msg) {
    
	if (msg.params.empty() || msg.params.size() < 2) { 
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }

     std::vector<std::string> _channels = split(msg.params[0], ',');
    std::vector<std::string> targets  = split(msg.params[1], ',');

    // Motivo (se houver)
    std::string reason;
    if (msg.params.size() > 2) {
        for (size_t i = 2; i < msg.params.size(); ++i) {
            reason += msg.params[i];
            if (i != msg.params.size() - 1)
                reason += " ";
        }
    } else {
        reason = client.getNickname();
    }

        ///// verificar o erro 442 e 441 em todos os comandos
    // Para cada canal
    for (size_t i = 0; i < _channels.size(); ++i) {
        std::string channelName = _channels[i];
        if (channels.find(channelName) == channels.end()) {
            client.sendReply(ERR_NOSUCHCHANNEL(channelName));
            continue;
        }
        
        Channel* channel = channels[channelName];
        if (!channel->isMember(&client)){
            client.sendReply(ERR_NOTONCHANNEL(channelName));
            continue;
        }

        if (!channel->isOperator(&client)) {
            client.sendReply(ERR_CHANOPRISNEEDED(client.getNickname(), channelName));
            continue;
        }

        // Para cada alvo
        for (size_t ti = 0; ti < targets.size(); ++ti) {
            std::string targetNick = targets[ti];
            Client* target = serverGetClientByNick(targetNick);
            if (!target) {
                client.sendReply(ERR_NOSUCHNICK(targetNick));
                continue;
            }

            if (!channel->isMember(target)) {
                client.sendReply(ERR_USERNOTINCHANNEL(targetNick, client.getNickname(), channelName));
                continue;
            }

            // Mensagem de KICK
            std::string kickMsg = ":" + client.getPrefix() + " KICK " + channelName + " " + targetNick + " :" + reason;

            // Notificar todos do canal
            channel->broadcast(kickMsg, &client);
            // Remover usuário
            channel->removeMember(target);
        }
    }

    return;
}