#include <stdio.h>
#include "cmeme.h"

int main() {
    CMEME *meme = getMeme();

    if (meme == NULL) {
        fprintf(stderr, "Failed to fetch meme\n");
        return 1;
    }

    printf("=== MEME ===\n");
    printf("Subreddit : %s\n", meme->subreddit);
    printf("Author    : %s\n", meme->author);
    printf("Title     : %s\n", meme->title);
    printf("URL       : %s\n", meme->url);
    printf("Upvotes   : %ld\n", (long)meme->ups);

    cleanMeme(meme);
    fflush(stdout);
    return 0;
}
