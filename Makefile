PCN_PATH ?= ./ProgrammingAssignment1Files/UnateRecursiveComplement
OUT_SUFFIX ?= C
CXX ?= g++
CXXFLAGS ?= -std=c++17 -g3
all: URPC

URPC: URPComplement.cpp
	$(CXX) $(CXXFLAGS) URPComplement.cpp -o URPComplement

$(PCN_PATH)/part%$(OUT_SUFFIX).pcn: URPC $(PCN_PATH)/part%.pcn
	./URPComplement < $(PCN_PATH)/part$*.pcn
# 	./URPComplement < $(PCN_PATH)/$*.pcn > $@

%: $(PCN_PATH)/part%$(OUT_SUFFIX).pcn
	@echo "Wrote $<"

clean:
	@rm -rf URPComplement

.PHONEY: clean