#include <bits/stdc++.h>
using namespace std;

// Minimal deque-like class supporting required operations used in tests:
// We implement a simple unrolled linked list to ensure good performance.

template <typename T, size_t BLOCK = 512>
struct Deque {
    struct Node {
        int sz;
        array<T, BLOCK> a;
        Node *prev, *next;
        Node(): sz(0), prev(nullptr), next(nullptr) {}
    };
    Node *head, *tail;
    size_t n;

    Deque(): head(new Node()), tail(new Node()), n(0) {
        head->next = tail; tail->prev = head;
    }
    ~Deque(){ clear(); delete head; delete tail; }

    bool empty() const { return n==0; }
    size_t size() const { return n; }

    void clear(){
        Node *p = head->next;
        while(p!=tail){ Node *q=p->next; delete p; p=q; }
        head->next = tail; tail->prev = head; n=0;
    }

    // helpers
    void insert_node_after(Node* u, Node* v){ v->next = u->next; v->prev = u; u->next->prev = v; u->next = v; }
    void erase_node(Node* u){ u->prev->next = u->next; u->next->prev = u->prev; delete u; }

    // locate index -> node and offset
    pair<Node*, int> locate(size_t idx){
        // idx in [0,n]
        Node *p = head->next;
        while(p!=tail){
            if(idx <= (size_t)p->sz) return {p,(int)idx};
            idx -= p->sz; p=p->next;
        }
        return {tail,0};
    }

    void split(Node* p){
        if(p->sz < (int)BLOCK) return;
        Node* q = new Node();
        int t = p->sz/2;
        int move = p->sz - t;
        for(int i=0;i<move;i++) q->a[i] = p->a[t+i];
        q->sz = move; p->sz = t;
        insert_node_after(p, q);
    }

    void merge_with_next(Node* p){
        Node* q = p->next; if(q==tail) return;
        if(p->sz + q->sz > (int)BLOCK) return;
        for(int i=0;i<q->sz;i++) p->a[p->sz+i] = q->a[i];
        p->sz += q->sz; erase_node(q);
    }

    void insert(size_t idx, const T& val){
        auto [p, off] = locate(idx);
        if(p==tail){ // append to last block
            if(tail->prev==head || tail->prev->sz==(int)BLOCK){
                Node* q = new Node(); insert_node_after(tail->prev, q); p = q; off=0;
            }else{ p = tail->prev; off = p->sz; }
        }
        // shift right within node
        for(int i=p->sz; i>off; --i) p->a[i]=p->a[i-1];
        p->a[off]=val; p->sz++; n++;
        split(p);
    }

    void erase(size_t idx){
        auto [p, off] = locate(idx);
        if(p==tail) return;
        for(int i=off+1;i<p->sz;i++) p->a[i-1]=p->a[i];
        p->sz--; n--;
        if(p->sz==0 && p!=head && p!=tail){ erase_node(p); return; }
        if(p->sz < (int)BLOCK/4 && p->next!=tail) merge_with_next(p);
    }

    T& at(size_t idx){ auto [p,off]=locate(idx); return p->a[off]; }
    const T& at(size_t idx) const { auto [p,off]=const_cast<Deque*>(this)->locate(idx); return p->a[off]; }

    void push_front(const T& x){ insert(0,x); }
    void push_back(const T& x){ insert(n,x); }
    void pop_front(){ if(n) erase(0); }
    void pop_back(){ if(n) erase(n-1); }
};

static inline bool is_number(const string& s){ if(s.empty()) return false; size_t i=0; if(s[0]=='-'||s[0]=='+') i=1; if(i>=s.size()) return false; for(;i<s.size();++i) if(!isdigit((unsigned char)s[i])) return false; return true; }

static void handle_op(const string& op, Deque<long long>& dq){
    if(op=="push_front"){ long long x; if(cin>>x) dq.push_front(x); }
    else if(op=="push_back"){ long long x; if(cin>>x) dq.push_back(x); }
    else if(op=="pop_front"){ if(!dq.empty()){ cout<<dq.at(0)<<"\n"; dq.pop_front(); } else cout<<"ERR\n"; }
    else if(op=="pop_back"){ if(!dq.empty()){ cout<<dq.at(dq.size()-1)<<"\n"; dq.pop_back(); } else cout<<"ERR\n"; }
    else if(op=="front"){ if(!dq.empty()) cout<<dq.at(0)<<"\n"; else cout<<"ERR\n"; }
    else if(op=="back"){ if(!dq.empty()) cout<<dq.at(dq.size()-1)<<"\n"; else cout<<"ERR\n"; }
    else if(op=="insert"){ size_t i; long long x; if(cin>>i>>x){ if(i<=dq.size()) dq.insert(i,x); }}
    else if(op=="erase"){ size_t i; if(cin>>i){ if(i<dq.size()) dq.erase(i); }}
    else if(op=="get"||op=="at"){ size_t i; if(cin>>i){ if(i<dq.size()) cout<<dq.at(i)<<"\n"; else cout<<"ERR\n"; }}
    else if(op=="set"){ size_t i; long long x; if(cin>>i>>x){ if(i<dq.size()) dq.at(i)=x; }}
    else if(op=="size"){ cout<<dq.size()<<"\n"; }
    else if(op=="clear"){ dq.clear(); }
    else if(op=="empty"){ cout<<(dq.empty()?"true":"false")<<"\n"; }
    else { string rest; getline(cin, rest); }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Deque<long long> dq;
    string first;
    if(!(cin>>first)) return 0;
    if(is_number(first)){
        long long q = atoll(first.c_str());
        for(long long i=0;i<q;i++){ string op; if(!(cin>>op)) break; handle_op(op, dq); }
    }else{
        handle_op(first, dq);
        string op;
        while(cin>>op){ handle_op(op, dq); }
    }
    return 0;
}
