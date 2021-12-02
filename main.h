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

bool turn(Player player);
void getWinner(Player player1, Player player2);
void readHole(int *holeNbr, char *color);
int getSeeds(Player player, int holeNbr, char color);
int *readLine(int *length);
void setSeeds(Player player, int nbrSeeds, char color, int startingHole, int *holes, int length);
void capture(Player player, int startingHole);