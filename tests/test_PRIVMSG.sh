#!/bin/bash

echo "=== IRC Server PRIVMSG Command Testing ==="

PORT=6667
PASSWORD="testpass123"
SERVER_PID=""

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_test() { echo -e "${BLUE}[TEST]${NC} $1"; }
print_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
print_error() { echo -e "${RED}[ERROR]${NC} $1"; }
print_info() { echo -e "${YELLOW}[INFO]${NC} $1"; }

cleanup() {
    print_info "Cleaning up..."
    if [ ! -z "$SERVER_PID" ]; then
        kill $SERVER_PID 2>/dev/null
        wait $SERVER_PID 2>/dev/null
    fi
    pkill -f "ircserv" 2>/dev/null

    # Fecha qualquer processo ainda rodando na porta
    if command -v lsof >/dev/null 2>&1; then
        PORT_PID=$(lsof -ti tcp:$PORT)
        if [ ! -z "$PORT_PID" ]; then
            kill -9 $PORT_PID 2>/dev/null
            print_info "Porta $PORT liberada"
        fi
    fi
    kill -9 $PORT_PID 2>/dev/null
    print_info "Porta $PORT liberada"
    rm -f test_output.tmp
    exit 0
}

trap cleanup SIGINT SIGTERM

print_info "Building IRC server..."
make fclean && make
[ $? -ne 0 ] && { print_error "Build failed!"; exit 1; }
print_success "Build completed"

print_info "Starting IRC server on port $PORT..."
./ircserv $PORT $PASSWORD > server_output.log 2>&1 &
SERVER_PID=$!
sleep 2

if ! kill -0 $SERVER_PID 2>/dev/null; then
    print_error "Server failed to start"
    cat server_output.log
    exit 1
fi
print_success "Server started with PID $SERVER_PID"
echo ""

# Função auxiliar para rodar cada teste e esperar terminar
run_test() {
    CMD="$1"
    echo -e "$CMD" | nc -C 0.0.0.0 $PORT > test_output.tmp 2>&1 &
    NC_PID=$!
    sleep 2
    wait $NC_PID
}

# Test 1: JOIN válido
print_test "Test 1: JOIN válido em #canal1 com '#'"
run_test "PASS $PASSWORD\r\nNICK user1\r\nUSER user1 0 * :User One\r\nJOIN #canal1\r\nQUIT\r\n"
if grep -q ":user1!user1@.* JOIN :#canal1" test_output.tmp && \
   grep -q "331 user1 #canal1" test_output.tmp && \
   grep -q "353 user1 = #canal1" test_output.tmp && \
   grep -q "366 user1 #canal1" test_output.tmp; then
    print_success "JOIN em #canal1 aceito corretamente"
else
    print_error "JOIN em #canal1 falhou"
    cat test_output.tmp
fi
echo ""


print_info "=== Test Summary ==="
print_info "Todos os testes de PRIVMSG completados."
print_info "Verifique os resultados acima."

# Final cleanup
cleanup

