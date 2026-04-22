#include "cmeme.h"
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

typedef struct {
  char *buffer;
  int length;
} MEMORY;

void cleanMeme(CMEME *meme) {
  free(meme);
}

size_t curl_callback(char *in_data, size_t in_size, size_t in_count, void *usr) {

  size_t total_bytes = in_size * in_count;
  MEMORY *mem = (MEMORY *)usr;

  char *temp = realloc(mem->buffer, mem->length + total_bytes + 1);
  if (!temp) return 0;

  mem->buffer = temp;

  memcpy(mem->buffer + mem->length, in_data, total_bytes);
  mem->length += total_bytes;
  mem->buffer[mem->length] = '\0';

  return total_bytes;
}

CMEME *getMeme() {

  CURL *curl = curl_easy_init();
  if (!curl) return NULL;

  MEMORY mem = {.buffer = NULL, .length = 0};

  curl_easy_setopt(curl, CURLOPT_URL, "https://meme-api.com/gimme");
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);

  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK) {
    free(mem.buffer);
    return NULL;
  }

  cJSON *json = cJSON_Parse(mem.buffer);
  free(mem.buffer);

  if (!json) return NULL;

  // required fields
  cJSON *author = cJSON_GetObjectItem(json, "author");
  cJSON *url = cJSON_GetObjectItem(json, "url");
  cJSON *title = cJSON_GetObjectItem(json, "title");
  cJSON *subreddit = cJSON_GetObjectItem(json, "subreddit");
  cJSON *postLink = cJSON_GetObjectItem(json, "postLink");
  cJSON *ups = cJSON_GetObjectItem(json, "ups");

  // optional fields
  cJSON *nsfw = cJSON_GetObjectItem(json, "nsfw");
  cJSON *spoiler = cJSON_GetObjectItem(json, "spoiler");
  cJSON *preview = cJSON_GetObjectItem(json, "preview");

  if (
    !cJSON_IsString(author) ||
    !cJSON_IsString(url) ||
    !cJSON_IsString(title) ||
    !cJSON_IsString(subreddit) ||
    !cJSON_IsString(postLink) ||
    !cJSON_IsNumber(ups)
  ) {
    cJSON_Delete(json);
    return NULL;
  }

  CMEME *cmeme = calloc(1, sizeof(CMEME));
  if (!cmeme) {
    cJSON_Delete(json);
    return NULL;
  }

  strncpy(cmeme->author, author->valuestring, sizeof(cmeme->author) - 1);
  strncpy(cmeme->subreddit, subreddit->valuestring, sizeof(cmeme->subreddit) - 1);
  strncpy(cmeme->title, title->valuestring, sizeof(cmeme->title) - 1);
  strncpy(cmeme->url, url->valuestring, sizeof(cmeme->url) - 1);
  strncpy(cmeme->postLink, postLink->valuestring, sizeof(cmeme->postLink) - 1);

  cmeme->ups = ups->valueint;
  cmeme->nsfw = nsfw ? nsfw->valueint : 0;
  cmeme->spoiler = spoiler ? spoiler->valueint : 0;

  // preview: take first image if available
  if (preview && cJSON_IsArray(preview) && cJSON_GetArraySize(preview) > 0) {
    cJSON *first = cJSON_GetArrayItem(preview, 0);
    if (cJSON_IsString(first) && first->valuestring) {
      strncpy(cmeme->preview, first->valuestring, sizeof(cmeme->preview) - 1);
    }
  }

  cJSON_Delete(json);
  return cmeme;
}
