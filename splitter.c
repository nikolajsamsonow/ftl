#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int the_real_main(int argc, char **argv);

int main (int argc, char **argv)
{
  const char sep[] = {0x7F, 0x45, 0x4C, 0x46};
  size_t sep_size = sizeof (sep) / sizeof (char);

  const char target_filename[] = ".cur_input";
  size_t target_filename_len = strlen (target_filename);

  const size_t MAX_FILES = 8192;

  int idx = 0;
  for (int i = 1; i < argc; ++i)
    {
      size_t len = strlen (argv[i]);

      if (len >= target_filename_len)
        {
          /* we know that afl's input always ends with ".cur_input" */
          len -= target_filename_len;
          if (strcmp (argv[i] + len, target_filename) == 0)
            {
              idx = i;
              break;
            }
        }
    }

  if (idx == 0)
    return the_real_main (argc, argv);

  FILE *cur_input = fopen (argv[idx], "rb");
  assert(cur_input && "couldn't open .cur_input!\n");

  /* copy .cur_input into a buffer */
  fseek (cur_input, 0, SEEK_END);
  size_t buf_size = ftell (cur_input);
  if (buf_size < sep_size)
    {
      fclose(cur_input);
      return the_real_main(argc, argv);
    }
  fseek (cur_input, 0, SEEK_SET);
  char *buf = (char *) malloc (buf_size);
  fread (buf, 1, buf_size, cur_input);
  fclose (cur_input);

  size_t file_count = 1;
  size_t split_offsets[MAX_FILES];

  split_offsets[0] = 0;
  /* slow! */
  for (size_t i = sep_size; i < buf_size - sep_size; ++i)
    {
      if (memcmp (buf + i, sep, sep_size) == 0)
        {
          split_offsets[file_count] = i;

          ++file_count;
          i += sep_size;

          if (file_count >= MAX_FILES)
            assert(0 && "way too much files!\n");
        }
    }
  split_offsets[file_count] = buf_size;

  int new_argc = argc - 1 + file_count;
  char **new_argv = (char **) malloc (sizeof (char *) * new_argc);
  const size_t SUFFIX_BUF_LEN = 16;
  char suffix_buf[SUFFIX_BUF_LEN];

  /* make new argv */
  int argv_offset = 0;
  for (int i = 0; i < argc; ++i)
    {
      if (i == idx)
        {
          new_argv[i] = (char *) malloc(strlen(argv[idx]) + SUFFIX_BUF_LEN);
          strcpy(new_argv[i], argv[idx]);
          strcat(new_argv[i], ".0.o");
          FILE *tmp = fopen (new_argv[i], "wb");
          assert(tmp && "couldn't open a temp file!\n");
          fwrite (buf, 1, split_offsets[1], tmp);
          fclose (tmp);

          for (int j = 1; j < file_count; ++j)
            {
              ++argv_offset;
              new_argv[i + argv_offset] = (char *) malloc(strlen(argv[idx]) + SUFFIX_BUF_LEN);
              strcpy(new_argv[i + argv_offset], argv[idx]);
              sprintf(suffix_buf, ".%d.o", j);
              strcat(new_argv[i + argv_offset], suffix_buf);

              FILE *tmp = fopen (new_argv[i + argv_offset], "wb");
              assert(tmp && "couldn't open a temp file!\n");
              fwrite (buf + split_offsets[j], 1, split_offsets[j + 1] - split_offsets[j], tmp);
              fclose (tmp);
            }
        }
      else
        {
          size_t len = strlen (argv[i]);
          new_argv[i + argv_offset] = (char *) malloc (len + 1);
          strcpy (new_argv[i + argv_offset], argv[i]);
        }
    }
  free (buf);

  /* finally, run the real main */
  int result = the_real_main (new_argc, new_argv);

  for (int i = 0; i < new_argc; ++i)
    free (new_argv[i]);
  free (new_argv);

  return result;
}

