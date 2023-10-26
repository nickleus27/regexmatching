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
public:
    enum transition
    {
        single,
        split
    };

    typedef struct _state
    {
        int exits;
        char currchar;
        bool matching;
        struct _state *trans;
        struct _state *backtrans;
    } State;

    typedef struct _entry
    {
        State *start;
        State *last;
    } StackEntry;

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
    linkstates(StackEntry *preventry, State *nextstate)
    {
        State *prevstate = preventry->last ? preventry->last : preventry->start;

        prevstate->trans = nextstate; // prev state transition points to next state
        // check for a backwards transition (* operator) with wildcard
        /**
         * TODO: Need to implemnent backtrans for nextsate == '.'
         */
        if (prevstate->exits == transition::split && (prevstate->currchar == '.' || nextstate->currchar == '.'))
        {
            nextstate->exits = transition::split;
            nextstate->backtrans = prevstate->backtrans;
        }
        preventry->last = nextstate; // maintain pointer to last state
    }

    void
    matchstate(std::vector<StackEntry> *stack)
    {
        State *last = stack->front().last ? stack->front().last : stack->front().start;
        if (last->exits == transition::split)
        {
            last->matching = true;
        }
        else
        {
            State *match = (State *)malloc(sizeof(State));
            match->matching = true;
            match->currchar = '\0';
            match->exits = transition::single;
            match->trans = nullptr;
            match->backtrans = nullptr;
            last->trans = match;
        }
    }

    std::vector<StackEntry>
    compile_nfa(std::string *postre)
    {
        std::vector<StackEntry> stack;
        for (std::string::iterator ch = postre->begin(); ch != postre->end(); ch++)
        {
            if (*ch != '#') // not concat, add state to stack
            {
                if (*ch != '*') // standard char (or wildcard) add it to stack
                {
                    State *state = (State *)malloc(sizeof(State));
                    state->currchar = *ch;
                    state->exits = transition::single;
                    state->matching = false;
                    state->trans = nullptr;
                    state->backtrans = nullptr;
                    StackEntry entry = {state, nullptr};
                    stack.push_back(entry);
                }
                else // 0 or more repitions, modify transition
                {
                    StackEntry preventry = stack.back();
                    stack.pop_back();
                    State *prevstate = preventry.last ? preventry.last : preventry.start;
                    prevstate->exits = transition::split;
                    prevstate->backtrans = prevstate;

                    stack.push_back(preventry);
                }
            }
            else // pop stack symbol and link transition states (concantenate)
            {
                StackEntry entry1 = stack.back();
                stack.pop_back();
                StackEntry entry2 = stack.back();
                stack.pop_back();
                State *state = entry1.last ? entry1.last : entry1.start;
                linkstates(&entry2, state);
                stack.push_back(entry2);
            }
        }
        matchstate(&stack);
        return stack;
    }

    bool
    isMatch(std::string s, std::string p)
    {
        std::string re = get_postfix_re(&p);
        std::cout << "post reg " << re << std::endl;
        State *state = compile_nfa(&re).front().start;
        std::string::iterator ch = s.begin();
        std::string::iterator end = s.end();

        while (ch != s.end())
        {
            if (!state)
            {
                return false;
            }
            if (state->exits == transition::single) // single state transition
            {
                if (state->currchar == *ch || state->currchar == '.')
                {
                    state = state->trans;
                }
                else
                {
                    return false;
                }
            }
            else // multi state transition
            {
                /**
                 * TODO: Need to implement backtracking (backtrans)
                 */
                State *lookahead = state->trans;
                if (lookahead && (lookahead->currchar == *ch || lookahead->currchar == '.'))
                {
                    if (lookahead->exits == transition::single)
                    {
                        state = lookahead->trans;
                    }
                    else
                    {
                        state = lookahead;
                    }
                }
                else if (state->currchar == *ch || state->currchar == '.')
                {
                    state = state->backtrans;
                }
                else // no match
                {
                    return false;
                }
            }
            ch++;
        }
        if ((state && !state->matching) || ch != end)
        {
            return false;
        }
        return true;
    }
};

/**
 * TODO: Failing testcases:
 * mississippi
 * mis*is*ip*.
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