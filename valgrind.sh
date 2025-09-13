# 1) Build with debug symbols
make fclean && make -j CXXFLAGS="-Wall -Wextra -Werror -std=c++98 -g"

# 2) Start under Valgrind (memcheck) with maximum diagnostics
PORT=6667
PASS=pass123

valgrind \
  --tool=memcheck \
  --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --errors-for-leak-kinds=all \
  --num-callers=50 \
  --read-var-info=yes \
  --track-fds=yes \
  --log-file=valgrind.%p.log \
  ./ircserv "$PORT" "$PASS"
