#ifndef CMEME_H
#define CMEME_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
  char subreddit[512];
  char author[512];
  char title[512];
  char url[512];
  int64_t ups;
} CMEME;

// MAIN lib
CMEME *getMeme();
void cleanMeme(CMEME *meme);

#endif
