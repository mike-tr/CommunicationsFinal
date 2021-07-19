CXX=g++
CXXFLAGS=-g -Wall -pthread
VALGRIND_FLAGS=-v --leak-check=full --show-leak-kinds=all  --error-exitcode=99

# name of the executable file
PROG_NAME := pragma.exe
PROG_NAME2 := pragma2.exe

# set the path of the executable file
EXE_PATH := out/

SRC_PATH := src/
OBJ_PATH := obj/
HPP_PATH := src/hpp/
INTERFACE_PATH := src/interfaces/

# get all the .hpp files that are inside the HPP folder.
HEADERS := $(wildcard $(HPP_PATH)*.hpp)
INTERFACES := $(wildcard $(INTERFACE_PATH)*.hpp)

# get all the .cpp files that are inside the SRC_PATH folder.
SOURCES := $(wildcard $(SRC_PATH)*.cpp)

# name of all .o files that needs to be created
# how it works
# assume SRC_PATH = "src/"
# OBJ_PATH = "obj/"
# SOURCES = src/f1.cpp src/f2.cpp
# get from $(SOURCES) all the names.
# find the sequence src/%.cpp in our case it would be f1, f2
# add to % the format obj/%.o so we get
# obj/f1.o obj/f2.o
# now OBJECTS = obj/f1.o obj/f2.o
# now we can use OBJECTS to compile all the needed files.
OBJECTS := $(patsubst $(SRC_PATH)%.cpp, $(OBJ_PATH)%.o, $(SOURCES)) 

MAIN := main
MAIN2 := sender

#compile and run the exe
run: $(EXE_PATH)$(PROG_NAME)
	./$^

run2: $(EXE_PATH)$(PROG_NAME2)
	./$^

run_sender:  $(EXE_PATH)sender
	./$^

#Compile all the .o files.
all: $(OBJECTS)

# Compline everything, and create an executable file
# Note there is no need to specify where is the main function.
# Note will crash if there is multiple main functions!
$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp $(HEADERS) $(INTERFACES)
	$(CXX) $(CXXFLAGS) --compile $< -o $@

$(EXE_PATH)$(PROG_NAME) : $(OBJ_PATH)$(MAIN).o $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $(EXE_PATH)$(PROG_NAME)

$(OBJ_PATH)$(MAIN).o: $(MAIN).cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) --compile $< -o $@

$(EXE_PATH)$(PROG_NAME2) : $(OBJ_PATH)$(MAIN2).o $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $(EXE_PATH)$(PROG_NAME2)

$(OBJ_PATH)$(MAIN2).o: $(MAIN2).cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) --compile $< -o $@

$(EXE_PATH)sender: $(OBJ_PATH)sender.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o  $@

$(OBJ_PATH)sender.o: sender.cpp $(HEADERS) $(INTERFACES)
	$(CXX) $(CXXFLAGS) --compile $< -o $@

tidy:
	clang-tidy $(SOURCES) -checks=bugprone-*,clang-analyzer-*,cppcoreguidelines-*,performance-*,portability-*,readability-*,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-owning-memory --warnings-as-errors=-* --

valgrind: test
	valgrind --leak-check=full --error-exitcode=99 --tool=memcheck $(VALGRIND_FLAGS) ./test 

clean:
	rm -f $(OBJ_PATH)*.o $(OBJECTS) $(EXE_PATH)$(PROG_NAME) *.o main
