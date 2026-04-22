cmeme

cmeme is a small C library for fetching meme data from a public API using libcurl and parsing JSON responses with cJSON.

FEATURES

- HTTP request handling via libcurl
- JSON parsing via cJSON
- Structured meme data output
- Shared library build support

DATA MODEL

The library returns a CMEME struct:

typedef struct {
  char subreddit[512];
  char author[512];
  char title[512];
  char url[512];
  char postLink[512];

  int nsfw;
  int spoiler;

  int64_t ups;
} CMEME;

API

CMEME *getMeme(void);

Fetches a meme from the API and returns a dynamically allocated CMEME struct.

Returns NULL on failure.

void cleanMeme(CMEME *meme);

Frees the allocated meme structure.

BUILD

mkdir -p build build/obj

gcc -fPIC -c src/cmeme.c -o build/obj/cmeme.o -Isrc

gcc -shared build/obj/cmeme.o -o build/libcmeme.so -lcurl -lcjson

gcc test.c -o build/test -Lbuild -lcmeme -lcurl -lcjson -Isrc

RUN

LD_LIBRARY_PATH=build ./build/test

DEPENDENCIES

- libcurl
- cJSON

NOTES

- Memory returned by getMeme must be freed using cleanMeme.
- Requires working HTTPS support and valid CA certificates.
