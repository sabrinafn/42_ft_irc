# ft\_irc

Um servidor IRC em C++98, construído do zero sem bibliotecas externas, usando `poll()` para gerenciar I/O de forma não bloqueante.

---

## 📋 Sumário

1. [Visão Geral](#visão-geral)
2. [Estrutura do Projeto](#estrutura-do-projeto)
3. [Como Executar](#como-executar)
4. [Fluxo de Desenvolvimento](#fluxo-de-desenvolvimento)
5. [Roadmap de Implementação](#roadmap-de-implementação)
6. [Testes e Robustez](#testes-e-robustez)
7. [Passos Lógicos para Conclusão](#passos-lógicos-para-conclusão)

---

## Visão Geral

O **ft\_irc** é um servidor de Internet Relay Chat (IRC) compatível com o protocolo RFC 1459, escrito em **C++98**. O objetivo é:

* Gerenciar múltiplos clientes em uma única thread, usando `poll()`.
* Implementar comandos básicos e de operador (NICK, USER, PASS, JOIN, PRIVMSG, KICK, INVITE, TOPIC, MODE).
* Seguir o padrão e normas da 42 School (C++98, sem libs externas, flags `-Wall -Wextra -Werror -std=c++98`).

---

## Estrutura do Projeto

```
ft_irc/                             # Project root
├── include/                        # Public headers
│   ├── ft_irc/                     # “Namespaced” headers
│   │   ├── server/                 # server-side types & APIs
│   │   ├── parser/                 # parsing interfaces
│   │   ├── command/                # command handler APIs
│   │   └── utils/                  # general-purpose utilities
│   └── ft_irc.hpp                  # umbrella include for consumers
├── src/                            # Implementation files
│   ├── server/
│   ├── parser/
│   ├── command/
│   └── utils/
├── config/                         # Runtime configuration (e.g. motd.conf)
├── tests/                          # Automated tests & helpers
│   ├── unit/                       # small, module-level tests
│   └── integration/                # end-to-end scripts
├── docs/                           # Design docs, protocol notes, RFC links
├── scripts/                        # Helper scripts (build, test runner, lint)
├── build/                          # Out-of-source build directory
│   ├── bin/                        # compiled executables
│   └── obj/                        # object files
├── Makefile                        # Orchestrates everything under build/
├── README.md                       # Project overview, build & usage
└── .gitignore

```

---

## Como Executar

```bash
./ircserv <port> <password>
```

Exemplo:

```bash
./ircserv 6667 senha123
```

Em outro terminal, conecte-se via nc:

```bash
nc -C 127.0.0.1 6667
PASS senha123
NICK user1
USER user1 0 * :User One
JOIN #canal
PRIVMSG #canal :Olá!
```

---

## Fluxo de Desenvolvimento

1. **Infraestrutura**: Makefile e classe `Server` com socket não-bloqueante.
2. **Loop de Eventos**: `poll()` e gerenciamento de múltiplos `Client`.
3. **Autenticação**: comandos PASS, NICK, USER e estados de autenticação.
4. **Parser**: buffer incremental e divisão de mensagens por `\r\n`.
5. **Command Handler**: padrão *dispatcher* para cada comando.
6. **Canal**: classe `Channel` gerenciando membros, modos e broadcast.
7. **Testes**: cenários de fragmentação, múltiplos clientes e clientes reais.
8. **CI e Linter**: integrar GitHub Actions para build e análise estática.

---

## Roadmap de Implementação

| Etapa | Objetivo                                                        |
| ----- | --------------------------------------------------------------- |
| 1     | Setup básico: Makefile, main.cpp, Server (socket, bind, listen) |
| 2     | Loop de eventos: vector<pollfd>, accept, gerenciamento de fds   |
| 3     | Autenticação: PASS → NICK → USER, controle de estados           |
| 4     | Parser: buffer e split por CRLF, tokens e parâmetros            |
| 5     | CommandHandler: map\<string, Func> → execução lógica            |
| 6     | Channels: criação, modos, join, part, broadcast                 |
| 7     | Comandos de operador (KICK, MODE, INVITE, TOPIC)                |
| 8     | Robustez: tratamento de erros, desconexões, timeouts            |
| 9     | Testes automatizados e scripts de integração contínua           |

---

## Testes e Robustez

* **Fragmentação de Mensagens**: simular envio de comandos em pedaços.
* **Multiplicidade**: abrir N clientes simultâneos.
* **Clientes Reais**: testar com HexChat, WeeChat ou irssi.
* **Erros de Protocolo**: inputs malformados, timeouts de autenticação.

---

## Passos Lógicos para Conclusão

1. **Verificar**: Makefile e flags de compilação.
2. **Inicializar**: socket não bloqueante e loop de `poll()` funcionando.
3. **Gerenciar**: aceitar clientes e armazená-los em `std::map<int, Client>`.
4. **Autenticar**: implementar e validar PASS, NICK, USER.
5. **Parsear**: criar buffer incremental e extrair linhas completas.
6. **Dispatcher**: ligar comando parseado à função handler.
7. **Channels**: instanciar `Channel`, gerenciar membros e modos.
8. **Implementar**: comandos de operador e regras de modo.
9. **Testar**: todos cenários de uso e casos de borda.
10. **Refinar**: adicionar CI, linter e documentação final.

---

# Convenções de Branch

main: branch estável, usada para releases.

dev: branch de integração contínua.

feature/<nome>: branches para cada nova feature ou módulo.


## Infraestrutura e Loop de Eventos

- [x] **Makefile padrão**  
  - [x] Definir variáveis `NAME`, `SRCDIR`, `INCDIR`, `OBJDIR`, `CXXFLAGS` (`-Wall -Wextra -Werror -std=c++98 -pedantic`)  
  - [x] Criar target `all` (compile e link)  
  - [x] Criar target `clean` (remove objetos)  
  - [x] Criar target `fclean` (remove objetos e binário)  
  - [x] Criar target `re` (fclean + all)  
  - [x] Criar regra `make format` (roda arquivo de formatacao clang-format)
  - [x] Criar regra `make lint` (roda arquivo de formatacao clang-tidy)

- [x] **Parse dos argumentos `<port> <password>`**  
  - [x] Verificar número de argumentos (`argc == 3`)  
  - [x] Converter e validar porta (inteiro entre 1024–65535)  
  - [x] Validar tamanho e caracteres da senha (mínimo 1, máximo 32)  
  - [x] Exibir mensagem de uso em caso de erro  

- [x] **Classe `Server` com socket não‐bloqueante**  
  - [x] Criar socket TCP (`socket(AF_INET, SOCK_STREAM, 0)`)  
  - [x] Setar `O_NONBLOCK` com `fcntl()`  
  - [x] `bind()` na porta passada  
  - [x] `listen()` com backlog definido (e.g. 10)  
  - [x] Tratar erros em cada chamada com `perror()` + exit status  

- [x] **Configuração do `poll()`**  
  - [x] Encapsular `std::vector<pollfd>` em classe `PollSet`  
  - [x] Método `add(fd)`, `remove(fd)`, `poll(timeout)`  
  - [x] Tratar retorno de `poll()` — distinguir `POLLIN`, `POLLHUP`, `POLLERR`  
  - [x] Implementar timeout global (PING/PONG, autenticação)  

- [x] **Gerenciamento de Conexões**  
  - [x] Ao detectar `POLLIN` no socket de escuta → `accept()`  
  - [x] Criar objeto `Client` e adicionar ao `PollSet`  
  - [x] Ao `POLLIN` em cliente → ler bytes para buffer interno  
  - [x] Ao `POLLHUP`/`POLLERR` → remover cliente, fechar fd, liberar recursos  

---

## Parser e CommandHandler

- [x] **Parser de mensagens IRC**  
  - [x] Manter buffer por cliente
  - [x] Acumular dados lidos até encontrar `\r\n`
  - [x] Extrair linha completa e deixar resto no buffer
  - [x] Tokenizar: separar comando e parâmetros (até `:` final)  
  - [x] Tratar com espaço após `:` como único parâmetro

- [x] **Implementar comando PASS**
  - [x] Validar senha recebida contra a senha do servidor
  - [x] Atualizar estado de autenticação do `Client` (enum)
  - [x] Enviar erro 464 “Password incorrect” se falhar

- [x] **Implementar comando NICK**
  - [x] Validar formato de nickname (regex `[A-Za-z][A-Za-z0-9\-\[\]\\\`^{}]*`)
  - [x] Rejeitar nick duplicado (broadcast 433)
  - [x] Atualizar atributo `nick` do `Client`

- [x] **Implementar comando USER**  
  - [x] Verificar recebimento completo de 4 parâmetros (`username`, `mode`, `unused`, `realname`)  
  - [x] Atualizar atributos `username` e `realname`  
  - [x] Após PASS+NICK+USER válidos, marcar cliente como “registered” e enviar mensagens 001–004

- [x] **Dispatcher de comandos**
  - [x] Criar classe `Commands`
  - [x] Armazenar em `std::map<std::string, CommandFunc>` (ex: `"NICK" → &Commands::handleNick`)
  - [x] No método `handler(Client&, Server&, IRCMessage&)`, buscar no map e chamar `(this->*func)(...)`
  - [x] Enviar `ERR_UNKNOWNCOMMAND` se o comando não existir

---

## Canal, Módulos de Canal, Utilitários e Testes

- [x] **Channel: modelos e operações básicas**  
  - [x] Classe `Channel` com nome, senha (`key`), tópico, limite (`limit`)  
  - [x] Containers `std::set<Client*> members`, `ops`, `invited`  
  - [x] Métodos `addMember()`, `removeMember()`, `broadcast()`  
  - [x] Gerenciar modos (`i, t, k, l, o`) com flags bitwise  

- [x] **JOIN / PRIVMSG / TOPIC**  
  - [x] **JOIN**: verificar modo `i` (invite), `k` (key), `l` (limit) antes de adicionar  
  - [x] **PRIVMSG**: destino usuário ou canal; falhar com erro 401/404 conforme necessário  
  - [x] **TOPIC**: se sem parâmetros → mostrar tópico; com parâmetros → somente operadores ou modo `t`  

- [x] **Comandos de Operador: KICK / INVITE / MODE**  
  - [x] **KICK**: remover membro, notificar canal; checar operador  
  - [x] **INVITE**: adicionar em `invited`, notificar convidado; checar modo `i`  
  - [x] **MODE**: alterar flags do canal ou do usuário; parâmetros variáveis  

- [x] **Utilitários e Protocol Helpers**  
  - [x] **Logger**: nível `DEBUG`/`INFO`/`ERROR`, console ou arquivo  
  - [x] **Enums**: códigos de resposta (001–005, 433, 464…) em `enum IRCCode`  
  - [x] **Formatters**: construir replies padronizadas `:<server> <code> <nick> :<message>\r\n`  

- [ ] **Testes e Robustez**  
  - [ ] Escrever scripts Bash em `tests/` para:  
    - [x] Autenticação completa (PASS+NICK+USER)  
    - [x] Fragmentação de linha (`echo -n "PRIV"; sleep 1; echo "MSG\r\n"`)  
    - [x] Múltiplos clientes simultâneos (loops `nc`)  
  - [ ] Testar logs de erro e comportamentos inválidos (`NICK !nv@l!d\r\n`)  
  - [ ] Conectar clientes reais (WeeChat/HexChat) e validar interoperabilidade  

- [ ] **Extras Futuramente**  
  - [ ] Mensagem de boas‐vindas via arquivo MOTD  
  - [ ] Implementar QUIT, WHO, NAMES, LIST
  - [ ] Implementar arquivo de configuracao irc

---
