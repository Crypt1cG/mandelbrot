# ----------------------------
# Makefile Options
# ----------------------------

NAME = DEMO
ICON = icon.png
DESCRIPTION = "CE C Toolchain Demo"
COMPRESSED = NO
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Ofast
CXXFLAGS = -Wall -Wextra -Ofast

# ----------------------------

include $(shell cedev-config --makefile)
