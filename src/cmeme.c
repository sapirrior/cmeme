#include "cmeme.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <sys/types.h>

typedef struct {
  char *buffer;
  int length;
} MEMORY;

void  cleanMeme(CMEME *meme) {
  free(meme);
};

size_t curl_callback(char *in_data, size_t in_size, size_t in_count, void *usr) {

  size_t total_bytes = in_count * in_size;
  MEMORY *mem = (MEMORY *)usr;

  char *temp = realloc(mem->buffer, mem->length + total_bytes + 1);
  if (temp == NULL) return 1;
  mem->buffer = temp;

  memcpy(mem->buffer + mem->length, in_data, total_bytes);
  mem->length += total_bytes;
  mem->buffer[mem->length] = '\0';

  return total_bytes;
}

CMEME *getMeme() {

  // ==========
  // CURL JOB
  // ==========
  
  CURL *curl = curl_easy_init();
  if (curl == NULL) return NULL;

  MEMORY mem = {.buffer = NULL, .length = 0};

  curl_easy_setopt(curl, CURLOPT_URL, "https://meme-api.com/gimme");
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    curl_easy_cleanup(curl);
    return NULL;
  }
  curl_easy_cleanup(curl);

  // ==========
  // CJSON JOB
  // ==========
  
  cJSON *json = cJSON_Parse(mem.buffer);
  if (json == NULL) {
    free(mem.buffer);
    return NULL;
  }
  free(mem.buffer);

  cJSON *author = cJSON_GetObjectItem(json, "author");
  cJSON *url = cJSON_GetObjectItem(json, "url");
  cJSON *title = cJSON_GetObjectItem(json, "title");
  cJSON *ups = cJSON_GetObjectItem(json, "ups");
  cJSON *subreddit = cJSON_GetObjectItem(json,  "subreddit");

  // ==========
  // MAIN JOB
  // ==========

  CMEME *cmeme = calloc(1, sizeof(CMEME));
  if (cmeme == NULL) {
    cJSON_Delete(json);
    return NULL;
  }
  
  if (
    !cJSON_IsString(author) ||
    !cJSON_IsString(url) ||
    !cJSON_IsString(title) ||
    !cJSON_IsNumber(ups) ||
    !cJSON_IsString(subreddit)
  ) {cJSON_Delete(json); free(cmeme); return NULL;}

  if (
    !author->valuestring ||
    !url->valuestring ||
    !title->valuestring ||
    !subreddit->valuestring
  ) {cJSON_Delete(json); free(cmeme); return NULL;}

  // SETUP STRUCT OF CMEME
  
  strncpy(cmeme->author, author->valuestring, sizeof(cmeme->author) - 1);
  cmeme->author[511] = '\0';

  strncpy(cmeme->subreddit, subreddit->valuestring, sizeof(cmeme->subreddit) - 1);
  cmeme->subreddit[511] = '\0';

  strncpy(cmeme->title, title->valuestring, sizeof(cmeme->title) - 1);
  cmeme->title[511] = '\0';

  strncpy(cmeme->url, url->valuestring, sizeof(cmeme->url) - 1);
  cmeme->url[511] = '\0';

  cmeme->ups = ups->valueint;

  cJSON_Delete(json);
  return cmeme;
}
