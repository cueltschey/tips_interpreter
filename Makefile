TARGET   = tips

LEX      = flex
CXX      = g++
CC       = gcc
RM       = rm -f

CXXFLAGS = -g -std=c++11 -Wall -Werror
CCFLAGS  = -g

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

LEX_SRC = lex.yy.c
LEX_OBJ = lex.yy.o

.PRECIOUS = *.l *.h *.cpp [Mm]akefile


$(TARGET): $(OBJ) $(LEX_OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

# Compile lex.yy.c
$(LEX_OBJ): $(LEX_SRC)
	$(CC) $(CCFLAGS) -c $< -o $@

# Generate lex.yy.c from rules.l
$(LEX_SRC): rules.l
	$(LEX) -o $@ $<

clean:
	$(RM) *.o lex.yy.c $(TARGET)

