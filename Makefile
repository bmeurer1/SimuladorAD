all: simulation

simulation: simulador.c
	gcc -o sim simulador.c -lm

clean:
	rm sim