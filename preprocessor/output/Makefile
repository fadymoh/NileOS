include $(CODE_HOME)/makefile.vars

SRC := $(CODE_HOME)/preprocessor/output
OBJ := $(OBJECT)

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

all: $(OBJECTS)


$(OBJ)/%.o: $(SRC)/%.c
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -I$(SRC) -c $< -o $@