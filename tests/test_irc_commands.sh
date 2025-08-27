#!/bin/bash

echo "=== IRC Server Command Testing ==="

PORT=6667
PASSWORD="testpass123"
SERVER_PID=""

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_test() {
    echo -e "${BLUE}[TEST]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_info() {
    echo -e "${YELLOW}[INFO]${NC} $1"
}

cleanup() {
    print_info "Cleaning up..."
    if [ ! -z "$SERVER_PID" ]; then
        kill $SERVER_PID 2>/dev/null
        wait $SERVER_PID 2>/dev/null
    fi
    pkill -f "ircserv" 2>/dev/null
    rm -f test_output.tmp
    exit 0
}

trap cleanup SIGINT SIGTERM

print_info "Building IRC server..."
make clean && make
if [ $? -ne 0 ]; then
    print_error "Build failed!"
    exit 1
fi
print_success "Build completed"

print_info "Starting IRC server on port $PORT..."
./ircserv $PORT $PASSWORD > server_output.log 2>&1 &
SERVER_PID=$!

sleep 2

# is server is running ? 
if ! kill -0 $SERVER_PID 2>/dev/null; then
    print_error "Server failed to start"
    cat server_output.log
    exit 1
fi

print_success "Server started with PID $SERVER_PID"
echo ""

# Test 1!: Authentication Flow
print_test "Test 1: Valid Authentication Flow"
echo -e "PASS $PASSWORD\r\nNICK testuser1\r\nUSER testuser1 0 * :Test User One\r\nQUIT :Goodbye\r\n" | nc -C 0.0.0.0 $PORT > test_output.tmp 2>&1 &
sleep 2

if grep -qE "Welcome|Host|created|1.0" test_output.tmp; then
    print_success "Valid authentication successful"
else
    print_error "Valid authentication failed"
    cat test_output.tmp
fi
echo ""

# Test 2!: Wron password
print_test "Test 2: Wrong Password"
echo -e "PASS wrongpassword\r\nNICK testuser2\r\nUSER testuser2 0 * :Test User Two\r\nQUIT\r\n" | nc -C 0.0.0.0 $PORT > test_output.tmp 2>&1 &
sleep 2

if grep -q "464.*Password incorrect" test_output.tmp; then
    print_success "Wrong password correctly rejected"
else
    print_error "Wrong password not properly handled"
    cat test_output.tmp
fi
echo ""

# Test 3:! invalid Nickname
print_test "Test 3: Invalid Nickname"
echo -e "PASS $PASSWORD\r\nNICK 123invalid\r\nNICK validuser\r\nUSER validuser 0 * :Valid User\r\nQUIT\r\n" | nc -C 0.0.0.0 $PORT > test_output.tmp 2>&1 &
sleep 2

if grep -q "432" test_output.tmp; then
    print_success "Invalid nickname correctly rejected"
else
    print_error "Invalid nickname not properly handled"
    cat test_output.tmp
fi
echo ""

# Test 4: missing parameters
print_test "Test 4: Missing Parameters"
echo -e "PASS $PASSWORD\r\nNICK\r\nUSER\r\nQUIT\r\n" | nc -C 0.0.0.0 $PORT > test_output.tmp 2>&1 &
sleep 2

if grep -q "431.*No nickname given\|461.*Not enough parameters" test_output.tmp; then
    print_success "Missing parameters correctly handled"
else
    print_error "Missing parameters not properly handled"
    cat test_output.tmp
fi
echo ""

# Test 5: PING/PONG ... 
print_test "Test 5: PING/PONG Test"
echo -e "PASS $PASSWORD\r\nNICK pinguser\r\nUSER pinguser 0 * :Ping User\r\nPING :testserver\r\nQUIT\r\n" | nc -C 0.0.0.0 $PORT > test_output.tmp 2>&1 &
sleep 2

if grep -q "PONG.*testserver" test_output.tmp; then
    print_success "PING/PONG working correctly"
else
    print_error "PING/PONG not working"
    cat test_output.tmp
fi
echo ""

# Test 6: Duplicate Nickname
print_test "Test 6: Duplicate Nickname Test"
# first client
echo -e "PASS $PASSWORD\r\nNICK sameuser\r\nUSER sameuser1 0 * :User One\r\n" | nc -C 0.0.0.0 $PORT &
sleep 1
# Try second client with same nickname
echo -e "PASS $PASSWORD\r\nNICK sameuser\r\nUSER sameuser2 0 * :User Two\r\nQUIT\r\n" | nc -C 0.0.0.0 $PORT > test_output.tmp 2>&1 &
sleep 2

if grep -q "433.*Nickname is already in use" test_output.tmp; then
    print_success "Duplicate nickname correctly rejected"
else
    print_error "Duplicate nickname not properly handled"
    cat test_output.tmp
fi
echo ""

# Test 7: Out of Order commands
print_test "Test 7: Out-of-Order Commands (USER before PASS)"
echo -e "USER outoforder 0 * :Out of Order\r\nPASS $PASSWORD\r\nNICK outoforder\r\nQUIT\r\n" | nc -C 0.0.0.0 $PORT > test_output.tmp 2>&1 &
sleep 2

if grep -q "464.*Password required" test_output.tmp; then
    print_success "Out-of-order commands correctly handled"
else
    print_error "Out-of-order commands not properly handled"
    cat test_output.tmp
fi
echo ""

# Test 8: fragmented nessage test! (error last time)
print_test "Test 8: Fragmented Message Test"
(echo -n "PASS $PASSWORD"; sleep 0.5; echo -e "\r\n"; sleep 0.5; echo -e "NICK fraguser\r\nUSER fraguser 0 * :Fragmented User\r\nQUIT\r\n") | nc -C 0.0.0.0 $PORT > test_output.tmp 2>&1 &
sleep 3

if grep -q ":<3_irc_server_<3 1.*Welcome" test_output.tmp; then
    print_success "Fragmented messages handled correctly"
else
    print_error "Fragmented messages not properly handled"
    cat test_output.tmp
fi
echo ""

# Test 9: big message test
print_test "Test 9: Long Message Test"
LONG_REALNAME="This is a very long real name that should still be handled correctly by the IRC server parser implementation"
echo -e "PASS $PASSWORD\r\nNICK longuser\r\nUSER longuser 0 * :$LONG_REALNAME\r\nQUIT\r\n" | nc -C 0.0.0.0 $PORT > test_output.tmp 2>&1 &
sleep 2

if grep -q ":<3_irc_server_<3 1.*Welcome" test_output.tmp; then
    print_success "Long messages handled correctly"
else
    print_error "Long messages not properly handled"
    cat test_output.tmp
fi
echo ""

# Test 10: Multiple Commands in Single Message
print_test "Test 10: Multiple Commands Test"
echo -e "PASS $PASSWORD\r\nNICK multiuser\r\nUSER multiuser 0 * :Multi User\r\nPING :test1\r\nPING :test2\r\nQUIT :Multiple commands\r\n" | nc -C 0.0.0.0 $PORT > test_output.tmp 2>&1 &
sleep 2

if grep -q "PONG.*test1" test_output.tmp && grep -q "PONG.*test2" test_output.tmp; then
    print_success "Multiple commands handled correctly"
else
    print_error "Multiple commands not properly handled"
    cat test_output.tmp
fi
echo ""

# result
echo ""
print_info "=== Test Summary ==="
print_info "All tests completed. Check the results above."
print_info "Server output log: server_output.log"
print_info "Press Ctrl+C to stop the server and exit."

# Keep running to show server logs
print_info "Showing live server logs (Ctrl+C to exit):"
tail -f server_output.log &
TAIL_PID=$!

# Wait for kill (cntrl c)
while kill -0 $SERVER_PID 2>/dev/null; do
    sleep 1
done

# Clean up
kill $TAIL_PID 2>/dev/null

cleanup
