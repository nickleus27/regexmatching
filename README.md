# regexmatching

I have been interested in regular expressions and I have been reading Ken Thompson's Regular Expression Search Algorithm paper that can be found here: https://dl.acm.org/doi/pdf/10.1145/363347.363387 Russ Cox's implements and explains this algorithm here, https://swtch.com/~rsc/regexp/regexp1.html, and provides code here https://swtch.com/~rsc/regexp/nfa.c.txt.

What sparked my curiosity was working on solving LeetCode problem # 10. Regular Expression Matching. So this repo is an attempt to modify Russ Cox's implementation of Ken Thompson's algorithm into CPP code to solve the LeetCode problem.

*NOTE* Getting some segfaults with the deconstructors (delete) of the heap memory. Need to fix that.