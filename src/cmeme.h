#ifndef CMEME_H
#define CMEME_H

#include <stdint.h>

typedef struct {
  char subreddit[512];
  char author[512];
  char title[512];
  char url[512];
  char postLink[512];

  // extra metadata
  int nsfw;
  int spoiler;
  int32_t ups;

  // optional preview (store first image URL only)
  char preview[1024];
} CMEME;

CMEME *getMeme();
void cleanMeme(CMEME *meme);

#endif
