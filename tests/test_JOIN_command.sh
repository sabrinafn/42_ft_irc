#!/bin/bash

echo "=== IRC Server JOIN Command Testing ==="

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

print_test "Test 2: JOIN válido em &canal1 com '&'"
run_test "PASS $PASSWORD\r\nNICK user1\r\nUSER user1 0 * :User One\r\nJOIN &canal1\r\nQUIT\r\n"
if grep -q ":user1!user1@.* JOIN :&canal1" test_output.tmp && \
   grep -q "331 user1 &canal1" test_output.tmp && \
   grep -q "353 user1 = &canal1" test_output.tmp && \
   grep -q "366 user1 &canal1" test_output.tmp; then
    print_success "JOIN em &canal1 aceito corretamente"
else
    print_error "JOIN em &canal1 falhou"
    cat test_output.tmp
fi
echo ""

# Test 3: JOIN de novo no mesmo canal
print_test "Test 3: JOIN no mesmo canal novamente"
run_test "PASS $PASSWORD\r\nNICK user2\r\nUSER user2 0 * :User Two\r\nJOIN #canal1\r\nJOIN #canal1\r\nQUIT\r\n"
if grep -q "443.*is already on channel" test_output.tmp; then
    print_success "JOIN duplicado corretamente rejeitado"
else
    print_error "JOIN duplicado não foi tratado corretamente"
    cat test_output.tmp
fi
echo ""

# Test 4: JOIN em outro canal estando já em um
print_test "Test 4: JOIN em #canal2 mesmo já estando em #canal1"
run_test "PASS $PASSWORD\r\nNICK user3\r\nUSER user3 0 * :User Three\r\nJOIN #canal1\r\nJOIN #canal2\r\nQUIT\r\n"
if grep -q ":user3!user3@.* JOIN :#canal2" test_output.tmp; then
    print_success "JOIN em múltiplos canais aceito corretamente"
else
    print_error "JOIN em múltiplos canais falhou"
    cat test_output.tmp
fi
echo ""

# Test 5: JOIN sem parametros
print_test "Test 5: JOIN sem parametros"
run_test "PASS $PASSWORD\r\nNICK user3\r\nUSER user3 0 * :User Three\r\nJOIN\r\nQUIT\r\n"
if grep -q "461.*JOIN" test_output.tmp; then
    print_success "JOIN sem parametros tratado corretamente"
else
    print_error "JOIN sem parametros tratado incorretamente"
    cat test_output.tmp
fi
echo ""

# Test 6: JOIN com canal invalido
print_test "Test 6: JOIN canalinvalido"
run_test "PASS $PASSWORD\r\nNICK user3\r\nUSER user3 0 * :User Three\r\nJOIN canalinvalido\r\nQUIT\r\n"
if grep -q "403.*No such channel" test_output.tmp; then
    print_success "JOIN canal sem '#' ou '&' recusado corretamente"
else
    print_error "JOIN canal incorreto aceito ou retorno incorreto de irc server"
    cat test_output.tmp
fi
echo ""

# Test 7: JOIN com senha
print_test "Test 7: JOIN #canal2 senhacorreta"
run_test "PASS $PASSWORD\r\nNICK user3\r\nUSER user3 0 * :User Three\r\nJOIN #chan1,#chan2,&chan3,&chan4\r\nQUIT\r\n"
for chan in "#chan1" "#chan2" "&chan3" "&chan4"; do
    if grep -q "353 user3 = $chan" test_output.tmp; then
        print_success "Entrou corretamente no canal $chan"
    else
        print_error "Falha ao entrar no canal $chan"
        cat test_output.tmp
    fi
done
echo ""

print_info "=== Test Summary ==="
print_info "Todos os testes de JOIN completados."
print_info "Verifique os resultados acima."

# Final cleanup
cleanup

