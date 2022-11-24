# Q1 Editors-Reporters Problem

<b> Question Description </b>
- Given the number of editors n and the number of articles m, we have to assign the articles to the editors.
- Each editor will randomly pick an article and then accept or reject it.
- Each editor requires exactly one second to read an article. While reading no other editor can read the same article.
- If an editor is reading an article, other editors should not wait on the same article.
- If an editor accepts an article, no other editor can read it.
- Program ends when there is no article that can be accepted by any editor. Some articles may not be accepted by anyone.

***
<b> Solution </b>
- The values n and m are passed as command line arguments. There is a list which keeps track of articles and the editors who picked the articles.
- A new thread is created for each editor and the thread loops until there is no article left to read.
- There is also a list of semaphores. Whenever an article is picked up,  the semaphore for that article if first locked and then the editor reads it for 1 second (simulated using the sleep function).
- Before picking up an article the thread will check if the article is already being read by someone (done using the sem_getvalue() function). If that article is already being read by someone, a different article will be picked up by the current thread. This makes sure that there is no waiting.
- After finishing reading the article, the semaphore is unlocked and other editors can read it if it has been rejected.
- Logs have been used to see which editor has picked up which article and whether the article has been accepted.



