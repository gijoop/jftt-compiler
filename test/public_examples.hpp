#pragma once

#include <string_view>

namespace PublicExamples {
    constexpr std::string_view example1 = R"(
# Równanie diofantyczne mx-ny=nwd(m,n) (z)

PROCEDURE de(I m,I n,O x,O y,O z) IS
  a,b,r,s,reszta,iloraz,rr,ss,tmp
IN
  a:=m;
  b:=n;
  x:=1;
  y:=0;
  r:=n;
  s:=m-1;
  WHILE b>0 DO
    # niezmiennik: NWD(m,n)=NWD(a,b) i a=mx-ny i b=mr-ns
    reszta:=a%b;
    iloraz:=a/b;
    a:=b;
    b:=reszta;
    rr:=r;
    tmp:=iloraz*r;
    IF x<tmp THEN
      r:=n*iloraz;
    ELSE
      r:=0;
    ENDIF
    r:=r+x;
    r:=r-tmp;
    ss:=s;
    tmp:=iloraz*s;
    IF y<tmp THEN
      s:=m*iloraz;
    ELSE
      s:=0;
    ENDIF
    s:=s+y;
    s:=s-tmp;
    x:=rr;
    y:=ss;
  ENDWHILE
  z:=a;
END

PROGRAM IS
  m,n,x,y,nwd
IN
  READ m;
  READ n;
  de(m,n,x,y,nwd);
  WRITE x;
  WRITE y;
  WRITE nwd;
END
    )";

    constexpr std::string_view example2 = R"(
# ? 0
# ? 1
# > 46368
# > 28657

PROCEDURE pa(a,b) IS
IN
  a:=a+b;
  b:=a-b;
END

PROCEDURE pb(a,b) IS
IN
  pa(a,b);
  pa(a,b);
END

PROCEDURE pc(a,b) IS
IN
  pb(a,b);
  pb(a,b);
  pb(a,b);
END

PROCEDURE pd(a,b) IS
IN
  pc(a,b);
  pc(a,b);
  pc(a,b);
  pc(a,b);
END

PROGRAM IS
  a,b
IN
  READ a;
  READ b;
  pd(a,b);
  WRITE a;
  WRITE b;
END
    )";

    constexpr std::string_view example3 = R"(
# Fibonacci 26
# ? 1
# > 121393

PROGRAM IS
  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
IN
  READ a;
  b := a;
  c := b + a;
  d := c + b;
  e := d + c;
  f := e + d;
  g := f + e;
  h := g + f;
  i := h + g;
  j := i + h;
  k := j + i;
  l := k + j;
  m := l + k;
  n := m + l;
  o := n + m;
  p := o + n;
  q := p + o;
  r := q + p;
  s := r + q;
  t := s + r;
  u := t + s;
  v := u + t;
  w := v + u;
  x := w + v;
  y := x + w;
  z := y + x;
  WRITE z;
END
    )";

    constexpr std::string_view example4 = R"(
# Kombinacje
# ? 20
# ? 9
# > 167960

PROCEDURE s(I n, O m) IS
    o
IN
    m:=1;
    FOR i FROM n DOWNTO 2 DO
        o:=i%2;
        IF o=1 THEN
            m:=m*i;
        ELSE
            m:=i*m;
        ENDIF
    ENDFOR
END

PROCEDURE bc(I n,I k,O m) IS
   p,a,b,c
IN
   s(n,a);
   p:=n-k;
   s(p,b);
   s(k,c);
   m:=a/b;
   m:=m/c;
END

PROGRAM IS
    n,k,w
IN
    READ n;
    READ k;
    bc(n,k,w);
    WRITE w;
END
    )";

    constexpr std::string_view example5 = R"(
# a ^ b mod c
# ? 1234567890
# ? 1234567890987654321
# ? 987654321
# > 674106858

PROCEDURE power(I a,I b,I c,O d) IS
    pot,wyk,o
IN
    d:=1;
    wyk:=b;
    pot:=a%c;
    WHILE wyk>0 DO
	o:=wyk%2;
	IF o=1 THEN
	    d:=d*pot;
	    d:=d%c;
	ENDIF
	wyk:=wyk/2;
	pot:=pot*pot;
	pot:=pot%c;
    ENDWHILE
END

PROGRAM IS
   a,b,c,d
IN
   READ a;
   READ b;
   READ c;
   power(a,b,c,d);
   WRITE d;
END
    )";

    constexpr std::string_view example6 = R"(
# Silnia+Fibonacci
# ? 20
# > 2432902008176640000
# > 6765

PROGRAM IS
    f[0:100], s[0:100], i[0:100], n, k, l
IN
    READ n;
    f[0] := 0;
    s[0] := 1;
    i[0] := 0;
    f[1] := 1;
    s[1] := 1;
    i[1] := 1;
    FOR j FROM 2 TO n DO
        k := j - 1;
        l := k - 1;
	i[j] := i[k] + 1;
	f[j] := f[k] + f[l];
        s[j] := s[k] * i[j];
    ENDFOR
    WRITE s[n];
    WRITE f[n];
END
    )";

    constexpr std::string_view example7 = R"(
# zagnieżdżone pętle 
#	0 0 0
#	31000 40900 2222010
#	
#	1 0 2
#	31001 40900 2222012

PROGRAM IS
	a, b, c, i, j, k
IN
	READ a;
	READ b;
	READ c;
	i:=111091;
	WHILE i<=111110 DO
		j:=209;
		WHILE j>=200 DO
			k:=11;
			WHILE k<=20 DO
				a:=a+k;
				k:=k+1;
			ENDWHILE
			b:=b+j;
			j:=j-1;
		ENDWHILE
		c:=c+i;
                i:=i+1;
	ENDWHILE
	WRITE a;
	WRITE b;
	WRITE c;
END
    )";

    constexpr std::string_view example8 = R"(
# sortowanie

PROCEDURE shuffle(T t, n) IS
  q, w
IN
  q:=5;
  w:=1;
  FOR i FROM 1 TO n DO
    w:=w*q;
    w:=w%n;
    t[i]:=w;
  ENDFOR
  t[n]:=0;
END

PROCEDURE sort(T t, n) IS
  x, j, k
IN
  FOR i FROM 2 TO n DO
    x:=t[i];
    j:=i;
    WHILE j>1 DO
      k :=j-1;
      IF t[k]>x THEN
        t[j]:=t[k];
        j:=j-1;
      ELSE
        k:=j;
        j:=1;
      ENDIF
    ENDWHILE
    t[k]:=x;
  ENDFOR
END

PROCEDURE write(T t, n) IS
IN
  FOR i FROM 1 TO n DO
    WRITE t[i];
  ENDFOR
END

PROGRAM IS
  t[1:23], n
IN
  n:=23;
  shuffle(t,n);
  write(t,n);
  WRITE 1234567890;
  sort(t,n);
  write(t,n);
END
    )";

    constexpr std::string_view example9 = R"(
# Kombinacje 2
# ? 20
# ? 9
# > 167960

PROCEDURE factorial(T s, I n) IS
  p
IN
  s[0]:=1;
  p:=s[0];
  FOR i FROM 1 TO n DO
    s[i]:=p*i;
    p:=s[i];
  ENDFOR
END

PROCEDURE bc(I n, I k, O m) IS
  s[0:100],p
IN
  factorial(s,n);
  p:=n-k;
  m:=s[n]/s[k];
  m:=m/s[p];
END

PROGRAM IS
    n,k,w
IN
    READ n;
    READ k;
    bc(n,k,w);
    WRITE w;
END
    )";

    constexpr std::string_view exampleA = R"(
PROGRAM IS
	n, j, ta[0:24], tb[0:24], tc[0:24]
IN
	n:=24;
	tc[0]:=n;
	tc[n]:=n-tc[0];
        j:=tc[0]+1;
        FOR i FROM tc[0] DOWNTO tc[n] DO
		ta[i]:=i+1;
		tb[i]:=j-i;
	ENDFOR
        j:=tc[n];
        FOR i FROM tc[n] TO tc[0] DO
		tc[i]:=ta[i]*tb[i];
	ENDFOR
        FOR i FROM 0 TO n DO
		WRITE tc[i];
	ENDFOR
END
    )";
}; // namespace PublicExamples