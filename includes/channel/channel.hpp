/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgonzaga <mgonzaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 16:19:07 by mgonzaga          #+#    #+#             */
/*   Updated: 2025/07/13 16:24:38 by mgonzaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP     // Evita múltiplas inclusões do mesmo cabeçalho
#define CHANNEL_HPP

#include <string>       // Para uso de std::string
#include <set>          // Para uso de std::set (conjunto de ponteiros para Client)
#include "../client/Client.hpp"

// Classe que representa um canal de comunicação (como #geral no IRC)
class Channel {
public:
    // Enumeração de modos do canal usando bits para representar flags
    enum ChannelMode {
        INVITE_ONLY       = 1 << 0, // +i → Apenas usuários convidados podem entrar
        TOPIC_RESTRICTED  = 1 << 1, // +t → Apenas operadores podem mudar o tópico
        KEY_REQUIRED      = 1 << 2, // +k → Entrada exige senha
        LIMIT_SET         = 1 << 3  // +l → Há limite de usuários no canal
    };

private:
    std::string name;                 // Nome do canal (ex: #canal)
    std::string key;                  // Senha do canal (se +k estiver ativo)
    std::string topic;                // Tópico atual do canal
    int limit;                        // Número máximo de usuários permitidos (se +l)
    int modes;                        // Variável de controle com os modos ativados (flags bitwise)

    std::set<Client*> members;        // Conjunto de todos os membros do canal
    std::set<Client*> ops;            // Operadores do canal (modo +o)
    std::set<Client*> invited;        // Usuários convidados (usado se canal for +i)

public:
    // Construtor que inicializa o canal com um nome
    Channel(const std::string& name);

    // Métodos "getters" — acessam dados do canal
    std::string getName() const;         // Retorna o nome do canal
    std::string getTopic() const;        // Retorna o tópico atual
    std::string getKey() const;          // Retorna a chave do canal
    int getLimit() const;                // Retorna o limite de usuários
    int getModes() const;                // Retorna os modos em formato de bitmask
    std::string getModesString() const;  // Retorna os modos em string, ex: "+itkl"

    // Métodos "setters" — modificam dados do canal
    void setTopic(const std::string& newTopic);  // Define novo tópico
    void setKey(const std::string& newKey);      // Define nova senha (key)
    void removeKey();                            // Remove a senha
    void setLimit(int newLimit);                 // Define limite de membros
    void removeLimit();                          // Remove o limite

    // Gerenciamento de modos
    void addMode(ChannelMode mode);              // Ativa um modo (ex: +i)
    void removeMode(ChannelMode mode);           // Desativa um modo (ex: -i)
    bool hasMode(ChannelMode mode) const;        // Verifica se um modo está ativo

    // Gerenciamento de membros
    void addMember(Client* client);              // Adiciona membro ao canal
    void removeMember(Client* client);           // Remove membro do canal
    bool isMember(Client* client) const;         // Verifica se é membro
    std::set<Client*> getMembers() const;        // Retorna todos os membros

    // Gerenciamento de operadores
    void addOperator(Client* client);            // Promove membro a operador
    void removeOperator(Client* client);         // Remove status de operador
    bool isOperator(Client* client) const;       // Verifica se é operador

    // Gerenciamento de convites
    void invite(Client* client);                 // Convida um cliente para o canal
    bool isInvited(Client* client) const;        // Verifica se o cliente foi convidado
    void removeInvite(Client* client);           // Remove cliente da lista de convidados

    // Envio de mensagens
    //void broadcast(const std::string& message, Client* sender = nullptr); // Envia mensagem para todos
};

#endif // CHANNEL_HPP  // Fim da proteção contra múltiplas inclusões