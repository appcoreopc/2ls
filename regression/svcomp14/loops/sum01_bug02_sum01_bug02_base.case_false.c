void __VERIFIER_assert(int cond) {
  if (!(cond)) {
    ERROR: assert(0);
  }
  return;
}
#define a (2)
extern unsigned int __VERIFIER_nondet_uint();
int main() { 
  int i, n=__VERIFIER_nondet_uint(), sn=0;
  for(i=1; i<=n; i++) {
    sn = sn + a;
    if (i==4) sn=-10;
  }
assert(sn==n*a || sn == 0);
}