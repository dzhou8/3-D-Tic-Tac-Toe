/*
ID: dzhouwave
PROG: 3DTicTacToe
LANG: C++11
*/

// @BEGIN_OF_SOURCE_CODE

#include <chrono>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <bits/stdc++.h>

#define INT_MAX 2147483647
#define INT_MIN -2147483647
#define pi acos(-1.0)
//#define N 1000000
#define LL unsigned long long

#define maxTrials 500000
#define exploration_constant 2
using namespace std;

//#define cin fin
//ifstream fin ("name.in");
//#define cout fout

tuple<int, int, int> winningCombinations[76][4];

void SetColor(int value)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),  value);
}

void initializeWinningCombinations()
{
    for(int x = 0; x < 4; x++) //FIRST 40
    {
        for(int y = 0; y < 4; y++)
        {
            for(int z = 0; z < 4; z++)
            {
                winningCombinations[x*10+y][z] = make_tuple(x, y, z);
            }
        }
        for(int z = 0; z < 4; z++)
        {
            for(int y = 0; y < 4; y++)
            {
                winningCombinations[x*10+4+z][y] = make_tuple(x, y, z);
            }
        }
        for(int diagindex = 0; diagindex < 4; diagindex++)
        {
            winningCombinations[x*10+8][diagindex] = make_tuple(x, diagindex, diagindex);
            winningCombinations[x*10+9][diagindex] = make_tuple(x, diagindex, 3-diagindex);
        }
    }
    for(int y = 0; y < 4; y++) //16 vertical lines
    {
        for(int z = 0; z < 4; z++)
        {
            for(int x = 0; x < 4; x++)
            {
                winningCombinations[40+4*y+z][x] = make_tuple(x, y, z);
            }
        }
    }
    for(int y = 0; y < 4; y++) //diag on plane y
    {
        for(int diagindex = 0; diagindex < 4; diagindex++)
        {
            winningCombinations[56+2*y+0][diagindex] = make_tuple(diagindex, y, diagindex);
            winningCombinations[56+2*y+1][diagindex] = make_tuple(diagindex, y, 3-diagindex);
        }
    }
    for(int z = 0; z < 4; z++) //diag on plane z
    {
        for(int diagindex = 0; diagindex < 4; diagindex++)
        {
            winningCombinations[64+2*z+0][diagindex] = make_tuple(diagindex, diagindex, z);
            winningCombinations[64+2*z+1][diagindex] = make_tuple(diagindex, 3-diagindex, z);
        }
    }
    for(int diagindex = 0; diagindex < 4; diagindex++)
    {
        winningCombinations[72][diagindex] = make_tuple(diagindex, diagindex, diagindex);
        winningCombinations[73][diagindex] = make_tuple(3-diagindex, diagindex, diagindex);
        winningCombinations[74][diagindex] = make_tuple(diagindex, 3-diagindex, diagindex);
        winningCombinations[75][diagindex] = make_tuple(diagindex, diagindex, 3-diagindex);
    }
}

struct Board
{
    vector<tuple<int, int, int> > moves;
    vector<tuple<int, int, int> > possibleMoves;
    tuple<int, int, int> currentMove;
    tuple<int, int, int> lastMove;
    int winningLines[76];
    char board[4][4][4];
    int Result;
    /* All 76 lines to win
    FIRST 40:
    x*10 + 0-3 = horizonal on plane x
    x*10 + 4-7 = vertical on plane x
    x*10 + 8 = diag on plane X
    x*10 + 9 = antidiag on plane X

    NEXT 16:
    40-55 = 16 vertical lines
    4y+z + 40

    NEXT 16:
    8 vertical planes, 2 diags each
    56-63:
    on same y plane
    odd is diag
    even is antidiag
    64-71
    on same z plane
    odd is diag
    even is antidiag

    LAST 4:
    72 is x==y==z
    73 is x+y==3 and y==z
    74 is x+y==3 and x==z
    75 is x+z==3 and x==y
    */

    Board()
    {
        for(int i = 0; i < 76; i++)
        {
            winningLines[i] = 0;
        }
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                for(int k = 0; k < 4; k++)
                {
                    board[i][j][k] = ' ';
                    possibleMoves.push_back(make_tuple(i, j, k));
                }
            }
        }
        Result = 0;
    }

    void add(tuple<int, int, int> input)
    {
        lastMove = input;
        moves.push_back(input);
        possibleMoves.erase(remove(possibleMoves.begin(), possibleMoves.end(), input), possibleMoves.end());
        int lastX = get<0>(input);
        int lastY = get<1>(input);
        int lastZ = get<2>(input);
        if(moves.size() % 2 == 0) //2nd player just went
        {
            board[lastX][lastY][lastZ] = 'O';
            winningLines[lastX*10+lastY]--; //horizonal plane x
            winningLines[lastX*10+4+lastZ]--; //vertical plane x
            if(lastY==lastZ)
            {
                winningLines[lastX*10+8]--; //diag plane x
            }
            else if(lastY+lastZ == 3)
            {
                winningLines[lastX*10+9]--; //antidiag plane x
            }
            winningLines[40+lastY*4+lastZ]--; //vertical line downwards
            if(lastX==lastZ)
            {
                winningLines[56+lastY*2+0]--; //diag plane y
            }
            else if(lastX+lastZ == 3) //antidiang plane y
            {
                winningLines[56+lastY*2+1]--;
            }
            if(lastX==lastY) //diag plane z
            {
                winningLines[64+lastZ*2+0]--;
            }
            else if(lastX+lastY==3) //antidiag plane z
            {
                winningLines[64+lastZ*2+1]--;
            }
            //special cases: space diagonals
            if(lastX==lastY && lastX==lastZ)
            {
                winningLines[72]--;
            }
            else if(lastX+lastY==3 && lastY==lastZ)
            {
                winningLines[73]--;
            }
            else if(lastX+lastY==3 && lastX==lastZ)
            {
                winningLines[74]--;
            }
            else if(lastX+lastZ==3 && lastX==lastY)
            {
                winningLines[75]--;
            }
        }
        else //1st player just went
        {
            board[lastX][lastY][lastZ] = 'X';
            winningLines[lastX*10+lastY]++; //horizonal plane x
            winningLines[lastX*10+4+lastZ]++; //vertical plane x
            if(lastY==lastZ)
            {
                winningLines[lastX*10+8]++; //diag plane x
            }
            else if(lastY+lastZ == 3)
            {
                winningLines[lastX*10+9]++; //antidiag plane x
            }
            winningLines[40+lastY*4+lastZ]++; //vertical line downwards
            if(lastX==lastZ)
            {
                winningLines[56+lastY*2+0]++; //diag plane y
            }
            else if(lastX+lastZ == 3) //antidiang plane y
            {
                winningLines[56+lastY*2+1]++;
            }
            if(lastX==lastY) //diag plane z
            {
                winningLines[64+lastZ*2+0]++;
            }
            else if(lastX+lastY==3) //antidiag plane z
            {
                winningLines[64+lastZ*2+1]++;
            }
            //special cases: space diagonals
            if(lastX==lastY && lastX==lastZ)
            {
                winningLines[72]++;
            }
            else if(lastX+lastY==3 && lastY==lastZ)
            {
                winningLines[73]++;
            }
            else if(lastX+lastY==3 && lastX==lastZ)
            {
                winningLines[74]++;
            }
            else if(lastX+lastZ==3 && lastX==lastY)
            {
                winningLines[75]++;
            }
        }
        Result = getResult();
    }

    int getResult()
    {
        for(int i = 0; i < 76; i++)
        {
            if(winningLines[i] == 4)
            {
                return 1;
            }
            else if(winningLines[i] == -4)
            {
                return -1;
            }
        }
        return 0;
    }

    void Print()
    {
        system("cls");
        cout << "Press a/d to move between boards." << endl;
        cout << "Use the arrow buttons to navigate within a board." << endl;
        cout << "Press space to make a move." << endl;
        cout << "Press tab if you get stuck" << endl;
        cout << "Press h for heuristic" << endl;
        cout << "Press p to print" << endl;
//        for(int i = 0; i < moves.size(); i++)
//        {
//            cout << get<0>(moves[i]) << " " << get<1>(moves[i]) << " " << get<2>(moves[i]) << endl;
//        }
        for(int height = 3; height >= 0; height--)
        {
            for(int box = 0; box < 4; box++)
            {
                for(int length = 0; length < 4; length++)
                {
                    if(board[box][length][height] == 'X')
                    {
                        SetColor(2); //Green
                        cout << "XX";
                    }
                    else if(board[box][length][height] == 'O')
                    {
                        SetColor(4); //Red
                        cout << "OO";
                    }
                    else if(currentMove == make_tuple(box, length, height))
                    {
                        SetColor(15); //White
                        cout << "??";
                    }
                    else
                    {
                        cout << "  ";
                    }
                    SetColor(7); //Default
                    if(length < 3)
                    {
                        cout << "|";
                    }
                }
                cout << "     ";
            }
            cout << endl;
            if(height > 0)
            {
                cout << "-----------     -----------     -----------     -----------" << endl;
            }
        }
        if(moves.size() > 0)
        cout << "Last Move: " << get<0>(lastMove) << " " << get<1>(lastMove) << " " << get<2>(lastMove) << endl;
//        for(int i = 0; i < 76; i++)
//        {
//            if(winningLines[i] == 3)
//            {
//                //Make a child for last one
//                for(int j = 0; j < 4; j++)
//                {
//                    if(board[get<0>(winningCombinations[i][j])][get<1>(winningCombinations[i][j])][get<2>(winningCombinations[i][j])] == ' ')
//                    {
//                        cout << "PLAYER 1 THREAT: " << get<0>(winningCombinations[i][j]) << " " << get<1>(winningCombinations[i][j]) << " " << get<2>(winningCombinations[i][j]) << endl;
//                    }
//                }
//            }
//        }
//        for(int i = 0; i < 76; i++)
//        {
//            if(winningLines[i] == -3)
//            {
//                //Make a child for last one
//                for(int j = 0; j < 4; j++)
//                {
//                    if(board[get<0>(winningCombinations[i][j])][get<1>(winningCombinations[i][j])][get<2>(winningCombinations[i][j])] == ' ')
//                    {
//                        cout << "PLAYER 2 THREAT: " << get<0>(winningCombinations[i][j]) << " " << get<1>(winningCombinations[i][j]) << " " << get<2>(winningCombinations[i][j]) << endl;
//                    }
//                }
//            }
//        }
    }

    void PrintOut()
    {
        ofstream fout ("GameRecord.txt");
        for(int i = 0; i < moves.size(); i++)
        {
            fout << get<0>(moves[i]) << " " << get<1>(moves[i]) << " " << get<2>(moves[i]) << endl;
        }
        fout.close();
    }

    bool notFinished()
    {
        if(Result != 0)
        {
            return false;
        }
        else if(moves.size() == 64)
        {
            return false;
        }
        return true;
    }

    void Tab()
    {
        int myMoveIndex = get<0>(currentMove) * 16 + get<1>(currentMove) * 4 + get<2>(currentMove);
        myMoveIndex++;
        if(myMoveIndex == 64)
        {
            myMoveIndex = 0;
        }
        get<0>(currentMove) = myMoveIndex/16;
        get<1>(currentMove) = (myMoveIndex % 16) / 4;
        get<2>(currentMove) = (myMoveIndex % 4);
        if(board[get<0>(currentMove)][get<1>(currentMove)][get<2>(currentMove)] != ' ') //notEmpty
        {
            Tab();
        }
    }

    void LeftBoard()
    {
        get<0>(currentMove)--;
        if(get<0>(currentMove) == -1)
        {
            get<0>(currentMove) = 3;
        }
        if(board[get<0>(currentMove)][get<1>(currentMove)][get<2>(currentMove)] != ' ') //notEmpty
        {
            Tab();
        }
    }

    void RightBoard()
    {
        get<0>(currentMove)++;
        if(get<0>(currentMove) == 4)
        {
            get<0>(currentMove) = 0;
        }
        if(board[get<0>(currentMove)][get<1>(currentMove)][get<2>(currentMove)] != ' ') //notEmpty
        {
            Tab();
        }
    }

    void Left()
    {
        get<1>(currentMove)--;
        if(get<1>(currentMove) == -1)
        {
            get<1>(currentMove) = 3;
        }
        if(board[get<0>(currentMove)][get<1>(currentMove)][get<2>(currentMove)] != ' ') //notEmpty
        {
            Left();
        }
    }

    void Up()
    {
        get<2>(currentMove)++;
        if(get<2>(currentMove) == 4)
        {
            get<2>(currentMove) = 0;
        }
        if(board[get<0>(currentMove)][get<1>(currentMove)][get<2>(currentMove)] != ' ') //notEmpty
        {
            Up();
        }
    }

    void Right()
    {
        get<1>(currentMove)++;
        if(get<1>(currentMove) == 4)
        {
            get<1>(currentMove) = 0;
        }
        if(board[get<0>(currentMove)][get<1>(currentMove)][get<2>(currentMove)] != ' ') //notEmpty
        {
            Right();
        }
    }

    void Down()
    {
        get<2>(currentMove)--;
        if(get<2>(currentMove) == -1)
        {
            get<2>(currentMove) = 3;
        }
        if(board[get<0>(currentMove)][get<1>(currentMove)][get<2>(currentMove)] != ' ') //notEmpty
        {
            Down();
        }
    }
};

struct Node
{
    Node * parent;
    vector<Node*> Children;
    double successes;
    int total;
    tuple<int, int, int> moveHere;
    bool madeChildren;
    int player; //1 is first, -1 is second
    bool forced;
    Node* lastGoodReply;
    bool justWon;

    void init(int playerInput) //ROOT NODE
    {
        parent = nullptr;
        successes = 0.0;
        total = 0;
        madeChildren = false;
        player = playerInput;
        forced = false;
        justWon = false;
    }

    void init(Node * parentNode, tuple<int, int, int> moveInput)
    {
        parent = parentNode;
        successes = 0.0;
        total = 0;
        moveHere = moveInput;
        madeChildren = false;
        player = -1*parentNode->player;
        forced = false;
        justWon = false;
    }

    void free_space()
    {
        if(madeChildren)
        {
            for(int i = 0; i < Children.size(); i++)
            {
                Children[i]->free_space();
            }
        }
        delete this;
    }

    void makeChildren(Board input)
    {
        madeChildren = true;
        for(int i = 0; i < 76; i++)
        {
            if(input.winningLines[i] == player*3)
            {
                //Make a child for last one
                for(int j = 0; j < 4; j++)
                {
                    if(input.board[get<0>(winningCombinations[i][j])][get<1>(winningCombinations[i][j])][get<2>(winningCombinations[i][j])] == ' ')
                    {
//                        if(parent != nullptr)
//                        {
//                            parent->Forced.push_back(this);
//                        }
                        Node* tmpNode = new Node();
                        tmpNode->init(this, winningCombinations[i][j]);
                        Children.push_back(tmpNode);
                        return;
                    }
                }
            }
        }
        for(int i = 0; i < 76; i++)
        {
            if(input.winningLines[i] == player*-3)
            {
                //Make a child for last one
                for(int j = 0; j < 4; j++)
                {
                    if(input.board[get<0>(winningCombinations[i][j])][get<1>(winningCombinations[i][j])][get<2>(winningCombinations[i][j])] == ' ')
                    {
                        if(parent != nullptr)
                        {
                            parent->forced = true;
                        }
                        Node* tmpNode = new Node();
                        tmpNode->init(this, winningCombinations[i][j]);
                        Children.push_back(tmpNode);
                        return;
                    }
                }
            }
        }
//        cout << "THIS: " << this << endl;
        for(int i = 0; i < input.possibleMoves.size(); i++)
        {
            Node* tmpNode = new Node();
            tmpNode->init(this, input.possibleMoves[i]);
            Children.push_back(tmpNode);
//            cout << "PARENT OF CHILD: " << Children[i]->parent << endl;
//            cout << "CHILD: " << &Children[i] << endl;
        }
    }

    double getUTC(int prevTotal, double constant, int movesMade)
    {
        if(total == 0)
        {
            return 100;//5 + (max(get<0>(moveHere), 3-get<0>(moveHere)) == max(get<1>(moveHere), 3-get<1>(moveHere)) && max(get<1>(moveHere), 3-get<1>(moveHere)) == max(get<2>(moveHere), 3-get<1>(moveHere)));
        }
        double underSquare = constant*(log(prevTotal))/total;
//        int x = max(get<0>(moveHere), 3-get<0>(moveHere));
//        int y = max(get<1>(moveHere), 3-get<1>(moveHere));
//        int z = max(get<2>(moveHere), 3-get<1>(moveHere));
//        if(max(get<0>(moveHere), 3-get<0>(moveHere)) == max(get<1>(moveHere), 3-get<1>(moveHere)) && max(get<1>(moveHere), 3-get<1>(moveHere)) == max(get<2>(moveHere), 3-get<1>(moveHere)))
//        {
//            underSquare *= max((double)16/(movesMade+1), 1.0);
//        }
        double result = (double) (successes/total) + sqrt(underSquare);
        if(forced)
        {
            result += 0.5/total;
        }
        return result;
    }

    Node* Select(int movesMade, double constant)
    {
        if(justWon)
        {
            return lastGoodReply;
        }
        double bestUTC = -1;
        int bestIndex = -1;
        for(int i = 0; i < Children.size(); i++)
        {
//            cout << "CHILD " << i << " UTC: " << Children[i]->getUTC(total) << endl;
            if(Children[i]->getUTC(total, constant, movesMade) > bestUTC)
            {
                bestUTC = Children[i]->getUTC(total, constant, movesMade);
                bestIndex = i;
            }
        }
//        cout << "BEST CHILD: " << &Children[bestIndex] << endl;
        return Children[bestIndex];
    }

    void Win(int trials)
    {
        successes += trials;
        total += trials;
        if(parent != nullptr)
        {
            parent->justWon = true;
            parent->lastGoodReply = this;
            parent->Lose(trials);
        }
    }

    void Draw(int trials)
    {
        successes += trials/2.0;
        total += trials;
        if(parent != nullptr)
        {
            parent->justWon = false;
            parent->Draw(trials);
        }
    }

    void Lose(int trials)
    {
        total += trials;
        if(parent != nullptr)
        {
            parent->justWon = false;
            parent->Win(trials);
        }
    }

    Node* getRoot()
    {
        if(parent == nullptr)
        {
            return this;
        }
        return parent->getRoot();
    }

    tuple<int, int, int> getMove()
    {
        int maxTotal = -1;
        tuple<int, int, int> bestMove(make_tuple(-1, -1, -1));
        for(int i = 0; i < Children.size(); i++)
        {
//            cout << "Children " << i << " Total:" << Children[i]->total << endl;
            if(Children[i]->total > maxTotal)
            {
                maxTotal = Children[i]->total;
                bestMove = Children[i]->moveHere;
            }
        }

        return bestMove;
    }
};

int MonteCarlo(Board input)
{
    while(input.possibleMoves.size() > 0 && input.Result == 0)
    {
        int randomIndex = rand() % input.possibleMoves.size();
        input.add(input.possibleMoves[randomIndex]);
    }
    return input.Result;
}

struct AI
{
    Node * Current;
    Board Real;
    Board Simulated;

    AI(Board input, int inputPlayer) //blank board
    {
        Real = input;
        Simulated = input;
        Current = new Node();
        Current->init(inputPlayer);
    }

    void runSimulations()
    {
        //Resetting
        Simulated = Real;
//        cout << "CURRENT TOTAL: " << Current->total << endl;
        //Selection
        while(Current->madeChildren == true && Simulated.notFinished())
        {
//            cout << "THIS: " << this << endl;
            Current = Current->Select(Simulated.moves.size(), exploration_constant);
//            cout << "Selected Child: " << Current << endl;
            Simulated.add(Current->moveHere);
        }
        //Expansion
        if(Simulated.notFinished())
        {
//            cout << "MAKING CHILDREN" << endl;
            Current->makeChildren(Simulated);
        }
//      cout << "MADECHILDREN: " << Current->madeChildren << endl;
        //Simulation
        int result = Simulated.Result;
        //Backpropagation
        if(result == 0)
        {
            result = MonteCarlo(Simulated);
        }
        if(result == -1*Current->player)
        {
            Current->Win(1);
        }
        else if(result == 0)
        {
            Current->Draw(1);
        }
        else if(result == Current->player)
        {
            Current->Lose(1);
        }
        Current = Current->getRoot();
    }

    void addMove(tuple<int, int, int> moveInput)
    {
        Real.add(moveInput);
        Node* tmpPointer;
        bool foundInTree = false;
        for(int i = 0; i < Current->Children.size(); i++)
        {
            if(Current->Children[i]->moveHere == moveInput)
            {
                tmpPointer = Current->Children[i];
                tmpPointer->parent = nullptr;
                foundInTree = true;
            }
            else
            {
                Current->Children[i]->free_space();
            }
        }
        if(foundInTree == false)
        {
            Node* tmpNode = new Node();
            tmpNode->init(Current, moveInput);
            tmpNode->parent = nullptr;
            tmpPointer = tmpNode;
        }
        Current = tmpPointer;
        return;
        cout << "CANNOT ADD MOVE" << endl;
    }

    tuple<int, int, int> getMove()
    {
        Current = Current->getRoot();
        tuple<int, int, int> moveInput = Current->getMove();
        addMove(moveInput);
        return moveInput;
    }
};

int main()
{
    srand(time(NULL));
    initializeWinningCombinations();
    Board Game;
    cout << "Enter which player you would like to be (1 for first, -1 for second)" << endl;
    int inputPlayer;
    cin >> inputPlayer;
    AI GameAI(Game, 1);
    bool displayHeuristic = false;
    int totalRun = 0;

    while(Game.Result == 0)
    {
        if(Game.moves.size() % 2 == (inputPlayer == 1)) //COMPUTERS TURN
        {
            Game.Print();
            if(displayHeuristic)
            {
                cout << "Heuristic: " << 1 - GameAI.Current->successes/GameAI.Current->total << endl;
            }
            cout << "Making Calculations..." << endl;
            double start = time(0);
            while(time(0) - start <= 15)
            {
                GameAI.runSimulations();
            }
//            cout << "TOTAL: " << GameAI.Current->total << endl;
            totalRun = GameAI.Current->total;
            for(int i = 0; i < GameAI.Current->Children.size(); i++)
            {
//                cout << "TOTAL FOR CHILD " << i << ": " << GameAI.Current->Children[i]->total << endl;
//                cout << GameAI.Current->Children[i]->successes/GameAI.Current->Children[i]->total << endl;//" " << GameAI.Current->Children[i]->total << " " << GameAI.Current->Children[i]->getUTC(GameAI.Current->total) << endl;
            }
            Game.add(GameAI.getMove());
        }
        else //PLAYERS TURN
        {
            int c;
//            _getch();
            Game.Print();
            if(displayHeuristic)
            {
                cout << "Heuristic: " << GameAI.Current->successes/GameAI.Current->total << endl;
            }
            cout << "Leaves: " << totalRun << endl;
            while(!kbhit())
            {
                if(GameAI.Current->total < maxTrials)
                {
                    GameAI.runSimulations();
                }
                if(GameAI.Current->total == maxTrials-1)
                {
                    cout << "Pondering Complete." << endl;
                }
            }
            c = getch();
            switch(c)
            {
            case 9:
                Game.Tab();
                break;
            case 97:
                Game.LeftBoard();
                break;
            case 100:
                Game.RightBoard();
                break;
            case 75:
                Game.Left();
                break;
            case 72:
                Game.Up();
                break;
            case 77:
                Game.Right();
                break;
            case 80:
                Game.Down();
                break;
            case 112:
                Game.PrintOut();
                break;
            case 104:
                displayHeuristic = !displayHeuristic;
                break;
            case 32:
                Game.add(Game.currentMove);
                GameAI.addMove(Game.currentMove);
                Game.Tab();
                break;
            default:
                cout << "That action is not currently supported." << endl;
                break;
            }
        }
    }
    Game.Print();
    cout << "Result: " << Game.Result << endl;
    _getch();
    return 0;
}

// @END_OF_SOURCE_CODE
