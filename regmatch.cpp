/**
 * Nick Anderson
 * October 27, 2023
 * 
 * I wanted to use se Ken Thompson's Regex Search Algorithm
 * to compile a nfa for the regex to solve Leetcode problem
 * # 10. Regular Expression Matching
 * 
 * So I modified code authored by Russ Cox which was written in C
 * and can be found at this web address
 * http://swtch.com/~rsc/regexp/
 * 
 * Here is a link to Ken Thompson's regex algorithm article
 * https://dl.acm.org/doi/pdf/10.1145/363347.363387
 */

#include <vector>
#include <string>
#include <iostream>

class Solution
{
private:
    /*
     * Comment from http://swtch.com/~rsc/regexp/
     * "Represents an NFA state plus zero or one or two arrows exiting.
     * if c == Match, no arrows out; matching state.
     * If c == Split, unlabeled arrows to out and out1 (if != NULL).
     * If c < 256, labeled arrow with character c to out."
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
            this->trans1 = trans1;
            this->trans2 = trans2;
        }
        State(Solution *nfa) // for match state
        {
            this->nfa = nfa;
            this->ch = transition::match;
            this->lastlist = 0;
            this->trans1 = nullptr;
            this->trans2 = nullptr;
        }
        ~State()
        {
            // if (this->trans1)
            // {
            //     delete this->trans1;
            // }
            // if (this->trans2)
            // {
            //     delete this->trans2;
            // }
        }
    };

    class StateList
    {
    public:
        int size;
        std::vector<State *> *state;
        StateList()
        {
            this->size = 0;
            this->state = new std::vector<State *>;
        }
        ~StateList()
        {
            // if (this->state)
            // {
            //     delete this->state;
            // }
        }
    };

    // ----- Unions & Structs -----

    /* Comment from (http://swtch.com/~rsc/regexp/)
     * "Since the out pointers in the list are always
     * uninitialized, we use the pointers themselves
     * as storage for the Ptrlists."
     *
     * Ptrlist seeems to be a little indirection magic to save space making transition list
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
    State *start;      // start state to the nfa

    // ----- Functions -----

    Frag frag(State *start, Ptrlist *trans)
    {
        Frag f {start, trans};
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
    linkstates(Frag *prevfrag, State *state)
    {
        Ptrlist *list = prevfrag->trans; // state transition list

        Ptrlist *nexttrans;

        /**
         * TODO: is loop necessary for leetcode specs?
         * we will always have null transition, so only one
         * state to set? This is for Or operator where there
         * can be a list of transitions from the append function
         */
        for (; list; list = nexttrans)
        {
            // set transitions to point to the next state
            // thank you union for syntactic sugar to transition to next sate with next
            // and set next state to sate all with the same space in memory
            nexttrans = list->next;
            list->state = state;
        }
    }

    /**
     *  Create singleton list containing just outp. */
    /**
     * Some syntactic magic going on here with making this list
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
        State *state;
        Frag next, prev;
        for (auto ch : *postre)
        {
            switch (ch)
            {
            default:
                state = new State(ch, nullptr, nullptr, this);
                stack.push_back(frag(state, makelist(&state->trans1)));
                break;
            case '#': /* catenate */
                next = stack.back();
                stack.pop_back();
                prev = stack.back();
                stack.pop_back();
                linkstates(&prev, next.start);
                stack.push_back(frag(prev.start, next.trans)); // combine into one frag, start = prev, Ptrlist = next.trans
                break;
            case '*': /* zero or more */
                prev = stack.back();
                stack.pop_back();
                // new split state. trans1 points back to prev char
                state = new State(Solution::transition::split, prev.start, nullptr, this);
                linkstates(&prev, state);
                prev = frag(state, makelist(&state->trans2));
                // fragment now holding split state trans2 (transition list2). trans1 points back to last char
                stack.push_back(prev); // NOTE: HERE WE ARE USING TRANS2 (transition 2) for the fragment
                break;
            }
        }
        prev = stack.back();
        stack.pop_back();
        // can check stack size for errors here;
        linkstates(&prev, this->matchstate);
        return prev.start;
    }

    // _________________ MATCH _________________________


    void addstate(State *state, StateList *list)
    {
        /**
         * TODO: comment this if statement
         */
        if (state == nullptr || state->lastlist == listid)
            return;
        state->lastlist = listid;
        if (state->ch == transition::split)
        {
            /* follow unlabeled arrows */
            addstate(state->trans1, list);
            addstate(state->trans2, list);
            return; // bail out so we dont add split state to list
        }
        list->state->at(list->size++) = state;
    }

    void startlist(State *start, StateList *list)
    {
        this->listid++;
        addstate(start, list);
    }

    /*
     * Comment from (http://swtch.com/~rsc/regexp/)
     * "Step the NFA from the states in [curr_statelist]
     * past the character [charval],
     * to create next NFA state set [next_statelist]."
     * variable names replace with names used in this file
     */
    void step(StateList *curr_statelist, int charval, StateList *next_statelist)
    {
        State *state;

        this->listid++;
        next_statelist->size = 0;
        for (int i = 0; i < curr_statelist->size; i++)
        {
            state = curr_statelist->state->at(i);
            if (state->ch == charval || state->ch == '.')
            {
                // all states in the list are not split states
                // so they will only have trans1 (transition 1)
                addstate(state->trans1, next_statelist);
            }
        }
    }

public:
    Solution()
    {
        this->size = 0;
        this->listid = 0;
        this->matchstate = new State(this);
    }
    ~Solution()
    {
        if (this->start)
        {
            delete this->start;
        }
        else
        {
            delete this->matchstate;
        }
    }

    /**
     *  Comment from (http://swtch.com/~rsc/regexp/)
     *  "Check whether state list contains a match."
     * */
    bool matched(StateList *l)
    {
        for (int i = 0; i < l->size; i++)
            if (l->state->at(i) == this->matchstate)
                return true;
        return false;
    }

    bool
    isMatch(std::string s, std::string p)
    {
        std::string postre = this->get_postfix_re(&p);
        StateList curr_statelist;
        StateList next_statelist;
        this->start = this->compile_nfa(&postre);
        curr_statelist.state->resize(this->size);
        next_statelist.state->resize(this->size);

        int i, charval;
        StateList *temp = nullptr;

        startlist(start, &curr_statelist);
        for (auto ch : s)
        {
            charval = ch & 0xFF;
            step(&curr_statelist, charval, &next_statelist);
            temp = &curr_statelist;
            curr_statelist = next_statelist;
            next_statelist = *temp; /* swap clist, nlist */
        }
        return matched(&curr_statelist);
    }
};

int main()
{
    Solution re;
    std::string restr;
    std::string str;
    std::cout << "Enter string: ";
    getline(std::cin, str);
    std::cout << "Enter regex: ";
    getline(std::cin, restr);
    std::cout << re.isMatch(str, restr) << std::endl;
    return 0;
}