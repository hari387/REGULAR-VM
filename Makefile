CXX=clang

# We have -Wno-unused-parameter so that the compiler
# doesn't complain too much about our stubs.

CXXFLAGS = -fsanitize=undefined -fsanitize=address -g

# Change to this before final submission:

# CXXFLAGS = -Wall -Wextra -Werror

BINARIES = assembler

assemble: assemble.c
	${CXX} $^ -o $@ ${CXXFLAGS}

run: run.c
	${CXX} $^ -o $@ ${CXXFLAGS}

test: test.c
	${CXX} $^ -o $@ ${CXXFLAGS}

clean:
	/bin/rm -f ${BINARIES} *.o
