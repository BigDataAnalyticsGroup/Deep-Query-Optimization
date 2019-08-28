# Deep Query Optimization
This repository contains the code that we used for our paper:

**The Case for Deep Query Optimisation**

Jens Dittrich, Joris Nix

Abstract:

Query Optimisation (QO) is the most important optimisation problem in databases. The goal of QO is to compute the best physical plan under a given cost model. In that process, physical operators are used as building blocks for the planning and optimisation process. In this paper, we propose to deepen that process. We present Deep Query Optimisation (DQO). In DQO, we break up the abstraction of a 'physical' operator to consider more fine-granular subcomponents. These subcomponents are then used to enumerate (sub-)plans both offline and at query time. This idea triggers several exciting research directions: (1) How exactly can DQO help to compute better plans than (shallow) QO and at which costs? (2) DQO can be used to precompute and synthesise database operators and any other database component as Algorithmic Views (AVs). (3) We identify the Algorithmic View Selection Problem (AVSP), i.e. which AVs should be materialized when?
This paper presents the high-level idea of DQO using an analogy inspired from biology. Then we proceed to question the terms 'physical' and 'physical operator'. We present experiments with a 'physical operator' formerly known as 'hash-based grouping'. We benchmark that operator both independently as well as in the context of DQO-enabled dynamic programming. We conclude by sketching a DQO research agenda. 

https://arxiv.org/abs/1908.08341
