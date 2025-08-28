#!/bin/bash

echo "=== IRC Parser Test ==="

cat > test_parser.cpp << 'EOF'
#include "includes/parser/Parser.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>

static void testExtractLines() {
    std::cout << "Testing extractLines..." << std::endl;

    std::string buffer = "NICK test\r\nUSER test 0 * :Test User\r\nPING :server\r\nincomplete";
    std::vector<std::string> lines = Parser::extractLines(buffer);

    assert(lines.size() == 3);
    assert(lines[0] == "NICK test");
    assert(lines[1] == "USER test 0 * :Test User");
    assert(lines[2] == "PING :server");
    assert(buffer == "incomplete");

    std::cout << "\u2713 extractLines test passed" << std::endl;
}

static void testParseMessageBasics() {
    std::cout << "Testing parseMessage basics..." << std::endl;

    IRCMessage msg1 = Parser::parseMessage("NICK testuser");
    assert(msg1.command == "NICK");
    assert(msg1.params.size() == 1);
    assert(msg1.params[0] == "testuser");
    assert(msg1.prefix.empty());
    assert(msg1.trailing.empty());

    IRCMessage msg2 = Parser::parseMessage("PRIVMSG #channel :Hello world!");
    assert(msg2.command == "PRIVMSG");
    assert(msg2.params.size() == 1);
    assert(msg2.params[0] == "#channel");
    assert(msg2.trailing == "Hello world!");

    IRCMessage msg3 = Parser::parseMessage(":server.com 001 nick :Welcome");
    assert(msg3.prefix == "server.com");
    assert(msg3.command == "001");
    assert(msg3.params.size() == 1);
    assert(msg3.params[0] == "nick");
    assert(msg3.trailing == "Welcome");

    IRCMessage msg4 = Parser::parseMessage("USER username 0 * :Real Name");
    assert(msg4.command == "USER");
    assert(msg4.params.size() == 3);
    assert(msg4.params[0] == "username");
    assert(msg4.params[1] == "0");
    assert(msg4.params[2] == "*");
    assert(msg4.trailing == "Real Name");

    std::cout << "\u2713 parseMessage basics passed" << std::endl;
}

static void testValidators() {
    std::cout << "Testing validators..." << std::endl;

    assert(Parser::isValidNickname("validnick"));
    assert(Parser::isValidNickname("test123"));
    assert(Parser::isValidNickname("user-name"));
    assert(!Parser::isValidNickname("123invalid"));
    assert(!Parser::isValidNickname(""));
    assert(!Parser::isValidNickname("nick with spaces"));

    assert(Parser::isValidCommand("NICK"));
    assert(Parser::isValidCommand("PRIVMSG"));
    assert(Parser::isValidCommand("001"));
    assert(!Parser::isValidCommand("INVALID"));
    assert(!Parser::isValidCommand(""));

    std::cout << "\u2713 validators test passed" << std::endl;
}

static void testNormalization() {
    std::cout << "Testing normalization (commands, nicks, channels)..." << std::endl;

    // Command should uppercase; nick should lowercase
    {
        IRCMessage m = Parser::parseMessage("nick Seiji");
        assert(m.command == "NICK");
        assert(m.params.size() == 1);
        assert(m.params[0] == "seiji");
    }

    // PRIVMSG target normalization (channel)
    {
        IRCMessage m = Parser::parseMessage("PrivMsg #Room :Hello");
        assert(m.command == "PRIVMSG");
        assert(m.params.size() == 1);
        assert(m.params[0] == "#room");
        assert(m.trailing == "Hello");
    }

    // JOIN list normalization
    {
        IRCMessage m = Parser::parseMessage("join #A,#B");
        assert(m.command == "JOIN");
        assert(m.params.size() >= 1);
        assert(m.params[0] == "#a,#b");
    }

    // KICK channel and targets normalization
    {
        IRCMessage m = Parser::parseMessage("KiCk #Chan Seiji,Other :r");
        assert(m.command == "KICK");
        assert(m.params.size() >= 2);
        assert(m.params[0] == "#chan");
        assert(m.params[1] == "seiji,other");
        assert(m.trailing == "r");
    }

    // INVITE nick and channel
    {
        IRCMessage m = Parser::parseMessage("Invite Seiji #Chan");
        assert(m.command == "INVITE");
        assert(m.params.size() == 2);
        assert(m.params[0] == "seiji");
        assert(m.params[1] == "#chan");
    }

    // TOPIC channel normalization
    {
        IRCMessage m = Parser::parseMessage("ToPiC #Chan :New Topic");
        assert(m.command == "TOPIC");
        assert(m.params.size() >= 1);
        assert(m.params[0] == "#chan");
        assert(m.trailing == "New Topic");
    }

    // Extra spaces should be tolerated and still normalize
    {
        IRCMessage m = Parser::parseMessage("   NiCk    TestUser   ");
        assert(m.command == "NICK");
        assert(m.params.size() == 1);
        assert(m.params[0] == "testuser");
    }

    std::cout << "\u2713 normalization test passed" << std::endl;
}

static void testRFC1459CaseMap() {
    std::cout << "Testing RFC1459 casemap..." << std::endl;
    // With RFC1459 enabled by default, channel and nick folding maps []\ to {}|
    {
        IRCMessage m = Parser::parseMessage("JOIN #[Room]");
        assert(m.command == "JOIN");
        // '[' -> '{', ']' -> '}' then lowercased
        assert(m.params.size() >= 1);
        assert(m.params[0] == "#{room}");
    }
    {
        IRCMessage m = Parser::parseMessage("NICK User|Name");
        assert(m.command == "NICK");
        // '|' -> '\\' casemap equivalent when folding lower
        assert(m.params[0] == "user|name" || m.params[0] == "user|name");
    }
    std::cout << "\u2713 RFC1459 casemap test passed" << std::endl;
}

static void testLengthCap() {
    std::cout << "Testing length cap..." << std::endl;
    std::string longParam(600, 'A');
    std::string buffer = "PRIVMSG #c :" + longParam + "\r\n";
    std::vector<std::string> lines = Parser::extractLines(buffer);
    assert(lines.size() == 1);
    assert(lines[0].size() <= 510);
    IRCMessage m = Parser::parseMessage(lines[0]);
    assert(m.command == "PRIVMSG");
    assert(m.params.size() == 1 && m.params[0] == "#c");
    assert(m.trailing.size() > 0);
    std::cout << "\u2713 length cap test passed" << std::endl;
}

int main() {
    testExtractLines();
    testParseMessageBasics();
    testValidators();
    testNormalization();
    testRFC1459CaseMap();
    testLengthCap();
    std::cout << "\nAll parser tests passed!" << std::endl;
    return 0;
}
EOF

echo "Compiling parser test..."
c++ -Wall -Wextra -Werror -pedantic -std=c++98 -g3 -Iincludes test_parser.cpp sources/parser/Parser.cpp -o test_parser

echo "Running parser test..."
./test_parser

rm -f test_parser.cpp test_parser

echo "=== Parser test completed ==="
