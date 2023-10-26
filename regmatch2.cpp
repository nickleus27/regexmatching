#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>

/**
 * @brief Use Ken Thompsons Regex Search Algorithm
 * to compile a state table for the regex. \n
 * https://dl.acm.org/doi/pdf/10.1145/363347.363387
 */
class Solution
{
    class State;

public:
    int size;   // keep track of number of states
    int listid; // keep track of where we are at in state list
    State *matchstate; // singleton matchstate

    bool
    isMatch(std::string s, std::string p)
    {
        this->size = 0;
        this->listid = 0;
        this->matchstate = new State(this);
        this->compile_nfa(&p);

        return false;
    }

private:
    enum transition
    {
        match = 256, // numbers out of char range
        split = 257 // numbers out of char range
    };

    class State // class object for different states
    {
    public:
        Solution *nfa;
        int ch;
        int lastlist;
        State *trans1;
        State *trans2;
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

    /* From (http://swtch.com/~rsc/regexp/)
     * Since the out pointers in the list are always
     * uninitialized, we use the pointers themselves
     * as storage for the Ptrlists.
     *
     * This seeems to be a little indirection magic to save space making transition list
     */
    typedef union _ptrlist
    {
        State *start;
        _ptrlist *next;
    } Ptrlist;

    typedef struct _frag // class objects for nfa fragments to keep on stack while constructing nfa
    {
        State *start;
        Ptrlist *trans;
    } Frag;
    
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
     * @param re
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
        Ptrlist *list = prevfrag->trans; // set transitions to point to the next state
        State *state = nextfrag->start;

        Ptrlist *nexttrans;

        for (; list; list = nexttrans)
        {
            nexttrans = list->next;
            list->start = state;
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
                linkstates (&prev, &next); // todo: need to pass in frag not state

                stack.push_back(frag(s, makelist(&s->trans1)));
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
};

/**
 * TODO: Failing testcases:
 * "aaa"
 * ab*a*c*a
 */

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