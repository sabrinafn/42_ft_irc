# ft\_irc

Um servidor IRC em C++98, construído do zero sem bibliotecas externas, usando `poll()` para gerenciar I/O de forma não bloqueante.

---

## 📋 Sumário

1. [Visão Geral](#visão-geral)
2. [Pré-requisitos](#pré-requisitos)
3. [Compilação e Build](#compilação-e-build)
4. [Estrutura do Projeto](#estrutura-do-projeto)
5. [Como Executar](#como-executar)
6. [Fluxo de Desenvolvimento](#fluxo-de-desenvolvimento)
7. [Roadmap de Implementação](#roadmap-de-implementação)
8. [Testes e Robustez](#testes-e-robustez)
9. [Passos Lógicos para Conclusão](#passos-lógicos-para-conclusão)

---

## Visão Geral

O **ft\_irc** é um servidor de Internet Relay Chat (IRC) compatível com o protocolo RFC 1459, escrito em **C++98**. O objetivo é:

* Gerenciar múltiplos clientes em uma única thread, usando `poll()`.
* Implementar comandos básicos e de operador (NICK, USER, PASS, JOIN, PRIVMSG, KICK, INVITE, TOPIC, MODE).
* Seguir o padrão e normas da 42 School (C++98, sem libs externas, flags `-Wall -Wextra -Werror -std=c++98`).

---

## Pré-requisitos

* GNU/Linux ou macOS
* g++ que suporte C++98
* Make
* (Opcional) `cppcheck` e `clang-tidy` para análise estática

---

## Compilação e Build

1. Clone o repositório:

   ```bash
   git clone https://github.com/seu-usuario/ft_irc-42sp.git
   cd ft_irc-42sp
   ```
2. Faça build com Make:

   ```bash
   make
   ```
3. Targets disponíveis:

   * `make all` (default): compila tudo
   * `make clean`: remove objetos
   * `make fclean`: remove objetos e binário
   * `make re`: `fclean` + `all`
   * `make style`: roda `cppcheck` e `clang-tidy`

---

## Estrutura do Projeto

```
ft_irc-42sp/
├── includes/               # Cabeçalhos (.hpp)
│   ├── server/
│   ├── parser/
│   ├── command/
│   └── utils/
├── sources/                # Implementações (.cpp)
│   ├── server/
│   ├── parser/
│   ├── command/
│   └── utils/
├── config/                 # Arquivos de configuração (ex.: motd)
├── tests/                  # Testes manuais e scripts
├── Makefile
├── README.md
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

### 👥 Trabalho em Equipe

# 1. Convenções de Branch

main: branch estável, usada para releases.

dev: branch de integração contínua.

feature/<nome>: branches para cada nova feature ou módulo.

# 2. Papéis e Responsabilidades

Part A: Infraestrutura e Loop de Eventos (Makefile, Server, poll()).

Part B: Parser e CommandHandler (Parser.hpp/cpp, CommandHandler).

Part C: Canal, Módulos de Canal, Utilitários e Testes.


## Part A: Infraestrutura e Loop de Eventos

- [ ] **Makefile padrão**  
  - [ ] Definir variáveis `NAME`, `SRCDIR`, `INCDIR`, `OBJDIR`, `CXXFLAGS` (`-Wall -Wextra -Werror -std=c++98 -pedantic`)  
  - [ ] Criar target `all` (compile e link)  
  - [ ] Criar target `clean` (remove objetos)  
  - [ ] Criar target `fclean` (remove objetos e binário)  
  - [ ] Criar target `re` (fclean + all)  
  - [ ] Criar target `style` que roda `cppcheck`/`clang-tidy`  

- [ ] **Parse dos argumentos `<port> <password>`**  
  - [ ] Verificar número de argumentos (`argc == 3`)  
  - [ ] Converter e validar porta (inteiro entre 1024–65535)  
  - [ ] Validar tamanho e caracteres da senha (mínimo 1, máximo 32)  
  - [ ] Exibir mensagem de uso em caso de erro  

- [ ] **Classe `Server` com socket não‐bloqueante**  
  - [ ] Criar socket TCP (`socket(AF_INET, SOCK_STREAM, 0)`)  
  - [ ] Setar `O_NONBLOCK` com `fcntl()`  
  - [ ] `bind()` na porta passada  
  - [ ] `listen()` com backlog definido (e.g. 10)  
  - [ ] Tratar erros em cada chamada com `perror()` + exit status  

- [ ] **Configuração do `poll()`**  
  - [ ] Encapsular `std::vector<pollfd>` em classe `PollSet`  
  - [ ] Método `add(fd)`, `remove(fd)`, `poll(timeout)`  
  - [ ] Tratar retorno de `poll()` — distinguir `POLLIN`, `POLLHUP`, `POLLERR`  
  - [ ] Implementar timeout global (PING/PONG, autenticação)  

- [ ] **Gerenciamento de Conexões**  
  - [ ] Ao detectar `POLLIN` no socket de escuta → `accept()`  
  - [ ] Criar objeto `Client` e adicionar ao `PollSet`  
  - [ ] Ao `POLLIN` em cliente → ler bytes para buffer interno  
  - [ ] Ao `POLLHUP`/`POLLERR` → remover cliente, fechar fd, liberar recursos  

---

## Part B: Parser e CommandHandler

- [ ] **Parser de mensagens IRC**  
  - [ ] Manter buffer por cliente 
  - [ ] Acumular dados lidos até encontrar `\r\n`  
  - [ ] Extrair linha completa e deixar resto no buffer  
  - [ ] Tokenizar: separar comando e parâmetros (até `:` final)  
  - [ ] Tratar com espaço após `:` como único parâmetro  

- [ ] **Implementar comando PASS**  
  - [ ] Validar senha recebida contra a senha do servidor  
  - [ ] Atualizar estado de autenticação do `Client` (enum)  
  - [ ] Enviar erro 464 “Password incorrect” se falhar  

- [ ] **Implementar comando NICK**  
  - [ ] Validar formato de nickname (regex `[A-Za-z][A-Za-z0-9\-\[\]\\\`^{}]*`)  
  - [ ] Rejeitar nick duplicado (broadcast 433)  
  - [ ] Atualizar atributo `nick` do `Client`  

- [ ] **Implementar comando USER**  
  - [ ] Verificar recebimento completo de 4 parâmetros (`username`, `mode`, `unused`, `realname`)  
  - [ ] Atualizar atributos `username` e `realname`  
  - [ ] Após PASS+NICK+USER válidos, marcar cliente como “registered” e enviar mensagens 001–004  

- [ ] **Dispatcher de comandos**  
  - [ ] Criar interface base `ICommand { virtual void execute(Client&, Server&) = 0; }`  
  - [ ] Mapear strings → objetos `ICommand` em `std::map<std::string, std::unique_ptr<ICommand>>`  
  - [ ] No loop principal, para cada linha parseada → buscar e chamar `execute()`  

---

## Part C: Canal, Módulos de Canal, Utilitários e Testes

- [ ] **Channel: modelos e operações básicas**  
  - [ ] Classe `Channel` com nome, senha (`key`), tópico, limite (`limit`)  
  - [ ] Containers `std::set<Client*> members`, `ops`, `invited`  
  - [ ] Métodos `addMember()`, `removeMember()`, `broadcast()`  
  - [ ] Gerenciar modos (`i, t, k, l, o`) com flags bitwise  

- [ ] **JOIN / PRIVMSG / TOPIC**  
  - [ ] **JOIN**: verificar modo `i` (invite), `k` (key), `l` (limit) antes de adicionar  
  - [ ] **PRIVMSG**: destino usuário ou canal; falhar com erro 401/404 conforme necessário  
  - [ ] **TOPIC**: se sem parâmetros → mostrar tópico; com parâmetros → somente operadores ou modo `t`  

- [ ] **Comandos de Operador: KICK / INVITE / MODE**  
  - [ ] **KICK**: remover membro, notificar canal; checar operador  
  - [ ] **INVITE**: adicionar em `invited`, notificar convidado; checar modo `i`  
  - [ ] **MODE**: alterar flags do canal ou do usuário; parâmetros variáveis  

- [ ] **Utilitários e Protocol Helpers**  
  - [ ] **Logger**: nível `DEBUG`/`INFO`/`ERROR`, console ou arquivo  
  - [ ] **Enums**: códigos de resposta (001–005, 433, 464…) em `enum IRCCode`  
  - [ ] **Formatters**: construir replies padronizadas `:<server> <code> <nick> :<message>\r\n`  

- [ ] **Testes e Robustez**  
  - [ ] Escrever scripts Bash em `tests/` para:  
    - [ ] Autenticação completa (PASS+NICK+USER)  
    - [ ] Fragmentação de linha (`echo -n "PRIV"; sleep 1; echo "MSG\r\n"`)  
    - [ ] Múltiplos clientes simultâneos (loops `nc`)  
  - [ ] Testar logs de erro e comportamentos inválidos (`NICK !nv@l!d\r\n`)  
  - [ ] Conectar clientes reais (WeeChat/HexChat) e validar interoperabilidade  

- [ ] **Extras Futuramente**  
  - [ ] Mensagem de boas‐vindas via arquivo MOTD  
  - [ ] Implementar QUIT, WHO, NAMES, LIST  

---