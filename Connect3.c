#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>

#define ROWS 4
#define COLS 4
#define EMPTY '.'
#define HUMAN 'X'
#define COMPUTER 'O'
#define RESULT_NONE 0
#define RESULT_HUMAN_WIN 1
#define RESULT_COMPUTER_WIN 2
#define RESULT_DRAW 3

//TABLA

typedef struct {
    char cells[ROWS][COLS];
}Board;

void init_board(Board *b) {
    for (int i = 0; i < ROWS; i++) {for (int j = 0; j < COLS; j++) {b->cells[i][j] = EMPTY;}}
}

void print_board(const Board *b) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c ", b->cells[i][j]);
        }
        printf("\n");
    }
}

int is_valid_move(const Board *b, int col) {
    if (col < 0 || col >= COLS || b == NULL) return 0;
    if (b->cells[0][col] != EMPTY) return 0;
    return 1;
}

int play_move(Board *b, int col, char player) {
    if (b == NULL) return 0;
    if (!is_valid_move(b, col)) return 0;
    for (int i = ROWS-1; i >= 0; i--) {
        if (b->cells[i][col] == EMPTY) {
            b->cells[i][col] = player;
            return 1;
        }
    }
    return 0;
}

void copy_board(Board *dest, const Board *src) {
    if (dest == NULL || src == NULL) return;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            dest->cells[i][j] = src->cells[i][j];
        }
    }
}

int check_winner(const Board *b, char player) {
    if (b == NULL) return 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (b->cells[i][j] == player) {
                if (i <= 1) if (b->cells[i+1][j] == player && b->cells[i+2][j] == player) return 1;
                if (j <= 1) if (b->cells[i][j+1] == player && b->cells[i][j+2] == player) return 1;
                if (i <= 1 && j >= 2) if (b->cells[i+1][j-1] == player && b->cells[i+2][j-2] == player) return 1;
                if (i <= 1 && j <= 1) if (b->cells[i+1][j+1] == player && b->cells[i+2][j+2] == player) return 1;
            }
        }
    }
    return 0;
}

int is_full(const Board *b) {
    if (b == NULL) return 0;
    for (int i = 0; i < COLS; i++) {
        if (b->cells[0][i] == EMPTY) return 0;
    }
    return 1;
}

int game_result(const Board *b) {
    if (b == NULL) return RESULT_NONE;
    if (check_winner(b, HUMAN)) return RESULT_HUMAN_WIN;
    if (check_winner(b, COMPUTER)) return RESULT_COMPUTER_WIN;
    if (is_full(b)) return RESULT_DRAW;
    return RESULT_NONE;
}

int count_player_tokens(const Board *b, char player) {
    if (b == NULL) return 0;
    int p = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (b->cells[i][j] == player) p++;
        }
    }
    return p;
}

int is_gravity_valid(const Board *b) {
    if (b == NULL) return 0;
    for (int i = 0; i < ROWS-1; i++) {
        for (int j = 0; j < COLS; j++) {
            if (b->cells[i][j] != EMPTY && b->cells[i+1][j] == EMPTY) return 0;
        }
    }
    return 1;
}

int validate_board(const Board *b) {
    if (b == NULL) return 0;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (b->cells[i][j] != EMPTY && b->cells[i][j] != HUMAN && b->cells[i][j] != COMPUTER) return 0;
        }
    }
    if (!is_gravity_valid(b)) return 0;
    int diff = count_player_tokens(b, HUMAN) - count_player_tokens(b, COMPUTER);
    if (diff > 1 || diff < 0) return 0;
    return 1;
}

char determine_next_player(const Board *b) {
    if (b == NULL) return '\0';
    if (!validate_board(b)) return '\0';
    int X = count_player_tokens(b, HUMAN);
    int O = count_player_tokens(b, COMPUTER);
    if (X == O) return HUMAN;
    if (X == O + 1) return COMPUTER;
    return '\0';
}

int load_board_from_stdin(Board *b) {
    if (b == NULL) return 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (scanf(" %c", &b->cells[i][j]) != 1) return 0;
        }
    }
    if (!validate_board(b)) return 0;
    return 1;
}

int load_board_from_file(Board *b, const char *filename) {
    if (b == NULL || filename == NULL) return 0;
    FILE *f = fopen(filename, "r");
    if (f == NULL) return 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (fscanf(f, " %c", &b->cells[i][j]) != 1) {
                fclose(f);
                return 0;
            }
        }
    }
    fclose(f);
    if (!validate_board(b)) return 0;
    return 1;
}

//STABLO ODLUCIVANJA

typedef struct TreeNode {
    Board board;
    char player_to_move;
    int move_column;
    int value;
    int level;
    int child_count;
    struct TreeNode *children[COLS];
}TreeNode;

TreeNode *create_node(const Board *b, char player_to_move, int move_column, int level) {
    if (b == NULL) return NULL;
    TreeNode *node = malloc(sizeof(TreeNode));
    if (node == NULL) return NULL;
    node->board = *b;
    node->player_to_move = player_to_move;
    node->move_column = move_column;
    node->level = level;
    node->child_count = 0;
    node->value = 0;

    for (int i = 0; i < COLS; i++) {node->children[i] = NULL;}
    return node;
}

char other_player(char player) {
    return player == HUMAN ? COMPUTER : player == COMPUTER ? HUMAN : '\0';
}

//RED POMOCNI

typedef struct QueueNode {
    TreeNode *tree_node;
    struct QueueNode *next;
}QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
}Queue;

void init_queue(Queue *q) {
    if (q == NULL) return;
    q->front = NULL;
    q->rear = NULL;
}

int is_queue_empty(const Queue *q) {
    if (q == NULL) return 1;
    return q->front == NULL;
}

int enqueue(Queue *q, TreeNode *node) {
    if (q == NULL || node == NULL) return 0;
    QueueNode *new_node = malloc(sizeof(QueueNode));
    if (new_node == NULL) return 0;
    new_node->next = NULL;
    new_node->tree_node = node;
    if (q->front == NULL) q->front = q->rear = new_node;
    else {q->rear->next = new_node; q->rear = new_node;}
    return 1;
}

TreeNode *dequeue(Queue *q) {
    if (q == NULL) return NULL;
    if (q->front == NULL) return NULL;
    TreeNode *result = q->front->tree_node;
    QueueNode *temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    return result;
}

void clear_queue(Queue *q) {
    if (q == NULL) return;
    QueueNode *p = q->front;
    QueueNode *tmp;

    while (p != NULL) {
        tmp = p;
        p = p->next;
        free(tmp);
    }

    q->front = NULL;
    q->rear = NULL;
}

//DINAMICKI NIZ

typedef struct {
    TreeNode **nodes;
    int size;
    int capacity;
}NodeArray;

void init_node_array(NodeArray *arr) {
    if (arr == NULL) return;
    arr->capacity = 64;
    arr->size = 0;
    arr->nodes = malloc(arr->capacity * sizeof(TreeNode*));
    if (arr->nodes == NULL) arr->capacity = 0;
}

int add_node(NodeArray *arr, TreeNode *node) {
    if (arr == NULL || node == NULL) return 0;
    if (arr->nodes == NULL) return 0;
    if (arr->size == arr->capacity) {
        arr->capacity = arr->capacity * 2;
        TreeNode** new = realloc(arr->nodes, arr->capacity * sizeof(TreeNode*));
        if (new == NULL) return 0;
        arr->nodes = new;
    }

    arr->nodes[arr->size] = node;
    arr->size++;
    return 1;
}

void free_node_array(NodeArray *arr) {
    if (arr == NULL) return;
    free(arr->nodes);
    arr->nodes = NULL;
    arr->capacity = 0;
    arr->size = 0;
}

//STEK

typedef struct StackNode {
    TreeNode *tree_node;
    struct StackNode *next;
}StackNode;

typedef struct{
    StackNode *top;
}Stack;

void init_stack(Stack *s) {
    if (s == NULL) return;
    s->top = NULL;
}

int is_stack_empty(const Stack *s) {
    if (s == NULL) return 1;
    return s->top == NULL;
}

int push(Stack *s, TreeNode *node) {
    if (s == NULL || node == NULL) return 0;
    StackNode *new_node = malloc(sizeof(StackNode));
    if (new_node == NULL) return 0;
    new_node->tree_node = node;
    new_node->next = s->top;
    s->top = new_node;
    return 1;
}

TreeNode *pop(Stack *s) {
    if (s == NULL || is_stack_empty(s)) return NULL;
    StackNode *result = s->top;
    TreeNode *node = result->tree_node;
    s->top = s->top->next;
    free(result);
    return node;
}

void clear_stack(Stack *s) {
    if (s == NULL) return;
    StackNode *p = s->top;
    while (p != NULL) {
        StackNode *tmp = p;
        p = p->next;
        free(tmp);
    }
    s->top = NULL;
}

//HEURISTIKA

int heuristic_potential(const Board *b) {
    if (b == NULL) return 0;
    int Fc, Fp, c, p;
    Fc = Fp = 0;
    for (int i = 0; i < ROWS; i++) {
        c = p = 0;
        for (int j = 0; j < COLS; j++) {
            if (b->cells[i][j] == COMPUTER) c++;
            if (b->cells[i][j] == HUMAN) p++;
        }
        if (c >= 1 && p == 0) Fc++;
        if (p >= 1 && c == 0) Fp++;
    }
    for (int i = 0; i < COLS; i++) {
        c = p = 0;
        for (int j = 0; j < ROWS; j++) {
            if (b->cells[j][i] == COMPUTER) c++;
            if (b->cells[j][i] == HUMAN) p++;
        }
        if (c >= 1 && p == 0) Fc++;
        if (p >= 1 && c == 0) Fp++;
    }

    return 7*(Fc-Fp);
}

int evaluate_node(const TreeNode *node) {
    if (node == NULL) return 0;
    switch(game_result(&node->board)) {
        case RESULT_COMPUTER_WIN:
            return 1000;
        case RESULT_HUMAN_WIN:
            return -1000;
        case RESULT_DRAW:
            return 0;
        default:
            return heuristic_potential(&node->board);
    }
}

TreeNode *build_decision_tree(const Board *start_board, char start_player, int h, NodeArray *arr) {
    if (arr == NULL || start_board == NULL || h < 0) return NULL;
    Queue q;
    init_queue(&q);
    TreeNode *root = create_node(start_board, start_player, -1, 0);
    if (root == NULL) return NULL;
    if (!enqueue(&q, root)) return NULL;
    if (!add_node(arr, root)) return NULL;
    while (!is_queue_empty(&q)) {
        TreeNode *node = dequeue(&q);

        if (node->level == h || game_result(&node->board) != RESULT_NONE) continue;

        for (int i = 0; i < COLS; i++) {
            if (!is_valid_move(&node->board, i)) continue;
            Board new_board;
            copy_board(&new_board, &node->board);
            play_move(&new_board, i, node->player_to_move);
            TreeNode *new_node = create_node(&new_board, other_player(node->player_to_move), i, node->level + 1);
            if (new_node == NULL) return NULL;
            if (!enqueue(&q, new_node)) return NULL;
            if (!add_node(arr, new_node)) return NULL;
            node->children[node->child_count] = new_node;
            node->child_count++;
        }
    }

    clear_queue(&q);
    return root;
}

void minimax(NodeArray *arr) {
    if (arr == NULL) return;
    int opt;

    for (int i = arr->size - 1; i >= 0; i--) {
        TreeNode *node = arr->nodes[i];
        if (node == NULL) continue;
        if (node->child_count == 0) {
            node->value = evaluate_node(node);
        }else {
            if (node->player_to_move == COMPUTER) {
                opt = INT_MIN;
                for (int j = 0; j < node->child_count; j++) {
                    if (node->children[j]->value > opt) {
                        opt = node->children[j]->value;
                    }
                }
                node->value = opt;
            }else {
                opt = INT_MAX;
                for (int j = 0; j < node->child_count; j++) {
                    if (node->children[j]->value < opt) {
                        opt = node->children[j]->value;
                    }
                }
                node->value = opt;
            }
        }
    }
}

TreeNode *choose_best_child(TreeNode *node) {
    if (node == NULL) return NULL;
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i] != NULL && node->value == node->children[i]->value) {
            return node->children[i];
        }
    }
    return NULL;
}

//PRINTAJ STABLO

void print_node(const TreeNode *node) {
    if (node == NULL) return;
    printf("Level: %d | Move: %d | Player to move: %c | Value: %d\n", node->level, node->move_column, node->player_to_move, node->value);
    print_board(&node->board);
    printf("\n");
}

void print_tree_preorder(TreeNode *root, int total_nodes) {
    if (root == NULL) return;
    Stack S;
    init_stack(&S);
    push(&S, root);
    while (!is_stack_empty(&S)) {
        TreeNode *node = pop(&S);
        print_node(node);
        for (int i = node->child_count - 1; i >= 0; i--) {
            if (node->children[i] != NULL) {
                push(&S, node->children[i]);
            }
        }
    }
    printf("Ukupan broj cvorova: %d\n", total_nodes);
}

void free_tree(TreeNode *root) {
    if (root == NULL) return;
    Stack S;
    init_stack(&S);
    push(&S, root);
    while (!is_stack_empty(&S)) {
        TreeNode *node = pop(&S);
        for (int i = node->child_count - 1; i >= 0; i--) {
            if (node->children[i] != NULL) {
                push(&S, node->children[i]);
            }
        }
        free(node);
    }
}

int random_valid_column(const Board *b) {
    if (b == NULL) return -1;
    int valid[COLS];
    int count = 0;

    for (int i = 0; i < COLS; i++) {
        if (is_valid_move(b, i)) {
            valid[count++] = i;
        }
    }

    if (count == 0) return -1;
    return valid[rand() % count];
}

TreeNode *find_child_by_move(TreeNode *node, int col) {
    if (node == NULL) return NULL;
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i] != NULL && node->children[i]->move_column == col) {
            return node->children[i];
        }
    }
    return NULL;
}

//SIMULACIJA IGRE

void simulate_game(const Board *start_board, TreeNode *root) {
    if (root == NULL || start_board == NULL) return;
    Board board;
    copy_board(&board, start_board);
    char player = determine_next_player(&board);
    TreeNode *current_node = root;
    int c;

    while (game_result(&board) == RESULT_NONE) {
        print_board(&board);
        if (player == HUMAN) {
            printf("Unesi kolonu: ");
            while (scanf("%d", &c) != 1 || c < 0 || c >= COLS || !is_valid_move(&board, c)) {
                printf("Nepravilan ulaz\nUnesi kolonu: ");
                int a;
                while ((a = getchar()) != '\n' && a != EOF);
            }
            TreeNode *tmp = find_child_by_move(current_node, c);
            if (tmp == NULL) {
                play_move(&board, c, player);
                current_node = NULL;
            }else {
                current_node = tmp;
                copy_board(&board, &current_node->board);
            }
        }else if (player == COMPUTER) {
            TreeNode *tmp = choose_best_child(current_node);
            if (tmp == NULL) {
                c = random_valid_column(&board);
                if (c == -1) break;
                play_move(&board, c, player);
                current_node = NULL;
            }else {
                current_node = tmp;
                copy_board(&board, &current_node->board);
                c = current_node->move_column;
            }
            printf("Kompjuter igra kolonu: %d\n", c);
        }else {
            printf("Neispravna tabla\n");
            return;
        }
        player = other_player(player);
    }

    print_board(&board);
    int result = game_result(&board);
    if (result == RESULT_HUMAN_WIN) {
        printf("Covek je pobedio!\n");
    }else if (result == RESULT_COMPUTER_WIN) {
        printf("Kompjuter je pobedio!\n");
    }else if (result == RESULT_DRAW) {
        printf("Nereseno!\n");
    }else {
        printf("Neispravna tabla\n");
    }
}

void print_menu() {
    printf("1. Nova prazna tabla\n2. Ucitaj tablu sa standardnog ulaza\n3. Ucitaj tablu iz fajla\n4. Ispisi trenutnu tablu\n5. Formiraj stablo odlucivanja\n"
           "6. Pokreni minimax\n7. Ispisi stablo preorder\n8. Simuliraj igru\n9. Obrisi stablo\n0. Kraj\nUnesi izbor: ");
}

int main() {
    srand(time(NULL));
    int choice;
    Board current_board;
    TreeNode *root = NULL;
    NodeArray arr;
    char current_player;
    int board_ready = 0;
    int tree_built = 0;
    int minimax_done = 0;
    init_board(&current_board);
    init_node_array(&arr);
    int h;
    char filename[256];

    while (1) {
        print_menu();
        while (scanf("%d", &choice) != 1 || choice < 0 || choice > 9) {
            printf("Nepravilan ulaz\nUnesi izbor: ");
            int a;
            while ((a = getchar()) != '\n' && a != EOF);
        }

        switch (choice) {
            case 1:
                free_tree(root);
                root = NULL;
                tree_built = 0;
                minimax_done = 0;
                board_ready = 1;
                free_node_array(&arr);
                init_node_array(&arr);
                init_board(&current_board);
                break;
            case 2:
                free_tree(root);
                root = NULL;
                tree_built = 0;
                minimax_done = 0;
                board_ready = 1;
                free_node_array(&arr);
                init_node_array(&arr);
                init_board(&current_board);
                if (!load_board_from_stdin(&current_board)) {
                    printf("Nepravilna tabla!\n");
                    init_board(&current_board);
                    board_ready = 0;
                }
                break;
            case 3:
                free_tree(root);
                root = NULL;
                tree_built = 0;
                minimax_done = 0;
                board_ready = 1;
                free_node_array(&arr);
                init_node_array(&arr);
                init_board(&current_board);
                printf("Unesi ime fajla: ");
                scanf("%255s", filename);
                if (!load_board_from_file(&current_board, filename)) {
                    printf("Nepravilna tabla!\n");
                    init_board(&current_board);
                    board_ready = 0;
                }
                break;
            case 4:
                if (board_ready){
                    print_board(&current_board);
                }else {
                    printf("Nije napravljena tabla!\n");
                }
                break;
            case 5:
                if (board_ready) {
                    printf("Unesi visinu h: ");
                    if (scanf("%d", &h) != 1 || h < 0) {
                        printf("Nepravilan ulaz!\n");
                        break;
                    }
                    current_player = determine_next_player(&current_board);
                    if (current_player == '\0') {printf("Ne moze se odrediti igrac na potezu!"); break;}
                    free_tree(root);
                    root = NULL;
                    free_node_array(&arr);
                    init_node_array(&arr);
                    tree_built = 0;
                    minimax_done = 0;
                    root = build_decision_tree(&current_board, current_player, h, &arr);
                    if (root != NULL) {
                        tree_built = 1;
                        minimax_done = 0;
                    } else {
                        printf("Greska pri formiranju stabla!\n");
                    }
                }else {
                    printf("Nije napravljena tabla!\n");
                }
                break;
            case 6:
                if (board_ready && tree_built) {
                    minimax(&arr);
                    minimax_done = 1;
                }else {
                    printf("Nisu napravljeni tabla ili stablo!\n");
                }
                break;
            case 7:
                if (tree_built) print_tree_preorder(root, arr.size);
                else printf("Nije napravljeno stablo!\n");
                break;
            case 8:
                if (board_ready && tree_built && minimax_done) {
                    simulate_game(&current_board, root);
                }else {
                    printf("Nije napravljena tabla ili stablo ili minimaks!\n");
                }
                break;
            case 9:
                free_tree(root);
                free_node_array(&arr);
                init_node_array(&arr);
                root = NULL;
                tree_built = 0;
                minimax_done = 0;
                break;
            case 0:
                free_tree(root);
                free_node_array(&arr);
                return 0;
        }
    }
    return 0;
}