#compiler settings
CC = gcc
CFLAGS = -g ${COPT}
LDLIBS = -lGL -lGLU -lSDL2 -lSDL2_net -lSDL2_ttf -lGLEW -lm
COPT = -O2 -march=native -mtune=native
SDLFLAGS = `sdl-config --cflags`
SDLLIBS = `sdl-config --libs`

#directory structure
SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

#directories with source code to include
INC_DIR = -I ${SRC_DIR} -I ${LIB_DIR}

#resulting executable filename 
OFILE = gprs

#a list of all the object files to be created
OBJS := $(addprefix ${OBJ_DIR}/,main.o engine.o engineevents.o enginetransitions.o render.o renderobject.o eventserver.o legacy.o) \
        $(addprefix ${LIB_DIR}/, stb_image.o)

# Windows (MinGW)
ifeq "$(OS)" "Windows_NT"
    LDLIBS = -lopengl32 -lglu32 -lglew32 -lmingw32 -lSDL2_net -lSDL2main -lSDL2 -lm
endif

#link the executable
${BIN_DIR}/${OFILE}: ${OBJS} | ${BIN_DIR}
	${CC} ${CFLAGS} $^ -o $@ ${LDLIBS}

#create the wanted object files provided the source files exist.
${OBJ_DIR}/%.o: ${SRC_DIR}/%.c | ${OBJ_DIR}
	${CC} ${CFLAGS} -c $< -o $@ ${INC_DIR}

#create the object files for source code in the lib directory
${LIB_DIR}/%.o: ${LIB_DIR}/%.c | ${OBJ_DIR}
	${CC} ${CFLAGS} -c $< -o $@ ${INC_DIR}

#creates the bin directory.
${BIN_DIR}:
	mkdir ${BIN_DIR}

#creates the obj directory
${OBJ_DIR}:
	mkdir ${OBJ_DIR}

#compiles with DEBUG flag defined
debug: setdebug ${BIN_DIR}/${OFILE}

#sets a DEBUG flag (as if defined to 1 in the source code) for every source file.
setdebug:
	$(eval CFLAGS += -DDEBUG)

#remove the compiled code
clean:
	rm -f ${OBJ_DIR}/*.o
	rm -f ${BIN_DIR}/${OFILE}


