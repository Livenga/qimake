CC    = gcc
FLAGS = -g -Wall

SOURCE = $(shell find src/* -name \*.c)
OBJDIR = objs
OBJS   = $(addprefix $(OBJDIR)/, $(patsubst %.c, %.o, $(SOURCE)))
BINDIR = $(dir $(OBJS))

TARGET = qimake



default:
	@echo $(BINDIR)
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) -v
	@[ -d "$(BINDIR)" ] || mkdir $(BINDIR) -vp

	make $(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(FLAGS)


$(OBJDIR)/%.o:%.c
	$(CC) -o $@ -c $<


clear:
	rm $(TARGET) $(OBJDIR) -r


all:
	make clear
	make
