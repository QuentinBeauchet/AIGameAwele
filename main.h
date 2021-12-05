struct Hole_s {
    int B;
    int R;
} newHole = {2, 2};

typedef struct Hole_s Hole;

struct Board_s {
    Hole holes[16];
    int index;
    int total;
} newBoard = {{{2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}}, 1, 64};

typedef struct Board_s Board;

struct Player_s {
    int seeds;
    bool even;
} newPlayer1 = {0, false}, newPlayer2 = {0, true};

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

char *Colors[7] = {"\033[91m", "\033[92m", "\033[93m", "\033[94m", "\033[95m", "\033[96m", "\033[0m"};

void draw();
void forbiddenMove(char *move);

bool turn(Player *player);
void getWinner(Player *player1, Player *player2);

void readHole(int *startingHole, char *color);
void readHoleBOT(Player *player, int *startingHole, char *color);

int getSeeds(Player *player, int startingHole, char color);

List_i *readLine();
List_i *readLineBOT(Player *player, char color, int startingHole, int nbrSeeds);

void setSeeds(Player *player, int nbrSeeds, char color, int startingHole, List_i *holes);
void capture(Player *player, int startingHole);

List_s *possiblesHolesToPick(Player *player);
List_ii *possiblesHolesToSow(Player *player, char color, int startingHole, int sampleSize);

char *moveToString(int i, char color);

void countSort(List_i *list);