targets	:= maxTweeter
objs	:= maxTweeter.o

CC	:= gcc
CFLAGS	:= -Wall -Werror
CFLAGS	+= -g

ifneq ($(V),1)
Q = @
endif

all: $(targets)

maxTweeter: $(objs)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "clean"
	$(Q)rm -f $(targets) $(objs)
