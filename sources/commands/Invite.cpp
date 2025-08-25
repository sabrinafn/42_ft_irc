#include "../includes/ft_irc.hpp"

/* HANDLEINVITE */
// INVITE <nickname> <channel>
void Commands::handleInvite(Client &client, Server &server, const IRCMessage &msg) {
	if (msg.params.empty() || msg.params.size() < 2) { 
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }

    std::string targetNick = msg.params[0];
    std::string channelName = msg.params[1];

    // 1. Checar se o canal existe
     std::map<std::string, Channel*> all_channels = server.get_channels();
    if (all_channels.find(channelName) != all_channels.end()) {
		client.sendReply(ERR_NOSUCHCHANNEL(channelName));
        return;
    }
	Channel* channel = all_channels[channelName];

    // 2. Checar se quem manda está no canal
    if (!channel->isMember(&client)) {
        client.sendReply(ERR_NOTONCHANNEL(channelName));
        return;
    }

    // 3. Buscar o convidado no servidor
    Client* target = server.serverGetClientByNick(targetNick);
    if (!target) {
        client.sendReply(ERR_NOSUCHNICK(targetNick));
        return;
    }
	///ver a diferença dos erros de nao achar o nick do destido e nao achar o nick no servidor
    // 4. Se canal for +i, só operador pode convidar
    if (channel->hasMode(Channel::INVITE_ONLY) && !channel->isOperator(&client)) {
        client.sendReply(ERR_CHANOPRISNEEDED(client.getNickname(), channelName));
        return;
    }

    // 5. Adicionar convidado na lista de convidados
    channel->invite(target);

    // 6. Notificar
    client.sendReply(RPL_INVITING(targetNick, client.getNickname(), channelName));
    target->sendReply(RPL_INVITE(targetNick, client.getNickname(), channelName));
}