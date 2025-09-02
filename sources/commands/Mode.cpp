#include "../includes/ft_irc.hpp"


void handleMode(Client &client, Server &server, const IRCMessage &msg) {
    if (msg.params.empty()) return;

    std::string channelName = msg.params[0];

    if (!server.hasChannel(channelName)) {
        client.sendReply(ERR_NOSUCHCHANNEL(channelName));
        return;
    }

    Channel *channel = server.get_channels()[channelName];

    if (msg.params.size() == 1) {
        // Apenas retornar modos do canal
        // client.sendReply(RPL_CHANNELMODEIS(channelName, channel->getModes(), channel->getModeParams()));
        return;
    }

    if (!channel->isOperator(&client)) {
        // client.sendReply(ERR_CHANOPRIVSNEEDED(client.get_nickname(), channelName));
        return;
    }

    std::string mode = msg.params[1];

    // Validação do primeiro caractere: deve ser '+' ou '-'
    if (mode.empty() || (mode[0] != '+' && mode[0] != '-')) {
        client.sendReply(ERR_UNKNOWNMODE(mode)); // opcional: envia erro
        return;
    }

    std::string arg  = (msg.params.size() > 2) ? msg.params[2] : "";
    bool add = (mode[0] == '+');

    for (size_t i = 1; i < mode.size(); ++i) {
        switch (mode[i]) {
            case 'i':
                if (add) channel->addMode(Channel::INVITE_ONLY);
                else channel->removeMode(Channel::INVITE_ONLY);
                break;
            case 't':
                if (add) channel->addMode(Channel::TOPIC_RESTRICTED);
                else channel->removeMode(Channel::TOPIC_RESTRICTED);
                break;
            case 'k':
                if (add) {
                    channel->addMode(Channel::KEY_REQUIRED);
                    channel->setKey(arg);
                } else {
                    channel->removeMode(Channel::KEY_REQUIRED);
                    channel->setKey(""); // remove a senha
                }
                break;
            case 'l':
                if (add) {
                    channel->addMode(Channel::LIMIT_SET);
                    if (!arg.empty())
                        channel->setLimit(std::atoi(arg.c_str()));
                } else {
                    channel->removeMode(Channel::LIMIT_SET);
                    channel->removeLimit(); // remove o limite
                }
                break;
            default:
                // caractere de modo desconhecido, pode ignorar ou enviar erro
                break;
        }
    }
}