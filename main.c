#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "main.h"
#include "List.h"

Move createNode(Move *move, bool p1Turn, int nodeNbr, bool blue) {
    Move copy = copyMove(*move);
    move->hole = nodeNbr;
    move->color = blue ? 'B' : 'R';
    return copy;
}

Move valeurMinMax(Move move, bool p1Turn, int profondeur, int profondeurMax) {
    // WIN JOUEUR 1
    if (move.p1.seeds > 33 || (!p1Turn && move.hole == -1)) {
        move.score = 100 - profondeur;
        return move;
    }
    // WIN JOUEUR 2
    if (move.p2.seeds > 33 || (p1Turn && move.hole == -1)) {
        move.score = -100 + profondeur;
        return move;
    }
    // PROFONDEUR MAX
    if (profondeur == profondeurMax || move.board.total <= 8) {
        // FONCTION EVALUATION
        move.score = move.p1.seeds - move.p2.seeds - profondeur;
        return move;
    }

    Move bestMax = {.score = -100, .hole = -1, .color = ' '};
    Move bestMin = {.score = 100, .hole = -1, .color = ' '};
    Player player = p1Turn ? move.p1 : move.p2;
    // Initialisation des parametres des nodes
    for (int i = player.even; i < 16; i += 2) {
        // Trous BLEU
        if (move.board.holes[i].B > 0) {
            Move copy = createNode(&move, p1Turn, i, true);

            // Joue le coup
            simulateMove(&move, p1Turn);
            Move res = valeurMinMax(move, !p1Turn, profondeur + 1, profondeurMax);
            move = copy;

            // Comparaison avec max/min
            if (res.score > bestMax.score && p1Turn) {
                if (profondeur == 0) {
                    bestMax = res;
                } else {
                    bestMax = move;
                }
            } else if (res.score < bestMin.score && !p1Turn) {
                if (profondeur == 0) {
                    bestMin = res;
                } else {
                    bestMin = move;
                }
            }
        }

        // Trous ROUGES
        if (move.board.holes[i].R > 0) {
            Move copy = createNode(&move, p1Turn, i, false);

            // Joue le coup
            simulateMove(&move, p1Turn);
            Move res = valeurMinMax(move, !p1Turn, profondeur + 1, profondeurMax);
            move = copy;

            // Comparaison avec max/min
            if (res.score > bestMax.score && p1Turn) {
                if (profondeur == 0) {
                    bestMax = res;
                } else {
                    bestMax = move;
                }
            } else if (res.score < bestMin.score && !p1Turn) {
                if (profondeur == 0) {
                    bestMin = res;
                } else {
                    bestMin = move;
                }
            }
        }
    }
    // Resultats
    if (p1Turn) {
        return bestMax;
    } else {
        return bestMin;
    }
}

List_i *sow;
int initSow(int startingHole, char color, bool p1Turn, int nbrSeeds) {
    int decalage = 1;
    if (color == 'B') {
        decalage = 2;
    }
    int index = startingHole + 1;
    sow->length = 0;
    for (int i = 0; i < nbrSeeds; ++i) {
        if (index == 16)
            index = 0;
        if (index == 17)
            index = 1;
        sow->tab[i] = index;
        sow->length++;
        index = index + decalage;
    }

    return sow->tab[sow->length - 1];
}

int simulateMove(Move *move, bool p1Turn) {
    Player *player = p1Turn ? &move->p1 : &move->p2;
    // Recuperation des graines
    int nbrSeeds = getSeeds(&move->board, player, move->hole, move->color);
    // Creation de la liste sow
    int lastHole = initSow(move->hole, move->color, p1Turn, nbrSeeds);
    // Semage
    setSeeds(&move->board, player, nbrSeeds, move->color, move->hole);
    // Recolte
    return capture(&move->board, player, lastHole);
}

void playMove(Board *board, Player *p1, Player *p2, Move move, bool p1Turn) {
    Player *player = p1Turn ? p1 : p2;
    int nbrSeeds = getSeeds(board, player, move.hole, move.color);
    int lastHole = initSow(move.hole, move.color, p1Turn, nbrSeeds);
    printMove(move);
    setSeeds(board, player, nbrSeeds, move.color, move.hole);
    int sum = capture(board, player, lastHole);
    printf("%sLe joueur %d a capturé %d graines.%s\n", Colors[MAGENTA], !p1Turn + 1, sum - board->total, Colors[DEFAULT]);
}

Move copyMove(Move move) {
    Move copy = move;
    copy.board = copyBoard(move.board);
    copy.p1 = (Player){move.p1.seeds, move.p1.even};
    copy.p2 = (Player){move.p2.seeds, move.p2.even};
    return copy;
}

Board copyBoard(Board board) {
    Board copy = {{{2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}}, 64};
    for (int i = 0; i < 16; i++) {
        copy.holes[i].R = board.holes[i].R;
        copy.holes[i].B = board.holes[i].B;
    }
    copy.total = board.total;
    return copy;
}

void forbiddenMove(char *move) {
    printf("%sCout interdit: %s%s%s\n", Colors[RED], Colors[YELLOW], move, Colors[DEFAULT]);
    exit(1);
}

void printMove(Move move) {
    if (move.color != 0) {
        printf("%sMove : %d%c   ", Colors[CYAN], move.hole + 1, move.color);
        for (int i = 0; i < sow->length; i++) {
            printf("%d ", sow->tab[i] + 1);
        }
        printf("\n%s", Colors[DEFAULT]);
    } else {
        printf("UNINITALIZED\n");
    }
    printBoard(move.board);
    printf("P1: %d   P2: %d\n", move.p1.seeds, move.p2.seeds);
}

void printBoard(Board board) {
    for (int i = 0; i < 16; i++) {
        printf("%d(%s%d%s,%s%d%s) ", i + 1, Colors[RED], board.holes[i].R, Colors[DEFAULT], Colors[BLUE], board.holes[i].B, Colors[DEFAULT]);
        if (i == 7 || i == 15) {
            printf("\n");
        }
    }
}

void readHole(int *startingHole, char *color) {
    char move[3];
    scanf("%3s", move);
    *startingHole = atoi(move) - 1;
    *color = *startingHole < 10 ? move[1] : move[2];
}

int getSeeds(Board *board, Player *player, int startingHole, char color) {
    if ((startingHole >= 0 && startingHole < 16) && (color == 'R' || color == 'B') && ((startingHole & 1) == player->even)) {
        int seeds;
        if (color == 'R') {
            seeds = board->holes[startingHole].R;
            board->holes[startingHole].R = 0;
        } else {
            seeds = board->holes[startingHole].B;
            board->holes[startingHole].B = 0;
        }
        if (seeds == 0) {
            forbiddenMove("Il n'y a aucune graines.");
        }
        return seeds;
    }
    if (startingHole == -1) {
        forbiddenMove("Joueur affamé.");
    }
    forbiddenMove("Trou inaccesible.");
}

void setSeeds(Board *board, Player *player, int nbrSeeds, char color, int startingHole) {
    if (nbrSeeds == sow->length) {
        int lastHole = startingHole;
        for (int i = 0; i < sow->length; i++) {
            int n = sow->tab[i];
            if (n == startingHole || (lastHole > n && n > startingHole) || n < 0 || n > 15) {
                forbiddenMove("Combinaison interdite.\n");
            }
            lastHole = n;
            if (color == 'B' && player->even != n % 2) {
                board->holes[n].B += 1;
            } else if (color == 'R') {
                board->holes[n].R += 1;
            } else {
                forbiddenMove("Les graines bleus vont seulement chez l'adversaire.");
            }
        }
    } else {
        forbiddenMove("Mauvais nombre de trous.");
    }
}

int capture(Board *board, Player *player, int lastSowedHole) {
    int i = lastSowedHole;
    int sum = board->total;
    do {
        int nbrSeeds = board->holes[i].R + board->holes[i].B;
        if (nbrSeeds == 2 || nbrSeeds == 3) {
            board->holes[i].R = 0;
            board->holes[i].B = 0;
            player->seeds += nbrSeeds;
            board->total -= nbrSeeds;
            sum += nbrSeeds;
            i--;
            if (i < 0) {
                i = 15;
            }
        } else {
            break;
        }
    } while (i != lastSowedHole);
    return sum;
}

void turn(Board *board, Player *p1, Player *p2, bool p1Turn) {
    printf("%sEntrez votre coup:%s\n", Colors[GREEN], Colors[DEFAULT]);
    int startingHole;
    char color;
    readHole(&startingHole, &color);

    Move move = {*board, *p1, *p2, startingHole, color};
    playMove(board, p1, p2, move, p1Turn);
}

float lastTime = 2;
int profondeur = 3;
void turnMinMax(Board *board, Player *p1, Player *p2, bool p1Turn) {
    Move move = {*board, *p1, *p2};
    Move copy = copyMove(move);

    if (lastTime < 0.5) {
        profondeur++;
    } else {
        profondeur--;
    }

    // TEMPS
    clock_t t1, t2;
    t1 = clock();
    copy = valeurMinMax(copy, p1Turn, 0, profondeur);
    t2 = clock();
    lastTime = (float)(t2 - t1) / CLOCKS_PER_SEC;
    printf("%s--------------PROFONDEUR %i--------------\ntemps = %f\n----------------------------------------%s\n", Colors[GREEN], profondeur, lastTime, Colors[DEFAULT]);

    playMove(board, p1, p2, copy, p1Turn);
}

void getWinner(Board board, Player p1, Player p2) {
    if (board.total <= 8) {
        if (p1.seeds > p2.seeds) {
            printf("%sLe joueur 1 a gagné.%s\n", Colors[RED], Colors[DEFAULT]);
        }
        if (p1.seeds < p2.seeds) {
            printf("%sLe joueur 2 a gagné.%s\n", Colors[RED], Colors[DEFAULT]);
        } else {
            printf("%sEgalité.\n%s", Colors[RED], Colors[DEFAULT]);
        }
    } else if (p1.seeds >= 33) {
        printf("%sLe joueur 1 a gagné.%s\n", Colors[RED], Colors[DEFAULT]);
    } else if (p2.seeds >= 33) {
        printf("%sLe joueur 2 a gagné.%s\n", Colors[RED], Colors[DEFAULT]);
    } else {
        return;
    }
    exit(0);
}

int main() {
    srand(time(NULL));
    Board board = {{{2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}}, 64};
    Player p1 = {0, false};
    Player p2 = {0, true};
    sow = newList_i(64);

    while (true) {
        printf("%s------------------------------Player 1------------------------------%s\n", Colors[YELLOW], Colors[DEFAULT]);
        // turnMinMax(&board, &p1, &p2, true);
        turnMinMax(&board, &p1, &p2, true);
        getWinner(board, p1, p2);
        printf("%s------------------------------Player 2------------------------------%s\n", Colors[YELLOW], Colors[DEFAULT]);
        turnMinMax(&board, &p1, &p2, false);
        getWinner(board, p1, p2);
    }
}
