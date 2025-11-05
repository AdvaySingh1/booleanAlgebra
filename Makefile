PCN_PATH ?= ../ProgrammingAssignment1Files/UnateRecursiveComplement
OUT_SUFFIX ?= C
CXX ?= g++
CXXFLAGS ?= -std=c++17 -g3
all: URPC


# debug
DEBUG_PRINT ?= 0
DEBUG_BREAKPOINT ?= 0

ifeq ($(DEBUG_PRINT),1)
CXXFLAGS += -DDEBUG_PRINT -O0
endif

ifeq ($(DEBUG_BREAKPOINT),1)
CXXFLAGS += -DDEBUG_BREAKPOINT -O0
endif


URPC: URPComplement.cpp
	$(CXX) $(CXXFLAGS) URPComplement.cpp -o URPComplement

$(PCN_PATH)/part%$(OUT_SUFFIX).pcn: $(PCN_PATH)/part%.pcn
	./URPComplement < $(PCN_PATH)/part$*.pcn
# 	./URPComplement < $(PCN_PATH)/$*.pcn > $@

%: $(PCN_PATH)/part%$(OUT_SUFFIX).pcn
	@echo "Wrote $<"

clean:
	@rm -rf URPComplement

.PHONEY: clean