#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "List.h"

struct Hole_s {
    int B;
    int R;
} newHole = {2, 2};

typedef struct Hole_s Hole;

struct Board_s {
    Hole holes[16];
    int total;
} newBoard = {{{2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}}, 64};

typedef struct Board_s Board;
List_i* generateNextHoles(int startingHole, int seeds, char color);
struct Player_s {
    int seeds;
    bool even;
};

typedef struct Player_s Player;

enum Colors {
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    DEFAULT
};

char *Colors[7] = {"", "", "", "", "", "", ""};

struct Move_s {
    Board board;
    Player p1;
    Player p2;
    int hole;
    char color;
    int nbrSeeds;
    List_i *sow;
    int score;
};

typedef struct Move_s Move;

Move valeurMinMax(Move move, bool p1Turn, int profondeur, int profondeurMax);
void getSow(Move *move);
Board copyBoard(Board board);
Player copyPlayer(Player player);
Move copyMove(Move move);
void forbiddenMove(char *move);
void printMove(Move move);
int playMove(Move *move, bool p1Turn);
void countSort(List_i *list);

Move valeurMinMax(Move move, bool p1Turn, int profondeur, int profondeurMax) {
    // WIN JOUEUR 1
    if (move.p1.seeds > 33) {
        move.score = 64;
        return move;
    }
    // WIN JOUEUR 2
    if (move.p2.seeds > 33) {
        move.score = -64;
        return move;
    }
    // PROFONDEUR MAX
    if (profondeur == profondeurMax || move.board.total <= 8) {
        // FONCTION EVALUATION
        move.score = move.p1.seeds - move.p2.seeds;
        return move;
    }

    Move bestMax;
    bestMax.score = -64;
    Move bestMin;
    bestMin.score = 64;
    Player player = p1Turn ? move.p1 : move.p2; 
    // Initialisation des parametres des nodes
    for (int i = player.even; i < 16; i += 2) {
        // Trous BLEU
        if (move.board.holes[i].B > 0) {
            Move copy = copyMove(move);
            move.hole = i;
            move.color = 'B';
            move.nbrSeeds = move.board.holes[move.hole].B;
            move.sow = generateNextHoles(i,move.nbrSeeds,'B');

            // Joue le coup
            playMove(&move, p1Turn);
            Move res = valeurMinMax(move, !p1Turn, profondeur + 1, profondeurMax);
            // printf("------Profondeur: %d------> %d\n", profondeur, res.score);
            // printMove(res);
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
            Move copy = copyMove(move);
            move.hole = i;
            move.color = 'R';
            move.nbrSeeds = move.board.holes[move.hole].R;
            move.sow = generateNextHoles(i,move.nbrSeeds,'R');

            // Joue le coup
            playMove(&move, p1Turn);
            Move res = valeurMinMax(move, !p1Turn, profondeur + 1, profondeurMax);
            // printf("------Profondeur: %d------> %d\n", profondeur, res.score);
            // printMove(res);
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

int playMove(Move *move, bool p1Turn) {
    Player *player = p1Turn ? &move->p1 : &move->p2;
    // Recuperation des graines
    if (move->color == 'B') {
        move->board.holes[move->hole].B = 0;
    } else {
        move->board.holes[move->hole].R = 0;
    }

    // Semage
    if (move->nbrSeeds == move->sow->length) {
        int lastHole = move->hole;
        for (int i = 0; i < move->sow->length; i++) {
            int n = move->sow->tab[i];
            if (n == move->hole || (lastHole > n && n > move->hole) || n < 0 || n > 15) {
                forbiddenMove("Combinaison interdite.\n");
            }
            lastHole = n;
            if (move->color == 'B' && player->even != n % 2) {
                move->board.holes[n].B += 1;
            } else if (move->color == 'R') {
                move->board.holes[n].R += 1;
            } else {
                printMove(*move);
                printf("\n%i\n",n);
                forbiddenMove("Les graines bleus vont seulement chez l'adversaire.");
            }
        }
    } else {
        forbiddenMove("Mauvais nombre de trous.");
    }
    // Recolte
    int i = move->hole;
    int sum = 0;
    do {
        int nbrSeeds = move->board.holes[i].R + move->board.holes[i].B;
        if (nbrSeeds == 2 || nbrSeeds == 3) {
            move->board.holes[i].R = 0;
            move->board.holes[i].B = 0;
            player->seeds += nbrSeeds;
            move->board.total -= nbrSeeds;
            sum += nbrSeeds;
            i--;
            if (i < 0) {
                i = 15;
            }
        } else {
            break;
        }
    } while (i != move->hole);
    return sum;
}

void getSow(Move *move) {
    move->sow = newList_i(move->nbrSeeds);
    List_i *smaller = newList_i(move->nbrSeeds);
    List_i *bigger = newList_i(move->nbrSeeds);
    while (smaller->length + bigger->length < move->nbrSeeds) {
        int value = rand() % 16;
        if (((move->color == 'B' && move->hole % 2 != value % 2) || move->color == 'R') && move->hole != value) {
            if (value < move->hole) {
                smaller->tab[smaller->length] = value;
                smaller->length++;
            } else {
                bigger->tab[bigger->length] = value;
                bigger->length++;
            }
        }
    }
    countSort(smaller);
    countSort(bigger);
    for (int i = 0; i < bigger->length; i++) {
        move->sow->tab[move->sow->length] = bigger->tab[i];
        move->sow->length++;
    }
    for (int i = 0; i < smaller->length; i++) {
        move->sow->tab[move->sow->length] = smaller->tab[i];
        move->sow->length++;
    }
    freeList_i(smaller);
    freeList_i(bigger);
}

void countSort(List_i *list) {
    List_i *output = newList_i(list->length);
    int count[16], i;
    memset(count, 0, sizeof(count));
    for (i = 0; i < list->length; i++) {
        count[list->tab[i]]++;
    }
    for (i = 1; i < 16; i++) {
        count[i] += count[i - 1];
    }
    for (i = 0; i < list->length; i++) {
        int value = list->tab[i];
        output->tab[count[value] - 1] = value;
        count[value]--;
    }
    for (i = 0; i < list->length; ++i) {
        list->tab[i] = output->tab[i];
    }
    freeList_i(output);
}

Move copyMove(Move move) {
    Move copy = move;
    copy.board = copyBoard(move.board);
    copy.p1 = copyPlayer(move.p1);
    copy.p2 = copyPlayer(move.p2);
    return copy;
}

void forbiddenMove(char *move) {
    printf("%sCout interdit: %s%s%s\n", Colors[RED], Colors[YELLOW], move, Colors[DEFAULT]);
    exit(1);
}

Board copyBoard(Board board) {
    Board copy = newBoard;
    for (int i = 0; i < 16; i++) {
        copy.holes[i].R = board.holes[i].R;
        copy.holes[i].B = board.holes[i].B;
    }
    copy.total = board.total;
    return copy;
}

Player copyPlayer(Player player) {
    Player copy = {0, false};
    copy.even = player.even;
    copy.seeds = player.seeds;
    return copy;
}

void printMove(Move move) {
    if (move.color != 0 && move.sow != NULL) {
        printf("Move : %d%c   ", move.hole + 1, move.color);
        for (int i = 0; i < move.sow->length; i++) {
            printf("%d ", move.sow->tab[i] + 1);
        }
        printf("\n");
    }
    for (int i = 0; i < 16; i++) {
        printf("%d(%s%d%s,%s%d%s) ", i + 1, Colors[RED], move.board.holes[i].R, Colors[DEFAULT], Colors[BLUE], move.board.holes[i].B, Colors[DEFAULT]);
        if (i == 7 || i == 15) {
            printf("\n");
        }
    }
    printf("P1: %d   P2: %d\n", move.p1.seeds, move.p2.seeds);
}

void draw(Board board) {
    for (int i = 0; i < 16; i++) {
        printf("%d(%s%d%s,%s%d%s) ", i + 1, Colors[RED], board.holes[i].R, Colors[DEFAULT], Colors[BLUE], board.holes[i].B, Colors[DEFAULT]);
        if (i == 7 || i == 15) {
            printf("\n");
        }
    }
}
int convertStr(char inputStr[]){
    int j=0;
    for (int i = 0; inputStr[i]; i++)
    {
        //5
        if (inputStr[i] >= '0' && inputStr[i] <= '9')
        {

            inputStr[j] = inputStr[i];
            j++;
        }
    }

    //6
    inputStr[j] = '\0';
    //printf("%i\n",(int)inputStr);
    return (int)inputStr;
}
int readHole(int *startingHole, char *color) {
    char move[3];
    scanf("%3s", move);
    *startingHole = atoi(move) - 1;
    *color = *startingHole < 10 ? move[1] : move[2];
    return *startingHole;
}


int getSeeds(Board *board, Player *player, int startingHole, char color) {
    if ((startingHole >= 0 && startingHole < 16) && (color == 'R' || color == 'B') &&
        ((startingHole & 1) == player->even)) {
        int seeds;
        if (color == 'R') {
            seeds = board->holes[startingHole].R;
            board->holes[startingHole].R = 0;
        } else {
            seeds = board->holes[startingHole].B;
            board->holes[startingHole].B = 0;
        }
        if (seeds == 0) {
            // TODO Starving
            forbiddenMove("Il n'y a aucune graines.");
        }
        return seeds;
    }
    forbiddenMove("Trou inaccesible.");
}

List_i *readLine() {
    char c;
    char nbr[3];
    List_i *holes = malloc(sizeof(List_i));
    holes->tab = malloc(sizeof(int) * 32);
    holes->length = 0;
    int i = 0;
    do {
        scanf("%c", &c);
        if (c == ' ' || c == '\n') {
            if (i == 0 && holes->length == 0) {
                c = ' ';
                continue;
            }
            nbr[i] = '\0';
            i = 0;
            holes->tab[holes->length] = atoi(nbr) - 1;
            holes->length++;
        } else {
            nbr[i] = c;
            i++;
        }
    } while (c != '\n');
    return holes;
}

void setSeeds(Board *board, Player *player, int nbrSeeds, char color, int startingHole, List_i *holes) {
    if (nbrSeeds == holes->length) {
        int lastHole = startingHole;
        for (int i = 0; i < holes->length; i++) {
            int n = holes->tab[i];
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

void capture(Board *board, Player *player, int startingHole) {
    int i = startingHole;
    do {
        int nbrSeeds = board->holes[i].R + board->holes[i].B;
        if (nbrSeeds == 2 || nbrSeeds == 3) {
            board->holes[i].R = 0;
            board->holes[i].B = 0;
            player->seeds += nbrSeeds;
            board->total -= nbrSeeds;
            i--;
            if (i < 0) {
                i = 15;
            }
        } else {
            break;
        }
    } while (i != startingHole);
    printf("%sLe joueur %d a %d graines.%s\n", Colors[MAGENTA], player->even + 1, player->seeds, Colors[DEFAULT]);
}

List_i* generateNextHoles(int startingHole, int seeds, char color){
    List_i *holes = newList_i(seeds);
    int decalage = 1;
    if(color == 'B'){
        decalage = 2;
    }
    int index = startingHole+1;
    for (int i = 0; i < seeds; ++i) {
        if (index==16)index = 0;
        if(index==17)index = 1;
        holes->tab[i] = index;
        holes->length = holes->length + 1;
        index = index + decalage;
    }
    return holes;
}

void turn(Board *board, Player *player) {
    draw(*board);

    printf("%sEntrez votre coup:%s\n", Colors[GREEN], Colors[DEFAULT]);
    int startingHole;
    char color;
    readHole(&startingHole, &color);
    int seeds = getSeeds(board, player, startingHole, color);
    List_i *holes = generateNextHoles(startingHole,seeds,color);
    setSeeds(board, player, seeds, color, startingHole, holes);
    freeList_i(holes);
    capture(board, player, startingHole);
}



int main() {
    srand(time(NULL));
    Board board = newBoard;
    Player p1 = {0, false};
    Player p2 = {0, true};

    while (true) {
        printf("%s------------------------------Player 1------------------------------%s\n", Colors[YELLOW], Colors[DEFAULT]);
        draw(board);
        Move move = {board, p1, p2};
        Move copy = copyMove(move);
        //TEMPS
        clock_t t1, t2;
        float temps;
        t1 = clock();
        int profondeur = 6;
        copy = valeurMinMax(copy, true, 0, profondeur);
        t2 = clock();
        temps = (float)(t2-t1)/CLOCKS_PER_SEC;
        printf("\n----------------PROFONDEUR %i\ntemps = %fs\n-------------------\n",profondeur, temps);

        printMove(copy);
        int seeds = getSeeds(&board, &p1, copy.hole, copy.color);
        List_i *holes = generateNextHoles(copy.hole,seeds,copy.color);
        setSeeds(&board, &p1, seeds, copy.color, copy.hole, holes);
        capture(&board, &p1, copy.hole);

        printf("%s------------------------------Player 2------------------------------%s\n", Colors[YELLOW], Colors[DEFAULT]);

        turn(&board, &p2);
        draw(board);
    }
}
