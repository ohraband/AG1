#include <iostream>
#include <cassert>
//using namespace std;


struct Node
{
    uint32_t popularity;
    int degree;
    Node *child;
    Node *sibling;
    Node *parent;

    std::string name;
    uint8_t gender;
    uint32_t id;
    uint32_t party;
    uint32_t changenum;
    uint32_t coalition;


    const std::string &getName() const {
        return name;
    }

    uint32_t getPop() const {
        return popularity;
    }

    uint8_t getGender() const {
        return gender;
    }



    Node(uint32_t popularity, uint32_t id, const std::string & name, uint8_t gender, uint32_t party, uint32_t changenum)
    {

        degree = 0;
        child = nullptr;
        parent = nullptr;
        sibling = nullptr;
        coalition = 0;

        this->name = name;
        this->gender = gender;
        this->id = id;
        this->party = party;
        this->changenum = changenum;
        this->popularity = popularity;
    }


};




class CPulitzer {
public:

    CPulitzer ( size_t N, size_t P ) : N(N), P(P){

        politicians = new Node*[P+5]; //politician id
        parties = new Node*[N+5]; //party id
        party_leaders = new Node*[N+5]; //


        for (unsigned int i = 0; i < (P+5); i++)
            politicians[i] = nullptr;

        for (unsigned  int i = 0; i < (N+5); i++)
            parties[i] = nullptr;

        for (unsigned  int i = 0; i < (N+5); i++)
            party_leaders[i] = nullptr;

    };

    ~CPulitzer(){
        for(unsigned int i=0; i<P; i++){
            delete politicians[i];
        }

        delete [] politicians;
        delete [] party_leaders;
        delete [] parties;
    }

    //obv. have a 10^6 array for each party id
    bool register_politician ( uint32_t id_party, uint32_t id_politician, const std::string & name, uint32_t popularity, uint8_t gender ){

        if( (id_party < N && id_party >= 0 && id_politician < P && id_politician >= 0) && politicians[id_politician] == nullptr ){
            Node* newpol = newNode(popularity, id_politician, name,gender,id_party, changenum);
            changenum ++;
            politicians[id_politician] = newpol;
            if(parties[id_party] == nullptr){
                parties[id_party] = newpol;
                party_leaders[id_party] = newpol;
            }
            else{
                newpol->coalition = parties[id_party]->coalition;
                parties[id_party] = Merge(newpol,parties[id_party]);
                FindLeaderInsert(id_party,newpol);
            }
            return true;
        }

        return false;

    };
    //heap insert value
    //keep a 10^6 array of used id's

    bool politician_name ( uint32_t id_politician, std::string & name ) const{
        if(politicians[id_politician] != nullptr){
            name = politicians[id_politician]->name;
            return true;
        }
        return false;
    };

    bool politician_gender ( uint32_t id_politician, uint8_t & gender ) const{
        if(politicians[id_politician] != nullptr){
            gender = politicians[id_politician]->gender;
            return true;
        }
        return false;
    };

    bool politician_popularity ( uint32_t id_politician, uint32_t & popularity ) const{
        if(politicians[id_politician] != nullptr){
            popularity = politicians[id_politician]->popularity;
            return true;
        }
        return false;
    };

    bool deregister_politician ( uint32_t id_politician ){

        if( (id_politician >= P || id_politician < 0 ) || politicians[id_politician] == nullptr){
            return false;
        }
        uint32_t party = politicians[id_politician]->party;
        change_popularity(id_politician, UINT32_MAX);
        sack_leader(party);
        return true;
    };
    //delete heap by value (possibly delete a whole subtree too)

    bool party_leader ( uint32_t id_party, uint32_t & id_leader ) const{
        if(party_leaders[id_party] != nullptr){
            id_leader = party_leaders[id_party]->id;
            //cout << id_leader << endl;
            return true;
        }
        else{
            return false;
        }
    };
    //just have 10^6 array of leaders for each party
    //get max value of subtree (likely root), warning, max gets replaced even if max1=max2
    template<typename T> void swapp(T& t1, T& t2) {
        T tmp(t1);
        t1=t2;
        t2=tmp;
    }


    bool change_popularity ( uint32_t id_politician, uint32_t popularity ){
        if(politicians[id_politician] != nullptr){
            Node * curr = politicians[id_politician];

            if(popularity >= curr->popularity){
                politicians[id_politician]->popularity = popularity;
                changenum++;
                politicians[id_politician]->changenum = changenum;
                uint32_t party = politicians[id_politician]->party;


                while(curr->parent != nullptr && (
                        curr->popularity >  curr->parent->popularity ||(
                                curr->popularity ==  curr->parent->popularity &&
                                curr->changenum >  curr->parent->changenum ) ) ){

                    swapp(politicians[curr->id],politicians[curr->parent->id]);
                    swapp(curr->popularity, curr->parent->popularity);
                    swapp(curr->gender, curr->parent->gender);
                    swapp(curr->name, curr->parent->name);
                    swapp(curr->id, curr->parent->id);
                    swapp(curr->changenum, curr->parent->changenum);
                    curr = curr->parent;
                }

                party_leaders[party] = FindLeaderRoots(party);

            }

            else if(popularity < curr->popularity){
                uint32_t id = politicians[id_politician]->id;
                uint32_t party = politicians[id_politician]->party;
                std::string name = politicians[id_politician]->name;
                uint8_t gender = politicians[id_politician]->gender;

                changenum++;
                politicians[id_politician]->changenum = changenum;
                deregister_politician(id);
                register_politician(party,id,name,popularity,gender);

                party_leaders[party] = FindLeaderRoots(party);
            }



            return true;
        }
        return false;
    };
    //change value of node, verify with party_leader too

    //reverse children order
    Node* reverse(Node* head)
    {
        Node* current = head;
        Node *prev = nullptr;
        Node *next = nullptr;

        while (current != nullptr) {
            next = current->sibling;
            current->sibling = prev;
            prev = current;
            current = next;
        }
        head = prev;
        return head;
    }


    //free memory here
    bool sack_leader ( uint32_t id_party ){
        if(party_leaders[id_party] == nullptr){
            return false;
        }


        Node* todelete = party_leaders[id_party];


        Node* tmp = parties[id_party];
        if(tmp == party_leaders[id_party]){

            if(tmp->child != nullptr){
                tmp = tmp->child;
                tmp->parent = nullptr;
                while(tmp->sibling != nullptr) {
                    tmp->sibling->parent = nullptr;
                    tmp = tmp->sibling;
                }
            }

            if(party_leaders[id_party]->sibling == nullptr && party_leaders[id_party]->child == nullptr){
                leave_coalition(id_party);
            }

            parties[id_party] = Merge(party_leaders[id_party]->sibling,reverse(party_leaders[id_party]->child));
        }
        else{
            while(tmp->sibling != party_leaders[id_party]){
                tmp = tmp->sibling;
            }


            Node * oldtmp = tmp;

            if(tmp->sibling->child != nullptr){
                tmp->sibling->child->parent = nullptr;
                tmp = tmp->sibling->child;
                while(tmp->sibling != nullptr) {
                    tmp->sibling->parent = nullptr;
                    tmp = tmp->sibling;
                }
            }

            if(oldtmp->sibling != nullptr){
                oldtmp->sibling = oldtmp->sibling->sibling;
            }

            if(parties[id_party] == nullptr && party_leaders[id_party]->child == nullptr){
                leave_coalition(id_party);
            }
            parties[id_party] = Merge(parties[id_party],reverse(party_leaders[id_party]->child));
        }

        politicians[todelete->id] = nullptr;
        delete todelete;
        party_leaders[id_party] = FindLeaderRoots(id_party);

        return true;
    };
    //extract max on heap


    void Changeid(Node * head, uint32_t party, uint32_t coalition ){
        head->party = party;
        head->coalition = coalition;
        if(head->child){
            head->child->party = party;
            head->child->coalition = coalition;
            Changeid(head->child, party,coalition);
        }
        if(head->sibling){
            head->sibling->party = party;
            head->sibling->coalition = coalition;
            Changeid(head->sibling, party,coalition);
        }
    }


    bool merge_parties ( uint32_t dest_party, uint32_t src_party ){
        if(parties[dest_party] == nullptr || parties[src_party] == nullptr ){
            return false;
        }
        if(dest_party == src_party){
            return true;
        }

        leave_coalition(src_party);

        uint32_t coalition = parties[dest_party]->coalition;
        parties[dest_party] = Merge(parties[dest_party],parties[src_party]);
        parties[dest_party]->party = dest_party;
        Changeid(parties[dest_party], dest_party,coalition);
        party_leaders[dest_party] = FindLeaderRoots(dest_party);

        parties[src_party] = nullptr;
        party_leaders[src_party] = nullptr;

        return true;
    };
    //merge two heaps into one, (build heap algo?)


    void ChangeCoalition(Node * head, uint32_t coalition ){
        head->coalition = coalition;
        if(head->child){
            head->child->party = coalition;
            ChangeCoalition(head->child, coalition);
        }
        if(head->sibling){
            head->sibling->party = coalition;
            ChangeCoalition(head->sibling, coalition);
        }
    }


    bool create_coalition ( uint32_t id_party1, uint32_t id_party2 ){
        if(parties[id_party1] == nullptr || parties[id_party2] == nullptr ){
            return false;
        }

        uint32_t coalition1 = parties[id_party1]->coalition;
        uint32_t coalition2 = parties[id_party2]->coalition;

        if( ( coalition1 == coalition2 && (coalition1 != 0 || coalition2 != 0) ) || id_party1 == id_party2){
            return true;
        }

        if(parties[id_party1]->coalition !=0){
            for (unsigned  int i = 0; i <N ; i++){
                if(parties[i] != nullptr &&  parties[i]->coalition == coalition2 ){
                    parties[i]->coalition = coalition1;
                    ChangeCoalition(parties[i],coalition1);
                }
            }
        }

        else if(parties[id_party2]->coalition !=0){
            for (unsigned  int i = 0; i <N ; i++){
                if(parties[i] != nullptr &&  parties[i]->coalition == coalition1 ){
                    parties[i]->coalition = coalition2;
                    ChangeCoalition(parties[i],coalition2);
                }
            }
        }
        else{
            coalition++;
            parties[id_party1]->coalition = coalition;
            ChangeCoalition(parties[id_party1],coalition);

            parties[id_party2]->coalition = coalition;
            ChangeCoalition(parties[id_party2],coalition);
        }
        return true;
    };

    bool leave_coalition ( uint32_t id_party ){
        if(parties[id_party] == nullptr || parties[id_party]->coalition == 0){
            return false;
        }
        uint32_t memberCounter = 0;
        uint32_t coalition = parties[id_party]->coalition;
        for (unsigned  int i = 0; i < N ; i++){
            if(parties[i] != nullptr && parties[i]->coalition == coalition ){
                memberCounter++;
            }
        }

        if(memberCounter <= 2){
            for (unsigned  int i = 0; i < N ; i++){
                if(parties[i] != nullptr && parties[i]->coalition == coalition ){
                    parties[i]->coalition = 0;
                    ChangeCoalition(parties[id_party],0);
                }
            }
        }
        parties[id_party]->coalition = 0;
        ChangeCoalition(parties[id_party],0);
        return true;
    };
    //false for now

    bool coalition_leader ( uint32_t id_party, uint32_t & id_leader ) const{
        if(parties[id_party] == nullptr){
            return false;
        }

        uint32_t coalition = parties[id_party]->coalition;
        if(coalition == 0){
            id_leader = party_leaders[id_party]->id;
            return true;
        }

        Node *  leader = nullptr;
        for (unsigned  int i = 0; i < N ; i++){
            if(parties[i] != nullptr && parties[i]->coalition == coalition){
                if(leader == nullptr || party_leaders[i]->popularity > leader->popularity ||
                (party_leaders[i]->popularity == leader->popularity && party_leaders[i]->changenum > leader->changenum) ){
                    leader = party_leaders[i];
                }
            }
        }

        id_leader = leader->id;
        return true;
    };
    //return party leader in &id_leader for now

    bool scandal_occured ( uint32_t id_party ){
        if(parties[id_party] == nullptr){
            return false;
        }
        uint32_t id_leader;
        coalition_leader(id_party, id_leader); //check reference
        if(id_leader == party_leaders[id_party]->id){
            sack_leader(id_party);
        }
        else{
            sack_leader(id_party);
            sack_leader(politicians[id_leader]->party);
        }
        return true;
    };
    //return true if party exists




    void FindLeaderInsert ( uint32_t id_party, Node* insertednode){
        if(insertednode->popularity > party_leaders[id_party]->popularity ||
           (insertednode->popularity == party_leaders[id_party]->popularity && insertednode->changenum > party_leaders[id_party]->changenum)  ){
            party_leaders[id_party] = insertednode;
        }
    }

    Node* FindLeaderRoots ( uint32_t id_party){
        Node * max = nullptr;
        Node * tmp = parties[id_party];
        while(tmp != nullptr){
            if(max == nullptr || tmp->popularity > max->popularity || (tmp->popularity == max->popularity && tmp->changenum > max->changenum )  ){ //also do timestamps to figure out ties
                max = tmp;
            }
            tmp = tmp->sibling;
        }
        return max;
    }


    void FindLeaderMerge ( uint32_t id_party_src, uint32_t id_party_dest){
        if(party_leaders[id_party_src]->popularity >= party_leaders[id_party_dest]->popularity){ //will need some timestamp checks too
            party_leaders[id_party_dest] = party_leaders[id_party_src];
            party_leaders[id_party_src] = nullptr;
        }
    }


    Node* MergeTree(Node *b1, Node *b2)
    {

        if (b1->popularity > b2->popularity || (b1->popularity == b2->popularity && b1->changenum > b2->changenum) ){
            b2->parent = b1;
            b2->sibling = b1->child;
            b1->child = b2;
            b1->degree++;
            return b1;
        }
        else{
            b1->parent = b2;
            b1->sibling = b2->child;
            b2->child = b1;
            b2->degree++;
            return b2;
        }

    }



    Node* Merge(Node *b1, Node *b2)
    {

        Node* items[3];
        Node* carry = nullptr;
        Node* c = nullptr;
        Node* head = nullptr;
        int nonempty = 2;
        int degree = 0;
        int numberofitems = 0;
        while(nonempty >= 2){
            nonempty = 0;
            numberofitems = 0;

            if(b1 != nullptr){
                nonempty++;
                Node* a = b1;
                if(a->degree == degree){
                    //remove from rest of tree
                    Node* temp = b1->sibling;
                    b1->sibling = nullptr;
                    b1 = temp;
                    //add to current iteration
                    items[numberofitems] = a;
                    numberofitems++;
                }

            }


            if(b2 != nullptr){
                nonempty++;
                Node* b = b2;
                //remove from rest of tree
                if(b->degree == degree){
                    Node* temp = b2->sibling;
                    b2->sibling = nullptr;
                    b2 = temp;
                    //add to current iteration
                    items[numberofitems] = b;
                    numberofitems++;
                }
            }

            if(carry != nullptr){
                nonempty++;
                items[numberofitems] = carry;
                numberofitems++;
                carry = nullptr;
            }


            if(numberofitems == 3){
                if(head == nullptr){
                    head = items[2];
                    c = items[2];
                }
                else{
                    //move siblings left to right, may not work
                    c->sibling = items[2];
                    c = items[2];
                }

                carry = MergeTree(items[0], items[1]);
            }



            if(numberofitems == 2){
                carry = MergeTree(items[0], items[1]);
            }


            if(numberofitems == 1){
                if(head == nullptr){
                    head = items[0];
                    c = items[0];
                }
                else{
                    //move siblings left to right, may not work
                    c->sibling = items[0];
                    c = items[0];
                }

            }

//            if(degree < 0 || degree > 1000){
//                std::cout << "degree" << degree << std::endl;
//            }
            degree++;


        }


        if(b1 != nullptr){
            if(c != nullptr){
                c->sibling = b1;
                c = b1;
            }
            else{
                head = b1;
                c = b1;
            }

        }

        if(b2 != nullptr){
            if(c != nullptr) {
                c->sibling = b2;
                c = b2;
            }
            else{
                head = b2;
                c = b2;
            }

        }

        //actually array and if will be faster in te end
        //assign max to party leader thing
        return head;
    }




    Node* newNode(uint32_t popularity, uint32_t id, const std::string & name, uint8_t gender, uint32_t party, uint32_t changenum)
    {
        Node *temp = new Node(popularity, id,  name,gender,  party,  changenum);
        return temp;
    }


    uint32_t changenum = 0;
    uint32_t coalition = 1;
    size_t N;
    size_t P;
//    Node ** politicians = new Node*[P+5]; //politician id
//    Node ** parties = new Node*[N+5]; //party id
//    Node ** party_leaders = new Node*[N+5]; //leader of a party
    Node ** politicians;
    Node ** parties;
    Node ** party_leaders;

};


int main() {

    return 0;
}
