EXEC := proberen verhogen delete

all: $(EXEC)

clean:
	$(RM) $(EXEC) *.o

run:
	tmux -f /dev/null new-session \; split-window -h \; send-keys './proberen' C-m \; select-pane -R \; send-keys './verhogen' C-m
