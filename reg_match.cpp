#include <vector>
#include <string>

class Solution
{
public:
    enum
    {
        single,
        split,
        match
    };

    typedef struct _state
    {
        int currstate;
        int lastlist;
        char currchar;
        struct _state *trans1;
        struct _state *trans2;
    } State;

    typedef struct _frag
    {
        State *start;
        State *trans[];
    } Frag;

    bool
    isMatch(std::string s, std::string p)
    {
        std::vector<Frag> stack;
        for (int i = 0; i < p.size(); i++)
        {
            if (p[i] != '.')
                ;
        }

        // construct state table

        return false;
    }
};

int main()
{
    return 0;
}