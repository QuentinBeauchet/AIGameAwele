struct Hole_s {
    int B;
    int R;
};

typedef struct Hole_s Hole;

struct Board_s {
    Hole holes[16];
    int total;
};

typedef struct Board_s Board;

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

char *Colors[7] = {"\033[91m", "\033[92m", "\033[93m", "\033[94m", "\033[95m", "\033[96m", "\033[0m"};

struct Move_s {
    Board board;
    Player p1;
    Player p2;
    int hole;
    char color;
    int score;
};

typedef struct Move_s Move;

Move valeurMinMax(Move move, bool p1Turn, int profondeur, int profondeurMax);
Move createNode(Move *move, bool p1Turn, int nodeNbr, bool blue);
int initSow(int startingHole, char color, bool p1Turn, int nbrSeeds);

void readHole(int *startingHole, char *color);

int getSeeds(Board *board, Player *player, int startingHole, char color);
void setSeeds(Board *board, Player *player, int nbrSeeds, char color, int startingHole);
int capture(Board *board, Player *player, int lastSowedHole);

Board copyBoard(Board board);
Move copyMove(Move move);

void forbiddenMove(char *move);
void getWinner(Board board, Player p1, Player p2);

void printMove(Move move);
void printBoard(Board board);

int simulateMove(Move *move, bool p1Turn);
void playMove(Board *board, Player *p1, Player *p2, Move move, bool p1Turn);