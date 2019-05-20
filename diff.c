#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARGC_ERROR 1
#define TOOMANYFILES_ERROR 2
#define CONFLICTING_OUTPUT_OPTIONS 3

#define MAXSTRINGS 1024
#define MAXPARAS 4096

#define HASHLEN 200
#define STAT_ERROR 5

#include "para.h"
#include "util.h"
#include "diff.h"
#include <sys/stat.h>
#include <time.h>

char buf[BUFLEN];
char *strings1[MAXSTRINGS], *strings2[MAXSTRINGS];
int showversion = 0, showbrief = 0, ignorecase = 0, report_identical = 0, showsidebyside = 0;
int showleftcolumn = 0, showunified = 0, showcontext = 0, suppresscommon = 0, diffnormal = 0;

int count1 = 0, count2 = 0;

char* diff_last_modified(const char* filename) {
  static char diff_formatted_lastmod[BUFLEN];
  char tmzone[10];
  struct stat sb;
  if (lstat(filename, &sb) == -1) {
    fprintf(stderr, "Could not read stat information from file: '%s'", filename);
    exit(STAT_ERROR);
  }
  memset(diff_formatted_lastmod, 0, sizeof(diff_formatted_lastmod));
  struct tm* ti = localtime(&sb.st_mtime);;                     // years start at 1900,  months zero-indexed
  struct tm* gmtimeinfo = gmtime(&sb.st_mtime);
  int gmt = gmtimeinfo->tm_hour;
  if (gmt < 12) { gmt += 24; }    // prevent wrap-around error  (e.g., gmt is 0400, but local time is 2100)
  snprintf(tmzone, sizeof(timezone), "%03d00", (ti->tm_hour - gmt));
  snprintf(diff_formatted_lastmod, sizeof(diff_formatted_lastmod), "%4d-%02d-%02d %02d:%02d:%012.9f %s",
           ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday,
           ti->tm_hour, ti->tm_min, (float)ti->tm_sec, tmzone);
  return diff_formatted_lastmod;
}



void version(void) {
  printf("\n\n\ndiff (CSUF diffutils) 1.0.0\n");
  printf("Copyright (C) 2014 CSUF\n");
  printf("This program comes with NO WARRANTY, to the extent permitted by law.\n");
  printf("You may redistribute copies of this program\n");
  printf("under the terms of the GNU General Public License.\n");
  printf("For more information about these matters, see the file named COPYING.\n");
  printf("Written by Brian Miller (based on the starter code by William McCarthy)\n");
}

void brief(para* p, para* q){
  para* p2 = p;
  para* q2 = q;
  while(p2 != NULL && q2 != NULL){
    if(para_equal(p2,q2) == 0){
      printf("\nFiles differ.\n");
      break;
    }
    p2 = para_next(p2);
    q2 = para_next(q2);
  }
  p2 = NULL;
  q2 = NULL;
  free(p2);
  free(q2);
}

void identical(para* p, para* q){
  para* p2 = p;
  para* q2 = q;
  int nogo = 0;
  while(p2 != NULL && q2 != NULL){
    if(para_equal(p2,q2) == 0){
      nogo = 1;
      break;
    }
    p2 = para_next(p2);
    q2 = para_next(q2);
  }
  if(nogo == 0){printf("\nFiles are identical.\n\n");}
  p2 = NULL;
  q2 = NULL;
  free(p2);
  free(q2);
}

void print_sidebyside(para* p, para* q){
  int foundmatch = 0;
  para* qlast = q;
if(suppresscommon){
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, printright);
        q = para_next(q);
        qlast = q;
      }
      //int flag = 0;
      //for(int i = p->start; i <= p->stop && i != p->filesize; ++i){
        //flag = strcmp(p->base[i],q->base[i]);
      //}
      //if(flag != 0){
        //para_print(p, printleftmarked);
        //para_print(q, printrightmarked);
      //}
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, printright);
    q = para_next(q);}
}
else{
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, printright);
        q = para_next(q);
        qlast = q;
      }
      //int flag = 0;
      //for(int i = p->start; i <= p->stop && i != p->filesize; ++i){
        //flag = strcmp(p->base[i],q->base[i]);
      //}
      //if(flag == 0)
      para_print(q, printboth);
      //else{
      //  para_print(p, printleftmarked);
      //  para_print(q, printrightmarked);
      //}
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, printright);
    q = para_next(q);}
}
}

void print_leftcolumn(para* p, para* q){
  int foundmatch = 0;

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, printright);
        q = para_next(q);
        qlast = q;
      }
        para_print(p, printleftonly);
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, printright);
    q = para_next(q);}
}

void print_context(para* p, para* q){
  para_printinfo(p, q);

}

void print_unified(para*p, para* q){
  para_printinfo(p, q);

}

void print_normal(para* p, para* q){
  int foundmatch = 0;

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, print_normalright);
        q = para_next(q);
        qlast = q;
      }
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, print_normalleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, print_normalright);
    q = para_next(q);}
}

void todo_list(void) {
  printf("\nTODO: Fix '|' comparison");
  printf("\nTODO: Normal");
  printf("\nTODO: Context");
  printf("\nTODO: Show unified\n");
}


void loadfiles(const char* filename1, const char* filename2) {
  memset(buf, 0, sizeof(buf));
  memset(strings1, 0, sizeof(strings1));
  memset(strings2, 0, sizeof(strings2));

  FILE *fin1 = openfile(filename1, "r");
  FILE *fin2 = openfile(filename2, "r");

  while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) { strings1[count1++] = strdup(buf); }  fclose(fin1);
  while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) { strings2[count2++] = strdup(buf); }  fclose(fin2);
}

void print_option(const char* name, int value) { printf("%17s: %s\n", name, yesorno(value)); }

void diff_output_conflict_error(void) {
  fprintf(stderr, "diff: conflicting output style options\n");
  fprintf(stderr, "diff: Try `diff --help' for more information.)\n");
  exit(CONFLICTING_OUTPUT_OPTIONS);
}

void setoption(const char* arg, const char* s, const char* t, int* value) {
  if ((strcmp(arg, s) == 0) || ((t != NULL && strcmp(arg, t) == 0))) {
    *value = 1;
  }
}

void showoptions(const char* file1, const char* file2) {
  printf("diff options...\n");
  print_option("diffnormal", diffnormal);
  print_option("show_version", showversion);
  print_option("show_brief", showbrief);
  print_option("report_identical", report_identical);
  print_option("show_sidebyside", showsidebyside);
  print_option("show_leftcolumn", showleftcolumn);
  print_option("suppresscommon", suppresscommon);
  print_option("showcontext", showcontext);
  print_option("show_unified", showunified);

  printf("file1: %s,  file2: %s\n\n\n", file1, file2);

  printline();
}


void init_options_files(int argc, const char* argv[]) {
  int cnt = 0;
  const char* files[2] = { NULL, NULL };

  while (argc-- > 0) {
    const char* arg = *argv;
    setoption(arg, "-v",       "--version",                  &showversion);
    setoption(arg, "-q",       "--brief",                    &showbrief);
    setoption(arg, "-s",       "--report-identical-files",   &report_identical);
    setoption(arg, "--normal", NULL,                         &diffnormal);
    setoption(arg, "-y",       "--side-by-side",             &showsidebyside);
    setoption(arg, "--left-column", NULL,                    &showleftcolumn);
    setoption(arg, "--suppress-common-lines", NULL,          &suppresscommon);
    setoption(arg, "-c",       "--context",                  &showcontext);
    setoption(arg, "-u",       "showunified",                &showunified);
    if (arg[0] != '-') {
      if (cnt == 2) {
        fprintf(stderr, "apologies, this version of diff only handles two files\n");
        fprintf(stderr, "Usage: ./diff [options] file1 file2\n");
        exit(TOOMANYFILES_ERROR);
      } else { files[cnt++] = arg; }
    }
    ++argv;   // DEBUG only;  move increment up to top of switch at release
  }

  if (!showcontext && !showunified && !showsidebyside && !showleftcolumn) {
    diffnormal = 1;
  }

  loadfiles(files[0], files[1]);

  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  function_calls(p,q);
  showoptions(files[0], files[1]);
}

void function_calls(para* p, para* q){


  if (showversion) { version(); }

  if(showbrief) { brief(p,q);}

  if(report_identical) {identical(p,q);}

  if(showsidebyside) {print_sidebyside(p,q);}

  if(showleftcolumn) {print_leftcolumn(p,q);}

  if(showcontext) {print_context(p,q);}

  if (((showsidebyside || showleftcolumn) && (diffnormal || showcontext || showunified)) ||
      (showcontext && showunified) || (diffnormal && (showcontext || showunified))) {

    diff_output_conflict_error();
  }
if(diffnormal){
  print_normal(p,q);
}

}


int main(int argc, const char * argv[]) {
  init_options_files(--argc, ++argv);
//  todo_list();
  return 0;
}
