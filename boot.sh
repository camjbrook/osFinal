#!/bin/bash

# =============================================================
#  boot.sh
#  Cleans /bin and /logs, recreates them, compiles all modules
# =============================================================

# Colour codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Colour

PASS=0
FAIL=0

echo -e "${CYAN}"
echo "============================================="
echo "   CSCI 3431 — OS Final Project Boot Script  "
echo "============================================="
echo -e "${NC}"

# -------------------------------------------------------------
# STEP 1 — Clean and recreate project directories
# -------------------------------------------------------------
echo -e "${YELLOW}[BOOT] Setting up project directories...${NC}"
rm -rf bin logs tmp
rm -f /tmp/logger_fifo
mkdir -p bin logs tmp bin/Documents

# -------------------------------------------------------------
# STEP 2 — Check that all source files exist
# -------------------------------------------------------------
echo -e "${YELLOW}[BOOT] Checking source files...${NC}"

REQUIRED_SOURCES=(
    "logger.c"
    "peterson.c"
    "fileManager.c"
    "memory.c"
    "amdahl.c"
    "main_menu.c"
)

ALL_SOURCES_PRESENT=true

for src in "${REQUIRED_SOURCES[@]}"; do
    if [ ! -f "$src" ]; then
        echo -e "${RED}[MISSING] $src${NC}"
        ALL_SOURCES_PRESENT=false
    else
        echo -e "${GREEN}[FOUND]   $src${NC}"
    fi
done

if [ "$ALL_SOURCES_PRESENT" = false ]; then
    echo ""
    echo -e "${RED}[ERROR] One or more source files are missing. Aborting.${NC}"
    exit 1
fi

echo ""

# -------------------------------------------------------------
# STEP 3 — Compile modules
#           logger.c MUST be compiled first (all others depend on it)
# -------------------------------------------------------------
echo -e "${YELLOW}[BOOT] Compiling modules...${NC}"
echo ""

compile() {
    local label=$1
    local src=$2
    local out=$3
    local flags=$4

    echo -n "  Compiling $label... "

    if gcc $src -o $out $flags 2>/tmp/compile_err; then
        echo -e "${GREEN}OK${NC}"
        ((PASS++))
    else
        echo -e "${RED}FAILED${NC}"
        echo -e "${RED}  --- Error output ---${NC}"
        cat /tmp/compile_err | sed 's/^/    /'
        echo ""
        ((FAIL++))
    fi
}

# logger first — other modules write to its FIFO
compile "logger"      logger.c      bin/logger      ""

# remaining modules
compile "peterson"    peterson.c    bin/peterson    "-lpthread"
compile "fileManager" fileManager.c bin/fileManager ""
compile "memory"      memory.c      bin/memory      ""
compile "amdahl"      amdahl.c      bin/amdahl      ""

# main_menu last — launches all others
compile "main_menu"   main_menu.c   bin/main_menu   ""

echo ""

# -------------------------------------------------------------
# STEP 4 — Create Documents folder required by fileManager
# -------------------------------------------------------------
mkdir -p bin/Documents
echo -e "${GREEN}[OK]   Created bin/Documents/ for file manager${NC}"
echo ""

# -------------------------------------------------------------
# STEP 5 — Summary
# -------------------------------------------------------------
echo "============================================="
echo -e "  Build complete: ${GREEN}${PASS} passed${NC}, ${RED}${FAIL} failed${NC}"
echo "============================================="
echo ""

if [ $FAIL -ne 0 ]; then
    echo -e "${RED}[WARN] Some modules failed to compile. Fix errors above before running.${NC}"
    exit 1
fi

# -------------------------------------------------------------
# STEP 6 — Launch
# -------------------------------------------------------------
echo -e "${CYAN}[BOOT] Launching main menu...${NC}"
echo ""
cd bin && ./main_menu
