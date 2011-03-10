clc
clear all

A = rand(4)
saveMatrix(A, 'A.mat')

B = rand(4)
saveMatrix(B, 'B.mat')

C = rand(4, 2)
saveMatrix(C, 'C.mat')

D = rand(2, 5)
saveMatrix(D, 'D.mat')

E = rand(4, 2)
saveMatrix(E, 'E.mat')

F = E'
saveMatrix(F, 'F.mat')

g = rand(4, 1);
saveVector(g, 'g.mat')

h = rand(4, 1);
saveVector(h, 'h.mat')

I = cov(rand(5, 4));
saveMatrix(I, 'I.mat')

J = rand(3, 2)
saveMatrix(J, 'J.mat')

k = rand(3, 1)
saveVector(k, 'k.mat')
 
eye(3)
zeros(3)

A+B
A-B
C*D
A\C
C'/A'
A.*B
sum((F*A).*F, 2)
dist2(A, F)

g'*A
A*g
g'*A*g

C+3.14
C-3.14
C*3.14
C/3.14

3.14+C
3.14-C
3.14*C
3.14./C

g'+h'
g'-h'
g'.*h'
g'./h'
g'*h

g'+3.14
g'-3.14
g'*3.14
g'/3.14

3.14+g'
3.14-g'
3.14*g'
3.14./g'

-C
-g
trace(A)
trace(A*B)
sum(C, 1)
sum(C, 2)
sum(g)
prod(g)

inv(A)
det(A)
logdet(I)
log(g')
g*g'

repmat(J, 2, 3)
[g' k']
diag(A)
diag(g)
A'

