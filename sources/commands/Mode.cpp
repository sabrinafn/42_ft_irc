void Commands::handleMode(Client &client, Server &server, const IRCMessage &msg) {
    if (msg.params.empty()) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }

    std::string target = msg.params[0];

    // Checar se é canal ou usuário
    if (!target.empty() && target[0] == '#') {
        // ----- MODE em Canal -----
        std::map<std::string, Channel*> all_channels = server.get_channels();
        if (all_channels.find(target) == all_channels.end()) {
            client.sendReply(ERR_NOSUCHCHANNEL(target));
            return;
        }
        Channel* channel = all_channels[target];

        // Sem modos → retornar os modos atuais
        if (msg.params.size() < 2) {
            client.sendReply(RPL_CHANNELMODEIS(channel->getName(), channel->getModesAsString()));
            return;
        }

        // Precisa ser operador
        if (!channel->isOperator(&client)) {
            client.sendReply(ERR_CHANOPRIVSNEEDED(client.getNickname(), target));
            return;
        }

        std::string modes = msg.params[1];
        bool adding = true;
        size_t paramIndex = 2;

        for (size_t i = 0; i < modes.size(); i++) {
            char flag = modes[i];
            if (flag == '+') { adding = true; continue; }
            if (flag == '-') { adding = false; continue; }

            switch (flag) {
                case 'i':
                    channel->setMode(Channel::INVITE_ONLY, adding);
                    break;
                case 't':
                    channel->setMode(Channel::TOPIC_RESTRICTED, adding);
                    break;
                case 'k':
                    if (adding) {
                        if (paramIndex >= msg.params.size()) {
                            client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
                            return;
                        }
                        channel->setKey(msg.params[paramIndex++]);
                    } else {
                        channel->clearKey();
                    }
                    break;
                case 'l':
                    if (adding) {
                        if (paramIndex >= msg.params.size()) {
                            client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
                            return;
                        }
                        int limit = atoi(msg.params[paramIndex++].c_str());
                        channel->setLimit(limit);
                    } else {
                        channel->clearLimit();
                    }
                    break;
                case 'o': {
                    if (paramIndex >= msg.params.size()) {
                        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
                        return;
                    }
                    std::string nick = msg.params[paramIndex++];
                    Client* targetClient = server.serverGetClientByNick(nick);
                    if (targetClient && channel->isMember(targetClient)) {
                        if (adding) channel->addOperator(targetClient);
                        else channel->removeOperator(targetClient);
                    }
                    break;
                }
                default:
                    client.sendReply(ERR_UNKNOWNMODE(flag, target));
            }
        }
    } else {
        // ----- MODE em Usuário -----
        Client* targetClient = server.serverGetClientByNick(target);
        if (!targetClient) {
            client.sendReply(ERR_NOSUCHNICK(target));
            return;
        }

        // Sem modos → retornar os modos atuais
        if (msg.params.size() < 2) {
            client.sendReply(RPL_UMODEIS(targetClient->getModesAsString()));
            return;
        }

        // Só o próprio usuário pode mudar seus modos (ou operador global se você implementar)
        if (&client != targetClient) {
            client.sendReply(ERR_USERSDONTMATCH());
            return;
        }

        std::string modes = msg.params[1];
        bool adding = true;

        for (size_t i = 0; i < modes.size(); i++) {
            char flag = modes[i];
            if (flag == '+') { adding = true; continue; }
            if (flag == '-') { adding = false; continue; }

            switch (flag) {
                case 'i': // invisível
                    targetClient->setMode('i', adding);
                    break;
                case 'o': // operador (normalmente só server pode dar)
                    if (adding)
                        client.sendReply(ERR_NOPRIVILEGES(client.getNickname()));
                    break;
                default:
                    client.sendReply(ERR_UMODEUNKNOWNFLAG());
            }
        }
    }
}
