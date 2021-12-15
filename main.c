#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
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

void compareBestMinMax(Move res, Move move, Move *bestMax, Move *bestMin, bool p1Turn, int profondeur) {
    if (res.score > bestMax->score && p1Turn) {
        if (profondeur == 0) {
            *bestMax = res;
        } else {
            *bestMax = move;
        }
        bestMax->score = res.score;
    } else if (res.score < bestMin->score && !p1Turn) {
        if (profondeur == 0) {
            *bestMin = res;
        } else {
            *bestMin = move;
        }
        bestMin->score = res.score;
    }
}

int nbrMinMax;
Move valeurMinMax(Move move, bool p1Turn, int profondeur, int profondeurMax, int eval) {
    // WIN JOUEUR 1
    if (move.p1.seeds > 33 || (!p1Turn && move.hole == -1) || (move.board.total <= 8 && move.p1.seeds > move.p2.seeds)) {
        move.score = 500 - profondeur;
        return move;
    }
    // WIN JOUEUR 2
    if (move.p2.seeds > 33 || (p1Turn && move.hole == -1) || (move.board.total <= 8 && move.p1.seeds < move.p2.seeds)) {
        move.score = -500 - profondeur;
        return move;
    }
    // PROFONDEUR MAX
    if (profondeur == profondeurMax || move.board.total <= 8) {
        // FONCTION EVALUATION
        /*
        if (eval == 0) {
            move.score = random() % 200 - 100;
        }
        if (eval == 1) {
            move.score = move.p1.seeds - move.p2.seeds;
        }
        if (eval == 2) {
            move.score = move.p1.seeds - move.p2.seeds - move.board.total;
        }
        if (eval == 3) {
            move.score = move.p1.seeds - move.p2.seeds + move.board.total;
        }
        if (eval == 4) {
            if (move.board.odd < 2 || (move.board.total - move.board.odd) < 2)
                move.score = -move.board.total;
            else
                move.score = move.p1.seeds - move.p2.seeds + move.board.total;
        }*/
        move.score = move.p1.seeds - move.p2.seeds;
        return move;
    }

    Move bestMax = {.score = -1000, .hole = -1, .color = ' '};
    Move bestMin = {.score = 1000, .hole = -1, .color = ' '};
    Player player = p1Turn ? move.p1 : move.p2;

    if (eval == 1) {
        // Initialisation des parametres des nodes
        for (int i = player.even; i < 16; i += 2) {
            // Trous BLEU
            if (move.board.holes[i].B > 0) {
                // Creation du coup
                Move copy = createNode(&move, p1Turn, i, true);
                // Joue le coup
                simulateMove(&move, p1Turn);
                // Min Max sur le fils
                Move res = valeurMinMax(move, !p1Turn, profondeur + 1, profondeurMax, eval);
                move = copy;
                // Comparaison avec max/min
                compareBestMinMax(res, move, &bestMax, &bestMin, p1Turn, profondeur);
                nbrMinMax++;
                if (p1Turn) {
                    if(res.score >= move.beta){
                        break;
                    }
                    if(res.score > move.alpha){
                        move.alpha = res.score;
                    }
                } else {
                    if(res.score <= move.alpha){
                        break;
                    }
                    if(res.score < move.beta){
                        move.beta = res.score;
                    }
                }
            }
            // Trous ROUGES
            if (move.board.holes[i].R > 0) {
                // Creation du coup
                Move copy = createNode(&move, p1Turn, i, false);
                // Joue le coup
                simulateMove(&move, p1Turn);
                // Min Max sur le fils
                Move res = valeurMinMax(move, !p1Turn, profondeur + 1, profondeurMax, eval);
                move = copy;
                // Comparaison avec max/min
                compareBestMinMax(res, move, &bestMax, &bestMin, p1Turn, profondeur);
                nbrMinMax++;
                if (p1Turn) {
                    if(res.score >= move.beta){
                        break;
                    }
                    if(res.score > move.alpha){
                        move.alpha = res.score;
                    }
                } else {
                    if(res.score <= move.alpha){
                        break;
                    }
                    if(res.score < move.beta){
                        move.beta = res.score;
                    }
                }
            }
        }

    }
    if (eval == 2) {
        // Initialisation des parametres des nodes
        for (int i = player.even; i < 16; i += 2) {
            // Trous ROUGES
            if (move.board.holes[i].R > 0) {
                Move copy = createNode(&move, p1Turn, i, false);
                // Joue le coup
                simulateMove(&move, p1Turn);
                Move res = valeurMinMax(move, !p1Turn, profondeur + 1, profondeurMax, eval);
                move = copy;
                // Comparaison avec max/min
                compareBestMinMax(res, move, &bestMax, &bestMin, p1Turn, profondeur);
                nbrMinMax++;
            }

            // Trous BLEU
            if (move.board.holes[i].B > 0) {
                Move copy = createNode(&move, p1Turn, i, true);
                // Joue le coup
                simulateMove(&move, p1Turn);
                Move res = valeurMinMax(move, !p1Turn, profondeur + 1, profondeurMax, eval);
                move = copy;
                // Comparaison avec max/min
                compareBestMinMax(res, move, &bestMax, &bestMin, p1Turn, profondeur);
                nbrMinMax++;
            }
        }
    }

    // Resultats
    return p1Turn ? bestMax : bestMin;
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
    setSeeds(board, player, nbrSeeds, move.color, move.hole);
    int sum = capture(board, player, lastHole);
    printMove(move);
    printf("Le joueur %d a capturé %s%d%s graines.\n", !p1Turn + 1, Colors[MAGENTA], sum, Colors[DEFAULT]);
}

Move copyMove(Move move) {
    Move copy = move;
    copy.board = copyBoard(move.board);
    copy.p1 = (Player){move.p1.seeds, move.p1.even};
    copy.p2 = (Player){move.p2.seeds, move.p2.even};
    return copy;
}

Board copyBoard(Board board) {
    Board copy = {{{2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}}, 64, 32};
    for (int i = 0; i < 16; i++) {
        copy.holes[i].R = board.holes[i].R;
        copy.holes[i].B = board.holes[i].B;
    }
    copy.total = board.total;
    copy.odd = board.odd;
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
        printf("ETAT DE FIN:\n");
    }
    printBoard(move.board);
    printf("Total: %d  even: %d  odd: %d\n", move.board.total, move.board.total - move.board.odd, move.board.odd);
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
    *color = *startingHole < 9 ? move[1] : move[2];
}
int getSeeds(Board *board, Player *player, int startingHole, char color) {
    if ((startingHole >= 0 && startingHole < 16) && (color == 'R' || color == 'B') && ((startingHole & 1) == player->even)) {
        int seeds;
        if (color == 'R') {
            seeds = board->holes[startingHole].R;
            board->holes[startingHole].R = 0;
            if (startingHole & 1)
                board->odd -= seeds;
        } else {
            seeds = board->holes[startingHole].B;
            board->holes[startingHole].B = 0;
            if (startingHole & 1)
                board->odd -= seeds;
        }
        if (seeds == 0) {
            forbiddenMove("Il n'y a aucune graines.");
        }
        return seeds;
    }
    if (startingHole == -1) {
        int p1Seeds, p2Seeds;
        if (player->even) {
            p1Seeds = 64 - player->seeds;
            p2Seeds = player->seeds;
        } else {
            p1Seeds = player->seeds;
            p2Seeds = 64 - player->seeds;
        }
        printMove((Move){*board, {p1Seeds, true}, {p2Seeds, false}});
        forbiddenMove("Joueur affamé.");
    }
    forbiddenMove("Trou inaccesible.");
}

void setSeeds(Board *board, Player *player, int nbrSeeds, char color, int startingHole) {
    if (nbrSeeds == sow->length) {
        int lastHole = startingHole;
        for (int i = 0; i < sow->length; i++) {
            int n = sow->tab[i];
            if (n == startingHole || n < 0 || n > 15) {
                printf("if %d %d %d n=%d startingHole=%d lastHole=%d\n", n == startingHole, n<0, n> 15, n, startingHole, lastHole);
                for (int j = 0; j < sow->length; j++) {
                    printf("%d ", sow->tab[j]);
                }
                printf("\n");
                forbiddenMove("Combinaison interdite.\n");
            }
            lastHole = n;
            if (color == 'B' && player->even != n % 2) {
                board->holes[n].B += 1;
                if (n & 1)
                    board->odd += 1;
            } else if (color == 'R') {
                board->holes[n].R += 1;
                if (n & 1)
                    board->odd += 1;
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
    int sum = 0;
    do {
        int nbrSeeds = board->holes[i].R + board->holes[i].B;
        if (nbrSeeds == 2 || nbrSeeds == 3) {
            board->holes[i].R = 0;
            board->holes[i].B = 0;
            player->seeds += nbrSeeds;
            board->total -= nbrSeeds;
            if (i & 1)
                board->odd -= nbrSeeds;
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


void turnMinMax(Board *board, Player *p1, Player *p2, bool p1Turn, int eval,int profondeur) {
    Move move = {*board, *p1, *p2, .alpha = -1000, .beta = 1000};
    Move copy = copyMove(move);
    copy = valeurMinMax(copy, p1Turn, 0, profondeur, eval);

    playMove(board, p1, p2, copy, p1Turn);
}

void getWinner(Board board, Player p1, Player p2) {
    if (board.total <= 8) {
        if (p1.seeds > p2.seeds) {
            printf("%sLe joueur 1 a gagné.%s\n", Colors[RED], Colors[DEFAULT]);
        } else if (p1.seeds < p2.seeds) {
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
    printMove((Move){board, p1, p2});
    exit(0);
}
float lastTime1 = 2;
float lastTime2 = 2;
int main() {
    srand(time(NULL));
    Board board = {{{2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}}, 64, 32};
    Player p1 = {0, false};
    Player p2 = {0, true};
    sow = newList_i(64);
    int profondeur1 = 6;
    int profondeur2 = 6;
    clock_t t1, t2;

    int tour = 1;
    while (true) {
        printf("%s------------------------------Player 1------------------------------> Tour %d%s\n", Colors[YELLOW], tour, Colors[DEFAULT]);
        nbrMinMax = 0;
        t1 = clock();
        turnMinMax(&board, &p1, &p2, true, 1,profondeur1);
        t2 = clock();
        lastTime1 = (float)(t2 - t1) / CLOCKS_PER_SEC;

        printf("%s--------------PROFONDEUR %i--------------\nTemps = %0.2fs\n----------------------------------------%s\n", Colors[GREEN], profondeur1, lastTime1, Colors[DEFAULT]);

        if (lastTime1 < 0.30) {
            profondeur1++;
        }
        if (lastTime1 > 2.0) {
            profondeur1--;
        }
        printf("NBRMINMAX ---> %d\n", nbrMinMax);
        getWinner(board, p1, p2);

        printf("%s------------------------------Player 2------------------------------> Tour %d%s\n", Colors[YELLOW], tour, Colors[DEFAULT]);
        nbrMinMax = 0;
        t1 = clock();
        turnMinMax(&board, &p1, &p2, false, 2,profondeur2);
        t2 = clock();
        lastTime2 = (float)(t2 - t1) / CLOCKS_PER_SEC;
        printf("%s--------------PROFONDEUR %i--------------\nTemps = %0.2fs\n----------------------------------------%s\n", Colors[GREEN], profondeur2, lastTime2, Colors[DEFAULT]);
        if (lastTime2 < 0.30) {
            profondeur2++;
        }
        if (lastTime1 > 2.0) {
            profondeur2--;
        }



        //turn(&board,&p1,&p2,false);
        printf("NBRMINMAX ---> %d\n", nbrMinMax);
        getWinner(board, p1, p2);
        tour++;
    }
}
