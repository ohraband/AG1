#include <iostream>
#include <math.h>
#include <new>
#include <sstream>

struct Node{

    Node(Node* np,int px, int py) : posx(px),posy(py){
        next = np;
    }
    Node* next;
    int posx;
    int posy;
};


class Queue{
public:
    //constructor
    Queue(){
        head = nullptr;
        tail = nullptr;
    };

    //destructor
    ~Queue(){
        while(head != nullptr){
            Pop();
        }
    };

    //push
    bool Push(Node* newnode){
        if(newnode == nullptr){
            return false;
        }

        if(head == nullptr){
            head = newnode;
            tail = newnode;
            return true;
        }
        else{
            tail->next = newnode;
            tail = tail->next;
            return true;
        }
    };

    //pop
    bool Pop(){
        if(head == nullptr){
            return false;
        }

        else{
            Node* tmpnode = head;
            head = head->next;
            delete tmpnode;
            return true;
        }

    };

    bool Empty(){
        return (head == nullptr);
    };

//private:
    Node* head;
    Node* tail;
};


int BFS(int **&coords, const int n, int trophyx, int trophyy, std::stringstream& strm){

    struct prevPos{
        prevPos(){
            prevposx = -1;
            prevposy = -1;
        }

        void Setx(int x){
            prevposx = x;
        }

        void Sety(int y){
            prevposy = y;
        }

        int prevposx;
        int prevposy;
    };

    auto **prevpos = new prevPos *[n];
    for(int i=0; i < n; i++) {
        prevpos[i] = new prevPos[n];
    }

    //while queue
    //for each thing in queue (if there isn't a tree, then change prevpos to -1) and (if it isn't == trophy)
    //check if prevpos there is nullptr, if not skip
    //if yes put current position there
    //create a new node with this position
    //push new node into queue
    //do for all sides
    //pop current one

    Queue q;
    if(coords[trophyy][trophyx] != 1){
        Node *trophy = new Node(nullptr, trophyx, trophyy);
        q.Push(trophy);
        prevpos[trophy->posy][trophy->posx].Setx(trophy->posy);
        prevpos[trophy->posy][trophy->posx].Sety(trophy->posx);
    }



    bool found = false;
    while(!q.Empty()){
        Node* curr = q.head;
        if(curr->posx == 0 && curr->posy == 0){
            //std::cout << "yes found it" << std::endl;
            found = true;
            break; // and ask for path
        }

        //go down
        if (curr->posy > 0 && coords[curr->posy -1][curr->posx] != 1 ){
            if(prevpos[curr->posy -1][curr->posx].prevposx == -1 &&
               prevpos[curr->posy -1][curr->posx].prevposy == -1){
                prevpos[curr->posy -1][curr->posx].Setx(curr->posx);
                prevpos[curr->posy -1][curr->posx].Sety(curr->posy);
                q.Push(new Node(nullptr, curr->posx, curr->posy -1) );
            }
        }


        //go up
        if (curr->posy < (n-1) && coords[curr->posy +1][curr->posx] != 1 ){
            if(prevpos[curr->posy +1][curr->posx].prevposx == -1 &&
               prevpos[curr->posy +1][curr->posx].prevposy == -1){
                prevpos[curr->posy +1][curr->posx].Setx(curr->posx);
                prevpos[curr->posy +1][curr->posx].Sety(curr->posy);
                q.Push(new Node(nullptr, curr->posx, curr->posy +1) );
            }
        }

        //go left
        if (curr->posx > 0 && coords[curr->posy][curr->posx -1] != 1 ){
            if(prevpos[curr->posy][curr->posx -1].prevposx == -1 &&
               prevpos[curr->posy][curr->posx -1].prevposy == -1){
                prevpos[curr->posy][curr->posx -1].Setx(curr->posx);
                prevpos[curr->posy][curr->posx -1].Sety(curr->posy);
                q.Push(new Node(nullptr, curr->posx -1, curr->posy) );
            }
        }

        //go right
        if (curr->posx < (n-1) && coords[curr->posy][curr->posx +1] != 1 ){
            if(prevpos[curr->posy][curr->posx +1].prevposx == -1 &&
               prevpos[curr->posy][curr->posx +1].prevposy == -1){
                prevpos[curr->posy][curr->posx +1].Setx(curr->posx);
                prevpos[curr->posy][curr->posx +1].Sety(curr->posy);
                q.Push(new Node(nullptr, curr->posx +1, curr->posy) );
            }
        }

        q.Pop();
    }


    int totaldist = 0;
    if(found){
        int x=0;
        int y=0;
        int tmpx, tmpy;
        while(x >= 0 && y >= 0 && (x != trophyx ||
                                   y != trophyy)){
            strm << "[" << x+1 << ";" << y+1 << "],";
            tmpx = prevpos[y][x].prevposx;
            tmpy = prevpos[y][x].prevposy;
            x = tmpx;
            y = tmpy;
            totaldist++;
        }
        strm << "[" << x+1 << ";" << y+1 << "]" << std::endl;
        //strm << "Distance: " << totaldist << std::endl;
    }

    for(int i=0; i < n; i++){
        delete[] prevpos[i];
    }
    delete[] prevpos;

    if(found){
        return  totaldist;
    }
    else{
        return -1;
    }
}

void InvertColumn(int **&coords, int column, int n){
    //every map [set x][variable y], change y to not y
    for(int i=0; i < n; i++){
        if(coords[i][column] == 1){
            coords[i][column] = 0;
        }

        else if(coords[i][column] == 0){
            coords[i][column] = 1;
        }
    }


//    for(int i = (n-1) ; i >= 0 ; i--){
//        for(int j = 0; j < n ; j++){
//            std::cout << coords[i][j];
//        }
//        std::cout << std::endl;
//    }
}



int main() {
    //std::cout << "Hello, World!" << std::endl;

    //get input
    int n, k;
    std::cin >> n;
    std::cin >> k;
    //std::cout << k << std::endl;

    int *leverdist = new int[k+1];

    int **levermask = new int *[k+1];
    for(int i=0; i < (k+1); i++){
        levermask[i] = new int[n+1];
    }

    for(int i=0; i < k; i++){

        std::cin >> leverdist[i];
        for(int j = 0; j < n ; j++ ){
            char tmp;
            scanf(" %c", &tmp);
            if(tmp =='0'){
                levermask[i][j] = 0;
            }
            else{
                levermask[i][j] = 1;
            }
        }
        //std::cout << "Leverdist " << leverdist[i] << std::endl;
    }


    int **coords = new int *[n];
    for(int i=0; i < n; i++){
        coords[i] = new int[n];
        for(int j = 0; j < n ; j++ ){
            char tmp;
            scanf(" %c", &tmp);
            if(tmp =='0'){
                coords[i][j] = 0;
            }
            else{
                coords[i][j] = 1;
            }
        }
    }

    //std::cout << std::endl << "Map" << std::endl;
//    for(int i = (n-1) ; i >= 0 ; i--){
//        for(int j = 0; j < n ; j++){
//            std::cout << coords[i][j];
//        }
//        std::cout << std::endl;
//    }
    int trophyx;// maybe this need to be offset, dunno if [1;1] is coords[0][0]
    int trophyy;
    std::cin >> trophyx >> trophyy;
    trophyx -= 1;
    trophyy -= 1;
    //std::cout << "Trophy location " << trophyx << " " << trophyy << std::endl;



    //PermutationChecker
    int mindist = -1;
    std::stringstream strm;
    std::stringstream retstrm;
    for(int i = 0 ; i < pow(2,k) ; i++){
        int totaldist = 0;
        int furthestlever = 0;
        bool minuslever = false;
        strm.str("");
        for(int j = 0; j < k; j++){
            if( i & (int)(pow(2,j))){
                if(leverdist[j] == -1 && !(minuslever) ){
                    totaldist += ( abs(leverdist[j]) * 2);
                    minuslever = true;
                }
                else if(leverdist[j] > furthestlever){
                    furthestlever = leverdist[j];
                }
                for(int o = 0; o < n ;o++){
                    if(levermask[j][o] == 1){
                        InvertColumn(coords, o, n);
                    }
                }
            }
        }

        //call BFS here

        int bfsdist = BFS(coords,n,trophyx,trophyy,strm);
        if(bfsdist >= 0 ){
            totaldist+= bfsdist;
            totaldist += ( abs(furthestlever) * 2);
            //std::cout << "Totaldist and levers: " << totaldist << std::endl;
            //std::cout <<  totaldist << std::endl;

            for(int j = 0; j < k; j++){
                if( i & (int)(pow(2,j))){
                    //std::cout << 1;
                }
                else{
                    //std::cout << 0;
                }
            }
            if(k){//std::cout << std::endl;
                 };
            //std::cout << strm.str();


        }
        else{
            totaldist = -1;
        }

        if ( (totaldist < mindist && totaldist >= 0 ) || (mindist == -1 && totaldist >=0) ){
            mindist = totaldist;
            //std::cout << "New mindist: " << mindist << std::endl;
            retstrm.str("");
            retstrm << mindist << std::endl;
            for(int j = 0; j < k; j++){
                if( i & (int)(pow(2,j))){
                    retstrm << 1;
                }
                else{
                    retstrm << 0;
                }
            }
            if(k){retstrm << std::endl;}
            retstrm << strm.str();
        }


        //invert back
        for(int j = 0; j < k; j++){
            if( i & (int)(pow(2,j))){
                for(int o = 0; o < n ;o++){
                    if(levermask[j][o] == 1){
                        InvertColumn(coords, o, n);
                    }
                }
            }
        }

    }

    for(int i=0; i < n; i++){
        delete[] coords[i];
    }
    delete[] coords;


    for(int i=0; i < (k+1); i++){
        delete[] levermask[i];
    }
    delete[] levermask;

    delete[] leverdist;

    //std::cout << "Mindist: " << mindist << std::endl;
    if(!retstrm.str().empty()){
        std::cout << retstrm.str();
    }
    else{
        std::cout << -1;
    }

    return 0;
}
