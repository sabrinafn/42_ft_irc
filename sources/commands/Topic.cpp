#include "../includes/ft_irc.hpp"

/* HANDLETOPIC */
void Commands::handleTopic(Client &client, Server &server, const IRCMessage &msg) {
   
	if (msg.params.empty() || msg.params.size() < 2) { 
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }
    std::string channelName = msg.params[0];
    std::map<std::string, Channel*> all_channels = server.get_channels();
    if(all_channels.find(channelName) != all_channels.end()) {
        client.sendReply(ERR_NOSUCHCHANNEL(channelName));
        return;
    }
    Channel* channel = all_channels[channelName];
    // Sem parâmetro de tópico → mostrar tópico atual
    if (msg.params.size() == 1) {
        std::string topic = channel->getTopic();
        if (topic.empty())
		{
			client.sendReply(RPL_NOTOPIC(client.getNickname(), channelName));
            return;
		}
        else
		{
			client.sendReply(RPL_TOPIC(client.getNickname(), channelName, channel->getTopic()));
            return;
		}
    }
    // Com parâmetro → tentar alterar tópico
    // Montar o novo tópico (suporta espaços)
    std::string newTopic;
    for (size_t i = 1; i < msg.params.size(); ++i) {
        newTopic += msg.params[i];
        if (i != msg.params.size() - 1)
            newTopic += " ";
    }

    // Checar permissões
    if (channel->hasMode(Channel::TOPIC_RESTRICTED) && !channel->isOperator(&client)) {
        //return "482 " + client.getNickname() + " " + channelName + " :You're not channel operator"; // ERR_CHANOPRIVSNEEDED
    }

    // Atualiza o tópico
    channel->setTopic(newTopic);

    // Notificar todos os membros do canal
    std::string response = RPL_TOPIC(client.getNickname(), channelName, newTopic);
    channel->broadcast(response, &client);

    return ; // já notificou todos, não precisa de retorno adicional
}
