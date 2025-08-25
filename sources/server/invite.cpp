#include "../includes/server/Server.hpp"
#include "../includes/channel/Channel.hpp"
#include "../includes/standardReplies/StdReplies.hpp"

void Server::handleInvite(Client &client, const IRCMessage &msg) {
    // INVITE <nickname> <channel>
	if (msg.params.empty() || msg.params.size() < 2) { 
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }

    std::string targetNick = msg.params[0];
    std::string channelName = msg.params[1];

    // 1. Checar se o canal existe
    if (channels.find(channelName) == channels.end()) {
		client.sendReply(ERR_NOSUCHCHANNEL(channelName));
        return;
    }
	Channel* channel = channels[channelName];

    // 2. Checar se quem manda está no canal
    if (!channel->isMember(&client)) {
        client.sendReply(ERR_NOTONCHANNEL(channelName));
        return;
    }

    // 3. Buscar o convidado no servidor
    Client* target = serverGetClientByNick(targetNick);
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