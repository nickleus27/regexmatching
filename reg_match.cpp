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
        int currstate;
        char currchar;
        struct _state *trans;
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
    linkstates(StackEntry *preventry, State *state)
    {
        if (preventry->last)
        {
            preventry->last->trans = state; // prev char points to next state
        }
        else
        {
            preventry->start->trans = state; // prev char points to next state
        }
        preventry->last = state;
    }

    std::vector<StackEntry>
    compile_nfa(std::string *postre)
    {
        std::vector<StackEntry> stack;
        for (std::string::iterator ch = postre->begin(); ch != postre->end(); ch++)
        {
            if (*ch != '#')
            {
                if (*ch != '*')
                {
                    State *state = (State*)malloc(sizeof(State));
                    state->currchar = *ch;
                    state->currstate = transition::single;
                    state->trans = nullptr;
                    StackEntry entry = {state, nullptr};
                    stack.push_back(entry);
                }
                else
                {
                    StackEntry preventry = stack.back();
                    stack.pop_back();
                    if (preventry.last)
                    {
                        preventry.last->currstate = transition::split;
                    }
                    else
                    {
                        preventry.start->currstate = transition::split;
                    }
                    stack.push_back(preventry);
                }
            }
            else
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
        return stack;
    }

    bool
    isMatch(std::string s, std::string p)
    {
        std::string re = get_postfix_re(&p);
        State *state = compile_nfa(&re).front().start;

        for (std::string::iterator ch = s.begin(); ch != s.end(); ch++)
        {
            if (!state)
            {
                return false;
            }
            if (state->currstate == transition::single)
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
            else
            {
                State *trans = state->trans;
                if (trans && trans->currchar == *ch)
                {
                    state = trans;
                }
                else
                {
                    if (state->currchar != *ch && state->currchar != '.')
                    {
                        return false;
                    }
                }
            }
        }
        if (state && state->trans)
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