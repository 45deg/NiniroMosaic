COMPILER = g++
CFLAGS   = --std=c++11 -MMD -MP
CFLAGS_DEBUG   = -g -Wall -Wextra -Winit-self -Wno-unused-parameter -Wfloat-equal
CFLAGS_RELEASE   = -O3
LDFLAGS  = $(shell pkg-config --libs opencv) -lzmq -framework OpenGl -lglfw3
LIBS     = 
INCLUDE  = -I../include $(shell pkg-config --cflags opencv)
TARGET   = ../bin/$(shell basename `readlink -f ..`)
OBJDIR   = ./obj
ifeq "$(strip $(OBJDIR))" ""
	OBJDIR = .
endif
SOURCES  = $(wildcard *.cpp)
OBJECTS  = $(addprefix $(OBJDIR)/, $(SOURCES:.cpp=.o))
	DEPENDS  = $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

debug: CFLAGS+=$(CFLAGS_DEBUG)
debug: all

release: CFLAGS+=$(CFLAGS_RELEASE)
release: all

all: clean $(TARGET)

clean:
	rm -f $(OBJECTS) $(DEPENDS) $(TARGET)
	@rmdir --ignore-fail-on-non-empty `readlink -f $(OBJDIR)`

-include $(DEPENDS)
