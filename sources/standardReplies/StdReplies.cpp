// #include "../includes/ft_irc.hpp"

// const std::string SERVER = "<3_irc_server_<3";
// const char        SPACE  = ' ';
// const char        COLON  = ':';
// const std::string CRLF   = "\r\n";

// std::string RPL_WELCOME(const std::string& nickname, const std::string& username) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << "00" << RPL_WELCOME_CODE << SPACE << nickname
//        << " :Welcome to the Internet Relay Chat, " << username << CRLF;
//     return ss.str();
// }

// std::string RPL_YOURHOST(const std::string& nickname) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << "00" << RPL_YOURHOST_CODE << SPACE << nickname
//        << " :Your host is " << SERVER << ", running version 1.0" << CRLF;
//     return ss.str();
// }

// std::string RPL_CREATED(const std::string& nickname, const std::string& startup_time) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << "00" << RPL_CREATED_CODE << SPACE << nickname
//        << " :This server was created on " << startup_time << CRLF;
//     return ss.str();
// }

// std::string RPL_MYINFO(const std::string& nickname, const std::string& usermodes,
//                        const std::string& channelmodes) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << "00" << RPL_MYINFO_CODE << SPACE << nickname << SPACE
//        << SERVER << " 1.0 " << usermodes << SPACE << channelmodes << CRLF;
//     return ss.str();
// }

// std::string RPL_NOTOPIC(const std::string& nickname, const std::string& channel) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << RPL_NOTOPIC_CODE << SPACE << nickname << SPACE << channel
//        << " :No topic is set" << CRLF;
//     return ss.str();
// }

// std::string RPL_TOPIC(const std::string& nickname, const std::string& channel,
//                       const std::string& topic) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << RPL_TOPIC_CODE << SPACE << nickname << SPACE << channel
//        << " :" << topic << CRLF;
//     return ss.str();
// }

// std::string RPL_NAMREPLY(const std::string& nickname, const std::string& channel,
//                          const std::string& members_list) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << RPL_NAMREPLY_CODE << SPACE << nickname << " = "
//        << channel << " :" << members_list << CRLF;
//     return ss.str();
// }

// std::string RPL_ENDOFNAMES(const std::string& nickname, const std::string& channel) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << RPL_ENDOFNAMES_CODE << SPACE << nickname << SPACE
//        << channel << " :End of NAMES list" << CRLF;
//     return ss.str();
// }

// std::string ERR_UNKNOWNCMD(const std::string& command) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_UNKNOWNCMD_CODE << " * " << command
//        << " :Unknown command" << CRLF;
//     return ss.str();
// }

// std::string ERR_NONICKNAMEGIVEN(void) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_NONICKNAMEGIVEN_CODE << " * :No nickname given"
//        << CRLF;
//     return ss.str();
// }

// std::string ERR_ERRONEUSNICKNAME(const std::string& nickname) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_ERRONEUSNICKNAME_CODE << " * " << nickname
//        << " :Erroneous nickname" << CRLF;
//     return ss.str();
// }

// std::string ERR_NICKNAMEINUSE(const std::string& nickname) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_NICKNAMEINUSE_CODE << " * " << nickname
//        << " :Nickname is already in use" << CRLF;
//     return ss.str();
// }

// std::string ERR_USERONCHANNEL(const std::string& nickname, const std::string& channel) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_USERONCHANNEL_CODE << " * " << nickname << SPACE
//        << channel << " :is already on channel" << CRLF;
//     return ss.str();
// }

// std::string ERR_NEEDMOREPARAMS(const std::string& command) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_NEEDMOREPARAMS_CODE << " * " << command
//        << " :Not enough parameters" << CRLF;
//     return ss.str();
// }

// std::string ERR_ALREADYREGISTRED(const std::string& nickname) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_ALREADYREGISTRED_CODE << " * " << nickname
//        << " :You may not re-register" << CRLF;
//     return ss.str();
// }

// std::string ERR_PASSWDMISMATCH(void) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_PASSWDMISMATCH_CODE << " * :Password incorrect"
//        << CRLF;
//     return ss.str();
// }

// std::string ERR_PASSREQUIRED(void) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_PASSWDMISMATCH_CODE << " * :Password required"
//        << CRLF;
//     return ss.str();
// }

// std::string ERR_CHANNELISFULL(const std::string& channel) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_CHANNELISFULL_CODE << " * " << channel
//        << " :Cannot join channel (+l)" << CRLF;
//     return ss.str();
// }

// std::string ERR_INVITEONLYCHAN(const std::string& channel) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_INVITEONLYCHAN_CODE << " * " << channel
//        << " :Cannot join channel (+i)" << CRLF;
//     return ss.str();
// }

// std::string ERR_BADCHANNELKEY(const std::string& username, const std::string& channel) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_BADCHANNELKEY_CODE << " * " << username << SPACE
//        << channel << " :Cannot join channel (+k)" << CRLF;
//     return ss.str();
// }

// std::string RPL_PRIVMSG(const std::string& prefix, const std::string& other,
//                         const std::string& message) {
//     std::stringstream ss;
//     ss << COLON << prefix << " PRIVMSG " << other << " :" << message << CRLF;
//     return ss.str();
// }

// std::string ERR_NOTEXTTOSEND(const std::string& nickname) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_NOTEXTTOSEND_CODE << " * " << nickname
//        << " :No text to send" << CRLF;
//     return ss.str();
// }

// std::string ERR_NOSUCHCHANNEL(const std::string& channel) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_NOSUCHCHANNEL_CODE << " * " << channel
//        << " :No such channel" << CRLF;
//     return ss.str();
// }

// std::string ERR_NOTONCHANNEL(const std::string& channel) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_NOTONCHANNEL_CODE << " * " << channel
//        << " :You're not on that channel" << CRLF;
//     return ss.str();
// }

// std::string ERR_NOSUCHNICK(const std::string& dest) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_NOSUCHNICK_CODE << " * " << dest << " :No such nick"
//        << CRLF;
//     return ss.str();
// }

// std::string ERR_CHANOPRIVSNEEDED(const std::string& nickname, const std::string& channel) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_CHANOPRIVSNEEDED_CODE << " * " << nickname << SPACE
//        << channel << " :You're not a channel operator" << CRLF;
//     return ss.str();
// }

// std::string ERR_USERNOTINCHANNEL(const std::string& targetNick, const std::string& nickname,
//                                  const std::string& channel) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << ERR_USERNOTINCHANNEL_CODE << " * " << targetNick << SPACE
//        << nickname << SPACE << channel << " :They aren't on that channel" << CRLF;
//     return ss.str();
// }

// std::string RPL_INVITING(const std::string& targetNick, const std::string& nickname,
//                          const std::string& channel) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << RPL_INVITING_CODE << " * " << nickname << SPACE
//        << targetNick << SPACE << channel << CRLF;
//     return ss.str();
// }

// std::string RPL_INVITE(const std::string& targetNick, const std::string& nickname,
//                        const std::string& channel) {
//     std::stringstream ss;
//     ss << COLON << SERVER << SPACE << nickname << " INVITE " << targetNick << SPACE << channel
//        << CRLF;
//     return ss.str();
// }

// std::string ERR_UNKNOWNMODE(const std::string& mode) {
//     return COLON + SERVER + " 472 * " + mode + " :is unknown mode char to me" + CRLF;
// }

// std::string RPL_CHANNELMODEIS(const std::string& channel, const std::string& mode,
//                               const std::string& modeParams) {
//     return COLON + SERVER + " 324 * " + channel + " " + mode + " " + modeParams + CRLF;
// }

// std::string ERR_NOTREGISTERED(const std::string& command) {
//     return COLON + SERVER + "451 " + command + " :You have not registered" + CRLF;
// }

// std::string ERR_SERVERSHUTDOWN(const std::string& nickname) {
//     return COLON + SERVER + " ERROR :Closing Link: " + nickname + " (Server shutting down)" +
//            CRLF;
// }
