#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "List.h"
#include "main.h"

Board board;

int main() {
    /*
    srand(time(NULL));
    board = newBoard;
    Player player1 = newPlayer1;
    Player player2 = newPlayer2;
    while (true) {
        printf("%s------------------------------Player 1------------------------------%s\n", Colors[YELLOW], Colors[DEFAULT]);
        if (turn(&player1)) {
            getWinner(&player1, &player2);
            break;
        }
        printf("%s------------------------------Player 2------------------------------%s\n", Colors[YELLOW], Colors[DEFAULT]);
        if (turn(&player2)) {
            getWinner(&player1, &player2);
            break;
        }
    }*/
}

void getWinner(Player *player1, Player *player2) {
    if (player1->seeds > player2->seeds) {
        printf("%sLe joueur 1 a gagné.%s\n", Colors[CYAN], Colors[DEFAULT]);
    } else if (player1->seeds < player2->seeds) {
        printf("%sLe joueur 2 a gagné.%s\n", Colors[CYAN], Colors[DEFAULT]);
    } else {
        printf("%sEgalité.%s\n", Colors[CYAN], Colors[DEFAULT]);
    }
}

bool turn(Player *player) {
    draw();

    printf("%sEntrez votre coup:%s\n", Colors[GREEN], Colors[DEFAULT]);
    int startingHole;
    char color;
    // MANUEL
    // readHole(&startingHole, &color);
    // AUTO
    readHoleBOT(player, &startingHole, &color);

    int seeds = getSeeds(player, startingHole, color);

    printf("%sVous devez semer %d graines:%s\n", Colors[GREEN], seeds, Colors[DEFAULT]);
    // MANUEL
    // List_i *holes = readLine();
    // AUTO
    List_i *holes = readLineBOT(player, color, startingHole, seeds);

    setSeeds(player, seeds, color, startingHole, holes);
    freeList_i(holes);
    capture(player, startingHole);

    return player->seeds >= 33 || board.total < 8;
}

void forbiddenMove(char *move) {
    printf("%sCout interdit: %s%s%s\n", Colors[RED], Colors[YELLOW], move, Colors[DEFAULT]);
    exit(1);
}

void readHole(int *startingHole, char *color) {
    char move[3];
    scanf("%3s", move);
    *startingHole = atoi(move) - 1;
    *color = *startingHole < 10 ? move[1] : move[2];
}

void readHoleBOT(Player *player, int *startingHole, char *color) {
    List_s *possibilities = possiblesHolesToPick(player);
    char *move = possibilities->tab[rand() % possibilities->length];
    *startingHole = atoi(move);
    *color = *startingHole < 10 ? move[1] : move[2];
    printf("%d%c\n", *startingHole + 1, *color);
    freeList_s(possibilities);
}

int getSeeds(Player *player, int startingHole, char color) {
    if ((startingHole >= 0 && startingHole < 16) && (color == 'R' || color == 'B') &&
        ((startingHole & 1) == player->even)) {
        int seeds;
        if (color == 'R') {
            seeds = board.holes[startingHole].R;
            board.holes[startingHole].R = 0;
        } else {
            seeds = board.holes[startingHole].B;
            board.holes[startingHole].B = 0;
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

List_i *readLineBOT(Player *player, char color, int startingHole, int nbrSeeds) {
    List_ii *possibilities = possiblesHolesToSow(player, color, startingHole, nbrSeeds);
    int index = rand() % possibilities->length;
    List_i *combination = possibilities->tab[index];
    freeListBlackList_ii(possibilities, index);

    for (int i = 0; i < combination->length; i++) {
        printf("%d ", combination->tab[i] + 1);
    }
    printf("\n");

    return combination;
}

void setSeeds(Player *player, int nbrSeeds, char color, int startingHole, List_i *holes) {
    if (nbrSeeds == holes->length) {
        int lastHole = startingHole;
        for (int i = 0; i < holes->length; i++) {
            int n = holes->tab[i];
            if (n == startingHole || (lastHole > n && n > startingHole) || n < 0 || n > 15) {
                forbiddenMove("Combinaison interdite.\n");
            }
            lastHole = n;
            if (color == 'B' && player->even != n % 2) {
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

void capture(Player *player, int startingHole) {
    int i = startingHole;
    do {
        int nbrSeeds = board.holes[i].R + board.holes[i].B;
        if (nbrSeeds == 2 || nbrSeeds == 3) {
            board.holes[i].R = 0;
            board.holes[i].B = 0;
            player->seeds += nbrSeeds;
            board.total -= nbrSeeds;
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

List_s *possiblesHolesToPick(Player *player) {
    List_s *holes = newList_s(16);
    for (int i = player->even; i < 16; i += 2) {
        if (board.holes[i].B > 0) {
            holes->tab[holes->length] = moveToString(i, 'B');
            holes->length++;
        }
        if (board.holes[i].R > 0) {
            holes->tab[holes->length] = moveToString(i, 'R');
            holes->length++;
        }
    }
    if (holes->length == 0) {
        forbiddenMove("Le joueur est affamé");
    }
    return holes;
}

char *moveToString(int i, char color) {
    char *str = malloc(sizeof(char) * 4);
    if (i >= 10) {
        str[0] = '1';
        str[1] = i - 10 + '0';
        str[2] = color;
        str[3] = '\0';
    } else {
        str[0] = i + '0';
        str[1] = color;
        str[2] = '\0';
    }
    return str;
}

List_ii *possiblesHolesToSow(Player *player, char color, int startingHole, int sampleSize) {
    int nbrPossibilities = color == 'B' ? 8 : 16;
    List_i *holes = newList_i(nbrPossibilities);
    int val = 0;
    while (val < nbrPossibilities) {
        if (val != startingHole) {
            if (color == 'B') {
                holes->tab[holes->length] = val * 2 + !player->even;
            } else {
                holes->tab[holes->length] = val;
            }
            holes->length++;
        }
        val++;
    }

    // TODO generer les combinaisons
    List_ii *combination = newList_ii(1);

    List_i *smaller = newList_i(sampleSize);
    List_i *bigger = newList_i(sampleSize);

    while (smaller->length + bigger->length < sampleSize) {
        int value = holes->tab[rand() % holes->length];
        if (value < startingHole) {
            smaller->tab[smaller->length] = value;
            smaller->length++;
        } else {
            bigger->tab[bigger->length] = value;
            bigger->length++;
        }
    }
    countSort(smaller);
    countSort(bigger);
    List_i *comb = newList_i(smaller->length + bigger->length);
    for (int i = 0; i < bigger->length; i++) {
        comb->tab[comb->length] = bigger->tab[i];
        comb->length++;
    }
    for (int i = 0; i < smaller->length; i++) {
        comb->tab[comb->length] = smaller->tab[i];
        comb->length++;
    }
    freeList_i(smaller);
    freeList_i(bigger);

    combination->tab[combination->length] = comb;
    combination->length++;
    freeList_i(holes);
    return combination;
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

void draw() {
    for (int i = 0; i < 16; i++) {
        printf("%d(%s%d%s,%s%d%s) ", i + 1, Colors[RED], board.holes[i].R, Colors[DEFAULT], Colors[BLUE], board.holes[i].B, Colors[DEFAULT]);
        if (i == 7 || i == 15) {
            printf("\n");
        }
    }
}