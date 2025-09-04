/*
 * Channel.hpp
 *
 * Defines the Channel class, responsible for managing IRC channels,
 * including their members, modes, and message broadcasting.
 */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>
#include "../client/Client.hpp"

class Channel {
   public:
    // Enum modos do canal
    enum ChannelMode {
        INVITE_ONLY,      // +i → Apenas usuários convidados podem entrar
        TOPIC_RESTRICTED, // +t → Apenas operadores podem mudar o tópico
        KEY_REQUIRED,     // +k → Entrada exige senha
        LIMIT_SET         // +l → Há limite de usuários no canal
    };

   private:
    std::string name;  // Nome do canal (ex: #canal)
    std::string key;   // Senha do canal (se +k estiver ativo)
    std::string topic; // Tópico atual do canal
    int         limit; // Número máximo de usuários permitidos (se +l)

    // Client* = ponteiros pra clientes que ja foram criados no server
    std::vector<ChannelMode> modes;   // Modos ativos
    std::vector<Client*>     members; // Todos os membros do canal
    std::vector<Client*>     ops;     // Operadores do canal (modo +o)
    std::vector<Client*>     invited; // Usuários convidados (usado se canal for +i)

   public:
    /* CONSTRUCTOR */
    Channel(const std::string& name);

    /* COPY CONSTRUCTOR */
    Channel(const Channel& other);

    /* OPERATOR = */
    Channel& operator=(const Channel& other);

    /* DESTRUCTOR */
    ~Channel(void);

    bool isEmptyChannel() const;

    // GETTERS
    std::string              getName() const;        // nome do canal
    std::string              getTopic() const;       // tópico atual
    std::string              getKey() const;         // chave do canal
    int                      getLimit() const;       // limite de usuários
    std::vector<ChannelMode> getModes() const;       // modos ativos
    std::string              getModesString() const; // modos em string, ex: "+itkl"

    // SETTERS
    void setTopic(const std::string& newTopic); // Define novo tópico
    void setKey(const std::string& newKey);     // Define nova senha (key)
    void removeKey();                           // Remove a senha
    void setLimit(int newLimit);                // Define limite de membros
    void removeLimit();                         // Remove o limite

    // Gerenciamento de modos
    void addMode(ChannelMode mode);       // Ativa um modo (ex: +i)
    void removeMode(ChannelMode mode);    // Desativa um modo (ex: -i)
    bool hasMode(ChannelMode mode) const; // Verifica se um modo está ativo

    // Gerenciamento de membros
    void                 addMember(Client* client);      // Adiciona membro ao canal
    void                 removeMember(Client* client);   // Remove membro do canal
    bool                 isMember(Client* client) const; // Verifica se é membro
    std::vector<Client*> getMembers() const;             // Retorna todos os membros

    // Gerenciamento de operadores
    void                 addOperator(Client* client);      // Promove membro a operador
    void                 removeOperator(Client* client);   // Remove status de operador
    bool                 isOperator(Client* client) const; // Verifica se é operador
    std::vector<Client*> getOperators() const;

    // Gerenciamento de convites
    void invite(Client* client);          // Convida um cliente para o canal
    bool isInvited(Client* client) const; // Verifica se o cliente foi convidado
    void removeInvite(Client* client);    // Remove cliente da lista de convidados

    // Envio de mensagens para todos os membros do canal
    void broadcast(const std::string& message, Client* sender = NULL);
};

#endif
