#!/bin/bash

echo "=== IRC Parser Test ==="

cat > test_parser.cpp << 'EOF'
#include "includes/parser/Parser.hpp"
#include <iostream>
#include <cassert>

void testExtractLines() {
    std::cout << "Testing extractLines..." << std::endl;
    
    std::string buffer = "NICK test\r\nUSER test 0 * :Test User\r\nPING :server\r\nincomplete";
    std::vector<std::string> lines = Parser::extractLines(buffer);
    
    assert(lines.size() == 3);
    assert(lines[0] == "NICK test");
    assert(lines[1] == "USER test 0 * :Test User");
    assert(lines[2] == "PING :server");
    assert(buffer == "incomplete");
    
    std::cout << "✓ extractLines test passed" << std::endl;
}

void testParseMessage() {
    std::cout << "Testing parseMessage..." << std::endl;
    
    // Test simple command
    IRCMessage msg1 = Parser::parseMessage("NICK testuser");
    assert(msg1.command == "NICK");
    assert(msg1.params.size() == 1);
    assert(msg1.params[0] == "testuser");
    assert(msg1.prefix.empty());
    assert(msg1.trailing.empty());
    
    // Test command with trailing
    IRCMessage msg2 = Parser::parseMessage("PRIVMSG #channel :Hello world!");
    assert(msg2.command == "PRIVMSG");
    assert(msg2.params.size() == 1);
    assert(msg2.params[0] == "#channel");
    assert(msg2.trailing == "Hello world!");
    
    // Test command with prefix
    IRCMessage msg3 = Parser::parseMessage(":server.com 001 nick :Welcome");
    assert(msg3.prefix == "server.com");
    assert(msg3.command == "001");
    assert(msg3.params.size() == 1);
    assert(msg3.params[0] == "nick");
    assert(msg3.trailing == "Welcome");
    
    // Test USER command
    IRCMessage msg4 = Parser::parseMessage("USER username 0 * :Real Name");
    assert(msg4.command == "USER");
    assert(msg4.params.size() == 3);
    assert(msg4.params[0] == "username");
    assert(msg4.params[1] == "0");
    assert(msg4.params[2] == "*");
    assert(msg4.trailing == "Real Name");
    
    std::cout << "✓ parseMessage test passed" << std::endl;
}

void testValidators() {
    std::cout << "Testing validators..." << std::endl;
    
    // Test nickname validation
    assert(Parser::isValidNickname("validnick"));
    assert(Parser::isValidNickname("test123"));
    assert(Parser::isValidNickname("user-name"));
    assert(!Parser::isValidNickname("123invalid"));
    assert(!Parser::isValidNickname(""));
    assert(!Parser::isValidNickname("nick with spaces"));
    
    // Test command validation
    assert(Parser::isValidCommand("NICK"));
    assert(Parser::isValidCommand("PRIVMSG"));
    assert(Parser::isValidCommand("001"));
    assert(!Parser::isValidCommand("INVALID"));
    assert(!Parser::isValidCommand(""));
    
    std::cout << "✓ validators test passed" << std::endl;
}

int main() {
    try {
        testExtractLines();
        testParseMessage();
        testValidators();
        
        // RFC 1459 length limit test: ensure lines longer than 510 chars (before CRLF)
        // are truncated by extractLines to 510.
        {
            std::string longParam(600, 'A'); // 600 A's
            std::string buffer = "PRIVMSG #c :" + longParam + "\r\n";
            std::vector<std::string> lines = Parser::extractLines(buffer);
            assert(lines.size() == 1);
            assert(lines[0].size() <= 510);
            // Ensure trailing content is present but capped
            IRCMessage m = Parser::parseMessage(lines[0]);
            assert(m.command == "PRIVMSG");
            assert(m.params.size() == 1 && m.params[0] == "#c");
            assert(m.trailing.size() > 0);
        }
        std::cout << "\nAll parser tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
EOF

echo "Compiling parser test..."
c++ -Wall -Wextra -Werror -pedantic -std=c++98 -g3 -Iincludes test_parser.cpp sources/parser/Parser.cpp -o test_parser

echo "Running parser test..."
./test_parser

rm -f test_parser.cpp test_parser

echo "=== Parser test completed ==="
