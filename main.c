#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

Board board;

int main() {
    board = newBoard;
    Player player1 = newPlayer1;
    Player player2 = newPlayer2;
    while (true) {
        printf("%s------------------------------Player 1------------------------------%s\n", Colors[YELLOW], Colors[DEFAULT]);
        if (turn(player1)) {
            getWinner(player1, player2);
            break;
        }
        printf("%s------------------------------Player 2------------------------------%s\n", Colors[YELLOW], Colors[DEFAULT]);
        if (turn(player2)) {
            getWinner(player1, player2);
            break;
        }
    }
}

void getWinner(Player player1, Player player2) {
    if (player1.seeds > player2.seeds) {
        printf("%sLe joueur 1 a gagné.%s\n", Colors[MAGENTA], Colors[DEFAULT]);
    } else if (player1.seeds < player2.seeds) {
        printf("%sLe joueur 2 a gagné.%s\n", Colors[MAGENTA], Colors[DEFAULT]);
    } else {
        printf("%sEgalité.%s\n", Colors[MAGENTA], Colors[DEFAULT]);
    }
}

bool turn(Player player) {
    draw();

    printf("%sEntrez votre coup:%s\n", Colors[GREEN], Colors[DEFAULT]);
    int holeNbr;
    char color;
    readHole(&holeNbr, &color);

    int seeds = getSeeds(player, holeNbr, color);

    printf("%sVous devez semer %d graines:%s\n", Colors[GREEN], seeds, Colors[DEFAULT]);
    int length = 0;
    int *holes = readLine(&length);
    setSeeds(player, seeds, color, holeNbr, holes, length);
    capture(player, holeNbr);

    return player.seeds == 33 || board.total < 8;
}

void forbiddenMove(char *move) {
    printf("%sCout interdit: %s%s%s\n", Colors[RED], Colors[YELLOW], move,
           Colors[DEFAULT]);
    exit(1);
}

void readHole(int *holeNbr, char *color) {
    char move[3];
    scanf("%3s", move);
    *holeNbr = atoi(move) - 1;
    *color = *holeNbr < 10 ? move[1] : move[2];
}

int getSeeds(Player player, int holeNbr, char color) {
    if ((holeNbr >= 0 && holeNbr < 16) && (color == 'R' || color == 'B') &&
        ((holeNbr & 1) == player.even)) {
        int seeds;
        if (color == 'R') {
            seeds = board.holes[holeNbr].R;
            board.holes[holeNbr].R = 0;
        } else {
            seeds = board.holes[holeNbr].B;
            board.holes[holeNbr].B = 0;
        }
        if (seeds == 0) {
            //TODO Starving
            forbiddenMove("Il n'y a aucune graines.");
        }
        return seeds;
    }
    forbiddenMove("Trou inaccesible.");
}

int *readLine(int *length) {
    char c;
    char nbr[3];
    int *holes = malloc(sizeof(int) * 32);
    int i = 0;
    do {
        scanf("%c", &c);
        if (c == ' ' || c == '\n') {
            if (i == 0 && *length == 0) {
                c = ' ';
                continue;
            }
            nbr[i] = '\0';
            i = 0;
            holes[*length] = atoi(nbr) - 1;
            *length += 1;
        } else {
            nbr[i] = c;
            i++;
        }
    } while (c != '\n');

    return holes;
}

void setSeeds(Player player, int nbrSeeds, char color, int startingHole, int *holes, int length) {
    if (nbrSeeds == length) {
        int lastHole = startingHole;
        for (int i = 0; i < length; i++) {
            int n = holes[i];
            if (n == startingHole || (lastHole > n && n > startingHole) || n < 0 || n > 15) {
                forbiddenMove("Combinaison interdite.\n");
            }
            lastHole = n;
            if (color == 'B' && player.even != n % 2) {
                board.holes[n].B += 1;
            } else if (color == 'R') {
                board.holes[n].R += 1;
            } else {
                forbiddenMove("Les graines bleus vont seulement chez l'adversaire.");
            }
        }
    } else {
        forbiddenMove("Mauvais nombre de trous.");
    }
}

void capture(Player player, int startingHole) {
    int i = startingHole;
    do {
        int nbrSeeds = board.holes[i].R + board.holes[i].B;
        if (nbrSeeds == 2 || nbrSeeds == 3) {
            board.holes[i].R = 0;
            board.holes[i].B = 0;
            player.seeds += nbrSeeds;
            board.total -= nbrSeeds;
        }
        i--;
        if (i < 0) {
            i = 15;
        }
    } while (i != startingHole);
    printf("%sLe joueur a %d graines.%s\n", Colors[MAGENTA], player.seeds, Colors[DEFAULT]);
}

void draw() {
    for (int i = 0; i < 16; i++) {
        printf("%d(%s%d%s,%s%d%s) ", i + 1, Colors[RED], board.holes[i].R,
               Colors[DEFAULT], Colors[BLUE], board.holes[i].B, Colors[DEFAULT]);
        if (i == 7 || i == 15) {
            printf("\n");
        }
    }
}