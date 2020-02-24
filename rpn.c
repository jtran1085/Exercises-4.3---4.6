//
//  main.c
//  rpn
//
//  Created by William McCarthy on 192//20.
//  Copyright © 2020 William McCarthy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define MAXOP   100    /* max size of operand/operator */
#define NUMBER '0'     /* signal that a number was found */
#define MAXVAL  100

size_t sp = 0;   // aka unsigned long -- printf using %zu
double val[MAXVAL];   // stack of values

char buf[BUFSIZ];
size_t bufp = 0;

int getch_(void) { return (bufp > 0) ? buf[--bufp] : getchar(); }
void ungetch_(int c) {
  if (bufp >= BUFSIZ) { fprintf(stderr, "ungetch: too many characters\n");  return; }
  buf[bufp++] = c;
}

int getop(char* s) {
  int i, c;
  while ((s[0] = c = getch_()) == ' ' || c == '\t') { }  // skip whitespace
  s[1] = '\0';
  
  if (!isdigit(c) && c != '.' && c!='-') { return c; }  // if not a digit, return

  i = 0;
  
  if(c=='-'){
    if(isdigit(c=getch_()) || c == '.'){
      s[++i]=c;}
    else{
      if(c!=EOF)
      ungetch_(c);
      return '-';}}
  if (isdigit(c)) {  // get digits before '.'
    while (isdigit(s[++i] = c = getch_())) { }
  }
  if (c == '.') {    // get digits after decimal (if any)
    while (isdigit(s[++i] = c = getch_())) { }
  }
  s[i] = '\0';
  if (c != EOF) { ungetch_(c); }
  
  return NUMBER;      // return type is NUMBER, number stored in s
}

double pop(void) {
  if (sp == 0) { fprintf(stderr, "stack underflow\n");  return 0.0; }
  return val[--sp];
}

void push(double f) {
  if (sp == MAXVAL) { fprintf(stderr, "stack overflow -- can't push %g\n", f);  return; }
  val[sp++] = f;
}

void rpn(void) {
  int type,va;
  double op1,op2, v;
  char s[BUFSIZ];
  double var[26];

  while ((type = getop(s)) != EOF) {
    switch(type) {
      case '\n':   v = pop(); printf("\t%.8g\n", v);  break;
      case NUMBER:  push(atof(s));              break;
      case '+':     push(pop() + pop());        break;
      case '*':     push(pop() * pop());        break;
      case '-':     push(-(pop() - pop()));     break;
      case '/':
        if ((op2 = pop()) == 0.0) { fprintf(stderr, "divide by zero\n");  break; }
        push(pop() / op2);
        break;

      case '%':
      if ((op2 = pop()) == 0.0) { fprintf(stderr, "mod by zero\n"); break; }
      push(fmod(pop(),op2)); break;//4.3

      case 'p': printf("\t%.8g", (op2=pop())); push(op2); break; //4.4
      case 'd': op2 = pop(); push(op2); push(op2); break;//4.4
      case '$': op2 = pop(); op1 = pop(); push(op2); push(op1); break; //4.4

      case 's': push(sin(pop())); break;//4.5
      case '^': op2 = pop(); push(pow(pop(),op2)); break;//4.5
      case 'e': push(exp(pop())); break;//4.5

      case '=': pop();
      if(va >= 'A' && va <= 'Z'){var[va-'A'] = pop(); break;}
      fprintf(stderr, "invalid variable name\n"); break;//4.6
      default:      
      if(type >= 'A' && type <= 'Z') {push(var[type- 'A']); break;}//4.6
      else if(type == 'v'){push(v); break;}//4.6

      fprintf(stderr, "unknown command %s\n", s);  break;
    }
    va = type;
  }
}

int main(int argc, const char * argv[]) {
  rpn();

  return 0;
}
