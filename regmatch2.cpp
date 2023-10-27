#include <vector>
#include <string>
#include <iostream>

/**
 * @brief Use Ken Thompsons Regex Search Algorithm
 * to compile a state table for the regex. \n
 * https://dl.acm.org/doi/pdf/10.1145/363347.363387
 */
class Solution
{
private:
    /*
     * From http://swtch.com/~rsc/regexp/
     * Represents an NFA state plus zero or one or two arrows exiting.
     * if c == Match, no arrows out; matching state.
     * If c == Split, unlabeled arrows to out and out1 (if != NULL).
     * If c < 256, labeled arrow with character c to out.
     */
    enum transition
    {
        match = 256, // numbers out of char range
        split = 257  // numbers out of char range
    };

    // ----- Classes -----

    class State // class object for different states
    {
    public:
        Solution *nfa;
        int ch;
        int lastlist;
        State *trans1; // transitions
        State *trans2; // transitions
        State(int ch, State *trans1, State *trans2, Solution *nfa)
        {
            this->nfa = nfa;
            this->nfa->size++;
            this->ch = ch;
            this->lastlist = 0;
            this->trans1 = nullptr;
            this->trans2 = nullptr;
        }
        State(Solution *nfa) // for match state
        {
            this->nfa = nfa;
            this->ch = transition::match;
            this->lastlist = 0;
            this->trans1 = nullptr;
            this->trans2 = nullptr;
        }
    };

    class List
    {
    public:
        int size;
        std::vector<State *> *state;
        List()
        {
            this->size = 0;
            this->state = new std::vector<State *>;
        }
        ~List()
        {
            delete this->state;
        }
    };

    // ----- Unions & Structs -----

    /* From (http://swtch.com/~rsc/regexp/)
     * Since the out pointers in the list are always
     * uninitialized, we use the pointers themselves
     * as storage for the Ptrlists.
     *
     * This seeems to be a little indirection magic to save space making transition list
     * Ptrlist is syntactic sugar. Ptrlist pointer is just a really a pointer to a state pointer.
     * This way we can make a chain (list) of connected states.
     */
    typedef union _ptrlist
    {
        State *state;
        _ptrlist *next; // next state
    } Ptrlist;

    typedef struct _frag // class objects for nfa fragments to keep on stack while constructing nfa
    {
        State *start;
        Ptrlist *trans; // state transition list
    } Frag;

    // ----- Member Vars -----

    int size;          // keep track of number of states
    int listid;        // keep track of where we are at in state list
    State *matchstate; // singleton matchstate
    State *start;

    // ----- Functions -----

    Frag frag(State *start, Ptrlist *trans)
    {
        Frag f;
        f.start = start;
        f.trans = trans;
        return f;
    }

    /**
     * @brief Takes a regex string and return s a postfix version
     *
     * @param re string
     * @return std::string
     */
    std::string
    get_postfix_re(std::string *re)
    {
        int stacklen = 0;
        std::string postre;
        for (std::string::iterator ch = re->begin(); ch != re->end(); ch++)
        {
            if (*ch == '*')
            {
                if (!stacklen)
                {
                    return ""; // error handling parsing regex string
                }
                postre.push_back(*ch);
            }
            else
            {
                if (stacklen > 1)
                {
                    stacklen--;
                    postre.push_back('#'); // since our regex support '.' wildcards we will use '#' to indicate poping stack
                }
                postre.push_back(*ch);
                stacklen++;
            }
        }
        while (stacklen > 1)
        {
            postre.push_back('#');
            stacklen--;
        }
        return postre;
    }

    void
    linkstates(Frag *prevfrag, Frag *nextfrag)
    {
        Ptrlist *list = prevfrag->trans; // state transition list
        State *state = nextfrag->start;  // state to transition to

        Ptrlist *nexttrans;

        for (; list; list = nexttrans)
        {
            // set transitions to point to the next state
            // thank you union for syntactic sugar to transition to next sate with next
            // and set next state to sate all with the same space in memory
            nexttrans = list->next;
            list->state = state;
        }
    }

    /* Create singleton list containing just outp. */
    /**
     * Some magic going on here with making this list.... keep example simple
     */
    Ptrlist *
    makelist(State **transPtr)
    {
        Ptrlist *list;

        list = (Ptrlist *)transPtr;
        list->next = nullptr;
        return list;
    }

    State *
    compile_nfa(std::string *postre)
    {
        std::vector<Frag> stack;
        State *s;
        Frag next, prev;
        for (auto ch : *postre)
        {
            std::cout << ch << std::endl;
            switch (ch)
            {
            default:
                s = new State(ch, nullptr, nullptr, this);
                stack.push_back(frag(s, makelist(&s->trans1)));
                break;
            case '#': /* catenate */
                next = stack.back();
                stack.pop_back();
                prev = stack.back();
                stack.pop_back();
                linkstates(&prev, &next);
                stack.push_back(frag(prev.start, next.trans)); // combine into one frag, start = prev, Ptrlist = next.trans
                break;
            case '*': /* zero or more */
                prev = stack.back();
                stack.pop_back();
                s = new State(Solution::transition::split, prev.start, nullptr, this);

                // fix this later?
                next = frag(s, makelist(&s->trans1));
                linkstates(&prev, &next); // todo: need to pass in frag not state

                stack.push_back(frag(s, makelist(&s->trans2))); // NOTE: HERE WE ARE USING TRANS2 (transition 2) for the fragment
                break;
            }
        }
        prev = stack.back();
        stack.pop_back();

        // can check stack size for errors here;
        next = frag(matchstate, makelist(&s->trans1)); // fix later

        linkstates(&prev, &next);
        return prev.start;
    }

public:
    ~Solution()
    {
    }

    bool
    isMatch(std::string s, std::string p)
    {
        std::string postre = this->get_postfix_re(&p);
        List charlist;
        List statelist;
        State *start;
        this->size = 0;
        this->listid = 0;
        this->matchstate = new State(this);
        this->start = this->compile_nfa(&postre);
        charlist.state->resize(this->size);
        statelist.state->resize(this->size);

        return false;
    }
};

int main()
{
    Solution re;
    std::string restr;
    std::string str;
    getline(std::cin, str);
    getline(std::cin, restr);
    std::cout << re.isMatch(str, restr) << std::endl;
    return 0;
}